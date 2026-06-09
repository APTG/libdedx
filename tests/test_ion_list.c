#include "test_helpers.h"

/* Verifies dedx_get_ion_list().
 *
 * Historically the DEDX_DEFAULT / DEDX_BETHE_EXT00 branch filled a function-scoped
 * static buffer on every call and returned a pointer into it, which was a data race
 * under concurrent use (issue #138). The list is now an immutable const table; these
 * checks lock in its contents (the full list and the restricted sub-lists) and that
 * repeated calls return identical contents. Contents are compared element-by-element
 * rather than by pointer identity, which is an implementation detail and not part of
 * the function's contract. */

/* Checks that list is exactly 1,2,...,count then -1 terminator. Returns 0 on pass. */
static int check_full_list(const int *list, int count, const char *label) {
    if (list == NULL) {
        fprintf(stderr, "FAIL %s: got NULL list\n", label);
        return 1;
    }
    for (int i = 0; i < count; i++) {
        if (list[i] != i + 1) {
            fprintf(stderr, "FAIL %s: list[%d]=%d expected %d\n", label, i, list[i], i + 1);
            return 1;
        }
    }
    if (list[count] != -1) {
        fprintf(stderr, "FAIL %s: list[%d]=%d expected -1 terminator\n", label, count, list[count]);
        return 1;
    }
    return 0;
}

/* Checks that list is -1 terminated within max entries. Returns 0 on pass. */
static int check_terminated(const int *list, int max, const char *label) {
    if (list == NULL) {
        fprintf(stderr, "FAIL %s: got NULL list\n", label);
        return 1;
    }
    for (int i = 0; i < max; i++) {
        if (list[i] == -1)
            return 0;
    }
    fprintf(stderr, "FAIL %s: no -1 terminator within %d entries\n", label, max);
    return 1;
}

/* Checks two -1-terminated lists have identical contents. Returns 0 on pass. */
static int check_lists_equal(const int *x, const int *y, const char *label) {
    if (x == NULL || y == NULL) {
        fprintf(stderr, "FAIL %s: NULL list\n", label);
        return 1;
    }
    for (int i = 0;; i++) {
        if (x[i] != y[i]) {
            fprintf(stderr, "FAIL %s: lists differ at index %d (%d vs %d)\n", label, i, x[i], y[i]);
            return 1;
        }
        if (x[i] == -1)
            return 0;
    }
}

int main(void) {
    int failures = 0;

    /* Unrestricted programs return the full 1..112 list. */
    failures += check_full_list(dedx_get_ion_list(DEDX_DEFAULT), 112, "DEFAULT full list");
    failures += check_full_list(dedx_get_ion_list(DEDX_BETHE_EXT00), 112, "BETHE_EXT00 full list");

    /* Both unrestricted program IDs, and repeated calls, must yield identical contents.
     * We compare contents rather than pointer identity, which is an implementation
     * detail and not part of the function's contract. */
    const int *a = dedx_get_ion_list(DEDX_DEFAULT);
    failures += check_lists_equal(a, dedx_get_ion_list(DEDX_BETHE_EXT00), "DEFAULT vs BETHE_EXT00 contents");
    failures += check_lists_equal(a, dedx_get_ion_list(DEDX_DEFAULT), "DEFAULT stable across calls");

    /* Restricted programs return their own const sub-lists, terminated with -1. */
    const int *pstar = dedx_get_ion_list(DEDX_PSTAR);
    failures += check_terminated(pstar, 20, "PSTAR list");
    if (pstar != NULL && (pstar[0] != 1 || pstar[1] != -1)) {
        fprintf(stderr, "FAIL: PSTAR list expected {1,-1}, got {%d,%d}\n", pstar[0], pstar[1]);
        failures++;
    }

    const int *astar = dedx_get_ion_list(DEDX_ASTAR);
    failures += check_terminated(astar, 20, "ASTAR list");
    if (astar != NULL && (astar[0] != 2 || astar[1] != -1)) {
        fprintf(stderr, "FAIL: ASTAR list expected {2,-1}, got {%d,%d}\n", astar[0], astar[1]);
        failures++;
    }

    if (failures == 0)
        printf("test_ion_list: all checks passed\n");
    return failures;
}
