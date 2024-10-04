#include <rcc/lex/lex.h>
#include "lex_config.h"

static inline bool _read(struct string_buffer* b, struct lexer_config* conf, struct input* in) {
	char c, prev, state = 0;
	while (state >= 0) {
		c = getch(in);
		if (c == '\\') {
			if ((c = getch(in)) == '\n') {
				continue;
			}
			ungetch(in, c);
		}
		prev = state;
		state = conf->jt[state][c];
		append(b, c);
	}
	ungetch(in, pop(b));
	return conf->final[prev];
}

static struct token _token(struct lexer* lex, struct input* in) {
	struct string_buffer b = {0};
	struct location loc = in->loc;
	while (!_read(&b, lex->_impl, in)) {
		error(loc, "bad token");
		if (b.len == 0) {
			getch(in);
		}
		rmvector(&b);
		loc = in->loc;
	}
	append(&b, 0);
	pop(&b);
	struct token t = mktoken(lex->a, loc, &b);
	rmvector(&b);
	return t;
}

struct lexer mklexer(arena a, struct lexer_config* config) {
	return (struct lexer){
		.a = a,
		.get = _token,
		._impl = config,
	};
}
