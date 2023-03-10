#include <vec2.h>
#include <core.h>

#include <stdlib.h>
#include <stdio.h>

__vec2_t vec2(long x, long y)
{
    __vec2_t tmp_vec2;
    tmp_vec2.x = x;
    tmp_vec2.y = y;

    return tmp_vec2;
}

void debug_vec2(__vec2_t* vec2)
{
    __DEBUG("vec2(x: ");
    printf("%lu, y: %lu)", vec2->x, vec2->y);
}