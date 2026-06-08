#include "test_helpers.h"

/* Verifies dedx_get_ion_list().
 *
 * Historically the DEDX_DEFAULT / DEDX_BETHE_EXT00 branch filled a function-scoped
 * static buffer on every call and returned a pointer to it, which was a data race
 * under concurrent use. The list is now a const table; these checks lock in its
 * contents and confirm the returned pointer is stable across calls. */

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

int main(void) {
    int failures = 0;

    /* Unrestricted programs return the full 1..112 list. */
    failures += check_full_list(dedx_get_ion_list(DEDX_DEFAULT), 112, "DEFAULT full list");
    failures += check_full_list(dedx_get_ion_list(DEDX_BETHE_EXT00), 112, "BETHE_EXT00 full list");

    /* The returned pointer must be stable: repeated calls return identical contents,
     * and concurrent readers must never observe a half-written buffer. With a const
     * table the pointer is also identical across calls. */
    const int *a = dedx_get_ion_list(DEDX_DEFAULT);
    const int *b = dedx_get_ion_list(DEDX_BETHE_EXT00);
    if (a != b) {
        fprintf(stderr, "FAIL: DEFAULT and BETHE_EXT00 should share the same const list pointer\n");
        failures++;
    }
    if (dedx_get_ion_list(DEDX_DEFAULT) != a) {
        fprintf(stderr, "FAIL: DEFAULT list pointer not stable across calls\n");
        failures++;
    }

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
