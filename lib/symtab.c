#include <rcc/lib/arena.h>
#include <rcc/lib/table.h>

#include <stdio.h>
#include <string.h>

static u32 _hash(string name, u32 scope) {
	return name ^ scope;
}

static void _insert(struct symtab* s, struct _hshent* b, string name, u32 scope) {
	b->symbol = alloc(s->a, s->itemsize);
	b->prev = s->last;
	b->name = name;
	b->scope = scope;
	s->last = b;
}

static struct _hshent* _lookup_bucket(struct symtab* s, string name, u32 scope) {
	for (u32 i = 0; i < s->hsh->len; i++) {
		struct _hshent* b = s->hsh->data + (_hash(name, scope) + i * i) % s->hsh->len;
		if (b->name == 0 || (b->name == name && b->scope == scope)) {
			return b;
		}
	}
	return NULL;
}

static void _rehash(struct symtab* s, u32 newsize) {
	struct _hshent_vector old = *s->hsh;
	*s->hsh = (struct _hshent_vector){0};
	extend_vector(s->hsh, sizeof(struct _hshent), newsize);
	struct _hshent* oldlast = s->last;
	s->last = NULL;
	for (struct _hshent* e = oldlast; e; e = e->prev) {
		struct _hshent* b = _lookup_bucket(s, e->name, e->scope);
		assert(b && !b->name);
		*b = *e;
		b->prev = s->last;
		s->last = b;
	}
	rmvector(&old);
}

void init_symtab(struct symtab* s, arena a, u32 itemsize) {
	s->a = a;
	s->hsh = vector(a);
	s->last = NULL;
	s->scope = 0;
	s->itemsize = itemsize;
}

void open_scope(struct symtab* s) {
	s->scope++;
}

void close_scope(struct symtab* s) {
	assert(s->scope);
	while (s->last && s->last->scope == s->scope) {
		memset(s->last->symbol, 0, s->itemsize);
		s->last->name = s->last->scope = 0;
		s->last = s->last->prev;
	}
	s->scope--;
}

void* lookup(struct symtab* s, string name) {
	assert(s->scope);
	for (u32 scope = s->scope; scope; scope--) {
		struct _hshent* b = _lookup_bucket(s, name, scope);
		if (b && b->name) {
			return b->symbol;
		}
	}
	return NULL;
}

void* lookup_local(struct symtab* s, string name) {
	assert(s->scope);
	struct _hshent* b = _lookup_bucket(s, name, s->scope);
	if (!b) {
		_rehash(s, s->hsh->len * 2 + 1);
		b = _lookup_bucket(s, name, s->scope);
		assert(b);
	}
	if (!b->name) {
		_insert(s, b, name, s->scope);
	}
	// fprintf(stderr, "ok\n");
	return b->symbol;
}
