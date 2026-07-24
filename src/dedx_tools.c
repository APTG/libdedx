#include "dedx_tools.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "dedx_data_access.h"

typedef struct {
    dedx_workspace *ws;
    dedx_config *cfg;
} dedx_tools_settings;

static double adapt24(double (*func)(double x, dedx_tools_settings *set),
                      dedx_tools_settings *set,
                      double a,
                      double b,
                      double f2,
                      double f3,
                      double acc,
                      double eps,
                      double *err) {
    double h = b - a;
    double f1 = (*func)(a + h / 6, set);
    double f4 = (*func)(a + h * 5 / 6, set);
    double q2 = (f1 + f2 + f3 + f4) * h / 4;
    double q4 = ((f1 + f4) / 3 + (f2 + f3) / 6) * h;
    double tol = acc + eps * fabs(q4);
    *err = fabs(q4 - q2) / 3;
    if (*err < tol)
        return q4;

    acc /= sqrt(2);
    double mid = (a + b) / 2;
    double el = 0;
    double er = 0;
    double ql = adapt24(func, set, a, mid, f1, f2, acc, eps, &el);
    double qr = adapt24(func, set, mid, b, f3, f4, acc, eps, &er);
    *err = sqrt(el * el + er * er);
    return ql + qr;
}

static double adapt(double (*func)(double x, dedx_tools_settings *set),
                    dedx_tools_settings *set,
                    double a,
                    double b,
                    double acc,
                    double eps,
                    double *err) {
    double h = b - a;
    return adapt24(func, set, a, b, (*func)(a + h / 3, set), (*func)(a + 2 * h / 3, set), acc, eps, err);
}

static double adapt_stp(double energy, dedx_tools_settings *set) {
    int err = 0;
    double stp = dedx_get_stp(set->ws, set->cfg, energy / (set->cfg->ion_a), &err);
    if (err != 0 || stp == 0.0)
        return INFINITY;
    return 1.0 / stp;
}

/* Number of log-spaced samples used to locate the energy of maximum stopping
 * power before bisecting; matches the proven sampling density from the
 * dedx_web reference implementation this was ported from (see #121). */
#define DEDX_INVERSE_STP_SAMPLES 40

/* Number of log-spaced samples used by dedx_get_max_stp() — denser than
 * DEDX_INVERSE_STP_SAMPLES since it reports the peak value itself rather
 * than just using it to pick a bisection branch (see #121). */
#define DEDX_MAX_STP_SAMPLES 300

/* Locate the energy of maximum stopping power within [emin, emax] by
 * sampling on a log-spaced grid of n_samples points. Also reports the STP at
 * the leftmost sample (emin) so callers can tell whether a requested STP
 * lies on the ascending branch. Returns 0 on success, -1 if no sample
 * succeeded. */
static int find_stp_peak(dedx_workspace *ws,
                         dedx_config *config,
                         double emin,
                         double emax,
                         int n_samples,
                         double *e_peak,
                         double *max_stp,
                         double *stp_at_emin) {
    double log_emin = log(emin);
    double log_emax = log(emax);
    double log_step = (log_emax - log_emin) / (n_samples - 1);
    int have_sample = 0;

    *max_stp = 0.0;
    *e_peak = emin;
    *stp_at_emin = 0.0;

    for (int i = 0; i < n_samples; i++) {
        /* Use emin itself for the first sample: exp(log(emin)) is not
         * guaranteed to round-trip to exactly emin, and a reconstructed
         * value fractionally below the dataset's real lower bound would be
         * rejected by dedx_get_stp(), leaving stp_at_emin stuck at 0 and
         * making the ascending-branch check in dedx_get_inverse_stp()
         * always pass. */
        double e = (i == 0) ? emin : exp(log_emin + i * log_step);
        int stp_err = 0;
        double s = dedx_get_stp(ws, config, (float) e, &stp_err);
        if (stp_err != 0)
            continue;
        have_sample = 1;
        if (i == 0)
            *stp_at_emin = s;
        if (s > *max_stp) {
            *max_stp = s;
            *e_peak = e;
        }
    }
    return have_sample ? 0 : -1;
}

double dedx_get_inverse_csda(dedx_workspace *ws, dedx_config *config, float range, int *err) {
    if (config->ion_a <= 0) {
        *err = DEDX_ERR_ION_A_REQUIRED;
        return -1;
    }
    double acc = 1e-5;
    double min = dedx_get_min_energy(config->program, config->ion);
    double max = dedx_get_max_energy(config->program, config->ion);

    double x_temp;
    double f_temp;

    while ((max - min) > acc) {
        x_temp = (max + min) / 2;
        f_temp = dedx_get_csda(ws, config, x_temp, err);
        if (*err != 0)
            return -1;

        if (f_temp >= range) {
            max = x_temp;
        } else {
            min = x_temp;
        }
    }
    return (min + max) / 2;
}

