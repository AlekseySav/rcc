#include <assert.h>
#include <stdlib.h>
#include <rcc/lib/alloc.h>

#ifdef TRACK_ALLOCATIONS
#define MAX_ALLOCATIONS 16000
static void* _addrs[MAX_ALLOCATIONS];

static void** _findmem(void* addr) {
	for (void** p = _addrs; p < _addrs + MAX_ALLOCATIONS; p++) {
		if (*p == addr) return p;
	}
	return NULL;
}

bool detect_memory_leaks(void) {
	for (void** p = _addrs; p < _addrs + MAX_ALLOCATIONS; p++) {
		if(*p) return true;
	}
	return false;
}
#endif

static inline void* _alloc(void* origin, size_t size) {
	void* r = origin ? realloc(origin, size) : malloc(size);
	assert(r);
#ifdef TRACK_ALLOCATIONS
	assert(_findmem(origin));
	*_findmem(origin) = r;
#endif
	return r;
}

static inline void _free(void* addr) {
#ifdef TRACK_ALLOCATIONS
	assert(_findmem(addr));
	*_findmem(addr) = NULL;
#endif
	free(addr);
}

static inline size_t _pool_id(size_t size) {
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

void* extend_vector(void* begin, size_t size, size_t item, bool force_realloc) {
	size--;
	if (!force_realloc && size & size - 1) {
		return begin;
	}
	return _alloc(begin, size ? (size << 1) * item : item);
}

void rmvector(void* v) {
	struct _arena_vector* vec = v;
	if (vec->data) {
		_free(vec->data);
	}
	vec->data = NULL;
	vec->len = 0;
}
