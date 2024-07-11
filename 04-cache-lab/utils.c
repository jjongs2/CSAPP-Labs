#include "utils.h"

#include <assert.h>
#include <err.h>
#include <stdlib.h>

void *safe_calloc(size_t nmemb, size_t size) {
    void *p = calloc(nmemb, size);
    if (!p)
        err(EXIT_FAILURE, "calloc()");
    return p;
}

FILE *safe_fopen(const char *pathname, const char *mode) {
    FILE *file = fopen(pathname, mode);
    if (!file)
        err(EXIT_FAILURE, "%s", pathname);
    return file;
}

void safe_free(void **pp) {
    assert(pp);
    free(*pp);
    *pp = NULL;
}
