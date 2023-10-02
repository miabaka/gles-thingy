#pragma once

#include <stdbool.h>

typedef struct {
	int x;
	int y;
} ivec2;

static inline ivec2 ivec2_add(ivec2 a, ivec2 b) {
	return (ivec2) {a.x + b.x, a.y + b.y};
}