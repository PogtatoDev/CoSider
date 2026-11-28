#ifndef __CS_ENGINE__
#define __CS_ENGINE__

#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <stdlib.h>

#include "drawpool.c"
#include "windowctx.c"
#include "drawitem.c"
#include "entity.c"
#include "utils.c"
#include "game.c"

void CS_PollEvents(CS_Game* game) {
    memcpy(game->keys_notnow, game->keys_now, SDL_NUM_SCANCODES);

    while (SDL_PollEvent(&game->window_ctx.event)) {
        if (game->window_ctx.event.type == SDL_QUIT) {
            game->is_open = 0;
        }
    }

    const uint8_t* texas = SDL_GetKeyboardState(NULL);
    memcpy(game->keys_now, texas, SDL_NUM_SCANCODES);

    SDL_GetMouseState(&(game->window_ctx.mouse_pos.x), &(game->window_ctx.mouse_pos.y));
}

void CS_Update(CS_Game* game) {
    CS_PollEvents(game);

    uint64_t now = SDL_GetPerformanceCounter();
    game->window_ctx.dt = (float)(now - game->window_ctx.last_now) / (float)SDL_GetPerformanceFrequency();
    game->window_ctx.last_now = now;
    if (game->window_ctx.dt > 0.05f) game->window_ctx.dt = 0.05f;

    for (int i = 0; i < game->entity_ctx.entity_pool_size; i++) {
        if (!(game->entity_ctx.entity_pool[i].flags & CS_ENTITY_ACTIVE)) continue;

        CS_EntityBase* current_entity = &game->entity_ctx.entity_pool[i];

        if (current_entity->update) {
            current_entity->update(current_entity, game, game->window_ctx.dt);
        }

        CS_DrawItem* draw_item = CS_DrawPoolGetItemFromID(&(game->draw_ctx), current_entity->draw_item_id);
        if (!(draw_item->flags & CS_DRAWITEM_ACTIVE)) continue;

        draw_item->rect.w = current_entity->sizeNOTINBYTES.x;
        draw_item->rect.h = current_entity->sizeNOTINBYTES.y;
        draw_item->rect.x = current_entity->position.x;
        draw_item->rect.y = current_entity->position.y;
    }
}


void CS_finalize_drawing(CS_DrawItem* draw_item, CS_WindowContext* window_ctx) {
    switch (draw_item->type) {
        case CS_DRAWITEM_RECT: {
            SDL_Color* c = &(draw_item->rect_color);

            SDL_SetRenderDrawColor(
                    window_ctx->renderer,
                    c->r,
                    c->g,
                    c->b,
                    c->a
            );

            SDL_RenderFillRectF(window_ctx->renderer, &(draw_item->rect));
            break;
        }

        case CS_DRAWITEM_SPRITE: {
            SDL_RenderCopyF(
                    window_ctx->renderer,
                    draw_item->sprite,
                    NULL,
                    &(draw_item->rect)
            );
            break;
         }
    }
}

void CS_Draw(CS_Game* game) {
    // seeyuh;
    SDL_SetRenderDrawColor(game->window_ctx.renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->window_ctx.renderer);

    CS_DrawItem* always_on_top_buffer[32];
    for (int i = 0; i < sizeof(always_on_top_buffer) / sizeof(always_on_top_buffer[0]); i++) {
        always_on_top_buffer[i] = NULL;
    }

    int always_on_top_buffer_idx = 0;

    for (int i = 0; i < game->draw_ctx.active_draw_items; i++) {
        if (!(game->draw_ctx.draw_pool[i].flags & CS_DRAWITEM_ACTIVE)) continue;
        CS_DrawItem* current_draw_item = &(game->draw_ctx.draw_pool[i]);

        if (current_draw_item->flags & CS_DRAWITEM_ALWAYS_ON_TOP) {
            always_on_top_buffer[always_on_top_buffer_idx] = current_draw_item;
            always_on_top_buffer_idx += 1;
            continue;
        }

        CS_finalize_drawing(current_draw_item, &(game->window_ctx));
    }

    for (int i = 0; i < sizeof(always_on_top_buffer) / sizeof(always_on_top_buffer[0]); i++) {
        if (always_on_top_buffer[i]) {
            CS_finalize_drawing(always_on_top_buffer[i], &(game->window_ctx));
        }
    }

    SDL_RenderPresent(game->window_ctx.renderer);
    // w faps!;
}


void CS_init_draw_ctx(CS_DrawContext* ctx, int pool_size) {
    ctx->draw_pool_size = pool_size;
    ctx->draw_pool = (CS_DrawItem*)CS_Alloc(sizeof(CS_DrawItem) * ctx->draw_pool_size);
    ctx->active_draw_items = 0;

    for (int i = 0; i < ctx->draw_pool_size; i++) {
        ctx->draw_pool[i].flags = 0;
        ctx->draw_pool[i].id = i;
    }
}

void CS_init_entity_ctx(CS_EntityContext* ctx, int pool_size) {
    ctx->entity_pool_size = pool_size;
    ctx->entity_pool = (CS_EntityBase*)CS_Alloc(sizeof(CS_EntityBase) * ctx->entity_pool_size);
    ctx->active_entities = 0;

    for (int i = 0; i < ctx->entity_pool_size; i++) {
        ctx->entity_pool[i].flags = 0;
        ctx->entity_pool[i].id = i;
    }
}

void CS_init_window_ctx(CS_WindowContext* ctx, int width, int height, char* title) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("shits fucked\n");
        exit(67);
    }

    IMG_Init(IMG_INIT_PNG);

    ctx->window = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    ctx->renderer = SDL_CreateRenderer(ctx->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    ctx->height = height;
    ctx->width = width;

    SDL_RenderSetLogicalSize(ctx->renderer, width, height);
    SDL_GetDesktopDisplayMode(-1, &ctx->display_mode);

    ctx->last_now = SDL_GetPerformanceCounter();
}

void CS_InitGame(
        CS_Game* game,
        int width, int height,
        char* title,
        int draw_pool_size, int entity_pool_size,
        int tile_size
) {
    CS_init_window_ctx(&game->window_ctx, width, height, title);
    CS_init_draw_ctx(&game->draw_ctx, draw_pool_size);
    CS_init_entity_ctx(&game->entity_ctx, entity_pool_size);

    game->tilemap_cfg.h = game->window_ctx.width;
    game->tilemap_cfg.w = game->window_ctx.height;
    game->tilemap_cfg.tile_size = tile_size;

    game->window_ctx.base_path = (char*)CS_Alloc(128 * sizeof(char));
    strncpy(game->window_ctx.base_path, SDL_GetBasePath(), 128);

    game->is_open = 1;
}

#endif
