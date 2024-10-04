#pragma once
#include <rcc/lib/std.h>
#include <rcc/lib/arena.h>

/*
 * implementation of strtab & symbol tables for cpp, as, cc, etc.
 */

struct string_buffer {
	char* data;
	size_t len;
	size_t cap;
};

struct _hshent {
	void* symbol;
	struct _hshent* prev;
	u32 name;
	u32 scope;
};

struct _hshent_vector {
	struct _hshent* data;
	size_t len;
	size_t cap;
};

struct symtab {
	arena a;
	struct _hshent_vector* hsh;
	struct _hshent* last;
	u32 scope;
	u32 itemsize;
};

void init_symtab(struct symtab* s, arena a, u32 itemsize);
void open_scope(struct symtab* s);
void close_scope(struct symtab* s);
void* lookup(struct symtab* s, string name); // return null if not found
void* lookup_local(struct symtab* s, string name); // create symbol in not found

void init_strtab(arena a);
const char* cstr(string s);
string str(const char* s);
