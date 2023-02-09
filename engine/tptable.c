#include <assert.h>
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <threadpool.h>
#include <tptable.h>
#include <stdlib.h>

static struct __transposition_entry __tp_table[TRANSPOSITION_TABLE_SIZE] = {0};
static zobrist_hash __protected_hash = 0;

#ifdef HAS_C11_CONCURRENCY
static mtx_t __tp_table_mutex;
#endif

#ifdef HAS_C11_CONCURRENCY
static void check_error(int __return)
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
    check_error(mtx_init(&__tp_table_mutex, mtx_plain));
#endif
}

void tptable_deinit(void)
{
#ifdef HAS_C11_CONCURRENCY
    mtx_destroy(&__tp_table_mutex);
#endif
}

struct __transposition_entry tptable_get(zobrist_hash __hash)
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
                                              .__type  = ENTRY_EXACT,
                                              .__value = 0};
    }

#ifdef HAS_C11_CONCURRENCY
    check_error(mtx_unlock(&__tp_table_mutex));
#endif

    return __t;
}

void tptable_put(struct __transposition_entry __entry)
{
#ifdef HAS_C11_CONCURRENCY
    check_error(mtx_lock(&__tp_table_mutex));
#endif

    struct __transposition_entry __prev = __tp_table[__entry.__hash % TRANSPOSITION_TABLE_SIZE];

    if ((__prev.__hash != __protected_hash && __entry.__hash != __prev.__hash) |
        (__prev.__hash == __entry.__hash && __prev.__depth <= __entry.__depth))
    {
        __tp_table[__entry.__hash % TRANSPOSITION_TABLE_SIZE] = __entry;
    }

#ifdef HAS_C11_CONCURRENCY
    check_error(mtx_unlock(&__tp_table_mutex));
#endif
}

void tptable_clear(void)
{
#ifdef HAS_C11_CONCURRENCY
    check_error(mtx_lock(&__tp_table_mutex));
#endif

    memset((void *) __tp_table, 0, TRANSPOSITION_TABLE_SIZE * sizeof(struct __transposition_entry));

#ifdef HAS_C11_CONCURRENCY
    check_error(mtx_unlock(&__tp_table_mutex));
#endif
}

void tptable_set_protected_hash(zobrist_hash __hash)
{
#ifdef HAS_C11_CONCURRENCY
    check_error(mtx_lock(&__tp_table_mutex));
#endif

    __protected_hash = __hash;

    struct __transposition_entry __entry = __tp_table[__hash % TRANSPOSITION_TABLE_SIZE];

    if (__entry.__hash != __hash)
    {
        __entry.__hash = __hash;
        __entry.__depth = 0;
        __entry.__value = 0;
        __entry.__best_move = (struct __move) {NULL_BOARDPOS, NULL_BOARDPOS};

        __tp_table[__hash % TRANSPOSITION_TABLE_SIZE] = __entry;
    }

#ifdef HAS_C11_CONCURRENCY
    check_error(mtx_unlock(&__tp_table_mutex));
#endif
}