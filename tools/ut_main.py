#!/usr/bin/env python3
# create ut_main.c for ut
# usage: tools/ut_main.py <source-dir> [>ut_main.c]
import os
import sys
import re


CODE_TEMPLATE = '''\
#define UT_MAIN
#include <rcc/ut/ut.h>

{declarations}

{suites}

struct _test_suite _all_suites[] = {{
\t{suites_refs},
\t{{ NULL, NULL }}
}};
'''


def filetree(root):
    for (path, _, files) in os.walk(root):
        for file in files:
            yield f'{path}/{file}'


def list_test_cases(file):
    with open(file) as f:
        for line in f.readlines():
            if r := re.match('TEST\\((.*), (.*)\\) {', line):
                yield r.groups()


def generate(test_cases):
    suite_template = 'static struct _test_case suite_{suite}[] = {{\n\t{cases},\n\t{{ NULL, NULL }}\n}};'
    suites = ''
    declarations = []
    for suite, cases in test_cases.items():
        suites += suite_template.format(
            suite=suite,
            cases=',\n\t'.join(f'{{ "{case}", test_{suite}_{case} }}' for case in cases),
        )
        for case in cases:
            declarations += [f'extern void test_{suite}_{case}(struct _test_results*);']
    print(CODE_TEMPLATE.format(
        declarations='\n'.join(declarations),
        suites=suites,
        suites_refs=',\n\t'.join(f'{{ "{suite}", suite_{suite} }}' for suite in test_cases),
    ), end='')


test_files = [f for f in filetree(sys.argv[1]) if f[-5:] == '_ut.c']
test_cases = {}
for file in test_files:
    for suite, test in list_test_cases(file):
        if suite not in test_cases:
            test_cases[suite] = []
        test_cases[suite].append(test)

generate(test_cases)
