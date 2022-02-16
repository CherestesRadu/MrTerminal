#pragma once
#include "defines.h"
#include "console.h"

#define CURSOR_START(c) strlen(c->cwd) + 2

struct Cell;
struct Console;

struct DirectoryList
{
    struct Node
    {
        char buff[32] = {};
        Node *next = NULL;
        Node *prev = NULL;
    };

    Node *first = NULL;
    Node *last = NULL;
};

void directory_add(DirectoryList *list, const char *dir_name);
void directory_remove(DirectoryList *list, const char *dir_name);
void directory_remove(DirectoryList *list, DirectoryList::Node *node);
void directory_print(DirectoryList *list);
void directory_erase(DirectoryList *list);

DirectoryList::Node *directory_find(DirectoryList *list, const char *dir_name);

struct CommandBuffer
{
	CommandBuffer(int32 w);
	~CommandBuffer();

	Cell operator[] (int index);
	const Cell operator[](int index) const;
	
	void char_input(WPARAM c);
	void key_input(Console *console, WPARAM c);
	void process_command(Console *console);
	void process_mouse_wheel(Console *console, int16 delta);
	
	bool check_print_command(Console *console, char *command);
	bool check_ls_command(Console *console, char *command);
	bool check_cd_command(Console *console, char *command);
	
	void make_directory_list();
	void make_cwd_from_directory();

	int32 width;
	int32 cursor;
	
	Cell *buffer;
	DirectoryList list;
	char cwd[256] = {};
};