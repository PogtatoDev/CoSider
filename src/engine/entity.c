#ifndef __CS_ENTITY__
#define __CS_ENTITY__

#include "drawpool.c"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define CS_ENTITY_COLLIDING (1 << 1)
#define CS_ENTITY_NOCLIP    (1 << 2)
#define CS_ENTITY_ACTIVE    (1 << 3)

typedef struct CS_EntityBase {
    SDL_Point position;

    int flags;
    int draw_item_id;
    int type;
    int id;

    void (*ready) (struct CS_EntityBase*);
    void (*update)(struct CS_EntityBase*, float dt);
} CS_EntityBase;

typedef struct {
    CS_EntityBase* entity_pool;
    int entity_pool_size;
    int active_entities;
} CS_EntityContext;

int CS_push_inactive(const void* a, const void* b) {
    const CS_EntityBase* item1 = (CS_EntityBase*)a;
    const CS_EntityBase* item2 = (CS_EntityBase*)b;

    if (!(item1->flags & CS_ENTITY_ACTIVE) && !(item1->flags & CS_ENTITY_ACTIVE)) return 0;
    if (!(item1->flags & CS_ENTITY_ACTIVE)) return 1;
    if (!(item2->flags & CS_ENTITY_ACTIVE)) return -1;

    return (item1->id - item2->id);
}

int CS_PlaceEntity(
        CS_EntityContext* ctx,
        SDL_Point position,
        int type,
        int draw_item_id,
        int flags,
        void (*ready) (struct CS_EntityBase*),
        void (*update)(struct CS_EntityBase*)
) {
    int idx = -1;
    for (int i = 0; i < ctx->entity_pool_size; i++) {
        if (!(ctx->entity_pool[i].flags & CS_ENTITY_ACTIVE)) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("br.c\n");
        exit(1);
    }

    ctx->entity_pool[idx].type = type;
    ctx->entity_pool[idx].position = position;
    ctx->entity_pool[idx].flags |= CS_ENTITY_ACTIVE;
    ctx->entity_pool[idx].draw_item_id = draw_item_id;

    return ctx->entity_pool[idx].id;
}

int CS_PlaceEntityWithSprite(
        CS_EntityContext* entity_ctx,
        CS_DrawContext* draw_ctx,
        SDL_Texture* sprite,
        SDL_Point position,

        int draw_item_flags,
        int entity_flags,
        int z_idx,

        int type,
        void (*ready) (struct CS_EntityBase*),
        void (*update)(struct CS_EntityBase*)
) {
    int idx = -1;
    for (int i = 0; i < entity_ctx->entity_pool_size; i++) {
        if (!(entity_ctx->entity_pool[i].flags & CS_ENTITY_ACTIVE)) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("br.c");
        exit(1);
    }

    entity_ctx->entity_pool[idx].type = type;
    entity_ctx->entity_pool[idx].position = position;
    entity_ctx->entity_pool[idx].flags |= CS_ENTITY_ACTIVE;

    int w, h;
    SDL_QueryTexture(sprite, NULL, NULL, &w, &h);

    int id = CS_DrawPoolInsertSprite(
            draw_ctx,
            sprite,
            position,
            (SDL_Point) { w, h },
            z_idx,
            draw_item_flags
    );

    entity_ctx->entity_pool[idx].draw_item_id = id;
    id = entity_ctx->entity_pool[idx].id;

    qsort(entity_ctx->entity_pool, entity_ctx->entity_pool_size, sizeof(CS_EntityBase), CS_push_inactive);
    return entity_ctx->entity_pool[idx].id;
}

#endif
