#include <rcc/lex/token.h>
#include <rcc/lib/table.h>

/*
 * implementation for strtoken
 */

static const char* _stok[] = {
	"", "", "", "",
	";", ",", "(", ")", "[", "]",
	"!", "~", "*", "/", "%", "+", "-", "<<", ">>",
	"<", "<=", ">", "<=", "==", "!=",
	"&", "^", "|", "&&", "||", "=",
	"+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", "^=", "|=",
	".", "->", "?", ":", "++", "--", "{", "}",
};

static inline void _numbuf(struct string_buffer* buf, u32 n, u32 base, bool special) {
	static const char* glyphs = "0123456789abcdef";
	assert(base == 8 || base == 10 || base == 16);
	u32 origin = n;
	u32 start = buf->len;

    do {
        append(buf, glyphs[n % base]);
        n /= base;
    } while (n);

	if (special) {
		if (base == 16) {
			append(buf, 'x');
		}
		if (base != 10) {
			append(buf, '0');
		}
	}

	char* p1 = buf->data + start;
	char* p2 = buf->data + buf->len - 1;
    while(p1 < p2) {
        char c = *p2;
        *p2-- = *p1;
        *p1++ = c;
    }
}

static inline void _charbuf(struct string_buffer* buf, u8 c) {
	if (c == '"' || c == '\'' || c == '\\') {
		append(buf, '\\');
		append(buf, c);
		return;
	}
	if (c < ' ' || c >= 0x7f) {
		append(buf, '\\');
		_numbuf(buf, c, 8, false);
		return;
	}
	append(buf, c);
}

static inline void _loadbuf(struct string_buffer* buf, const char* p, size_t len) {
	bool raw = len == 0;
	while (p && raw && *p || len--) {
		_charbuf(buf, *p++);
	}
}

void strtoken(struct token t, struct string_buffer* buf) {
	switch (t.token) {
		case L_sym:
			_loadbuf(buf, cstr(t.as_symbol), 0);
			break;
		case L_str:
			append(buf, '"');
			_loadbuf(buf, t.as_string->data, t.as_string->len);
			append(buf, '"');
			break;
		case L_num:
			if (t._nbase) {
				_numbuf(buf, t.as_number, t._nbase ? t._nbase : 10, true);
			}
			else {
				append(buf, '\'');
				do {
					_charbuf(buf, t.as_number & 255);
					t.as_number >>= 8;
				} while (t.as_number);
				append(buf, '\'');
			}
			break;
		default:
			_loadbuf(buf, _stok[t.token], 0);
			break;
	}
}
