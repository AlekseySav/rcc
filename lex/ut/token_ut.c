#include <rcc/lib/arena.h>
#include <rcc/lib/table.h>
#include <rcc/lex/token.h>
#include <rcc/ut/ut.h>

#include <stdio.h>
#include <string.h>

static const char _string_parsed[] =
	"\000\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017"
	"\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037"
	"\040\041\042\043\044\045\046\047\050\051\052\053\054\055\056\057"
	"\060\061\062\063\064\065\066\067\070\071\072\073\074\075\076\077"
	"\100\101\102\103\104\105\106\107\110\111\112\113\114\115\116\117"
	"\120\121\122\123\124\125\126\127\130\131\132\133\134\135\136\137"
	"\140\141\142\143\144\145\146\147\150\151\152\153\154\155\156\157"
	"\160\161\162\163\164\165\166\167\170\171\172\173\174\175\176\177"
	"\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217"
	"\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237"
	"\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257"
	"\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277"
	"\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317"
	"\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337"
	"\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357"
	"\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377";

static const char* _string_raw =
	"\""
	"\\0\\1\\2\\3\\4\\5\\6\\7\\10\\11\\12\\13\\14\\15\\16\\17"
	"\\20\\21\\22\\23\\24\\25\\26\\27\\30\\31\\32\\33\\34\\35\\36\\37"
	" !\\\"#$%&\\\'()*+,-./0123456789:;<=>?"
	"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\\\]^_"
	"`abcdefghijklmnopqrstuvwxyz{|}~\\177"
	"\\200\\201\\202\\203\\204\\205\\206\\207\\210\\211\\212\\213\\214\\215\\216\\217"
	"\\220\\221\\222\\223\\224\\225\\226\\227\\230\\231\\232\\233\\234\\235\\236\\237"
	"\\240\\241\\242\\243\\244\\245\\246\\247\\250\\251\\252\\253\\254\\255\\256\\257"
	"\\260\\261\\262\\263\\264\\265\\266\\267\\270\\271\\272\\273\\274\\275\\276\\277"
	"\\300\\301\\302\\303\\304\\305\\306\\307\\310\\311\\312\\313\\314\\315\\316\\317"
	"\\320\\321\\322\\323\\324\\325\\326\\327\\330\\331\\332\\333\\334\\335\\336\\337"
	"\\340\\341\\342\\343\\344\\345\\346\\347\\350\\351\\352\\353\\354\\355\\356\\357"
	"\\360\\361\\362\\363\\364\\365\\366\\367\\370\\371\\372\\373\\374\\375\\376\\377"
	"\"";

