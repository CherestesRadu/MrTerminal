#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#include "platform.h"
#include "font_render.h"
#include "console.h"
#include "command_buffer.h"

int main()
{
    System system = init_system(GetModuleHandle(NULL));
    sys = &system;

    FontRender fr = init_font_render();
    Console console(80, 30);
    global_console = &console;

    resize_window(&system, &fr, &console);

    while(!system.quit)
    {
        MSG msg;
        while(PeekMessage(&msg, system.window, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        console.render(&fr);

        SwapBuffers(system.dc);
    }

    release_font_render(&fr);
    release_system(&system);

    return 0;
}

LRESULT __stdcall window_proc(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch(msg)
    {
        case WM_CREATE:
        {
            printf("Window created!");
        } break;

        case WM_DESTROY:
        {
            sys->quit = true;
        } break;

        case WM_CHAR:
		{
		    global_console->command_buffer->char_input(wparam);
		} break;
		
		case WM_KEYDOWN:
		{
			if(wparam == VK_ESCAPE)
				sys->quit = true;
			global_console->command_buffer->key_input(global_console, wparam);
			
		} break;
		
		case WM_MOUSEWHEEL:
		{
			int16 delta = (int16) HIWORD(wparam);
			global_console->command_buffer->process_mouse_wheel(global_console, delta);
		} break;

        default:
        {
            return DefWindowProc(handle, msg, wparam, lparam);
        }
    }

    return 1;
}
