#include <stdbool.h>
#include <genvector/genvector.h>
#include "playfield.h"

typedef struct {
    int x;
    int y;
} Span;

GVEC_INSTANTIATE(Span, span, GENA_USE_SAMPLE);

static void fill(Playfield *field, Span span, Tile srcTile, Tile dstTile) {
    bool spanAbove;
    bool spanBelow;

    gvec_span_h stack = gvec_span_new(32);

    if (!stack)
        return;

    gvec_span_push(&stack, span);

    while (!gvec_empty(stack)) {
        span = gvec_span_pop(stack);

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
                gvec_span_push(&stack, (Span) {span.x, span.y - 1});
                spanAbove = true;
            } else if (spanAbove && span.y > 0 && !tileAboveMatches) {
                spanAbove = false;
            }

            if (!spanBelow && span.y < field->height - 1 && tileBelowMatches) {
                gvec_span_push(&stack, (Span) {span.x, span.y + 1});
                spanBelow = true;
            } else if (spanBelow && span.y < field->height - 1 && !tileBelowMatches) {
                spanBelow = false;
            }

            span.x++;
        }
    }

    gvec_free(stack);
}

void Playfield_fill(Playfield *this, int x, int y, Tile tile) {
    Tile srcTile = Playfield_getTile(this, x, y);

    if (tile == srcTile)
        return;

    fill(this, (Span) {x, y}, srcTile, tile);
}