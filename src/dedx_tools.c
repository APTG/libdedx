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
 * stopping[i]), so scanning them finds every feature present in the
 * tabulated values -- though not necessarily every feature of the
 * interpolated curve dedx_get_stp() actually evaluates, since a spline can
 * in principle overshoot slightly between two knots. That's a property of
 * the interpolation, not something this scan (or any sampling density) can
 * see; it hasn't been observed in the tables this library ships.
 *
 * `static` (file-private, not declared in dedx_tools.h): only this
 * translation unit needs it, unlike the dedx_internal_* functions declared
 * in headers such as dedx_data_access.h for use across multiple .c files. */
static dedx_internal_lookup_data *get_loaded_dataset(dedx_workspace *ws, dedx_config *config, int *err) {
    /* Public entrypoints reset *err themselves (e.g. dedx_get_stp()) rather
     * than treating an incoming nonzero value as a precondition -- do the
     * same here so a stale error code left over in the caller's variable
     * from an unrelated earlier call can't make a valid request fail. */
    *err = DEDX_OK;

    /* config->loaded == 1 only means *some* workspace holds this config's
     * data at config->cfg_id -- not necessarily this ws (the config could
     * have been loaded into a workspace that was since freed and replaced,
     * or the caller could be reusing one dedx_config across workspaces).
     * Reload whenever the config hasn't been loaded at all, or its cfg_id
     * isn't a valid slot in *this* workspace; otherwise skip the reload on
     * the common already-loaded-here path. */
    int needs_load = 0;
    if (config->loaded == 0)
        needs_load = 1;
    else if (config->cfg_id < 0 || config->cfg_id >= ws->active_datasets)
        needs_load = 1;
    if (needs_load)
        dedx_load_config(ws, config, err);
    if (*err != 0)
        return NULL;

    int id = config->cfg_id;
    if (id < 0 || id >= ws->active_datasets) {
        /* Defensive: dedx_load_config() succeeding is expected to always
         * leave a valid cfg_id, so this should be unreachable in practice. */
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

/* Bisection convergence, as an absolute tolerance in *log-energy* space (see
 * bisect_monotonic_run() below for why bisecting there instead of in the raw
 * energy value). For small differences, d(ln x) = dx/x, i.e. a difference in
 * log-space is directly a *relative* difference in the original energy --
 * so this one constant fixes the achieved relative precision on the energy
 * result regardless of scale. 1e-6 is about ten times coarser than a
 * `float`'s own precision (~1.19e-7 relative, i.e. 24 bits of mantissa):
 * dedx_get_stp() truncates the search value to a float before evaluating
 * the spline, so refining much further than the float itself can represent
 * would just spend extra iterations re-measuring rounding noise. */
#define DEDX_INVERSE_STP_LOG_ACC 1e-6

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

    /* This run's STP values span [range_min, range_max] regardless of
     * whether the run is rising or falling; stp must land in that span for
     * a solution to exist anywhere on this run. */
    double range_min;
    double range_max;
    if (lo_v < hi_v) {
        range_min = lo_v;
        range_max = hi_v;
    } else {
        range_min = hi_v;
        range_max = lo_v;
    }
    if (stp < range_min || stp > range_max)
        return -1;

    /* Whether STP rises (ascending) or falls (descending) from lo_x to
     * hi_x tells us which half to keep at each bisection step below. */
    int ascending;
    if (hi_v >= lo_v)
        ascending = 1;
    else
        ascending = 0;

    /* Bisect in log-energy space rather than raw energy. Two reasons:
     *
     * 1. The tabulated energy grid is itself predominantly log-spaced (e.g.
     *    NIST PSTAR/ASTAR-derived tables step by a roughly constant energy
     *    *ratio*, not a constant energy difference, so that a fixed number
     *    of points can cover ~0.001 to 10000 MeV/nucleon with even relative
     *    resolution). Bisecting in the same log-scale as the data matches
     *    the coordinate the curve actually varies smoothly in.
     *
     * 2. It makes the convergence tolerance exactly a relative tolerance on
     *    the energy result (see DEDX_INVERSE_STP_LOG_ACC above), rather than
     *    an approximation of one. A run spanning many decades (e.g. the
     *    post-peak run for proton/water PSTAR, ~0.08 to 10000 MeV/nucleon)
     *    then converges in a number of steps set by log2(ln(hi_x/lo_x) /
     *    DEDX_INVERSE_STP_LOG_ACC) -- a few dozen steps regardless of scale
     *    or of where in the run the root happens to fall, since bisection
     *    always halves the bracket every step no matter which half contains
     *    the root.
     *
     * dedx_get_stp() evaluates the actual interpolating spline at any energy
     * (not just at knots), so this is correct even on the tail of a table
     * where the raw knot spacing itself happens to be linear rather than
     * log-spaced (observed for a few tables at their highest energies) --
     * the spline doesn't care how its own knots were spaced, and neither
     * does bisecting the search variable in log space. */
    double log_lo = log(lo_x);
    double log_hi = log(hi_x);
    while (fabs(log_lo - log_hi) > DEDX_INVERSE_STP_LOG_ACC) {
        double log_mid = (log_lo + log_hi) / 2;
        double x_temp = exp(log_mid);
        double f_temp = dedx_get_stp(ws, config, (float) x_temp, err);
        if (*err != 0)
            return -1;
        if (ascending) {
            /* STP too low at the midpoint -> the root is further up. */
            if (f_temp <= stp)
                log_lo = log_mid;
            else
                log_hi = log_mid;
        } else {
            /* STP still at/above target at the midpoint -> the root is
             * further up (we're descending, so STP keeps falling as energy
             * rises). */
            if (f_temp >= stp)
                log_lo = log_mid;
            else
                log_hi = log_mid;
        }
    }
    *solution = exp((log_lo + log_hi) / 2);
    return 0;
}

double dedx_get_inverse_stp(dedx_workspace *ws, dedx_config *config, float stp, int side, int *err) {
    if (config->ion_a <= 0) {
        *err = DEDX_ERR_ION_A_REQUIRED;
        return -1;
    }
    dedx_internal_lookup_data *data = get_loaded_dataset(ws, config, err);
    if (data == NULL)
        return -1;

    /* Stopping power vs. energy is not simply unimodal: real tables can rise
     * to the Bragg peak, fall through the minimum-ionizing point, and rise
     * again at relativistic energies (e.g. PSTAR/ICRU protons up to 10 GeV)
     * -- an arbitrary requested STP can be reachable on more than one of
     * these monotonic runs. Walk the exact tabulated knots once, bisecting
     * every run that brackets stp, and keep the lowest- and highest-energy
     * solutions found across ALL of them; side == 0 returns the lowest, any
     * other side value returns the highest. For the common single-peak case
     * this is exactly the old ascending/descending branch choice. See #121. */
    int found = 0;
    double x_min_found = 0;
    double x_max_found = 0;
    int seg_start = 0;
    int prev_dir = 0;

    for (int i = 1; i <= data->n; i++) {
        /* dir is the sign of the step from knot i-1 to knot i: +1 rising,
         * -1 falling, 0 flat (a flat step doesn't end the current run --
         * prev_dir is left untouched so a flat plateau stays part of
         * whichever run it interrupts). Only computed for i < data->n,
         * since there is no knot i to compare against once i == data->n. */
        int is_turning = 0;
        int dir = 0; /* stays 0 (flat) unless one of the branches below fires */
        if (i < data->n) {
            double delta = (double) data->base[i].a - (double) data->base[i - 1].a;
            if (delta > 0)
                dir = 1;
            else if (delta < 0)
                dir = -1;
            if (dir != 0) {
                if (prev_dir == 0)
                    prev_dir = dir;
                else if (dir != prev_dir)
                    is_turning = 1;
            }
        }
        /* A run ends either at a direction reversal (is_turning, and the
         * turning knot i-1 is shared with the next run) or at the last
         * knot (i == data->n, since there's nowhere further to walk). */
        if (i == data->n || is_turning) {
            int seg_end;
            if (i == data->n)
                seg_end = data->n - 1;
            else
                seg_end = i - 1;
            if (seg_end > seg_start) {
                double solution;
                if (bisect_monotonic_run(ws, config, data, seg_start, seg_end, stp, err, &solution) == 0) {
                    /* This run brackets stp, so it contributes one solution.
                     * Track the lowest- and highest-energy solutions seen so
                     * far across every run processed so far in this loop --
                     * not just this one run -- so that once the whole table
                     * has been walked, x_min_found/x_max_found hold the
                     * overall lowest/highest reachable energy regardless of
                     * how many separate runs bracketed stp along the way.
                     * `!found` covers the very first solution: there is
                     * nothing yet to compare it against, so it always
                     * becomes both the running min and the running max. */
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
    if (side == 0)
        return x_min_found;
    return x_max_found;
}

double dedx_get_max_stp(dedx_workspace *ws, dedx_config *config, int *err) {
    dedx_internal_lookup_data *data = get_loaded_dataset(ws, config, err);
    if (data == NULL)
        return -1;
    double min_stp;
    double max_stp;
    min_max_stp_over_table(data, &min_stp, &max_stp);
    return max_stp;
}

double dedx_get_min_stp(dedx_workspace *ws, dedx_config *config, int *err) {
    dedx_internal_lookup_data *data = get_loaded_dataset(ws, config, err);
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
