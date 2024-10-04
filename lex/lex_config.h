#pragma once

typedef char lex_jmptab[128];

struct lexer_config {
	lex_jmptab* jt;
	bool* final;
};
