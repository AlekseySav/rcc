#pragma once
#include <rcc/lib/std.h>
#include <rcc/lib/arena.h>

struct location {
	string filename;
	u32 line;
};

struct input {
	arena a;
	struct location loc;
	char (*getch)(struct input* in);

	void* _impl;
	char _peekc;
};

struct input input_stdin(arena a);
struct input input_file(arena a, string filename);
struct input input_memory(arena a, const char* buf, u32 size);
char ungetch(struct input* in, char c);
char getch(struct input* in);

void error(struct location loc, const char* fmt, ...);
noreturn void fatal(const char* fmt, ...);
bool errored(void);
