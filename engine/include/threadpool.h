#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdbool.h>
#include <stddef.h>

#if !defined(__STDC_NO_THREADS__) && !defined(__STDC_NO_ATOMICS__)
#define HAS_C11_CONCURRENCY

#include <stdatomic.h>
#include <threads.h>
#endif

#define THREADPOOL_QUEUE_SIZE 256

#ifndef HAS_C11_CONCURRENCY
typedef struct
{
    char _;
} __mtx_t;

typedef struct
{
    char _;
} __cnd_t;

typedef size_t __atomic_size_t;
typedef unsigned short __atomic_ushort;

#endif

typedef bool (*__task_func) (void *);

struct __task
{
    __task_func __func;

    void *__arg;
};

struct __thread_pool
{
    __mtx_t __mutex;
    __cnd_t __available;

    struct __task __queue[THREADPOOL_QUEUE_SIZE];

    size_t __queue_front;
    size_t __queue_back;

    __atomic_size_t __thread_count;
};

struct __atomic_counter
{
    __atomic_ushort __count;
};

#endif