double dedx_get_inverse_stp(dedx_workspace *ws, dedx_config *config, float stp, int side, int *err) {
    if (config->ion_a <= 0) {
        *err = DEDX_ERR_ION_A_REQUIRED;
        return -1;
    }
    if (*err != 0)
        return -1;
    if (config->loaded == 0)
        dedx_load_config(ws, config, err);
    if (*err != 0)
        return -1;

    double acc = 1e-5;
    double emin = dedx_get_min_energy(config->program, config->ion);
    double emax = dedx_get_max_energy(config->program, config->ion);

    /* Sample the curve to find the energy of maximum stopping power, then
     * bisect the physically correct monotone branch:
     *   - no interior peak (monotone descending over the full range):
     *     bisect [emin, emax] on the single descending branch.
     *   - interior peak: side == 0 selects the low/ascending branch
     *     [emin, e_peak]; side == 1 (or any stp below the ascending
     *     branch's floor at emin) selects the high/descending branch
     *     [e_peak, emax].
     * See #121 for why the previous find_min()-based approach failed. */
    double e_peak;
    double max_stp;
    double stp_at_emin;
    if (find_stp_peak(ws, config, emin, emax, DEDX_INVERSE_STP_SAMPLES, &e_peak, &max_stp, &stp_at_emin) != 0) {
        *err = DEDX_ERR_ENERGY_OUT_OF_RANGE;
        return -1;
    }

    int stp_err = 0;
    double stp_at_emax = dedx_get_stp(ws, config, (float) emax, &stp_err);
    if (stp_err != 0 || max_stp == 0.0 || stp > max_stp || stp < stp_at_emax) {
        *err = (stp_err != 0) ? stp_err : DEDX_ERR_ENERGY_OUT_OF_RANGE;
        return -1;
    }

    double log_step = (log(emax) - log(emin)) / (DEDX_INVERSE_STP_SAMPLES - 1);
    int has_peak = e_peak > emin * exp(log_step);

    double x1;
    double x2;
    int ascending;
    if (!has_peak) {
        x1 = emin;
        x2 = emax;
        ascending = 0;
    } else if (side == 0 && stp >= stp_at_emin) {
        x1 = emin;
        x2 = e_peak;
        ascending = 1;
    } else {
        x1 = e_peak;
        x2 = emax;
        ascending = 0;
    }

    double x_temp;
    double f_temp;
    while (fabs(x1 - x2) > acc) {
        x_temp = (x1 + x2) / 2;
        f_temp = dedx_get_stp(ws, config, (float) x_temp, err);
        if (*err != 0)
            return -1;

        if (ascending) {
            if (f_temp <= stp)
                x1 = x_temp;
            else
                x2 = x_temp;
        } else {
            if (f_temp >= stp)
                x1 = x_temp;
            else
                x2 = x_temp;
        }
    }
    return (x1 + x2) / 2;
}

double dedx_get_max_stp(dedx_workspace *ws, dedx_config *config, int *err) {
    if (*err != 0)
        return -1;
    if (config->loaded == 0)
        dedx_load_config(ws, config, err);
    if (*err != 0)
        return -1;

    double emin = dedx_get_min_energy(config->program, config->ion);
    double emax = dedx_get_max_energy(config->program, config->ion);

    double e_peak;
    double max_stp;
    double stp_at_emin;
    if (find_stp_peak(ws, config, emin, emax, DEDX_MAX_STP_SAMPLES, &e_peak, &max_stp, &stp_at_emin) != 0) {
        *err = DEDX_ERR_ENERGY_OUT_OF_RANGE;
        return -1;
    }
    return max_stp;
}

double dedx_get_csda(dedx_workspace *ws, dedx_config *config, float energy, int *err) {
    if (config->ion_a <= 0) {
        *err = DEDX_ERR_ION_A_REQUIRED;
        return -1;
    }
    double calculation_error = 0;
    double acc = 1e-6;
    double eps = 1e-6;
    dedx_tools_settings set;
    double range = 0.0;
    double A = config->ion_a;

    if (*err != 0)
        return -1;
    if (config->loaded == 0)
        dedx_load_config(ws, config, err);
    if (*err != 0)
        return -1;
    set.cfg = config;
    set.ws = ws;
    range = adapt(adapt_stp,
                  &set,
                  dedx_get_min_energy(config->program, config->ion) * A,
                  energy * A,
                  acc,
                  eps,
                  &calculation_error);
    return range;
}

static float conversion_factor(const int old_unit, const int new_unit, const int material, int *err) {
    const float density = dedx_internal_read_density(material, err);

    float conversion_rate;

    // convert from any old unit to MeV/cm
    switch (old_unit) {
    case DEDX_MEVCM2G:
        conversion_rate = density; // conversion MeV cm2/g --> MeV/cm
        break;
    case DEDX_MEVCM:
        conversion_rate = 1.f; // MeV/cm -> MeV/cm
        break;
    case DEDX_KEVUM:
        conversion_rate = 10.f; // keV/um -> MeV/cm
        break;
    default:
        conversion_rate = 1.f;
    }

    // convert from MeV/cm to any new unit
    switch (new_unit) {
    case DEDX_MEVCM2G:
        conversion_rate /= density; // MeV/cm -> MeV cm2/g
        break;
    case DEDX_MEVCM:
        conversion_rate /= 1.f; // MeV/cm -> MeV/cm
        break;
    case DEDX_KEVUM:
        conversion_rate /= 10.f; // MeV/cm -> keV/um
        break;
    default:
        conversion_rate = 1.f;
    }
    return conversion_rate;
}

int convert_units(const int old_unit,
                  const int new_unit,
                  const int material,
                  const int no_of_points,
                  const float *old_values,
                  float *new_values) {
    int err = 0;
    int i;
    if (old_unit == new_unit)
        return err;

    float conversion_rate = conversion_factor(old_unit, new_unit, material, &err);
    for (i = 0; i < no_of_points; i++) {
        new_values[i] = old_values[i] * conversion_rate;
    }
    return err;
}
