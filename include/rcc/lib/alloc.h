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

typedef struct arena {
	struct _arena_vector _pool[6];
	struct _arena_vector _freelist;
} *arena;

void* alloc(arena a, size_t size);
void dealloc(arena a, void* p, size_t size);
void rmarena(arena a);

void* _extend_vector(void* begin, size_t size, size_t item);
void _rmvector(void* v);

#define append(v, a) ({ \
	(v)->data = _extend_vector((v)->data, (v)->len, sizeof(*(v)->data)); \
	(v)->data[(v)->len++] = (a); \
})

#define pop(v) ({ \
	assert((v)->len); \
	(v)->data[--(v)->len]; \
})

#define rmvector(v) ({ \
	assert((void*)&((typeof(v))NULL)->data == (void*)&((struct _arena_vector*)NULL)->data); \
	assert((void*)&((typeof(v))NULL)->len == (void*)&((struct _arena_vector*)NULL)->len); \
	_rmvector(v); \
})
