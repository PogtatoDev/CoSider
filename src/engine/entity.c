#ifndef __CS_ENTITY__
#define __CS_ENTITY__

#include "drawitem.c"
#include <SDL2/SDL.h>

#define CS_ENTITY_COLLIDING (1 << 1)
#define CS_ENTITY_NOCLIP    (1 << 2)
#define CS_ENTITY_ACTIVE    (1 << 3)

#include "drawpool.c"

typedef struct CS_Game CS_Game;

typedef struct CS_EntityBase {
    SDL_FPoint position;
    SDL_FPoint sizeNOTINBYTES;

    int flags;
    int draw_item_id;
    int id;

    void (*ready) (struct CS_EntityBase*, CS_Game*);
    void (*update)(struct CS_EntityBase*, CS_Game*, float);
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
    }

    return NULL;
}

int CS_entity_finalize_creation(
        CS_EntityBase* entity,
        SDL_FPoint position,
        SDL_FPoint size,
        int draw_item_id,
        int flags,
        void (*ready) (struct CS_EntityBase*, CS_Game* game),
        void (*update)(struct CS_EntityBase*, CS_Game*, float),
        CS_EntityContext* ctx,
        CS_Game* game
) {
    entity->position = position;
    entity->sizeNOTINBYTES = size;
    entity->flags |= (CS_ENTITY_ACTIVE | flags);
    entity->draw_item_id = draw_item_id;
    entity->ready = ready;
    entity->update = update;
    ctx->active_entities += 1;
    entity->ready(entity, game);

    int id = entity->id;
    qsort(ctx->entity_pool, ctx->entity_pool_size, sizeof(CS_EntityBase), CS_push_inactive);
    return id;
}

int CS_EntityCreate(
        CS_Game* game,
        CS_EntityContext* entity_ctx,
        CS_DrawContext* draw_ctx,
        SDL_Texture* sprite,
        SDL_FPoint position,
        SDL_FPoint* size,

        int draw_item_flags,
        int entity_flags,
        int z_idx,

        void (*ready) (struct CS_EntityBase*, CS_Game*),
        void (*update)(struct CS_EntityBase*, CS_Game*, float)
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


    int draw_item_id = CS_DrawPoolInsertSprite(
            draw_ctx,
            sprite,
            position,
            size,
            z_idx,
            draw_item_flags
    );

    SDL_FRect* rect = &CS_DrawPoolGetItemFromID(draw_ctx, draw_item_id)->rect;
    return CS_entity_finalize_creation(
            &(entity_ctx->entity_pool[idx]),
            position, (SDL_FPoint) { rect->w, rect->h },
            draw_item_id, entity_flags,
            ready, update,
            entity_ctx, game
    );

}


#endif