static const char* _chars_raw[] = {
	"'\\0'", "'\\1'", "'\\2'", "'\\3'", "'\\4'", "'\\5'", "'\\6'", "'\\7'", "'\\10'", "'\\11'", "'\\12'", "'\\13'", "'\\14'", "'\\15'", "'\\16'", "'\\17'",
	"'\\20'", "'\\21'", "'\\22'", "'\\23'", "'\\24'", "'\\25'", "'\\26'", "'\\27'", "'\\30'", "'\\31'", "'\\32'", "'\\33'", "'\\34'", "'\\35'", "'\\36'", "'\\37'",
	"' '", "'!'", "'\\\"'",
	"'#'", "'$'", "'%'", "'&'",
	"'\\\''",
	"'('", "')'", "'*'", "'+'", "','", "'-'", "'.'", "'/'", "'0'", "'1'", "'2'", "'3'", "'4'", "'5'", "'6'", "'7'", "'8'", "'9'", "':'", "';'", "'<'", "'='", "'>'", "'?'",
	"'@'", "'A'", "'B'", "'C'", "'D'", "'E'", "'F'", "'G'", "'H'", "'I'", "'J'", "'K'", "'L'", "'M'", "'N'", "'O'", "'P'", "'Q'", "'R'", "'S'", "'T'", "'U'", "'V'", "'W'", "'X'", "'Y'", "'Z'", "'['",
	"'\\\\'",
	"']'", "'^'", "'_'",
	"'`'", "'a'", "'b'", "'c'", "'d'", "'e'", "'f'", "'g'", "'h'", "'i'", "'j'", "'k'", "'l'", "'m'", "'n'", "'o'", "'p'", "'q'", "'r'", "'s'", "'t'", "'u'", "'v'", "'w'", "'x'", "'y'", "'z'", "'{'", "'|'", "'}'", "'~'",
	"'\\177'",
	"'\\200'", "'\\201'", "'\\202'", "'\\203'", "'\\204'", "'\\205'", "'\\206'", "'\\207'", "'\\210'", "'\\211'", "'\\212'", "'\\213'", "'\\214'", "'\\215'", "'\\216'", "'\\217'",
	"'\\220'", "'\\221'", "'\\222'", "'\\223'", "'\\224'", "'\\225'", "'\\226'", "'\\227'", "'\\230'", "'\\231'", "'\\232'", "'\\233'", "'\\234'", "'\\235'", "'\\236'", "'\\237'",
	"'\\240'", "'\\241'", "'\\242'", "'\\243'", "'\\244'", "'\\245'", "'\\246'", "'\\247'", "'\\250'", "'\\251'", "'\\252'", "'\\253'", "'\\254'", "'\\255'", "'\\256'", "'\\257'",
	"'\\260'", "'\\261'", "'\\262'", "'\\263'", "'\\264'", "'\\265'", "'\\266'", "'\\267'", "'\\270'", "'\\271'", "'\\272'", "'\\273'", "'\\274'", "'\\275'", "'\\276'", "'\\277'",
	"'\\300'", "'\\301'", "'\\302'", "'\\303'", "'\\304'", "'\\305'", "'\\306'", "'\\307'", "'\\310'", "'\\311'", "'\\312'", "'\\313'", "'\\314'", "'\\315'", "'\\316'", "'\\317'",
	"'\\320'", "'\\321'", "'\\322'", "'\\323'", "'\\324'", "'\\325'", "'\\326'", "'\\327'", "'\\330'", "'\\331'", "'\\332'", "'\\333'", "'\\334'", "'\\335'", "'\\336'", "'\\337'",
	"'\\340'", "'\\341'", "'\\342'", "'\\343'", "'\\344'", "'\\345'", "'\\346'", "'\\347'", "'\\350'", "'\\351'", "'\\352'", "'\\353'", "'\\354'", "'\\355'", "'\\356'", "'\\357'",
	"'\\360'", "'\\361'", "'\\362'", "'\\363'", "'\\364'", "'\\365'", "'\\366'", "'\\367'", "'\\370'", "'\\371'", "'\\372'", "'\\373'", "'\\374'", "'\\375'", "'\\376'", "'\\377'",
};

