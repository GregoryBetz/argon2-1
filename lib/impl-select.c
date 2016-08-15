#include <time.h>
#include <string.h>

#include "impl-select.h"

#include "argon2-opt.h"

#define log_maybe(file, args...) \
    do { \
        if (file) { \
            fprintf(file, args); \
        } \
    } while((void)0, 0)

#define BENCH_SAMPLES 2048
#define BENCH_MEM_BLOCKS 512

static argon2_impl selected_argon_impl = {
    "(default)", NULL, fill_segment_default
};

/* the benchmark routine is not thread-safe, so we can use a global var here: */
static block memory[BENCH_MEM_BLOCKS];

static uint64_t benchmark_impl(const argon2_impl *impl) {
    clock_t time;
    unsigned int i;
    uint64_t bench;
    argon2_instance_t instance;
    argon2_position_t pos;

    memset(memory, 0, sizeof(memory));

    instance.version = ARGON2_VERSION_NUMBER;
    instance.memory = memory;
    instance.passes = 1;
    instance.memory_blocks = BENCH_MEM_BLOCKS;
    instance.segment_length = BENCH_MEM_BLOCKS / ARGON2_SYNC_POINTS;
    instance.lane_length = instance.segment_length * ARGON2_SYNC_POINTS;
    instance.lanes = 1;
    instance.threads = 1;
    instance.type = Argon2_i;

    pos.lane = 0;
    pos.pass = 0;
    pos.slice = 0;
    pos.index = 0;

    /* warm-up cache: */
    impl->fill_segment(&instance, pos);

    /* OK, now measure: */
    bench = 0;
    time = clock();
    for (i = 0; i < BENCH_SAMPLES; i++) {
        impl->fill_segment(&instance, pos);
    }
    time = clock() - time;
    bench = (uint64_t)time;
    return bench;
}

static void select_impl(FILE *out)
{
    argon2_impl_list impls;
    unsigned int i;
    const argon2_impl *best_impl = NULL;
    uint64_t best_bench = UINT_MAX;

    log_maybe(out, "Selecting fill_segment function implementation...\n");

    argon2_get_impl_list(&impls);

    for (i = 0; i < impls.count; i++) {
        const argon2_impl *impl = &impls.entries[i];
        uint64_t bench;

        log_maybe(out, "  Checking implementation '%s'... ", impl->name);
        if (!impl->check || impl->check()) {
            log_maybe(out, "OK!\n");

            log_maybe(out, "    Measuring...\n");
            bench = benchmark_impl(impl);
            log_maybe(out, "    Benchmark result: %llu\n",
                    (unsigned long long)bench);

            if (bench < best_bench) {
                best_bench = bench;
                best_impl = impl;
            }
        } else {
            log_maybe(out, "CHECK FAILED!\n");
        }
    }

    if (best_impl != NULL) {
        log_maybe(out,
                  "  Selecting best implementation: '%s' (bench %llu)...\n",
                  best_impl->name, (unsigned long long)best_bench);

        selected_argon_impl = *best_impl;
    } else {
        log_maybe(out,
                  "  No optimized implementation available, using default!\n");
    }
    log_maybe(out, "  Done!\n");
}

void fill_segment(const argon2_instance_t *instance, argon2_position_t position)
{
    selected_argon_impl.fill_segment(instance, position);
}

void argon2_select_impl(FILE *out)
{
    select_impl(out);
}
