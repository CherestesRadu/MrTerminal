#pragma once

#include "defines.h"
#include "font_render.h"

struct CommandBuffer;

#define GET_SCREEN_Y(y, console) console->offscreen_buffer_height + y
#define SCREEN_HEIGHT(console) console->offscreen_buffer_height + screen_height

struct Cell
{
    Cell() = default;
    Cell(char c, uint32 color);

    void operator=(Cell& v);

    char c;
    uint32 color;
};

struct Console
{
    Console(int32 w, int32 h);
    ~Console();

    void print(char *text);
	void render(FontRender *fr);
	void move_up();
	void move_down();
	
	CommandBuffer *command_buffer;
	
	int32 screen_width, screen_height;
	int32 width, height;
	int32 offscreen_buffer_height;
	int32 xpad = 5, ypad = 5;
	
	int32 wheel_delta = 0;
	
	int32 off_buffer_height;
	int32 buffer_size;
	Cell *buffer;
};

extern Console *global_console;