#include <assert.h>
#include <stdlib.h>
#include <rcc/lib/alloc.h>

static inline void* _alloc(void* origin, size_t size) {
	void* r = origin ? realloc(origin, size) : malloc(size);
	assert(r);
	return r;
}

static inline size_t _pool_id(size_t size) { // = [log_2(size * 2 - 1)] - 4
	assert(size && size <= 512);
	size = (size - 1) >> 4;
	return size < 2 ? size : size < 8 ? (size >> 2) + 2 : (size >> 4) + 4;
}

void* alloc(arena a, size_t size) {
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
	size_t pool = _pool_id(size);
	append(&a->_pool[pool], p);
}

void rmarena(arena a) {
	for (void** p = a->_freelist.data; p < a->_freelist.data + a->_freelist.len; p++) {
		free(*p);
	}
	for (struct _arena_vector* v = a->_pool; v < a->_pool + sizeof(a->_pool) / sizeof(a->_pool[0]); v++) {
		rmvector(v);
	}
	rmvector(&a->_freelist);
}

void* _extend_vector(void* begin, size_t size, size_t item) {
	if (size & size - 1) {
		return begin;
	}
	return _alloc(begin, size ? (size << 1) * item : item);
}

void _rmvector(void* v) {
	struct _arena_vector* vec = v;
	if (vec->data) {
		free(vec->data);
	}
	vec->data = NULL;
	vec->len = 0;
}
