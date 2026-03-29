#include <stdio.h>

#include <dedx.h>

int main(void) {
    int major = 0;
    int minor = 0;
    int patch = 0;

    dedx_get_version(&major, &minor, &patch);
    printf("%d.%d.%d\n", major, minor, patch);
    return 0;
}
