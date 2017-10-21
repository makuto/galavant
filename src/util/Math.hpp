#pragma once

#define CLAMP(value, min, max) if (value > max) value = max; else if (value < min) value = min; 
#define MIN(a, b) (a <= b ? a : b)
#define MAX(a, b) (a >= b ? a : b)