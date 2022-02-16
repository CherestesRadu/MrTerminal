#include "command_buffer.h"
#include "console.h"

#include <string>

void directory_add(DirectoryList *list, const char *dir_name)
{
    if(!list->first)
    {
        list->first = MALLOC(DirectoryList::Node, 1);
        list->first->next = NULL;
        list->first->prev = NULL;
        list->last = list->first;
        strcpy(list->first->buff, dir_name);
    }
    else
    {
        DirectoryList::Node *new_dir = MALLOC(DirectoryList::Node, 1);
        strcpy(new_dir->buff, dir_name);
        list->last->next = new_dir;
        new_dir->prev = list->last;
        new_dir->next = NULL;
        list->last = new_dir;
    }
}

void directory_remove(DirectoryList *list, const char *dir_name)
{
    DirectoryList::Node *dir = NULL;
    auto iterator = list->first;
    while(iterator)
    {
        if(strcmp(iterator->buff, dir_name) == 0)
        {
            dir = iterator;
            break;
        }

        iterator = iterator->next;
    }

    if(dir)
    {
        DirectoryList::Node *prev = dir->prev;
        DirectoryList::Node *next = dir->next;

        prev->next = next;
        next->prev = prev;
        free(dir);
    }
}

void directory_remove(DirectoryList *list, DirectoryList::Node *node)
{
    DirectoryList::Node *prev = node->prev;
    DirectoryList::Node *next = node->next;
    prev->next = next;
    next->prev = prev;
    free(node);
}

void directory_print(DirectoryList *list)
{
    auto iterator = list->first;

    while(iterator)
    {
        printf("%s\n", iterator->buff);
        iterator = iterator->next;
    }
}

void directory_erase(DirectoryList *list)
{
    auto iterator = list->first;
    while(iterator)
    {
        auto tmp = iterator;
        iterator = iterator->next;
        free(tmp);
    }
}

DirectoryList::Node *directory_find(DirectoryList *list, const char *dir_name)
{
    auto iterator = list->first;
    while(iterator)
    {
        if(strcmp(iterator->buff, dir_name) == 0)
            break;
    }

    return iterator;
}

CommandBuffer::CommandBuffer(int32 w)
{
	width = w;
	cursor = 1;
	
	buffer = new Cell[width];
	ZeroMemory(buffer, sizeof(Cell) * width);
	buffer[0] = Cell('>', 0xFFFFFFFF);
	
	GetCurrentDirectory(256, cwd);
	for(int i = 0; i < strlen(cwd); ++i)
	{
		buffer[i+1] = Cell(cwd[i], 0xFFFFFFFF);
		if(buffer[i+1].c == '\\')
			buffer[i+1].c = '/';
	}
	cursor = CURSOR_START(this);
	
	make_directory_list();
}

CommandBuffer::~CommandBuffer()
{
	delete[] buffer;
}

Cell CommandBuffer::operator[] (int index)
{
	return buffer[index];
}

const Cell CommandBuffer::operator[] (int index) const
{
	return buffer[index];
}

void CommandBuffer::char_input(WPARAM c)
{
	//ASCII only
	if(cursor < width && c >= 32 && c <= 126)
		buffer[cursor++] = Cell(c, 0xFFFFFFFF);
	
}

void CommandBuffer::key_input(Console *console, WPARAM c)
{
	if(c == VK_BACK)
	{
		if(cursor > CURSOR_START(this))
		{
			buffer[cursor- 1] = Cell(0, 0);
			--cursor;
		}
	}
	
	if(c == VK_RETURN)
		process_command(console);
	
}

void CommandBuffer::process_command(Console *console)
{
	//If there is stuff to parse
	if(buffer[CURSOR_START(this)].c != 0)
	{
		char *buff = MALLOC(char, width);
		ZeroMemory(buff, sizeof(char) * width);
		//Parse command
		int i = CURSOR_START(this);
		while(buffer[i].c != 0)
			buff[i-CURSOR_START(this) - 4] = buffer[i++].c;
		
		
		bool result = false;
		if(check_print_command(console, buff))
			result = true;
		else if(check_ls_command(console, buff))
			result = true;
		else if(check_cd_command(console, buff))
			result = true;
		
		if(!result) 
			console->print("Unknown command.");
		
		//Clear command buffer
		for(int i = CURSOR_START(this)-1; i < width; ++i)
			buffer[i] = Cell(0, 0);
		//Reset cursor
		cursor = CURSOR_START(this);
		
		
		free(buff);
	}
}

