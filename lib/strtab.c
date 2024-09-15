#include <rcc/lib/alloc.h>
#include <rcc/lib/symtab.h>
#include <string.h>

struct string_vector {
	u32* data;
	size_t len;
};

static struct {
	arena a;
	struct string_buffer* buf;
	struct string_vector* buckets;
} strtab;

static u32 _hash(const char* s) {
	u32 r = 0;
	while (*s) {
		r = r * 97 + *s++;
	}
	return r;
}

static u32* _get_bucket(u32 hash, const char* s) {
	for (u32 i = 0; i < (strtab.buckets->len * 2 / 3); i++) {
		u32* id = &strtab.buckets->data[(hash + i) % strtab.buckets->len];
		if (*id == 0 || !strcmp(strtab.buf->data + *id, s)) {
			return id;
		}
	}
	return NULL;
}

static void _insert_string(u32* bucket, const char* s) {
	if (*bucket) {
		return;
	}
	*bucket = strtab.buf->len;
	while (*s) {
		append(strtab.buf, *s++);
	}
	append(strtab.buf, 0);
}

static void _rehash(void) {
	struct string_vector old_buckets = {strtab.buckets->data, strtab.buckets->len};
	strtab.buckets->data = NULL;
	strtab.buckets->len = (strtab.buckets->len ? (strtab.buckets->len - 1) * 2 : 16) + 1,
	strtab.buckets->data = extend_vector(strtab.buckets->data, strtab.buckets->len, sizeof(strtab.buckets->data[0]), true);
	memset(strtab.buckets->data, 0, strtab.buckets->len * sizeof(strtab.buckets->data[0]));
	for (u32* id = old_buckets.data; id < old_buckets.data + old_buckets.len; id++) {
		if (!*id) {
			continue;
		}
		u32* new_bucket;
		assert(new_bucket = _get_bucket(_hash(cstr(*id)), cstr(*id)));
		*new_bucket = *id;
	}
	rmvector(&old_buckets);
}

void init_strtab(arena a) {
	strtab.a = a;
	strtab.buckets = vector(a);
	strtab.buf = vector(a);
	append(strtab.buf, 0);
}

string str(const char* s) {
	u32 hash = _hash(s);
	u32* bucket;
	if ((bucket = _get_bucket(hash, s))) {
		_insert_string(bucket, s);
		return *bucket;
	}
	_rehash();
	assert(bucket = _get_bucket(hash, s));
	_insert_string(bucket, s);
	return *bucket;
}

const char* cstr(string s) {
	return strtab.buf->data + s;
}
