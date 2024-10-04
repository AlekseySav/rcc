#pragma once
#include <rcc/lex/token.h>

struct lexer {
	arena a;
	struct token (*get)(struct lexer* lex, struct input* in);

	void* _impl;
};

extern struct lexer_config lexer_as;
extern struct lexer_config lexer_cc;

struct lexer mklexer(arena a, struct lexer_config* config);
