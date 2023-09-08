#include "playfield.h"

#include "../utils/array.h"

typedef struct {
    int x;
    int y;
} Span;

typedef struct {
    size_t _topIndex;
    Span _spans[128];
} SpanStack;

static void SpanStack_init(SpanStack *this) {
    this->_topIndex = 0;
}

static bool SpanStack_isEmpty(const SpanStack *this) {
    return this->_topIndex < 1;
}

static bool SpanStack_push(SpanStack *this, Span span) {
    if (this->_topIndex >= ARRAY_SIZE(this->_spans))
        return false;

    this->_spans[this->_topIndex++] = span;

    return true;
}

static Span SpanStack_pop(SpanStack *this) {
    return SpanStack_isEmpty(this) ? (Span) {} : this->_spans[--this->_topIndex];
}

static bool fill(Playfield *field, Span span, Tile srcTile, Tile dstTile) {
    bool spanAbove;
    bool spanBelow;

    SpanStack stack;

    SpanStack_init(&stack);
    SpanStack_push(&stack, span);

    while (!SpanStack_isEmpty(&stack)) {
        span = SpanStack_pop(&stack);

        while (span.x >= 0 && Playfield_getTile(field, span.x, span.y) == srcTile)
            span.x--;

        span.x++;

        spanAbove = false;
        spanBelow = false;

        while (span.x < field->width && Playfield_getTile(field, span.x, span.y) == srcTile) {
            Playfield_setTile(field, span.x, span.y, dstTile);

            bool tileAboveMatches = (Playfield_getTile(field, span.x, span.y - 1) == srcTile);
            bool tileBelowMatches = (Playfield_getTile(field, span.x, span.y + 1) == srcTile);

            if (!spanAbove && span.y > 0 && tileAboveMatches) {
                if (!SpanStack_push(&stack, (Span) {span.x, span.y - 1}))
                    return false;

                spanAbove = true;
            } else if (spanAbove && span.y > 0 && !tileAboveMatches) {
                spanAbove = false;
            }

            if (!spanBelow && span.y < field->height - 1 && tileBelowMatches) {
                if (!SpanStack_push(&stack, (Span) {span.x, span.y + 1}))
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

bool Playfield_fill(Playfield *this, int x, int y, Tile tile) {
    Tile srcTile = Playfield_getTile(this, x, y);

    if (tile == srcTile)
        return false;

    return fill(this, (Span) {x, y}, srcTile, tile);
}