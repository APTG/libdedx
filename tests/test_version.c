#include <dedx.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    int major = 0;
    int minor = 0;
    int patch = 0;
    const char *version = NULL;

    dedx_get_version(&major, &minor, &patch);
    version = dedx_get_version_string();
    if (version == NULL || strlen(version) == 0) {
        fprintf(stderr, "version string is empty\n");
        return 1;
    }
    printf("%d.%d.%d\n", major, minor, patch);
    printf("%s\n", version);
    return 0;
}
