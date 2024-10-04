#include <rcc/lib/arena.h>
#include <rcc/lib/table.h>

#include <string.h>

struct _string_vector {
	string* data;
	size_t len;
	size_t cap;
};

static struct {
	arena a;
	struct _string_vector* hsh;
	struct string_buffer* buf;
} t;

static u32 _hash(const char* s) {
	u32 n = 0;
	while (*s) {
		n = n * 97 + *s++;
	}
	return n;
}

static string _insert(const char* s) {
	string res = t.buf->len;
	while (*s) {
		append(t.buf, *s++);
	}
	append(t.buf, 0);
	return res;
}

static string* _lookup_bucket(const char* s, u32 hash) {
	for (u32 i = 0; i < t.hsh->len; i++) {
		string* b = t.hsh->data + (hash + i * i) % t.hsh->len;
		if (*b == 0 || !strcmp(cstr(*b), s)) {
			return b;
		}
	}
	return NULL;
}

static void _rehash(u32 newsize) {
	struct _string_vector old = *t.hsh;
	*t.hsh = (struct _string_vector){0};
	extend_vector(t.hsh, sizeof(string), newsize);
	for (string* s = old.data; s < old.data + old.len; s++) {
		if (!*s) {
			continue;
		}
		string* b = _lookup_bucket(cstr(*s), _hash(cstr(*s)));
		assert(b && !*b);
		*b = *s;
	}
	rmvector(&old);
}

void init_strtab(arena a) {
	t.a = a;
	t.hsh = vector(a);
	t.buf = vector(a);
	append(t.buf, 0);
}

const char* cstr(string s) {
	assert(s && s < t.buf->len);
	return t.buf->data + s;
}

string str(const char* s) {
	u32 hash = _hash(s);
	string* b = _lookup_bucket(s, hash);
	if (!b) {
		_rehash(t.hsh->len * 2 + 1);
		b = _lookup_bucket(s, hash);
		assert(b);
	}
	if (!*b) {
		*b = _insert(s);
	}
	return *b;
}
