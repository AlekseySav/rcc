#include <rcc/lib/std.h>

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

noreturn void _assert(void) {
	fprintf(stderr, "internal error\n");
	exit(1);
}

noreturn void _assert_debug(const char* file, u32 line, const char* msg) {
	fprintf(stderr, "\e[31massertion failed:\e[0m %s:%d: %s\n", file, line, msg);
	abort();
}
