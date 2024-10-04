#!/usr/bin/env python3
# usage: tools/main_ut.py <root> <output>
import sys
import re

import lib.make as make


CODE_TEMPLATE = '''\
#include <rcc/ut/ut.h>

{declarations}

{suites}

struct _test_suite _all_suites[] = {{
\t{suites_refs},
\t{{ NULL, NULL }}
}};
'''


def list_test_cases(file):
	with open(file) as f:
		for line in f.readlines():
			if r := re.match('^TEST\\((.*), (.*)\\)', line):
				yield r.groups()


def generate(test_cases, f):
	suite_template = 'static struct _test_case suite_{suite}[] = {{\n\t{cases},\n\t{{ NULL, NULL }}\n}};'
	suites = ''
	declarations = []
	for suite, cases in test_cases.items():
		suites += suite_template.format(
			suite=suite,
			cases=',\n\t'.join(f'{{ "{suite}", "{case}", test_{suite}_{case} }}' for case in cases),
		)
		for case in cases:
			declarations += [f'void test_{suite}_{case}(struct _test_meta*);']
	print(CODE_TEMPLATE.format(
		declarations='\n'.join(declarations),
		suites=suites,
		suites_refs=',\n\t'.join(f'{{ "{suite}", suite_{suite} }}' for suite in test_cases),
	), end='', file=f)


def main(argv):
	test_cases = {}
	root, output = argv[1], argv[2]
	config = make.config('rcc.ut', root=root)
	for file in config.sourcefiles():
		for suite, test in list_test_cases(file):
			if suite not in test_cases:
				test_cases[suite] = []
			test_cases[suite].append(test)
	with open(output, 'wt') as f:
		generate(test_cases, f)


if __name__ == '__main__':
	main(sys.argv)
