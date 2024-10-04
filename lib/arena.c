#include <rcc/lib/arena.h>
#include <rcc/lib/alloc.h>
#include <rcc/lib/std.h>

#include <stdlib.h>

static inline size_t _pool_id(size_t size) {
	assert(size && size <= 512);
	size = (size - 1) >> 4;
	return size < 2 ? size : size < 8 ? (size >> 2) + 2 : (size >> 4) + 4;
}

void* alloc(arena a, size_t size) {
	if (size > 512) {
		return append(&a->_freelist, _alloc(NULL, size));
	}
	size_t pool = _pool_id(size);
	size = 16 << pool;
	if (!a->_pool[pool].len) {
		void* buf = append(&a->_freelist, _alloc(NULL, 512));
		for (void* p = buf; p < buf + 512; p += size) {
			append(&a->_pool[pool], p);
		}
	}
	return pop(&a->_pool[pool]);
}

void dealloc(arena a, void* p, size_t size) {
	size_t pool = size > 512 ? 6 : _pool_id(size);
	append(&a->_pool[pool], p);
}

void rmarena(arena a) {
	for (struct _arena_vector** v = a->_vectors.data; v < a->_vectors.data + a->_vectors.len; v++) {
		rmvector(*v);
	}
	for (struct _arena_vector* v = a->_pool; v < a->_pool + sizeof(a->_pool) / sizeof(a->_pool[0]); v++) {
		rmvector(v);
	}
	for (void** p = a->_freelist.data; p < a->_freelist.data + a->_freelist.len; p++) {
		_free(*p);
	}
	rmvector(&a->_freelist);
	rmvector(&a->_vectors);
}

void* vector(arena a) {
	struct _arena_vector* v = alloc(a, sizeof(*v));
	append(&a->_vectors, v);
	v->data = NULL;
	v->len = 0;
	return v;
}

void extend_vector(void* v, size_t item, size_t new) {
	struct _arena_vector* vec = v;
	vec->len = new;
	if (vec->len > vec->cap) {
		vec->cap = vec->cap * 2;
		if (vec->len > vec->cap) {
			vec->cap = vec->len;
		}
		vec->data = _alloc(vec->data, vec->cap * item);
	}
}

void rmvector(void* v) {
	struct _arena_vector* vec = v;
	if (vec->data) {
		_free(vec->data);
	}
	vec->data = NULL;
	vec->len = 0;
	vec->cap = 0;
}
