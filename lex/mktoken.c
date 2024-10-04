#include <rcc/lex/token.h>
#include <rcc/lib/table.h>

/*
 * implementation for mktoken
 */

static u32 _digit[] = {
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 16, 16, 16, 16, 16, 16,
	16, 10, 11, 12, 13, 14, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	16, 10, 11, 12, 13, 14, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
};

static inline u8 _if(const char* p, const char* e, char c, u8 if_true, u8 if_false) {
	if (p == e) {
		return if_false;
	}
	return *p == c ? if_true : if_false;
}

static inline u8 _type(const struct string_buffer* buf) {
	const char* p = buf->data;
	const char* e = buf->data + buf->len;

	if (buf->len == 0) {
		return 0;
	}
	switch (*p++) {
		case 0:   return 0;
		case '(': return L_open_paren;
		case ')': return L_close_paren;
		case '[': return L_open_bracket;
		case ']': return L_close_bracket;
		case '{': return L_open_brace;
		case '}': return L_close_brace;
		case ';': return L_semicolon;
		case ',': return L_comma;
		case '.': return L_dot;
		case '?': return L_question;
		case ':': return L_colon;
		case '~': return L_inv;
		case '!': return _if(p, e, '=', L_neq, L_not);
		case '*': return _if(p, e, '=', L_imul, L_mul);
		case '/': return _if(p, e, '/', 1, 0) ? L_linebreak : _if(p, e, '*', 1, 0) ? L_space : _if(p, e, '=', L_idiv, L_div);
		case '%': return _if(p, e, '=', L_imod, L_mod);
		case '+': return _if(p, e, '+', 1, 0) ? L_inc : _if(p, e, '=', L_iadd, L_add);
		case '-': return _if(p, e, '-', 1, 0) ? L_dec : _if(p, e, '>', 1, 0) ? L_arrow : _if(p, e, '=', L_isub, L_sub);
		case '<': return _if(p, e, '<', 1, 0) ? _if(p + 1, e, '=', L_ishl, L_shl) : _if(p, e, '=', L_leq, L_lt);
		case '>': return _if(p, e, '>', 1, 0) ? _if(p + 1, e, '=', L_ishr, L_shr) : _if(p, e, '=', L_geq, L_gt);
		case '&': return _if(p, e, '&', 1, 0) ? L_andl : _if(p, e, '=', L_iand, L_and);
		case '^': return _if(p, e, '=', L_ixor, L_xor);
		case '|': return _if(p, e, '|', 1, 0) ? L_orl : _if(p, e, '=', L_ior, L_or);
		case '=': return _if(p, e, '=', L_equ, L_assign);
		case ' ': return L_space;
		case '\t':
				  return L_tabspace;
		case '\n':
				  return L_linebreak;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': case '\'':
				  return L_num;
		case '"': return L_str;
		default:  return L_sym;
	}
}

static inline const char* strbuf(const struct string_buffer* buf) {
	struct string_buffer* non_const = (void*)buf;
	append(non_const, 0);
	pop(non_const);
	return buf->data;
}

static inline u32 _num(struct location loc, const struct string_buffer* buf, u8* nbase) {
	u32 n = 0, base = 10;
	const char* b = buf->data;

	if (buf->len > 1 && *b == '0') {
		base = 8;
		b++;
		if (*b == 'x' || *b == 'X') {
			base = 16;
			b++;
		}
	}
	*nbase = base;

	while (b < buf->data + buf->len) {
		u32 d = _digit[*b++];
		if (d >= base || n > (0xffffffffu - d) / base) {
			error(loc, "bad number: '%s'", strbuf(buf));
			return 0;
		}
		n = n * base + d;
	}
	return n;
}

static inline char _char(struct location loc, const char** p) {
	char c;
	if (**p != '\\') {
		return **p;
	}
	switch (*++(*p)) {
		case 'b':  return '\b';
		case 'e':  return '\e';
		case 'n':  return '\n';
		case 'r':  return '\r';
		case 't':  return '\t';
		case '\\': return '\\';
		case '\'': return '\'';
		case '\"': return '\"';
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
			do {
				c = c * 8 + *(*p)++ - '0';
			} while (*(*p) >= '0' && *(*p) <= '7');
			(*p)--;
			return c;
		default:
			error(loc, "unknown espace sequence: '\\%c'", **p);
			return 0;
	}
}

static inline u32 _chars(struct location loc, const struct string_buffer* buf) {
	u32 r = 0;
	for (const char* p = buf->data + 1; p < buf->data + buf->len - 1; p++) {
		if (r >= 0x01000000) {
			error(loc, "char integer overflow");
		}
		r = r << 8 | _char(loc, &p);
	}
	return r;
}

static inline struct string_buffer* _str(arena a, struct location loc, const struct string_buffer* buf) {
	struct string_buffer* r = vector(a);
	for (const char* p = buf->data + 1; p < buf->data + buf->len - 1; p++) {
		append(r, _char(loc, &p));
	}
	return r;
}

struct token mktoken(arena a, struct location loc, const struct string_buffer* buf) {
	struct token res = {0};
	switch (res.token = _type(buf)) {
		case L_num:
			res.as_number = buf->data[0] == '\'' ? _chars(loc, buf) : _num(loc, buf, &res._nbase);
			break;
		case L_sym:
			res.as_symbol = str(strbuf(buf));
			break;
		case L_str:
			res.as_string = _str(a, loc, buf);
			break;
	}
	return res;
}
