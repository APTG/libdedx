#include "dedx_tools.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "dedx_data_access.h"
#include "dedx_lookup_data.h"

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

/* Load (if needed) and return the internal dataset backing a config, so
 * callers can walk its exact tabulated (energy, STP) knots directly instead
 * of re-sampling the curve at an arbitrary density. dedx_get_stp() evaluates
 * a spline built from precisely these knots (dedx_spline.c: coef[i].a =
 * stopping[i]), so scanning them finds every feature the public curve can
 * actually exhibit -- no synthetic sampling can do better or worse. */
static dedx_internal_lookup_data *load_and_get_dataset(dedx_workspace *ws, dedx_config *config, int *err) {
    if (*err != 0)
        return NULL;
    if (config->loaded == 0)
        dedx_load_config(ws, config, err);
    if (*err != 0)
        return NULL;
    int id = config->cfg_id;
    if (id < 0 || id >= ws->active_datasets) {
        *err = DEDX_ERR_INVALID_DATASET_ID;
        return NULL;
    }
    return ws->loaded_data[id];
}

static void min_max_stp_over_table(const dedx_internal_lookup_data *data, double *min_stp, double *max_stp) {
    double lo = data->base[0].a;
    double hi = data->base[0].a;
    for (int i = 1; i < data->n; i++) {
        double v = data->base[i].a;
        if (v < lo)
            lo = v;
        if (v > hi)
            hi = v;
    }
    *min_stp = lo;
    *max_stp = hi;
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

/* Bisect the monotonic run of knots [lo_idx, hi_idx] for the energy where
 * the spline equals stp, refining via dedx_get_stp() (not linear knot
 * interpolation) so the result matches what callers would measure with the
 * public curve. Returns 0 and sets *solution on success; returns -1 without
 * touching *err if stp is not bracketed by this run. */
static int bisect_monotonic_run(dedx_workspace *ws,
                                dedx_config *config,
                                const dedx_internal_lookup_data *data,
                                int lo_idx,
                                int hi_idx,
                                float stp,
                                int *err,
                                double *solution) {
    double lo_x = data->base[lo_idx].x;
    double hi_x = data->base[hi_idx].x;
    double lo_v = data->base[lo_idx].a;
    double hi_v = data->base[hi_idx].a;
    double range_min = lo_v < hi_v ? lo_v : hi_v;
    double range_max = lo_v > hi_v ? lo_v : hi_v;
    if (stp < range_min || stp > range_max)
        return -1;

    int ascending = hi_v >= lo_v;
    double x1 = lo_x;
    double x2 = hi_x;
    double acc = 1e-5;
    while (fabs(x1 - x2) > acc) {
        double x_temp = (x1 + x2) / 2;
        double f_temp = dedx_get_stp(ws, config, (float) x_temp, err);
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
    *solution = (x1 + x2) / 2;
    return 0;
}

double dedx_get_inverse_stp(dedx_workspace *ws, dedx_config *config, float stp, int side, int *err) {
    if (config->ion_a <= 0) {
        *err = DEDX_ERR_ION_A_REQUIRED;
        return -1;
    }
    dedx_internal_lookup_data *data = load_and_get_dataset(ws, config, err);
    if (data == NULL)
        return -1;

    /* Stopping power vs. energy is not simply unimodal: real tables can rise
     * to the Bragg peak, fall through the minimum-ionizing point, and rise
     * again at relativistic energies (e.g. PSTAR/ICRU protons up to 10 GeV)
     * -- an arbitrary requested STP can be reachable on more than one of
     * these monotonic runs. Walk the exact tabulated knots once, bisecting
     * every run that brackets stp, and keep the lowest- and highest-energy
     * solutions found; side == 0 returns the low-energy one, side == 1 the
     * high-energy one. For the common single-peak case this is exactly the
     * old ascending/descending branch choice. See #121. */
    int found = 0;
    double x_min_found = 0;
    double x_max_found = 0;
    int seg_start = 0;
    int prev_dir = 0;

    for (int i = 1; i <= data->n; i++) {
        int is_turning = 0;
        int dir = 0;
        if (i < data->n) {
            double delta = (double) data->base[i].a - (double) data->base[i - 1].a;
            dir = (delta > 0) ? 1 : (delta < 0 ? -1 : 0);
            if (dir != 0) {
                if (prev_dir == 0)
                    prev_dir = dir;
                else if (dir != prev_dir)
                    is_turning = 1;
            }
        }
        if (i == data->n || is_turning) {
            int seg_end = (i == data->n) ? (data->n - 1) : (i - 1);
            if (seg_end > seg_start) {
                double solution;
                if (bisect_monotonic_run(ws, config, data, seg_start, seg_end, stp, err, &solution) == 0) {
                    if (!found || solution < x_min_found)
                        x_min_found = solution;
                    if (!found || solution > x_max_found)
                        x_max_found = solution;
                    found = 1;
                } else if (*err != 0) {
                    return -1;
                }
            }
            if (is_turning) {
                seg_start = i - 1;
                prev_dir = dir;
            }
        }
    }

    if (!found) {
        *err = DEDX_ERR_ENERGY_OUT_OF_RANGE;
        return -1;
    }
    return (side == 0) ? x_min_found : x_max_found;
}

double dedx_get_max_stp(dedx_workspace *ws, dedx_config *config, int *err) {
    dedx_internal_lookup_data *data = load_and_get_dataset(ws, config, err);
    if (data == NULL)
        return -1;
    double min_stp;
    double max_stp;
    min_max_stp_over_table(data, &min_stp, &max_stp);
    return max_stp;
}

double dedx_get_min_stp(dedx_workspace *ws, dedx_config *config, int *err) {
    dedx_internal_lookup_data *data = load_and_get_dataset(ws, config, err);
    if (data == NULL)
        return -1;
    double min_stp;
    double max_stp;
    min_max_stp_over_table(data, &min_stp, &max_stp);
    return min_stp;
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
