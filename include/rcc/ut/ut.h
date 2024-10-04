#pragma once
#include <rcc/lib/std.h>

struct _test_meta {
	const char* suite;
	const char* name;
	const char* expr;
	const char* file;
	u32 line;
	bool logs;
	bool fail;
	bool leak;
	bool abrt;
};

struct _test_case {
	const char* suite;
	const char* name;
	void (*test)(struct _test_meta*);
};

struct _test_suite {
	const char* name;
	struct _test_case* tests;
};

void _test_trace(struct _test_meta* tm, const char* file, int line, const char* fmt, ...);

#define TEST(suite, test) void test_##suite##_##test(struct _test_meta* _test_meta)

#define TRACE(fmt, ...) _test_trace(_test_meta, __FILE__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)

#define ASSERT(e) ({ \
	if (!(e)) { \
		_test_meta->fail = true; \
		_test_meta->expr = #e; \
		_test_meta->file = __FILE__; \
		_test_meta->line = __LINE__; \
		return; \
	} \
})
