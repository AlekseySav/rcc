#include <rcc/lib/table.h>
#include <rcc/lib/io.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

static int _errors = 0;

void error(struct location loc, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, "\e[1mrcc: %s:%d: \e[0;31merror:\e[0m ", cstr(loc.filename), loc.line);
	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);
	va_end(ap);
	_errors++;
}

noreturn void fatal(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, "\e[1mrcc: \e[31mfatal error:\e[0m ");
	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);
	va_end(ap);
	exit(1);
}

bool errored(void) {
	return _errors;
}
