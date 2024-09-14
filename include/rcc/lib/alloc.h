#pragma once
#include <stddef.h>
#include <assert.h>

/*
 * arena
 * usage:
 *		struct arena _a = {0};				// create arena
 *		int* p = alloc(&a, sizeof(int)); 	// allocate memory
 *		dealloc(p);							// optional, return memory to arena pool
 *		rmarena(&a);						// free arena elements
 *
 * vector
 * usage:
 *		struct V { int* data; size_t len; }	// create vector
 *		struct V v = {0};
 *		append(&v, 10);						// append to vector
 *		int p = pop(v);						// pop from vector
 *		rmvector(&v);						// free vector buffer
 */

struct _arena_vector {
	void** data;
	size_t len;
};

struct _arena_vector_list {
	struct _arena_vector** data;
	size_t len;
};

typedef struct arena {
	struct _arena_vector _pool[6];
	struct _arena_vector _freelist;
	struct _arena_vector_list _vectors;
} *arena;

void* alloc(arena a, size_t size);
void dealloc(arena a, void* p, size_t size);
void rmarena(arena a);

void* vector(arena a);
void* _extend_vector(void* begin, size_t size, size_t item);
void _rmvector(void* v);

#define append(v, a) ({ \
	typeof(v) _v = (v); \
	_v->data = _extend_vector(_v->data, _v->len + 1, sizeof(typeof(*(_v->data)))); \
	_v->data[_v->len++] = (a); \
})

#define emplace(v) ({ \
	typeof(v) _v = (v); \
	_v->data = _extend_vector(_v->data, _v->len + 1, sizeof(typeof(*(_v->data)))); \
	&_v->data[_v->len++]; \
})

#define pop(v) ({ \
	typeof(v) _v = (v); \
	assert(_v->len); \
	_v->data[--_v->len]; \
})

#define rmvector(v) ({ \
	typeof(v) _v = (v); \
	assert((void*)&((typeof(_v))NULL)->data == (void*)&((struct _arena_vector*)NULL)->data); \
	assert((void*)&((typeof(_v))NULL)->len == (void*)&((struct _arena_vector*)NULL)->len); \
	_rmvector(_v); \
})
