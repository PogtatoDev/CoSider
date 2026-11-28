#ifndef __GAME__
#define __GAME__

#define __CS_DEBUG__

#include "engine/room.c"
#define WINDOW_W 640
#define WINDOW_H 480

#include "engine/engine.c"

typedef struct {


} HellHathNoFury;

void test_update(CS_ENTITY_UPDATE_ARGS) {
    base->position.x += 50 * dt;
    base->sizeNOTINBYTES.x -= 500 * dt;
}

void test_ready(CS_ENTITY_READY_ARGS) {
    printf("hi im ready\n");
    printf("%f\n", base->sizeNOTINBYTES.x);
}

void game_ready(CS_Game* game) {
    CS_InitGame(game, WINDOW_W, WINDOW_H, "CoSider test i guess", 128, 64, 20);

    CS_TileSet* tileset = CS_CreateTileset("assets/tileset/test.tileset", &(game->window_ctx));
    CS_TileMap* tilemap = CS_CreateTileMap("assets/tilemap/hello.map", &(game->tilemap_cfg));

    CS_DrawPoolLoadTilemap(tilemap, tileset, &(game->draw_ctx), game->window_ctx.width);
    SDL_Texture* test = CS_LoadSprite("assets/sprite/hi.png", &(game->window_ctx));

    CS_EntityCreate(
            game,
            &(game->entity_ctx),
            &(game->draw_ctx),
            test,
            (SDL_FPoint) { 67, 67 },
            NULL,
            0, 0, 0,
            test_ready, test_update
    );
}

#endif
