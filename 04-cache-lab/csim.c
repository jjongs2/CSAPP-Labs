#include "csim.h"

#include "cachelab.h"
#include "utils.h"

static const char *usage_format =
    "Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n"
    "Options:\n"
    "  -h         Print this help message.\n"
    "  -v         Optional verbose flag.\n"
    "  -s <num>   Number of set index bits.\n"
    "  -E <num>   Number of lines per set.\n"
    "  -b <num>   Number of block offset bits.\n"
    "  -t <file>  Trace file.\n"
    "\n"
    "Examples:\n"
    "  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n"
    "  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n";

static const char *access_memory(uint64_t address, t_cache *cache,
                                 t_count *count);
static void free_cache(t_cache *cache);
static void init_cache(t_option *option, t_cache *cache);
static void parse_arguments(int argc, char *const argv[], t_option *option);
static void print_usage_and_exit(const char *program_name, int exit_code);
static void simulate(t_option *option, t_cache *cache, t_count *count);

int main(int argc, char *argv[]) {
    t_count count = {0};
    t_option option = {0};
    t_cache cache;

    parse_arguments(argc, argv, &option);
    init_cache(&option, &cache);
    simulate(&option, &cache, &count);
    printSummary(count.hit, count.miss, count.eviction);
    free_cache(&cache);
    return EXIT_SUCCESS;
}

static const char *access_memory(uint64_t address, t_cache *cache,
                                 t_count *count) {
    const char *result = "miss";
    t_line *target_line = NULL;
    size_t E = cache->E;
    uint64_t tag = address >> (cache->s + cache->b);
    uint64_t set_index = (address >> cache->b) & ((1 << cache->s) - 1);
    t_set *set = &cache->sets[set_index];
    t_line *lru_line = &set->lines[0];
    t_line *line;
    size_t i;

    cache->access_count += 1;
    for (i = 0; i < E; ++i) {
        line = &set->lines[i];
        if (line->is_valid && line->tag == tag) {
            line->last_used = cache->access_count;
            count->hit += 1;
            return "hit";
        }
        if (target_line)
            continue;
        if (!line->is_valid)
            target_line = line;
        if (line->last_used < lru_line->last_used)
            lru_line = line;
    }
    count->miss += 1;
    if (!target_line) {
        target_line = lru_line;
        count->eviction += 1;
        result = "miss eviction";
    }
    target_line->is_valid = true;
    target_line->tag = tag;
    target_line->last_used = cache->access_count;
    return result;
}

static void free_cache(t_cache *cache) {
    t_set *sets = cache->sets;
    size_t S = cache->S;
    size_t i;

    for (i = 0; i < S; ++i)
        safe_free((void **)&sets[i].lines);
    safe_free((void **)&sets);
}

static void init_cache(t_option *option, t_cache *cache) {
    t_set *sets;
    size_t E, S;
    size_t i;

    cache->access_count = 0;
    E = cache->E = option->E;
    cache->b = option->b;
    cache->s = option->s;
    S = cache->S = 1 << option->s;
    sets = cache->sets = (t_set *)safe_calloc(S, sizeof(t_set));
    for (i = 0; i < S; ++i)
        sets[i].lines = (t_line *)safe_calloc(E, sizeof(t_line));
}

static void parse_arguments(int argc, char *const argv[], t_option *option) {
    const char *program_name = argv[0];
    int opt;

    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
        case 'h':
            print_usage_and_exit(program_name, EXIT_SUCCESS);
        case 'v':
            option->v = true;
            break;
        case 's':
            option->s = atoi(optarg);
            break;
        case 'E':
            option->E = atoi(optarg);
            break;
        case 'b':
            option->b = atoi(optarg);
            break;
        case 't':
            option->t = optarg;
            break;
        default:
            print_usage_and_exit(program_name, EXIT_FAILURE);
        }
    }
    if (option->s <= 0 || option->E <= 0 || option->b <= 0 || !option->t) {
        fprintf(stderr, "%s: Missing required command line argument\n",
                program_name);
        print_usage_and_exit(program_name, EXIT_FAILURE);
    }
    option->trace_file = safe_fopen(option->t, "r");
}

static void print_usage_and_exit(const char *program_name, int exit_code) {
    FILE *stream = exit_code == EXIT_SUCCESS ? stdout : stderr;

    fprintf(stream, usage_format, program_name, program_name, program_name);
    exit(exit_code);
}

static void simulate(t_option *option, t_cache *cache, t_count *count) {
    FILE *trace_file = option->trace_file;
    char operation;
    uint64_t address;
    size_t size;
    const char *result;

    while (fscanf(trace_file, " %c %lx,%zu", &operation, &address, &size) == 3) {
        if (operation == 'I')
            continue;
        if (operation == 'M')
            count->hit += 1;
        result = access_memory(address, cache, count);
        if (option->v) {
            printf("%c %lx,%zu %s", operation, address, size, result);
            if (operation == 'M')
                printf(" hit");
            printf(" \n");
        }
    }
    fclose(trace_file);
}
