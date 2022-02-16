#pragma once

#include "defines.h"

#include <stdint.h>

typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;


typedef const char *					  (WINAPI * PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);
extern PFNWGLSWAPINTERVALEXTPROC SwapInterval;

void error_message(const char *message);
bool WGLExtensionSupported(const char *extension_name);

LRESULT __stdcall window_proc(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam);

struct FontRender;
struct Console;

struct System
{
    HWND window = NULL;
    HDC dc = NULL;
    HGLRC gl_context = NULL;

    int32 width = 200;
    int32 height = 100;
    bool quit = false;
};

extern System *sys;

System init_system(HINSTANCE instance);
void release_system(System *system);

void resize_window(System *system, FontRender *fr, Console *console);