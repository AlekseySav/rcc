#include <rcc/ut/ut.h>
#include <rcc/lib/alloc.h>

#include <signal.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>

extern struct _test_suite _all_suites[];

static jmp_buf _jmpbuf;

static void _abort_handler(int signum) {
	longjmp(_jmpbuf, 1);
}

static bool _process_results(struct _test_meta* tm) {
	if (tm->fail) {
		fprintf(stderr, "\n\e[1m%s.%s: \e[31mfailed:\e[0m '%s' is false at %s:%u\n", tm->suite, tm->name, tm->expr, tm->file, tm->line);
		return false;
	}
	if (tm->abrt) {
		fprintf(stderr, "\e[1m%s.%s: \e[31mabort recieved\e[0m\n", tm->suite, tm->name);
		return false;
	}
	if (tm->leak) {
		fprintf(stderr, "\e[1m%s.%s: \e[31mmemory leak detected\e[0m\n", tm->suite, tm->name);
		return false;
	}
	return true;
}

static bool _run(struct _test_case* tc) {
	struct _test_meta tm = {0};
	struct sigaction s = { .sa_handler = _abort_handler };

	if (setjmp(_jmpbuf)) {
		tm.abrt = true;
		_cleanup();
		return _process_results(&tm);
	}

	sigaction(SIGABRT, &s, NULL);
	tm.suite = tc->suite;
	tm.name = tc->name;
	tc->test(&tm);
	tm.leak = _cleanup();
	return _process_results(&tm);
}

void _test_trace(struct _test_meta* tm, const char* file, int line, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	if (!tm->logs) {
		tm->logs = true;
		fprintf(stderr, "%s.%s: \n", tm->suite, tm->name);
	}
	fprintf(stderr, " >>>  ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
}

int main(int argc, char** argv) {
	int total_suites = 0, ok_suites = 0;
	for (struct _test_suite* suite = _all_suites; suite->name; suite++) {
		int total = 0, ok = 0;
		for (struct _test_case* test = suite->tests; test->name; test++) {
			if (total == 0) {
				fprintf(stderr, "test suite '%s' \n", suite->name);
			}
			ok += _run(test);
			total++;
		}
		if (total) {
			total_suites++;
			ok_suites += total == ok;
			fprintf(stderr, total != ok ? "\n" : "(%d tests) \e[32mok\e[0m\n", total);
		}
	}
	int bad_suites = total_suites - ok_suites;
	fprintf(
		stderr,
		"total suites: %d, %sfailed:\e[0m %d, %sok:\e[0m %d\n",
		total_suites,
		bad_suites ? "\e[31m" : "",
		bad_suites, ok_suites ? "\e[32m" : "",
		ok_suites
	);
	return 0;
}
