#include <rcc/ut/ut.h>
#include <rcc/lib/alloc.h>
#include <rcc/lib/io.h>
#include <rcc/lib/table.h>

#include <unistd.h>

TEST(io, input) {
	struct arena a = {0};
	init_strtab(&a);

	char buf[] = "hello\nyo\n";
	u32 line = 1;
	struct input in = input_memory(&a, buf, sizeof(buf) - 1);
	for (char* s = buf; *s; s++) {
		ASSERT(getch(&in) == *s);
		line += *s == '\n';
		ASSERT(line == in.loc.line);
	}
	ungetch(&in, '\n');
	ASSERT(line == in.loc.line + 1);
	ASSERT(getch(&in) == '\n');
	ASSERT(line == in.loc.line);
	for (int i = 0; i < 100; i++) {
		ASSERT(getch(&in) == 0);
	}

	int p[2];
	pipe(p);
	int fdup = dup(0);
	close(0);
	dup2(p[0], 0);
	write(p[1], buf, sizeof(buf) - 1);
	close(p[0]);
	close(p[1]);
	in = input_stdin(&a);
	for (char* s = buf; *s; s++) {
		ASSERT(getch(&in) == *s);
		line += *s == '\n';
	}
	ungetch(&in, '\n');
	ASSERT(getch(&in) == '\n');
	for (int i = 0; i < 100; i++) {
		ASSERT(getch(&in) == 0);
	}
	in = input_stdin(&a);
	in = input_stdin(&a);
	close(0);
	dup2(fdup, 0);
	close(fdup);

	rmarena(&a);
}
