#pragma once
#include <rcc/lib/std.h>
#include <rcc/lib/alloc.h>

struct symbol {
	string name;
	u32 type;
};

struct string_buffer {
	char* data;
	size_t len;
};

/*
 * strtab and symtab are global tables, though binded to arena
 *	and are automatically reloaded when arena is freed,
 *	with all symbols and strings being invalidated
 */

void init_strtab(arena a);
string str(const char* s);
const char* cstr(string s);

void init_symtab(arena a);
void open_scope(void);
void close_scope(void);
struct symbol* lookup(string name);
struct symbol* lookup_local(string name);
