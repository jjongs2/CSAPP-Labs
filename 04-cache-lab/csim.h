#ifndef CSIM_H
#define CSIM_H

#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct s_count {
    size_t eviction;
    size_t hit;
    size_t miss;
} t_count;

typedef struct s_option {
    size_t E, b, s;
    const char *t;
    FILE *trace_file;
    bool v;
} t_option;

typedef struct s_line {
    bool is_valid;
    size_t last_used;
    uint64_t tag;
} t_line;

typedef struct s_set {
    t_line *lines;
} t_set;

typedef struct s_cache {
    size_t access_count;
    size_t E, S;
    size_t b, s;
    t_set *sets;
} t_cache;

#endif
