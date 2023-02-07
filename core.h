#pragma once

#define __DEBUG(msg) \
    printf("\x1b[0;31mDEBUG\x1b[0;0m: %s: %s", __FILE__, msg);     