void CommandBuffer::process_mouse_wheel(Console *console, int16 delta)
{
	if(delta > 0)
	{
		//If there are elements up
		int pos = (console->offscreen_buffer_height - 1) * width;
		if(console->buffer[pos].c != 0)
		{
			console->move_down();
			++console->wheel_delta;
		}
	}
	else if(delta < 0)
	{
		//If there are elements down
		int pos = (console->offscreen_buffer_height + console->screen_height) * width;
		if(console->buffer[pos].c != 0)
		{
			console->move_up();
			--console->wheel_delta;
		}
	}
}

bool CommandBuffer::check_print_command(Console *console, char *command)
{
	if(command[0] == 'p' && command[1] == 'r' && command[2] == 'i' &&
	  command[3] == 'n' && command[4] == 't')
	{
		if(command[5] != ' ' && command[5] != 0)
			return false;
		if(command[6] != 0)
		{
			char string_to_print[256] = {};
			int i = 6;
			while(command[i])
				string_to_print[i-6] = command[i++];
			console->print(string_to_print);
		} else console->print("You need to type something.");
	} else return false;
	
	return true;
}

bool CommandBuffer::check_ls_command(Console *console, char *command)
{
	std::string buff = cwd;
	buff += "/*";

	if(command[0] == 'l' && command[1] == 's' && command[2] == 0)
	{
		WIN32_FIND_DATA file_data = {};
		
		HANDLE first_file = FindFirstFileA(buff.c_str(), &file_data);
		buff = "";
		while(FindNextFileA(first_file, &file_data) > 0)
		{
			if(strcmp(file_data.cFileName, ".") && strcmp(file_data.cFileName, ".."))
			{
				buff += file_data.cFileName;
				buff += " | ";
			}
		}
	
		const char *str = buff.c_str();
	
		console->print((char*)str);
		FindClose(first_file);
		return true;
	}
	return false;
}

bool CommandBuffer::check_cd_command(Console *console, char *command)
{    
    if(command[0] == 'c' && command[1] == 'd' && command[2] == ' ')
	{
        char buff[32] = {};    
        for(int i = 3, j = 0; i < strlen(command); ++i, ++j)
        {
            buff[j] = command[i];
        }
    
        SetCurrentDirectory(buff);
        ZeroMemory(cwd, 256);
        GetCurrentDirectory(256, cwd);

        for(int i = 0; i < strlen(cwd); ++i)
	    {
	    	buffer[i+1] = Cell(cwd[i], 0xFFFFFFFF);
	    	if(buffer[i+1].c == '\\')
	    		buffer[i+1].c = '/';
	    }
	    cursor = CURSOR_START(this);
        return true;
    }

	return false;
}

void CommandBuffer::make_directory_list()
{
    directory_erase(&list);

    char buff[32] = {};
    for(int i = 0, j = 0; i < strlen(cwd); ++i)
    {
        if(cwd[i] == '\\')
        {
            directory_add(&list, buff);
            ZeroMemory(buff, sizeof(char) * 32);
            j = 0;
            continue;
        }

        buff[j] = cwd[i];
        ++j;
    }
}

void CommandBuffer::make_cwd_from_directory()
{
    ZeroMemory(cwd, sizeof(char) * 256);
    int i = 0;
    
    auto iterator = list.first;
    while(iterator)
    {
        strcat(cwd, iterator->buff);
        strcat(cwd, "\\");
    }

	for(int i = 0; i < strlen(cwd); ++i)
	{
		buffer[i+1] = Cell(cwd[i], 0xFFFFFFFF);
		if(buffer[i+1].c == '\\')
			buffer[i+1].c = '/';
	}
	cursor = CURSOR_START(this);

    //Change win32 cwd
}