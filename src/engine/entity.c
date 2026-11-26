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

    if (!(item1->flags & CS_ENTITY_ACTIVE) && !(item2->flags & CS_ENTITY_ACTIVE)) return 0;
    if (!(item1->flags & CS_ENTITY_ACTIVE)) return 1;
    if (!(item2->flags & CS_ENTITY_ACTIVE)) return -1;

    return (item1->id - item2->id);
}

CS_EntityBase* CS_EntityGetFromID(CS_EntityContext* ctx, int id) {
    for (int i = 0; i < ctx->active_entities; i++) {
        if (ctx->entity_pool[i].id == id) {
            return &(ctx->entity_pool[i]);
    }

    return NULL;
}

int CS_entity_finalize_creation(
        CS_EntityBase* entity,
        SDL_Point position,
        int type,
        int draw_item_id,
        int flags,
        void (*ready) (struct CS_EntityBase*),
        void (*update)(struct CS_EntityBase*, float dt),
        CS_EntityContext* ctx
) {
    entity->type = type;
    entity->position = position;
    entity->flags |= (CS_ENTITY_ACTIVE | flags);
    entity->draw_item_id = draw_item_id;
    entity->ready = ready;
    entity->update = update;
    ctx->active_entities += 1;
    entity->ready(entity);

    int id = entity->id;
    qsort(ctx->entity_pool, ctx->entity_pool_size, sizeof(CS_EntityBase), CS_push_inactive);
    return id;
}

int CS_EntityCreate(
        CS_EntityContext* ctx,
        SDL_Point position,
        int type,
        int draw_item_id,
        int flags,
        void (*ready) (struct CS_EntityBase*),
        void (*update)(struct CS_EntityBase*, float dt)
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

    return CS_entity_finalize_creation(
            &(ctx->entity_pool[idx]),
            position, type,
            draw_item_id, flags,
            ready, update,
            ctx
    );
}

int CS_EntityCreateWithSprite(
        CS_EntityContext* entity_ctx,
        CS_DrawContext* draw_ctx,
        SDL_Texture* sprite,
        SDL_Point position,
        SDL_Point* size,

        int draw_item_flags,
        int entity_flags,
        int z_idx,

        int type,
        void (*ready) (struct CS_EntityBase*),
        void (*update)(struct CS_EntityBase*, float dt)
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

    int w, h;

    int draw_item_id = CS_DrawPoolInsertSprite(
            draw_ctx,
            sprite,
            position,
            size,
            z_idx,
            draw_item_flags
    );

    return CS_entity_finalize_creation(
            &(entity_ctx->entity_pool[idx]),
            position, type,
            draw_item_id, entity_flags,
            ready, update,
            entity_ctx
    );
}

#endif
