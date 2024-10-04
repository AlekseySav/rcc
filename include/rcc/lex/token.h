#pragma once
#include <rcc/lib/table.h>
#include <rcc/lib/io.h>

enum {
    L_sym = 1,			// <identifier>
	L_num,				// <numerical constant>
    L_str,				// <string literal>

	L_semicolon,		// ;
	L_comma,			// ,
	L_open_paren,		// (
	L_close_paren,		// )
	L_open_bracket,		// [
	L_close_bracket,	// ]

	L_not,				// !
	L_inv,				// ~
	L_mul,				// *
	L_div,				// /
	L_mod,				// %
	L_add,				// +
	L_sub,				// -
	L_shl,				// <<
	L_shr,				// >>
	L_lt,				// <
	L_leq,				// <=
	L_gt,				// >
	L_geq,				// >=
	L_equ,				// ==
	L_neq,				// !=
	L_and,				// &
	L_xor,				// ^
	L_or,				// |
	L_andl,				// &&
	L_orl,				// ||
	L_assign,			// =

	L_iadd,				// +=
	L_isub,				// -=
	L_imul,				// *=
	L_idiv,				// /=
	L_imod,				// %=
	L_ishl,				// <<=
	L_ishr,				// >>=
	L_iand,				// &=
	L_ixor,				// ^=
	L_ior,				// |=

	L_dot,				// .
	L_arrow,			// ->
	L_question,			// ?
	L_colon,			// :
	L_inc,				// ++
	L_dec,				// --
	L_open_brace,		// {
	L_close_brace,		// }

	L_space,
	L_tabspace,
	L_linebreak,

	L_COUNT,
};

struct token {
	struct location loc;
	union {
		struct {
			u32 as_number;
			u8 _nbase;
		};
		string as_symbol;
		struct string_buffer* as_string;
	};
	u8 token;
};

struct token mktoken(arena a, struct location loc, const struct string_buffer* buf);
void strtoken(struct token t, struct string_buffer* buf);
