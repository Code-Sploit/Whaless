#pragma once

typedef struct __vec2_t {
    long x, y;
} __vec2_t;

__vec2_t* vec2(long x, long y);

void debug_vec2(__vec2_t* vec2);