#pragma once
#include <stddef.h>

void* _alloc(void* old, size_t newsize); // allocated memory is zeroed
void _free(void* p);

bool _cleanup(void);
