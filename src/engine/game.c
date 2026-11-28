#ifndef __CS_GAME__
#define __CS_GAME__

#include "windowctx.c"
#include "drawitem.c"
#include "entity.c"
#include "room.c"

typedef struct CS_Game {
    CS_WindowContext window_ctx;
    CS_DrawContext draw_ctx;
    CS_EntityContext entity_ctx;

    CS_TileMapConfig tilemap_cfg;

    uint8_t keys_now   [SDL_NUM_SCANCODES];
    uint8_t keys_notnow[SDL_NUM_SCANCODES];

    int is_open;
} CS_Game;

#endif
