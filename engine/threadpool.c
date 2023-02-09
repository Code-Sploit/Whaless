#include <threadpool.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#endif

static unsigned long nproc(void)
{
    char *__count = getenv("CHESS_NPROC");

    if (__count != NULL)
    {
        return MAX(atol(__count), 1);
    }
    else
    {
#ifdef _SC_NPROCESSORS_ONLN
        long __onl = sysconf(_SC_NPROCESSORS_ONLN);

        return MAX(1, __onl);
#endif
        return 1;
    }
}

#ifdef HAS_C11_CONCURRENCY
static void check_error(int __return)
{
    if (__return != thrd_success)
    {
        printf("Threadpool C11-Threads Error!\n");

        exit(EXIT_FAILURE);
    }
}
#endif

#ifdef HAS_C11_CONCURRENCY
static bool enqueue(struct __thread_pool *__pool, struct __task __task)
{
    check_error(mtx_lock(&__pool->__mutex));

    if (__pool->__queue_back == __pool->__queue_front)
    {
        check_error(mtx_unlock(&__pool->__mutex));

        return false;
    }

    size_t __iat = __pool->__queue_back;

    __pool->__queue[__pool->__queue_back] = __task;
    __pool->__queue_back = (__pool->__queue_back + 1) % THREADPOOL_QUEUE_SIZE;

    if (__pool->__queue_front == THREADPOOL_QUEUE_SIZE)
    {
        __pool->__queue_front = __iat;
    }

    check_error(mtx_unlock(&__pool->__mutex));

    return true;
}

#endif

#ifdef HAS_C11_CONCURRENCY
static bool dequeue_already_locked(struct __thread_pool *__pool, struct __task *__task)
{
    if (__pool->__queue_back == __pool->__queue_front || __pool->__queue_front == THREADPOOL_QUEUE_SIZE)
    {
        return false;
    }

    *__task = __pool->__queue[__pool->__queue_front];

    __pool->__queue_front = (__pool->__queue_front + 1) % THREADPOOL_QUEUE_SIZE;

    if (__pool->__queue_back == __pool->__queue_front)
    {
        __pool->__queue_front = THREADPOOL_QUEUE_SIZE;
    }

    return true;
}

#endif

#ifdef HAS_C11_CONCURRENCY
static int thread_start_func(struct __thread_pool *__pool)
{
    for (;;)
    {
        check_error(mtx_lock(&__pool->__mutex));

        struct __task __task;

        if (dequeue_already_locked(__pool, &__task))
        {
            goto __received_task;
        }
        else
        {
            check_error(cnd_wait(&__pool->__available, &__pool->__mutex));
        }

        if (!dequeue_already_locked(__pool, &__task))
        {
            check_error(mtx_unlock(&__pool->__mutex));

            break;
        }

    __received_task:
        check_error(mtx_unlock(&__pool->__mutex));

        if (!__task.__func(__task.__arg))
        {
            break;
        }
    }

    atomic_fetch_sub(&__pool->__thread_count, 1);

    return 0;
}

#endif

static bool task_stop(void *__arg)
{
    (void) __arg;

    return false;
}

#ifdef HAS_C11_CONCURRENCY
static void threadpool_stop(struct __thread_pool *__pool)
{
    struct __task __stop = {.__func = task_stop, .__arg = NULL};

    while (atomic_load(&__pool->__thread_count) > 0)
    {
        while (!enqueue(__pool, __stop))
        {
            /* Wait */
        }

        check_error(cnd_signal(&__pool->__available));
    }
}

#endif

struct __thread_pool *threadpool_init(void)
{
    struct __thread_pool *__pool = malloc(sizeof(*__pool));

    __pool->__queue_front = THREADPOOL_QUEUE_SIZE;
    __pool->__queue_back  = 0;

#ifdef HAS_C11_CONCURRENCY
    check_error(mtx_init(&__pool->__mutex, mtx_plain));
    check_error(cnd_init(&__pool->__available));

    size_t __thread_count = nproc();

    atomic_init(&__pool->__thread_count, __thread_count);

    printf("[THREADPOOL]: Starting %zu threads. Reading from CHESS_NPROC env variable!\n", __thread_count);

    for (size_t i = 0; i < __thread_count; i++)
    {
        thrd_t thread;

        check_error(thrd_create(&thread, (thrd_start_t) thread_start_func, __pool));
        check_error(thrd_detach(thread));
    }
#else
    printf("[THREADPOOL]: Compiled without C11-threads support. Deactivating threadpool\n");

    __pool->__thread_count = 0;
#endif

    return __pool;
}

void threadpool_enqueue(struct __thread_pool *__pool, __task_func __func, void *__arg)
{
#ifdef HAS_C11_CONCURRENCY
    struct __task __task = {.__func = __func, .__arg = __arg};

    if (atomic_load(&__pool->__thread_count) == 0 || !enqueue(__pool, __task))
    {
        __func(__arg);
    }
    else
    {
        check_error(cnd_signal(&__pool->__available));
    }
#else
    __func(__arg);
#endif
}

void threadpool_deinit(struct __thread_pool *__pool)
{
#ifdef HAS_C11_CONCURRENCY
    threadpool_stop(__pool);

    mtx_destroy(&__pool->__mutex);
    cnd_destroy(&__pool->__available);
#endif

    free(__pool);
}

struct __atomic_counter *acnt_init(unsigned short __val)
{
    struct __atomic_counter *__c = malloc(sizeof(*__c));

#ifdef HAS_C11_CONCURRENCY
    atomic_init(&__c->__count, __val);
#else
    __c->__count = __val;
#endif

    return __c;
}

bool acnt_dec(struct __atomic_counter *__counter)
{
#ifdef HAS_C11_CONCURRENCY
    return atomic_fetch_sub(&__counter->__count, 1) == 1;
#else
    return __counter->__count-- == 1;
#endif
}