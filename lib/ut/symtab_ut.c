#include <rcc/lib/table.h>
#include <rcc/ut/ut.h>

#include <string.h>

static const char* gen(int i) {
	if (!i) return "0";
	static char buf[100];
	memset(buf, 0, sizeof(buf));
	char* s = buf + sizeof(buf);
	while (i > 0) {
		assert(s > buf + 1);
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
	string save[10000];
	for (int i = 0; i < 10000; i++) {
		save[i] = str(gen(i));
		ASSERT(!strcmp(cstr(save[i]), gen(i)));
	}
	for (int i = 0; i < 10000; i++) {
		ASSERT(save[i] == str(gen(i)));
		ASSERT(!strcmp(cstr(save[i]), gen(i)));
		ASSERT(save[i] > (i ? save[i - 1] : 0));
	}
	rmarena(&a);
}

TEST(symtab, symtab_basic) {
	struct symtab s;
	struct arena a = {0};
	init_strtab(&a);
	init_symtab(&s, &a, sizeof(u32));
	open_scope(&s);
	u32* id = lookup_local(&s, str("hello"));
	u32* i2 = lookup(&s, str("hello"));
	ASSERT(id == i2 && id);
	u32* no = lookup_local(&s, str("no"));
	u32* n2 = lookup(&s, str("no"));
	u32* nf = lookup(&s, str("not-found"));
	ASSERT(no != id && n2 == no && no && !nf);
	rmarena(&a);

	init_strtab(&a);
	init_symtab(&s, &a, sizeof(u32));
	open_scope(&s);
	struct symbol* save[512];
	for (int i = 0; i < 248; i++) {
		save[i] = lookup_local(&s, str(gen(i)));
		ASSERT(save[i]);
	}
	for (int i = 0; i < 248; i++) {
		ASSERT(save[i] == lookup(&s, str(gen(i))));
	}

	rmarena(&a);
}

TEST(symtab, scopes) {
	struct symtab s;
	struct arena a = {0};
	init_strtab(&a);
	init_symtab(&s, &a, sizeof(u32));
	open_scope(&s);

	struct symbol* global = lookup_local(&s, str("s"));
	ASSERT(global);
	ASSERT(lookup(&s, str("s")) == global);
	ASSERT(lookup_local(&s, str("s")) == global);

	open_scope(&s);
	ASSERT(lookup(&s, str("s")) == global);
	struct symbol* local = lookup_local(&s, str("s"));
	ASSERT(local);
	ASSERT(local != global);
	ASSERT(lookup(&s, str("s")) == local);
	ASSERT(lookup_local(&s, str("s")) == local);

	close_scope(&s);
	ASSERT(lookup(&s, str("s")) == global);
	ASSERT(lookup_local(&s, str("s")) == global);
	open_scope(&s);
	ASSERT(lookup(&s, str("s")) == global);

	open_scope(&s);
	for (int i = 0; i < 512; i++) {
		lookup(&s, str(gen(i)));
		ASSERT(lookup(&s, str("s")) == global);
	}
	close_scope(&s);
	for (int i = 0; i < 512; i++) {
		lookup(&s, str(gen(i)));
		ASSERT(lookup(&s, str("s")) == global);
	}

	rmarena(&a);
}
