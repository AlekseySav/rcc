#include <rcc/ut/ut.h>
#include <rcc/lex/lex.h>
#include "../lex_config.h"

// re: a*b
#define Q -1

static lex_jmptab _jt[] = {
	{
		2, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		2, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, 1, 2, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
	},
	{
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, 1, 2, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
	},
	{
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
		Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q, Q,
	},
};

static bool _final[] = {
	false, false, true,
};

static struct lexer_config _conf = { .jt = _jt, .final = _final };

TEST(lex, basic_config) {
	struct arena a = {0};
	init_strtab(&a);
	struct token t;
	char buf[] = "aa\\\naab ab b ";
	struct input in = input_memory(&a, buf, sizeof(buf) - 1);
	struct lexer lex = mklexer(&a, &_conf);

	ASSERT((t = lex.get(&lex, &in)).token == L_sym && t.as_symbol == str("aaaab"));
	ASSERT((t = lex.get(&lex, &in)).token == L_space);
	ASSERT((t = lex.get(&lex, &in)).token == L_sym && t.as_symbol == str("ab"));
	ASSERT((t = lex.get(&lex, &in)).token == L_space);
	ASSERT((t = lex.get(&lex, &in)).token == L_sym && t.as_symbol == str("b"));
	ASSERT((t = lex.get(&lex, &in)).token == L_space);
	ASSERT((t = lex.get(&lex, &in)).token == 0);

	rmarena(&a);
}
