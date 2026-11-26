#ifndef __CS_DRAWPOOL__
#define __CS_DRAWPOOL__

#include <stdio.h>
#include <SDL2/SDL.h>

#include "drawitem.c"

int CS_cmp_z_idx(const void* a, const void* b) {
    const CS_DrawItem* item1 = (CS_DrawItem*)a;
    const CS_DrawItem* item2 = (CS_DrawItem*)b;

    if (!(item1->flags & CS_DRAWITEM_ACTIVE) && !(item1->flags & CS_DRAWITEM_ACTIVE)) return 0;
    if (!(item1->flags & CS_DRAWITEM_ACTIVE)) return 1;
    if (!(item2->flags & CS_DRAWITEM_ACTIVE)) return -1;

    return (item1->z_idx - item2->z_idx);
}

void CS_draw_pool_finalize_insertion(
        CS_DrawContext* ctx,
        int z_idx,
        int idx,
        int flags
) {
    if (z_idx == -1) {
        ctx->draw_pool[idx].z_idx = idx;
    } else {
        ctx->draw_pool[idx].z_idx = z_idx;
    }

    ctx->draw_pool[idx].flags |= CS_DRAWITEM_ACTIVE;
    ctx->draw_pool[idx].flags |= flags;
    ctx->active_draw_items += 1;
    qsort(ctx->draw_pool, ctx->draw_pool_size, sizeof(CS_DrawItem), CS_cmp_z_idx);
}

int CS_DrawPoolInsertSprite(
        CS_DrawContext* ctx,
        SDL_Texture* sprite,
        SDL_Point position,
        SDL_Point* size,
        int z_idx,
        int flags
) {
    int idx = -1;
    for (int i = 0; i < ctx->draw_pool_size; i++) {
        if (!(ctx->draw_pool[i].flags & CS_DRAWITEM_ACTIVE)) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        fputs(".cat (seey.c version)", stderr);
        exit(-1);
    }

    // seeyuhis

    SDL_Rect dest_rect;

    if (size) {
        dest_rect.w = size->x;
        dest_rect.h = size->y;
    } else {
        SDL_QueryTexture(sprite, NULL, NULL, &(dest_rect.w), &(dest_rect.h));
    }

    // im back form santo domingo
    dest_rect.x = position.x;
    dest_rect.y = position.y;

    ctx->draw_pool[idx].type = CS_DRAWITEM_SPRITE;
    ctx->draw_pool[idx].Sprite.sprite = sprite;
    ctx->draw_pool[idx].Sprite.dest_rect = dest_rect;

    int id = ctx->draw_pool[idx].id;
    CS_DrawItem* temp_drawitem = &(ctx->draw_pool[idx]);

    CS_draw_pool_finalize_insertion(ctx, z_idx, idx, flags);

#ifdef DEBUG
    printf(
            "created sprite CS_DrawItem\n pos: %d %d\n size: %d %d\n texture: %p\n id: %d\n CS_DrawItem: %p\n",
            position.x, position.y,
            size.x, size.y,
            sprite,
            id, temp_drawitem
     );
#endif

    return id;
}

int CS_DrawPoolInsertRect(
        CS_DrawContext* ctx,
        SDL_Rect* rect,
        SDL_Color color,
        int z_idx,
        int flags
) {
    int idx = -1;
    for (int i = 0; i < ctx->draw_pool_size; i++) {
        if (!(ctx->draw_pool[i].flags & CS_DRAWITEM_ACTIVE)) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("hihihi\n");
        exit(-1);
    }

    ctx->draw_pool[idx].type = CS_DRAWITEM_RECT;
    ctx->draw_pool[idx].Rect.rect = *rect;
    ctx->draw_pool[idx].Rect.rect_color = color;

    int id = ctx->draw_pool[idx].id;
    CS_DrawItem* temp_drawitem = &(ctx->draw_pool[idx]);
    CS_draw_pool_finalize_insertion(ctx, z_idx, idx, flags);
#ifdef __DEBUG__
    printf(
            "created rect CS_DrawItem\n pos: %d %d\n size: %d %d\n rect: %p\n id: %d\n CS_DrawItem: %p\n",
            rect->x, rect->y,
            rect->w, rect->h,
            rect,
            id, temp_drawitem
     );
#endif
    return id;
}

static inline CS_DrawItem* CS_DrawPoolGetItemFromID(CS_DrawContext* ctx, int id) {
    for (int i = 0; i < ctx->draw_pool_size; i++) {
        if (ctx->draw_pool[i].id == id) {
            return &ctx->draw_pool[i];
        }
    }

    return NULL;
}

static inline void CS_DrawPoolClear(CS_DrawContext* ctx) {
    for (int i = 0; i < ctx->draw_pool_size; i++) {
        ctx->draw_pool[i].flags &= ~CS_DRAWITEM_ACTIVE;
    }

    ctx->active_draw_items = 0;
}

void CS_DrawPoolRemove(
        CS_DrawContext* ctx,
        int draw_item_id
) {
    for (int i = 0; i < ctx->draw_pool_size; i++) {
        if (ctx->draw_pool[i].id == draw_item_id) {
            ctx->draw_pool[i].flags &= ~CS_DRAWITEM_ACTIVE;
            break;
        }
    }

#ifdef __DEBUG__
    printf("deleted CS_DrawItem with id %d\n", draw_item_id);
#endif

    ctx->active_draw_items -= 1;
    qsort(ctx->draw_pool, ctx->draw_pool_size, sizeof(CS_DrawItem), CS_cmp_z_idx);
}

#endif
