#include <rcc/lib/alloc.h>
#include <rcc/lib/symtab.h>
#include <rcc/ut/ut.h>
#include <stdio.h>
#include <string.h>

const char* gen(int i) {
	if (!i) return "0";
	static char buf[10];
	char* s = buf + 10;
	while (i > 0) {
		assert(s > buf);
		*--s = (i & 1) + '0';
		i >>= 1;
	}
	return s;
}

TEST(symtab, strtab_basic) {
	struct arena a = {0};
	init_strtab(&a);
	string id = str("hello");
	string i2 = str("hello");
	ASSERT(id == 1 && i2 == 1);
	string no = str("no");
	string n2 = str("no");
	ASSERT(no != id && n2 == no);
	rmarena(&a);

	init_strtab(&a);
	string save[512];
	for (int i = 0; i < 512; i++) {
		save[i] = str(gen(i));
		ASSERT(!strcmp(cstr(save[i]), gen(i)));
	}
	for (int i = 0; i < 512; i++) {
		ASSERT(save[i] == str(gen(i)));
		ASSERT(!strcmp(cstr(save[i]), gen(i)));
		ASSERT(save[i] > (i ? save[i - 1] : 0));
	}
	rmarena(&a);
}

TEST(symtab, symtab_basic) {
	struct arena a = {0};
	init_strtab(&a);
	init_symtab(&a);
	open_scope();
	struct symbol* id = lookup(str("hello"));
	struct symbol* i2 = lookup(str("hello"));
	ASSERT(id == i2);
	ASSERT(id->name == str("hello"));
	struct symbol* no = lookup(str("no"));
	struct symbol* n2 = lookup(str("no"));
	ASSERT(no != id && n2 == no);
	ASSERT(no->name == str("no"));
	rmarena(&a);

	init_strtab(&a);
	init_symtab(&a);
	open_scope();
	struct symbol* save[512];
	for (int i = 0; i < 512; i++) {
		save[i] = lookup(str(gen(i)));
		ASSERT(save[i]->name == str(gen(i)));
		ASSERT(!strcmp(cstr(save[i]->name), gen(i)));
	}
	for (int i = 0; i < 512; i++) {
		ASSERT(save[i] == lookup(str(gen(i))));
		ASSERT(save[i]->name == str(gen(i)));
		ASSERT(!strcmp(cstr(save[i]->name), gen(i)));
	}

	rmarena(&a);
}

TEST(symtab, scopes) {
	struct arena a = {0};
	init_strtab(&a);
	init_symtab(&a);
	open_scope();

	struct symbol* global = lookup(str("s"));
	ASSERT(global);
	ASSERT(lookup(str("s")) == global);
	ASSERT(lookup_local(str("s")) == global);

	open_scope();
	ASSERT(lookup(str("s")) == global);
	struct symbol* local = lookup_local(str("s"));
	ASSERT(local);
	ASSERT(local != global);
	ASSERT(lookup(str("s")) == local);
	ASSERT(lookup_local(str("s")) == local);

	close_scope();
	ASSERT(lookup(str("s")) == global);
	ASSERT(lookup_local(str("s")) == global);
	open_scope();
	ASSERT(lookup(str("s")) == global);

	open_scope();
	for (int i = 0; i < 512; i++) {
		lookup(str(gen(i)));
		ASSERT(lookup(str("s")) == global);
	}
	close_scope();
	for (int i = 0; i < 512; i++) {
		lookup(str(gen(i)));
		ASSERT(lookup(str("s")) == global);
	}

	rmarena(&a);
}