TEST(lex, strtoken) {
	struct string_buffer b;
	// simple ones
	rmvector(&b); strtoken((struct token){.token = L_semicolon}, &b); append(&b, 0); ASSERT(!strcmp(";", b.data));
	rmvector(&b); strtoken((struct token){.token = L_comma}, &b); append(&b, 0); ASSERT(!strcmp(",", b.data));
	rmvector(&b); strtoken((struct token){.token = L_open_paren}, &b); append(&b, 0); ASSERT(!strcmp("(", b.data));
	rmvector(&b); strtoken((struct token){.token = L_close_paren}, &b); append(&b, 0); ASSERT(!strcmp(")", b.data));
	rmvector(&b); strtoken((struct token){.token = L_open_bracket}, &b); append(&b, 0); ASSERT(!strcmp("[", b.data));
	rmvector(&b); strtoken((struct token){.token = L_close_bracket}, &b); append(&b, 0); ASSERT(!strcmp("]", b.data));
	rmvector(&b); strtoken((struct token){.token = L_not}, &b); append(&b, 0); ASSERT(!strcmp("!", b.data));
	rmvector(&b); strtoken((struct token){.token = L_inv}, &b); append(&b, 0); ASSERT(!strcmp("~", b.data));
	rmvector(&b); strtoken((struct token){.token = L_mul}, &b); append(&b, 0); ASSERT(!strcmp("*", b.data));
	rmvector(&b); strtoken((struct token){.token = L_div}, &b); append(&b, 0); ASSERT(!strcmp("/", b.data));
	rmvector(&b); strtoken((struct token){.token = L_mod}, &b); append(&b, 0); ASSERT(!strcmp("%", b.data));
	rmvector(&b); strtoken((struct token){.token = L_add}, &b); append(&b, 0); ASSERT(!strcmp("+", b.data));
	rmvector(&b); strtoken((struct token){.token = L_sub}, &b); append(&b, 0); ASSERT(!strcmp("-", b.data));
	rmvector(&b); strtoken((struct token){.token = L_shl}, &b); append(&b, 0); ASSERT(!strcmp("<<", b.data));
	rmvector(&b); strtoken((struct token){.token = L_shr}, &b); append(&b, 0); ASSERT(!strcmp(">>", b.data));
	rmvector(&b); strtoken((struct token){.token = L_lt}, &b); append(&b, 0); ASSERT(!strcmp("<", b.data));
	rmvector(&b); strtoken((struct token){.token = L_leq}, &b); append(&b, 0); ASSERT(!strcmp("<=", b.data));
	rmvector(&b); strtoken((struct token){.token = L_gt}, &b); append(&b, 0); ASSERT(!strcmp(">", b.data));
	rmvector(&b); strtoken((struct token){.token = L_geq}, &b); append(&b, 0); ASSERT(!strcmp("<=", b.data));
	rmvector(&b); strtoken((struct token){.token = L_equ}, &b); append(&b, 0); ASSERT(!strcmp("==", b.data));
	rmvector(&b); strtoken((struct token){.token = L_neq}, &b); append(&b, 0); ASSERT(!strcmp("!=", b.data));
	rmvector(&b); strtoken((struct token){.token = L_and}, &b); append(&b, 0); ASSERT(!strcmp("&", b.data));
	rmvector(&b); strtoken((struct token){.token = L_xor}, &b); append(&b, 0); ASSERT(!strcmp("^", b.data));
	rmvector(&b); strtoken((struct token){.token = L_or}, &b); append(&b, 0); ASSERT(!strcmp("|", b.data));
	rmvector(&b); strtoken((struct token){.token = L_andl}, &b); append(&b, 0); ASSERT(!strcmp("&&", b.data));
	rmvector(&b); strtoken((struct token){.token = L_orl}, &b); append(&b, 0); ASSERT(!strcmp("||", b.data));
	rmvector(&b); strtoken((struct token){.token = L_assign}, &b); append(&b, 0); ASSERT(!strcmp("=", b.data));
	rmvector(&b); strtoken((struct token){.token = L_iadd}, &b); append(&b, 0); ASSERT(!strcmp("+=", b.data));
	rmvector(&b); strtoken((struct token){.token = L_isub}, &b); append(&b, 0); ASSERT(!strcmp("-=", b.data));
	rmvector(&b); strtoken((struct token){.token = L_imul}, &b); append(&b, 0); ASSERT(!strcmp("*=", b.data));
	rmvector(&b); strtoken((struct token){.token = L_idiv}, &b); append(&b, 0); ASSERT(!strcmp("/=", b.data));
	rmvector(&b); strtoken((struct token){.token = L_imod}, &b); append(&b, 0); ASSERT(!strcmp("%=", b.data));
	rmvector(&b); strtoken((struct token){.token = L_ishl}, &b); append(&b, 0); ASSERT(!strcmp("<<=", b.data));
	rmvector(&b); strtoken((struct token){.token = L_ishr}, &b); append(&b, 0); ASSERT(!strcmp(">>=", b.data));
	rmvector(&b); strtoken((struct token){.token = L_iand}, &b); append(&b, 0); ASSERT(!strcmp("&=", b.data));
	rmvector(&b); strtoken((struct token){.token = L_ixor}, &b); append(&b, 0); ASSERT(!strcmp("^=", b.data));
	rmvector(&b); strtoken((struct token){.token = L_ior}, &b); append(&b, 0); ASSERT(!strcmp("|=", b.data));
	rmvector(&b); strtoken((struct token){.token = L_dot}, &b); append(&b, 0); ASSERT(!strcmp(".", b.data));
	rmvector(&b); strtoken((struct token){.token = L_arrow}, &b); append(&b, 0); ASSERT(!strcmp("->", b.data));
	rmvector(&b); strtoken((struct token){.token = L_question}, &b); append(&b, 0); ASSERT(!strcmp("?", b.data));
	rmvector(&b); strtoken((struct token){.token = L_colon}, &b); append(&b, 0); ASSERT(!strcmp(":", b.data));
	rmvector(&b); strtoken((struct token){.token = L_inc}, &b); append(&b, 0); ASSERT(!strcmp("++", b.data));
	rmvector(&b); strtoken((struct token){.token = L_dec}, &b); append(&b, 0); ASSERT(!strcmp("--", b.data));
	rmvector(&b); strtoken((struct token){.token = L_open_brace}, &b); append(&b, 0); ASSERT(!strcmp("{", b.data));
	rmvector(&b); strtoken((struct token){.token = L_close_brace}, &b); append(&b, 0); ASSERT(!strcmp("}", b.data));

	// numbers
	char buf[100];
	for (u32 i = 0; i < 100000; i++) {
		snprintf(buf, 100, "%u", i);
		rmvector(&b); strtoken((struct token){.token = L_num, .as_number = i, ._nbase = 10}, &b); append(&b, 0); ASSERT(!strcmp(buf, b.data));
		snprintf(buf, 100, "0%o", i);
		rmvector(&b); strtoken((struct token){.token = L_num, .as_number = i, ._nbase = 8}, &b); append(&b, 0); ASSERT(!strcmp(buf, b.data));
		snprintf(buf, 100, "0x%x", i);
		rmvector(&b); strtoken((struct token){.token = L_num, .as_number = i, ._nbase = 16}, &b); append(&b, 0); ASSERT(!strcmp(buf, b.data));
	}

	// chars
	rmvector(&b);
	struct string_buffer b2 = {0};
	append(&b, '"');
	for (u32 i = 0; i < sizeof(_string_parsed) - 1; i++) {
		rmvector(&b2);
		strtoken((struct token){.token = L_num, .as_number = (u8)_string_parsed[i]}, &b2);
		ASSERT(b2.len > 2 && b2.data[0] == '\'' && b2.data[b2.len - 1] == '\'');
		for (const char* p = b2.data + 1; p < b2.data + b2.len - 1; p++) {
			append(&b, *p);
		}
	}
	append(&b, '"');
	append(&b, 0);
	ASSERT(!strcmp(_string_raw, b.data));

	// strings
	rmvector(&b2);
	for (const char* s = _string_parsed; s < _string_parsed + sizeof(_string_parsed) - 1; s++) {
		append(&b2, *s);
	}
	rmvector(&b); strtoken((struct token){.token = L_str, .as_string = &b2}, &b); append(&b, 0); append(&b2, 0); ASSERT(!strcmp(_string_raw, b.data));

	// ident
	struct arena a = {0};
	init_strtab(&a);
	rmvector(&b2);
	append(&b2, 'a');
	append(&b2, 'b');
	append(&b2, 0);
	rmvector(&b); strtoken((struct token){.token = L_sym, .as_symbol = str(b2.data)}, &b); append(&b, 0); ASSERT(!strcmp(b2.data, b.data));
	rmvector(&b);
	rmvector(&b2);
	rmarena(&a);
}

