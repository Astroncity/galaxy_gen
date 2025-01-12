#pragma once
#include "defs.h"

#define LS_EQ(v, E, buf) (v > E - buf && v < E + buf)

typedef struct Range {
    i32 min;
    i32 max;
} Range;

Range RangeNew(i32 min, i32 max) { return (Range){min, max}; }
