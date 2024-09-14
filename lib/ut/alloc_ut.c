#include <rcc/lib/alloc.h>
#include <rcc/ut/ut.h>

struct vec {
	int* data;
	size_t len;
};

TEST(alloc, vector_basic) {
	struct vec v = {0};
	append(&v, 1);
	ASSERT(v.len == 1);
	append(&v, 2);
	ASSERT(v.len == 2);
	append(&v, 3);
	ASSERT(v.len == 3);
	ASSERT(v.data[0] == 1);
	ASSERT(v.data[1] == 2);
	ASSERT(v.data[2] == 3);
	pop(&v);
	ASSERT(v.len == 2);
	ASSERT(v.data[0] == 1);
	ASSERT(v.data[1] == 2);
	append(&v, 4);
	ASSERT(v.len == 3);
	ASSERT(v.data[0] == 1);
	ASSERT(v.data[1] == 2);
	ASSERT(v.data[2] == 4);
	rmvector(&v);
	ASSERT(v.data == NULL && v.len == 0);
}

TEST(alloc, vector_capacity) {
	struct vec v = {0};
	append(&v, 1);
	append(&v, 2);
	int* data = v.data;
	append(&v, 3);
	ASSERT(data == v.data);
	rmvector(&v);
	for (int i = 0; i < 200; i++) {
		append(&v, i);
	}
	data = v.data;
	for (int i = 200; i < 256; i++) {
		append(&v, i);
		ASSERT(data == v.data);
	}
	for (int i = 255; i >= 0; i--) {
		ASSERT(pop(&v) == i);
	}
	rmvector(&v);
}

TEST(alloc, arena_dealloc) {
	struct arena a = {0};
	int* p = alloc(&a, sizeof(int));
	*p = 4;
	dealloc(&a, p, sizeof(int));
	int* q = alloc(&a, sizeof(int));
	ASSERT(p == q);
	rmarena(&a);
}

TEST(alloc, arena_no_extra_allocations) {
	struct arena a = {0};
	alloc(&a, 16);
	assert(a._pool[0].len == 512 / 16 - 1);
	rmarena(&a);

	int* p;
	for (int i = 0; i < 512; i += 16) {
		p = alloc(&a, 16);
		p[3] = i;
	}
	ASSERT(a._freelist.len == 1);
	TRACE("%p %p", p, a._freelist.data[0]);
	ASSERT(p == a._freelist.data[0]);
	rmarena(&a);
}

struct arena_pool_info {
	int id;
	int item_size;
};

static struct arena_pool_info get_pool_id(size_t size) {
	struct arena a = {0};
	void* p = alloc(&a, size);
	for (int i = 0; i < sizeof(a._pool) / sizeof(a._pool[0]); i++) {
		if (a._pool[i].data) {
			return (struct arena_pool_info){
				.id = i,
				.item_size = a._pool[i].len ? p - a._pool[i].data[a._pool[i].len - 1] : 512,
			};
		}
	}
	rmarena(&a);
	return (struct arena_pool_info){.id = -1};
}

TEST(alloc, arena_pool_id) {
	for (int size = 1; size <= 16; size++) {
		ASSERT(get_pool_id(size).id == 0);
		ASSERT(get_pool_id(size).item_size == 16);
	}
	for (int size = 17; size <= 32; size++) {
		ASSERT(get_pool_id(size).id == 1);
		ASSERT(get_pool_id(size).item_size == 32);
	}
	for (int size = 33; size <= 64; size++) {
		ASSERT(get_pool_id(size).id == 2);
		ASSERT(get_pool_id(size).item_size == 64);
	}
	for (int size = 65; size <= 128; size++) {
		ASSERT(get_pool_id(size).id == 3);
		ASSERT(get_pool_id(size).item_size == 128);
	}
	for (int size = 129; size <= 256; size++) {
		ASSERT(get_pool_id(size).id == 4);
		ASSERT(get_pool_id(size).item_size == 256);
	}
	for (int size = 257; size <= 512; size++) {
		ASSERT(get_pool_id(size).id == 5);
		ASSERT(get_pool_id(size).item_size == 512);
	}
}
