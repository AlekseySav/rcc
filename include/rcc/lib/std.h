#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdnoreturn.h>

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef u32 string;

#undef assert
#ifdef DEBUG
noreturn void _assert_debug(const char* file, u32 line, const char* msg);
#define assert(e) ({ if (!(e)) _assert_debug(__FILE__, __LINE__, #e); })
#else
noreturn void _assert(void);
#define assert(e) ({ if (!(e)) _assert(); })
#endif
