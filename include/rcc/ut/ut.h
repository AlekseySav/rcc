#pragma once
#include <rcc/lib/std.h>

#define MAX_TEST_LOGS 10000

struct _test_results {
	bool fail, leak;
	const char* expr;
	const char* file;
	int line;
	char logs[MAX_TEST_LOGS];
};

void _test_trace(struct _test_results* tr, const char* fmt, ...);

#define TEST(test_suite, test_name) void test_##test_suite##_##test_name(struct _test_results* _test_results)

#define ASSERT(e) ({ \
	if (!(e)) { \
		_test_results->fail = true; \
		_test_results->expr = #e; \
		_test_results->file = __FILE__; \
		_test_results->line = __LINE__; \
		return; \
	} \
})

#define TRACE(fmt, ...) _test_trace(_test_results, fmt __VA_OPT__(,) __VA_ARGS__)


#ifdef UT_MAIN
/*
 * ut implementation
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
struct _test_case {
	const char* name;
	void (*test)(struct _test_results*);
};

struct _test_suite {
	const char* name;
	struct _test_case* tests;
};

extern struct _test_suite _all_suites[];

static int suite_total = 0, suite_errors = 0, errors = 0, total = 0;

static void print(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

static bool specified_test(const char* suite, const char* name, int argc, char** argv) {
	static char buf[512];

	if (argc == 1) {
		return true;
	}
	for (int i = 1; i < argc; i++) {
		snprintf(buf, 512, "%s.%s", suite, name);
		if (!strcmp(buf, argv[i])) {
			return true;
		}
	}
	return false;
}

static void run_test(const char* suite, const char* name, void (*test)(struct _test_results*)) {
	struct _test_results r = {0};
	suite_total++;
	test(&r);
#ifdef TRACK_ALLOCATIONS
	extern bool detect_memory_leaks();
	if (detect_memory_leaks()) {
		r.leak = true;
	}
#endif
	if (!r.fail && !r.leak) {
		return;
	}

	if (r.fail) {
		print("\n\e[1m%s.%s: \e[31mfailed:\e[0m '%s' is false at %s:%d", suite, name, r.expr, r.file, r.line);
	}
	if (r.leak) {
		print("\n\e[1m%s.%s: \e[31mdetected memory leak\e[0m", suite, name);
	}
	suite_errors++;
	if (r.logs[0]) {
		print("\ncaptured logs:\n%s", r.logs);
	}
}

void _test_trace(struct _test_results* tr, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	strcat(tr->logs, " >>> ");
	vsnprintf(tr->logs + strlen(tr->logs), MAX_TEST_LOGS - strlen(tr->logs), fmt, ap);
	strcat(tr->logs, "\n");
	va_end(ap);
}

int main(int argc, char** argv) {
	for (struct _test_suite* suite = _all_suites; suite->name; suite++) {
		suite_total = suite_errors = 0;
		for (struct _test_case* test = suite->tests; test->name; test++) {
			if (!specified_test(suite->name, test->name, argc, argv)) {
				continue;
			}
			if (suite_total == 0) {
				print("test suite '%s' ", suite->name);
			}
			run_test(suite->name, test->name, test->test);
		}
		if (suite_total) {
			total++;
			print(suite_errors ? "\n" : "(%d tests) \e[32mok\e[0m\n", suite_total);
			errors += suite_errors != 0;
		}
	}
	int ok = total - errors;
	print("total suites: %d, %sfailed:\e[0m %d, %sok:\e[0m %d\n", total, errors ? "\e[31m" : "", errors, ok ? "\e[32m" : "", ok);
	return errors != 0;
}
#endif
