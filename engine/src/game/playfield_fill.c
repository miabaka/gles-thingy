#include "playfield.h"

#include "../utils/common.h"

typedef struct {
	size_t _topIndex;
	ivec2 _spans[128];
} SpanStack;

static void SpanStack_init(SpanStack *this) {
	this->_topIndex = 0;
}

static bool SpanStack_isEmpty(const SpanStack *this) {
	return this->_topIndex < 1;
}

static bool SpanStack_push(SpanStack *this, ivec2 span) {
	if (this->_topIndex >= ARRAY_SIZE(this->_spans))
		return false;

	this->_spans[this->_topIndex++] = span;

	return true;
}

static ivec2 SpanStack_pop(SpanStack *this) {
	return SpanStack_isEmpty(this) ? (ivec2) {0, 0} : this->_spans[--this->_topIndex];
}

static bool fill(Playfield *field, ivec2 span, Tile srcTile, Tile dstTile) {
	SpanStack stack;

	SpanStack_init(&stack);
	SpanStack_push(&stack, span);

	while (!SpanStack_isEmpty(&stack)) {
		span = SpanStack_pop(&stack);

		while (span.x >= 0 && Playfield_getTile(field, span) == srcTile)
			span.x--;

		span.x++;

		bool spanAbove = false;
		bool spanBelow = false;

		while (span.x < field->width && Playfield_getTile(field, span) == srcTile) {
			Playfield_setTile(field, span, dstTile);

			bool tileAboveMatches = (Playfield_getTile(field, (ivec2) {span.x, span.y - 1}) == srcTile);
			bool tileBelowMatches = (Playfield_getTile(field, (ivec2) {span.x, span.y + 1}) == srcTile);

			if (!spanAbove && span.y > 0 && tileAboveMatches) {
				if (!SpanStack_push(&stack, (ivec2) {span.x, span.y - 1}))
					return false;

				spanAbove = true;
			} else if (spanAbove && span.y > 0 && !tileAboveMatches) {
				spanAbove = false;
			}

			if (!spanBelow && span.y < field->height - 1 && tileBelowMatches) {
				if (!SpanStack_push(&stack, (ivec2) {span.x, span.y + 1}))
					return false;

				spanBelow = true;
			} else if (spanBelow && span.y < field->height - 1 && !tileBelowMatches) {
				spanBelow = false;
			}

			span.x++;
		}
	}

	return true;
}

bool Playfield_fill(Playfield *this, ivec2 pos, Tile tile) {
	Tile srcTile = Playfield_getTile(this, pos);
	return (tile == srcTile) || fill(this, pos, srcTile, tile);
}