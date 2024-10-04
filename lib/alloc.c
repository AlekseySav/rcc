#include <rcc/lib/std.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include <malloc/malloc.h>
#elif __linux__
#include <malloc.h>
#define malloc_size malloc_usable_size
#else
#error unsupported platform
#endif

static void** _memory_map;
static size_t _map_len;

static void** _lookup(void* p) {
	for (void** a = _memory_map; a < _memory_map + _map_len; a++) {
		if (*a == p) {
			return a;
		}
	}
	assert(p == NULL && "trying to free/reallocate invalid memory");
	size_t new_len = _map_len ? _map_len * 2 : 16;
	void** new = calloc(new_len, sizeof(void*));
	if (_map_len) {
		memcpy(new, _memory_map, _map_len * sizeof(void*));
		free(_memory_map);
	}
	_memory_map = new;
	_map_len = new_len;
	return &_memory_map[_map_len - 1];
}

bool _cleanup(void) {
	bool res = false;
	for (void** a = _memory_map; a < _memory_map + _map_len; a++) {
		if (*a) {
			free(*a);
			res = true;
		}
	}
	free(_memory_map);
	_memory_map = NULL;
	_map_len = 0;
	return res;
}

void* _alloc(void* old, size_t newsize) {
	void* p;
	assert(newsize);

	if (!old) {
		assert(p = calloc(newsize, 1));
	}
	else {
		size_t oldsize = malloc_size(old);
		assert(p = realloc(old, newsize));
		newsize = malloc_size(p);
		if (oldsize < newsize) {
			memset(p + oldsize, 0, newsize - oldsize);
		}
	}

#ifdef DEBUG
	*_lookup(old) = p;
#endif
	return p;
}

void _free(void* p) {
#ifdef DEBUG
	*_lookup(p) = NULL;
#endif
	free(p);
}
