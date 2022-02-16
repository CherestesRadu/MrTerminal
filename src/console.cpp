#include "console.h"
#include "command_buffer.h"

Cell::Cell(char c, uint32 color): c(c), color(color) {}

Console *global_console;

void Cell::operator=(Cell& v)
{
	this->c = v.c;
	this->color = v.color;
}

Console::Console(int32 w, int32 h)
{
	command_buffer = new CommandBuffer(w);
	
	screen_width = w;
	screen_height = h;
	
	width = screen_width;
	offscreen_buffer_height = screen_height * 5;
	height = offscreen_buffer_height * 2;
	
	buffer_size = width * height;
	buffer = MALLOC(Cell, buffer_size);
	for(int i = 0; i < buffer_size; ++i)
		buffer[i] = Cell(0, 0);
}

Console::~Console()
{
	delete command_buffer;
}

void Console::print(char *text)
{
	//TODO: Bug legat de scroll si print
	//(trebuie sa se dea print la urmatoarea linie valida)
	
	//Find next available line
	int line = -1;
	for(int y = 0; y < screen_height; ++y)
	{
		int py = GET_SCREEN_Y(y, this);
		if(buffer[py * screen_width].c == 0)
		{
			line = py;
			break;
		}
	}
	
	//No line available move buffer up 1 cell
	if(line == -1)
	{
		move_up();
		line = GET_SCREEN_Y(screen_height - 1, this);
	}
	
	//Write to buffer
	
	int x = 0;
	int pos = 0;
	
	for(int ch = 0; ch < strlen(text); ++ch)
	{
		if(x == width)
		{
			x = 0;
			++line;
			if(line > SCREEN_HEIGHT(this))
			{
				move_up();
				--line;
			}
		}
		
		pos = line * width + x;
		buffer[line * width + x].c = text[ch];
		buffer[line * width + x].color = 0xFFFFFFFF;
	
		++x;
	}
	
	//If there are any more lines under screen buffer move up
	//TODO: Don't let move_up move things if last element has text in it
	//Or it will delete that
	pos = (offscreen_buffer_height + screen_height) * width;
	if(buffer[pos].c != 0)
		move_up();
}

void Console::render(FontRender *fr)
{
	//Render onscreen buffer
	for(int y = 0; y < screen_height; ++y)
	{
		for(int x = 0; x < screen_width; ++x)
		{
			int py = GET_SCREEN_Y(y, this);
			
			Cell *cell = &buffer[py * screen_width + x];
			if(cell->c)
				render_glyph(fr, cell->c, x * fr->width + xpad, y * fr->height + ypad, cell->color);
		}
	}
	
	int y = ypad + (screen_height + 1) * fr->height;
	
	//Render command buffer
	for(int x = 0; x < screen_width; ++x)
	{
		Cell cell = (*command_buffer)[x];
		if(cell.c)
			render_glyph(fr, cell.c, x * fr->width + xpad, y, cell.color);
	}
}

void Console::move_up()
{
	//TODO: only move from where it is needed
	
	//Move up till last element
	for(int y = 0; y < height-1; ++y)
	{
		for(int x = 0; x < width; ++x)
		{
			Cell *cell = &buffer[y * width + x];
			Cell *next = &buffer[(y+1) * width + x];
			*cell = *next;
			ZeroMemory(next, sizeof(Cell));
		}
	}
	
	//Zero last element
	for(int x = 0; x < width; ++x)
		ZeroMemory(&buffer[(height-1) * width + x], sizeof(Cell));
	
}

void Console::move_down()
{
	//TODO: only move from where it is needed
	
	for(int y = height - 1; y >= 1; --y)
	{
		for(int x = 0; x < width; ++x)
		{
			Cell *cell = &buffer[y * width + x];
			Cell *last = &buffer[(y-1) * width + x];
			*cell = *last;
			ZeroMemory(last, sizeof(Cell));
		}
	}
	
	//Zero first element
	for(int x = 0; x < width; ++x)
		ZeroMemory(&buffer[x], sizeof(Cell));
}