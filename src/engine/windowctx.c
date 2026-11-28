#ifndef __WINDOWCTX__
#define __WINDOWCTX__

#include <SDL2/SDL.h>

typedef struct {
    SDL_Event event;
    SDL_DisplayMode display_mode;
    SDL_Renderer* renderer;
    SDL_Window* window;
    SDL_Point mouse_pos;

    char* base_path;

    int width;
    int height;

    float dt;
    float last_now;
} CS_WindowContext;

#endif
