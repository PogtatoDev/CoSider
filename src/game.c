#ifndef __GAME__
#define __GAME__

#include "engine/drawitem.c"
#include "engine/room.c"
#define WINDOW_W 640
#define WINDOW_H 480

#include "engine/engine.c"

void test_update(CS_EntityBase* base, float dt) {
    printf("a\n");
}

void test_ready(CS_EntityBase* base) {
    printf("hi im ready\n");
}

void game_ready(CS_Game* game) {
    CS_InitGame(game, WINDOW_W, WINDOW_H, "CoSider test i guess", 128, 64, 8, 20);

    CS_TileSet* tileset = CS_CreateTileset("assets/tileset/test.tileset", &(game->window_ctx));
    CS_TileMap* tilemap = CS_CreateTileMap("assets/tilemap/hello.map", &(game->tilemap_cfg));

    CS_DrawPoolLoadTilemap(tilemap, tileset, &(game->draw_ctx), game->window_ctx.width);
    SDL_Texture* test = CS_LoadSprite("assets/sprite/hi.png", &(game->window_ctx));

    CS_EntityCreate(
            &(game->entity_ctx),
            &(game->draw_ctx),
            test,
            (SDL_Point) { 0, 0 },
            &(SDL_Point) { 20, 20 },
            CS_DRAWITEM_ALWAYS_ON_TOP, 0,
            6767,
            test_ready, test_update
    );
}

#endif
