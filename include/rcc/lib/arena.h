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
	size_t cap;
};

struct _arena_vector_list {
	struct _arena_vector** data;
	size_t len;
	size_t cap;
};

typedef struct arena {
	struct _arena_vector _pool[7];
	struct _arena_vector _freelist;
	struct _arena_vector_list _vectors;
} *arena;

void* alloc(arena a, size_t size);
void dealloc(arena a, void* p, size_t size);
void rmarena(arena a);

void* vector(arena a);
void extend_vector(void* v, size_t item, size_t new);
void rmvector(void* v);

/*
 * vector helper functions
 */

#define _get_vector_and_assert_ok(v) ({ \
	typeof(v) _v = (v); \
	static_assert((void*)&((typeof(_v))NULL)->data == (void*)&((struct _arena_vector*)NULL)->data); \
	static_assert((void*)&((typeof(_v))NULL)->len == (void*)&((struct _arena_vector*)NULL)->len); \
	static_assert((void*)&((typeof(_v))NULL)->cap == (void*)&((struct _arena_vector*)NULL)->cap); \
	_v; \
})

#define append(v, a) ({ \
	typeof(v) _v = _get_vector_and_assert_ok(v); \
	extend_vector(_v, sizeof(typeof(*(_v->data))), _v->len + 1); \
	_v->data[_v->len - 1] = (a); \
})

#define emplace(v) ({ \
	typeof(v) _v = _get_vector_and_assert_ok(v); \
	extend_vector(_v, sizeof(typeof(*(_v->data))), v->len + 1); \
	&_v->data[_v->len - 1]; \
})

#define pop(v) ({ \
	typeof(v) _v = _get_vector_and_assert_ok(v); \
	assert(_v->len); \
	_v->data[--_v->len]; \
})
