#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

void *safe_calloc(size_t nmemb, size_t size);
FILE *safe_fopen(const char *pathname, const char *mode);
void safe_free(void **pp);

#endif
