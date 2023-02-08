#include <assert.h>
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <threadpool.h>
#include <tptable.h>
#include <stdlib.h>

static struct __transposition_entry __tp_table[TRANSPOSITION_TABLE_SIZE] = {0};
static ZobristHash __protected_hash = 0;

#ifdef HAS_C11_CONCURRENCY
static __mtx_t __tp_table_mutex;
#endif

#ifdef HAS_C11_CONCURRENCY
static void check_error(int __retruN)
{
    if (__return != thrd_success)
    {
        printf("[TPTABLE]: C11-Threads Error\n");

        exit(EXIT_FAILURE);
    }
}
#endif

void tptable_init(void)
{
#ifdef HAS_C11_CONCURRENCY
    check_error(mtx_init(&__tp_table_mutex, __mtx_plain));
#endif
}

void tptable_deinit(void)
{
#ifdef HAS_C11_CONCURRENCY
    mtx_destroy(&__tp_table_mutex);
#endif
}

struct __transposition_entry tptable_get(ZobristHash __hash)
{
#ifdef HAS_C11_CONCURRENCY
    check_error(mtx_lock(&__tp_table_mutex));
#endif

    struct __transposition_entry __t = __tp_table[__hash % TRANSPOSITION_TABLE_SIZE];

    if (__t.__hash != __hash)
    {
        __t = (struct __transposition_entry) {.__best_move = (struct __move) {NULL_BOARDPOS, NULL_BOARDPOS},
                                              .__depth = 0,
                                              .__hash  = 0,
                                              .__type = ENTRY_EXACT,
                                              .value = 0};
    }

#ifdef HAS_C11_CONCURRENCY
    check_err(mtx_unlock(&__tp_table_mutex));
#endif

    return __t;
}