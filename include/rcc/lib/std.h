#pragma once
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef u32 string;

#define interface(a, interface, impl) alloc(a, sizeof(*interface) + sizeof(impl))
#define implementation(interface) ((void*)(&interface[1]))
