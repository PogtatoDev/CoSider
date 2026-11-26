#ifndef __CS_DRAWITEM__
#define __CS_DRAWITEM__

#include <SDL2/SDL.h>

#define CS_DRAWITEM_ACTIVE        (1 << 1)
#define CS_DRAWITEM_ALWAYS_ON_TOP (1 << 2)

#define CS_DRAWITEM_RECT 1
#define CS_DRAWITEM_SPRITE 0

typedef struct {
    union {
        struct {
            SDL_Rect rect;
            SDL_Color rect_color;
        } Rect;

        struct {
            SDL_Texture* sprite;
            SDL_Rect dest_rect;
        } Sprite;
    };

    int flags;

    int type;
    int z_idx;
    int id;
} CS_DrawItem;

typedef struct {
    CS_DrawItem* draw_pool;
    int draw_pool_size;
    int active_draw_items;
} CS_DrawContext;

#endif