static struct string_buffer* _load(struct string_buffer* b, const char* p) {
	rmvector(b);
	while (*p) {
		append(b, *p++);
	}
	return b;
}

TEST(lex, mktoken) {
	struct arena a = {0};
	struct string_buffer* b = vector(&a);
	struct token t;

	ASSERT(mktoken(&a, (struct location){}, _load(b, "")).token == 0);
	ASSERT(mktoken(&a, (struct location){}, _load(b, ";")).token == L_semicolon);
	ASSERT(mktoken(&a, (struct location){}, _load(b, ",")).token == L_comma);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "(")).token == L_open_paren);
	ASSERT(mktoken(&a, (struct location){}, _load(b, ")")).token == L_close_paren);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "[")).token == L_open_bracket);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "]")).token == L_close_bracket);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "!")).token == L_not);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "~")).token == L_inv);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "*")).token == L_mul);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "/")).token == L_div);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "%")).token == L_mod);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "+")).token == L_add);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "-")).token == L_sub);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "<<")).token == L_shl);
	ASSERT(mktoken(&a, (struct location){}, _load(b, ">>")).token == L_shr);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "<")).token == L_lt);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "<=")).token == L_leq);
	ASSERT(mktoken(&a, (struct location){}, _load(b, ">")).token == L_gt);
	ASSERT(mktoken(&a, (struct location){}, _load(b, ">=")).token == L_geq);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "==")).token == L_equ);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "!=")).token == L_neq);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "&")).token == L_and);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "^")).token == L_xor);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "|")).token == L_or);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "&&")).token == L_andl);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "||")).token == L_orl);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "=")).token == L_assign);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "+=")).token == L_iadd);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "-=")).token == L_isub);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "*=")).token == L_imul);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "/=")).token == L_idiv);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "%=")).token == L_imod);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "<<=")).token == L_ishl);
	ASSERT(mktoken(&a, (struct location){}, _load(b, ">>=")).token == L_ishr);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "&=")).token == L_iand);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "^=")).token == L_ixor);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "|=")).token == L_ior);
	ASSERT(mktoken(&a, (struct location){}, _load(b, ".")).token == L_dot);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "->")).token == L_arrow);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "?")).token == L_question);
	ASSERT(mktoken(&a, (struct location){}, _load(b, ":")).token == L_colon);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "++")).token == L_inc);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "--")).token == L_dec);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "{")).token == L_open_brace);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "}")).token == L_close_brace);
	ASSERT(mktoken(&a, (struct location){}, _load(b, " ")).token == L_space);

	// numbers
	char buf[100];
	for (u32 i = 0; i < 100000; i++) {
		snprintf(buf, 100, "0%o", i);
		ASSERT((t = mktoken(&a, (struct location){}, _load(b, buf))).token == L_num && t._nbase == 8 && t.as_number == i);
		snprintf(buf, 100, "%u", i);
		ASSERT((t = mktoken(&a, (struct location){}, _load(b, buf))).token == L_num && t._nbase == 10 && t.as_number == i);
		snprintf(buf, 100, "0x%x", i);
		ASSERT((t = mktoken(&a, (struct location){}, _load(b, buf))).token == L_num && t._nbase == 16 && t.as_number == i);
		snprintf(buf, 100, "0X%X", i);
		ASSERT((t = mktoken(&a, (struct location){}, _load(b, buf))).token == L_num && t._nbase == 16 && t.as_number == i);
	}

	// chars
	for (size_t i = 0; i < sizeof(_chars_raw) / sizeof(*_chars_raw); i++) {
		ASSERT((t = mktoken(&a, (struct location){}, _load(b, _chars_raw[i]))).token == L_num && t._nbase == 0 && t.as_number == _string_parsed[i]);
	}

	// strings
	t = mktoken(&a, (struct location){}, _load(b, _string_raw));
	ASSERT(t.token == L_str);
	ASSERT(!memcmp(t.as_string->data, _string_parsed, sizeof(_string_parsed) - 1));

	// ident
	init_strtab(&a);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "hello")).token == L_sym);
	ASSERT(mktoken(&a, (struct location){}, _load(b, "hello")).as_symbol == str("hello"));

	rmarena(&a);
}
