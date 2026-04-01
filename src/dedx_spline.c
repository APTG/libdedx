/*
    This file is part of libdedx.

    libdedx is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libdedx is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libdedx.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "dedx_spline.h"

#include <math.h>
#include <stddef.h>

static int binary_search(dedx_internal_spline_base *coef, float value, int n) {
    int head = n - 1;
    int tail = 0;
    int guess = n / 2;
    while (head != tail + 1) {
        if (coef[guess].x <= value) {
            tail = guess;
        } else {
            head = guess;
        }
        guess = (head - tail) / 2 + tail;
    }
    return guess;
}

static void calculate_linear_coefficients(dedx_internal_spline_base *coef, const float *energy,
                                          const float *stopping, int n) {
    int i;
    float h[DEDX_MAX_ELEMENTS];
    float alpha[DEDX_MAX_ELEMENTS];
    float l[DEDX_MAX_ELEMENTS];
    float my[DEDX_MAX_ELEMENTS];
    float z[DEDX_MAX_ELEMENTS];

    l[0] = 1.0f;
    my[0] = 0.0f;
    z[0] = 0.0f;

    l[n - 1] = 1.0f;
    z[n - 1] = 0.0f;
    coef[n - 1].c = 0.0f;

    for (i = 0; i < n; i++) {
        coef[i].a = stopping[i];
        coef[i].x = energy[i];
        coef[i].log_a = NAN;
        coef[i].log_x = NAN;
    }
    for (i = 0; i < n - 1; i++) {
        h[i] = energy[i + 1] - energy[i];
    }
    for (i = 1; i < n - 1; i++) {
        alpha[i] = 3.0f / h[i] * (stopping[i + 1] - stopping[i]) - 3.0f / h[i - 1] * (stopping[i] - stopping[i - 1]);
    }
    for (i = 1; i < n - 1; i++) {
        l[i] = 2.0f * (energy[i + 1] - energy[i - 1]) - h[i - 1] * my[i - 1];
        my[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }
    for (i = n - 2; i >= 0; i--) {
        coef[i].c = z[i] - my[i] * coef[i + 1].c;
        coef[i].b = (coef[i + 1].a - coef[i].a) / h[i] - h[i] * (coef[i + 1].c + 2.0f * coef[i].c) / 3.0f;
        coef[i].d = (coef[i + 1].c - coef[i].c) / (3.0f * h[i]);
    }
}

void dedx_internal_calculate_coefficients(
    dedx_internal_spline_base *coef, float *energy, float *stopping, int n, int interpolation_mode) {
    int i;
    float log_energy[DEDX_MAX_ELEMENTS];
    float log_stopping[DEDX_MAX_ELEMENTS];
    float h[DEDX_MAX_ELEMENTS];
    float alpha[DEDX_MAX_ELEMENTS];
    float l[DEDX_MAX_ELEMENTS];
    float my[DEDX_MAX_ELEMENTS];
    float z[DEDX_MAX_ELEMENTS];

    if (n < 2)
        return;

    if (interpolation_mode == DEDX_INTERPOLATION_LINEAR) {
        calculate_linear_coefficients(coef, energy, stopping, n);
        return;
    }

    for (i = 0; i < n; i++) {
        if (energy[i] <= 0.0f || stopping[i] <= 0.0f) {
            calculate_linear_coefficients(coef, energy, stopping, n);
            return;
        }
        coef[i].a = stopping[i];
        coef[i].x = energy[i];
        coef[i].log_x = logf(energy[i]);
        coef[i].log_a = logf(stopping[i]);
        log_energy[i] = coef[i].log_x;
        log_stopping[i] = coef[i].log_a;
    }

    l[0] = 1.0f;
    my[0] = 0.0f;
    z[0] = 0.0f;

    l[n - 1] = 1.0f;
    z[n - 1] = 0.0f;
    coef[n - 1].c = 0.0f;

    for (i = 0; i < n - 1; i++) {
        h[i] = log_energy[i + 1] - log_energy[i];
    }
    for (i = 1; i < n - 1; i++) {
        alpha[i] = 3.0f / h[i] * (log_stopping[i + 1] - log_stopping[i]) -
                   3.0f / h[i - 1] * (log_stopping[i] - log_stopping[i - 1]);
    }
    for (i = 1; i < n - 1; i++) {
        l[i] = 2.0f * (log_energy[i + 1] - log_energy[i - 1]) - h[i - 1] * my[i - 1];
        my[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }
    for (i = n - 2; i >= 0; i--) {
        coef[i].c = z[i] - my[i] * coef[i + 1].c;
        coef[i].b = (log_stopping[i + 1] - log_stopping[i]) / h[i] -
                    h[i] * (coef[i + 1].c + 2.0f * coef[i].c) / 3.0f;
        coef[i].d = (coef[i + 1].c - coef[i].c) / (3.0f * h[i]);
    }
}

float dedx_internal_evaluate_spline(
    dedx_internal_spline_base *coef, float x, dedx_internal_lookup_accelerator *acc, int n,
    int interpolation_mode) {
    int i;
    int lookup = 1;
    if (acc != NULL) {
        if ((coef[acc->cache].x <= x) && (x < coef[acc->cache + 1].x)) {
            lookup = 0;
            i = acc->cache;
        }
    }
    if (lookup) {
        i = binary_search(coef, x, n);
        if (acc != NULL)
            acc->cache = i;
    }

    if (x == coef[i].x) {
        return coef[i].a;
    }
    if (i + 1 < n && x == coef[i + 1].x) {
        return coef[i + 1].a;
    }

    {
        const float dx = x - coef[i].x;
        const float dx2 = dx * dx;

        if (interpolation_mode == DEDX_INTERPOLATION_LINEAR || isnan(coef[i].log_x)) {
            return coef[i].a + coef[i].b * dx + coef[i].c * dx2 + coef[i].d * dx2 * dx;
        }
    }

    {
        const float log_dx = logf(x) - coef[i].log_x;
        const float log_dx2 = log_dx * log_dx;
        const float log_stopping =
            coef[i].log_a + coef[i].b * log_dx + coef[i].c * log_dx2 + coef[i].d * log_dx2 * log_dx;

        return expf(log_stopping);
    }
}
