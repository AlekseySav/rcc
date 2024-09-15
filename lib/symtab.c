#include <rcc/lib/alloc.h>
#include <rcc/lib/symtab.h>
#include <string.h>

struct ext_sym {
	struct symbol s;
	struct ext_sym* prev;
	u32 scope_id;
	u32 hsh_link;
};

struct sym_buckets {
	struct ext_sym** data;
	size_t len;
};

static struct {
	arena a;
	struct sym_buckets* buckets;
	u32 scope;
	struct ext_sym* last;
} symtab;

static u32 _hash(string name, u32 scope) {
	return ~(name ^ scope);
}

static struct ext_sym** _get_bucket(string name, u32 scope) {
	for (u32 i = 0; i < (symtab.buckets->len * 2 / 3); i++) {
		struct ext_sym** s = &symtab.buckets->data[(_hash(name, scope) + i) % symtab.buckets->len];
		if (!*s || (*s)->s.name == name && (*s)->scope_id == scope) {
			return s;
		}
	}
	return NULL;
}

static void _insert_symbol(struct ext_sym** bucket, string name, u32 scope) {
	*bucket = alloc(symtab.a, sizeof(**bucket));
	(*bucket)->s.name = name;
	(*bucket)->prev = symtab.last;
	(*bucket)->scope_id = scope;
	(*bucket)->hsh_link = bucket - symtab.buckets->data;
	symtab.last = *bucket;
}

static void _rehash(void) {
	struct ext_sym** bucket;
	symtab.buckets->len = (symtab.buckets->len ? (symtab.buckets->len - 1) * 2 : 16) + 1,
	symtab.buckets->data = extend_vector(symtab.buckets->data, symtab.buckets->len, sizeof(struct ext_sym*), true);
	memset(symtab.buckets->data, 0, symtab.buckets->len * sizeof(struct ext_sym*));
	for (struct ext_sym* s = symtab.last; s; s = s->prev) {
		assert(bucket = _get_bucket(s->s.name, s->scope_id));
		*bucket = s;
	}
}

void init_symtab(arena a) {
	symtab.a = a;
	symtab.buckets = vector(a);
	symtab.scope = 0;
	symtab.last = NULL;
}

void open_scope(void) {
	symtab.scope++;
}

void close_scope(void) {
	assert(symtab.scope);
	struct ext_sym* s;
	for (s = symtab.last; s && s->scope_id == symtab.scope; s = s->prev) {
		assert(s->scope_id);
		symtab.buckets->data[s->hsh_link] = NULL;
		s->scope_id = 0;
	}
	symtab.scope--;
	symtab.last = s;
}

struct symbol* lookup(string name) {
	struct ext_sym** top;
	struct ext_sym** s;
	for (u32 scope = symtab.scope; scope > 0; scope--) {
		if ((s = _get_bucket(name, scope)) && *s) {
			return &(*s)->s;
		}
		if (scope == symtab.scope) {
			top = s;
		}
	}
	if (!top) {
		_rehash();
	}
	assert(top = _get_bucket(name, symtab.scope));
	_insert_symbol(top, name, symtab.scope);
	return &(*top)->s;
}

struct symbol* lookup_local(string name) {
	struct ext_sym** s;
	if ((s = _get_bucket(name, symtab.scope))) {
		if (!*s) {
			_insert_symbol(s, name, symtab.scope);
		}
		return &(*s)->s;
	}
	_rehash();
	assert(s = _get_bucket(name, symtab.scope));
	_insert_symbol(s, name, symtab.scope);
	return &(*s)->s;
}
