#ifndef __CS_UTILS__
#define __CS_UTILS__

#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "windowctx.c"

#define CS_ENTITY_UPDATE_ARGS CS_EntityBase* base, CS_Game* game, float dt
#define CS_ENTITY_READY_ARGS  CS_EntityBase* base, CS_Game* game

static inline void* CS_Alloc(size_t bytes) {
    void* p = malloc(bytes);
    if (p == NULL) exit(1);

    memset(p, 0, bytes);
    return p;
}

static inline void CS_Free(void* p) {
    if (p) free(p);
}

static inline SDL_Texture* CS_LoadSprite(const char* filename, CS_WindowContext* ctx) {
    SDL_Texture* sprite = IMG_LoadTexture(ctx->renderer, filename);
    if (sprite == NULL) {
        printf("hi it doesnt exist\n");
        printf("%s\n", IMG_GetError());
        exit(2);
    }

    return sprite;
}

static inline void CS_FreeSprite(SDL_Texture* sprite) {
    if (sprite) SDL_DestroyTexture(sprite);
}

static inline FILE* CS_OpenFile(const char* filename, const char* modes) {
    FILE* f = fopen(filename, modes);
    if (f == NULL) {
        perror("hi it doesnt exist (fopen edition)");
        exit(3);
    }

    return f;
}

static inline void CS_CloseFile(FILE* f) {
    if (f) fclose(f);
}

static inline int CS_IsKeyDown(uint8_t* keys_now, SDL_Scancode key) {
    if (keys_now[key]) {
        return 1;
    }

    return 0;
}

static inline int CS_IsKeyPressed(uint8_t* keys_now, uint8_t* keys_notnow, SDL_Scancode key) {
    if (keys_now[key] && !keys_notnow[key]) {
        return 1;
    }

    return 0;
}

#endif
