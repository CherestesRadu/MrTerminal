#pragma once

#include <stdlib.h>
#include <math.h>

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include <stdint.h>
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;

typedef float float32;
typedef double float64;

#define MALLOC(type, count) (type*)malloc(sizeof(type) * count);