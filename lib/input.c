#include <rcc/lib/table.h>
#include <rcc/lib/io.h>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

struct _mem_impl {
	const char* buf;
	u32 pos, size;
};

struct _fd_impl {
	char buf[512];
	size_t pos, size;
	int fd;
};

static inline char _process_get(struct input* in, char c) {
	if (c >= 0x7f || c < ' ' && c != '\n' && c != '\t') {
		error(in->loc, "bad char: 0x%02x", c);
		c = ' ';
	}
	if (c == '\n') {
		in->loc.line++;
	}
	return c;
}

static char _mem_get(struct input* in) {
	struct _mem_impl* f = in->_impl;
	if (!f) {
		return 0;
	}
	if (f->size == f->pos) {
		dealloc(in->a, f, sizeof(*f));
		return 0;
	}
	return _process_get(in, f->buf[f->pos++]);
}

static char _fd_get(struct input* in) {
	struct _fd_impl* f = in->_impl;
	if (!f) {
		return 0;
	}
	if (f->size == f->pos) {
		f->pos = 0;
		f->size = read(f->fd, f->buf, 512);
		if (f->size == (size_t)-1) {
			fatal("error reading file: %s: %s", cstr(in->loc.filename), strerror(errno));
		}
		if (f->size == 0) {
			dealloc(in->a, f, sizeof(*f));
			return 0;
		}
	}
	return _process_get(in, f->buf[f->pos++]);
}

struct input input_stdin(arena a) {
	return (struct input){
		.loc = (struct location){ str("<stdin>"), 1 },
		.getch = _fd_get,
		.a = a,
		._impl = alloc(a, sizeof(struct _fd_impl)),
	};
}

struct input input_file(arena a, string filename) {
	int fd = open(cstr(filename), O_RDONLY);
	if (fd == -1) {
		fatal("unable to open file: %s: %s", cstr(filename), strerror(errno));
	}
	return (struct input){
		.loc = (struct location){ filename, 1 },
		.getch = _fd_get,
		.a = a,
		._impl = alloc(a, sizeof(struct _fd_impl)),
	};
}

struct input input_memory(arena a, const char* buf, u32 size) {
	struct _mem_impl* f = alloc(a, sizeof(*f));
	f->buf = buf;
	f->size = size;
	return (struct input){
		.loc = (struct location){ str("<memory>"), 1 },
		.getch = _mem_get,
		.a = a,
		._impl = f,
	};
}

char ungetch(struct input* in, char c) {
	assert(in->_peekc == 0);
	if (c == '\n') {
		in->loc.line--;
	}
	return in->_peekc = c;
}

char getch(struct input* in) {
	char c = in->_peekc;
	in->_peekc = 0;
	if (c) {
		return _process_get(in, c);
	}
	return in->getch(in);
}
