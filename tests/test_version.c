#include <dedx.h>
#include <stdio.h>

int main(void) {
    int major = 0;
    int minor = 0;
    int patch = 0;
    const char *version = NULL;

    dedx_get_version(&major, &minor, &patch);
    version = dedx_get_version_string();
    printf("%d.%d.%d\n", major, minor, patch);
    printf("%s\n", version);
    return 0;
}
