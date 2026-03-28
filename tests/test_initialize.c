#include <stdio.h>

#include <dedx.h>

int main(void) {
    int err = 0;
    dedx_workspace *ws = dedx_allocate_workspace(1, &err);

    if (err != 0 || ws == NULL) {
        fprintf(stderr, "failed to allocate workspace: err=%d\n", err);
        return 1;
    }

    dedx_free_workspace(ws, &err);
    if (err != 0) {
        fprintf(stderr, "failed to free workspace: err=%d\n", err);
        return 1;
    }

    return 0;
}
