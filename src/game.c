#ifndef __GAME__
#define __GAME__

#include "engine/drawitem.c"
#include <SDL2/SDL_image.h>
#define WINDOW_W 640
#define WINDOW_H 480

#include "engine/engine.c"

void game_ready(CS_Game* game) {
    CS_InitGame(game, WINDOW_W, WINDOW_H, "CoSider test i guess", 128, 64, 8, 20);

    CS_TileSet* tileset = CS_CreateTileset("assets/tileset/test.tileset", &(game->window_ctx));
    CS_TileMap* tilemap = CS_CreateTileMap("assets/tilemap/hello.map", &(game->tilemap_cfg));

    CS_DrawPoolLoadTilemap(tilemap, tileset, &(game->draw_ctx), game->window_ctx.width);

    SDL_Texture* hi = IMG_LoadTexture(game->window_ctx.renderer, "assets/sprite/hi.png");
    if (hi == NULL) { exit(1); }

    int hi2 = CS_DrawPoolInsertSprite(
            &(game->draw_ctx),
            hi,
            (SDL_Point) { 50, 50 }, NULL,
            0, CS_DRAWITEM_ALWAYS_ON_TOP
    );

    CS_DrawItem* hi3 = CS_DrawPoolGetItemFromID(&(game->draw_ctx), hi2);
}

#endif
