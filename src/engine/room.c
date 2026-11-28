#ifndef __ROOM__
#define __ROOM__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <ctype.h>

#include "drawpool.c"
#include "windowctx.c"
#include "utils.c"

#define FULL_PATH_SIZE 128 + sizeof("assets/tilemap/img/") + sizeof(fuck) + sizeof(line)

typedef struct {
    int count;
    SDL_Texture** textures;
} CS_TileSet;

typedef struct {
    int row_size;
    int tile_size;
    int tile_arr_size;

    int* tiles;
    int* tile_draw_pool_ids;
} CS_TileMap;

typedef struct {
    int tile_size;
    int w;
    int h;
} CS_TileMapConfig;

CS_TileSet* CS_CreateTileset(char *filename, CS_WindowContext* window_ctx) {
    FILE* tileset_f = CS_OpenFile(filename, "r");
    if (tileset_f == NULL) {
        printf("HI file doesnt exist edition\n");
        exit(1);
    }

    char line[32];
    fgets(line, sizeof(line), tileset_f);
    line[strcspn(line, "\n")] = 0;

    if (strncmp(line, "NAME", 4)) {
        printf("No br.c\n");
        exit(1);
    }

    char fuck[16];
    strncpy(fuck, line + 5, 16);

    int count = 0;
    char** paths = CS_Alloc(32 * sizeof(char*));
    while (fgets(line, sizeof(line), tileset_f))  {
        if (isspace(line[0])) continue;
        line[strcspn(line, "\n")] = 0;

        char full_path[FULL_PATH_SIZE];
        snprintf(full_path, sizeof(full_path), "%sassets/tileset/img/%s/%s", window_ctx->base_path, fuck, line);
        printf("%s\n", full_path);

        paths[count] = strdup(full_path);
        count++;
    }

    CS_CloseFile(tileset_f);

    SDL_Texture** textures = CS_Alloc((1 + count) * sizeof(SDL_Texture*));
    for (int i = 0; i < count; i++) {
        textures[i + 1] = CS_LoadSprite(paths[i], window_ctx);
    }

    textures[0] = NULL;

    CS_TileSet* fuckmebro = CS_Alloc(sizeof(CS_TileSet));
    fuckmebro->count = count;
    fuckmebro->textures = textures;

    return fuckmebro;
}

void CS_DestroyTileset(CS_TileSet* tileset) {
    for (int i = 0; i <= tileset->count; i++) {
        SDL_DestroyTexture(tileset->textures[i]);
    }

    SDL_free(tileset->textures);
    free(tileset);
}

CS_TileMap* CS_CreateTileMap(char* filename, CS_TileMapConfig* cfg) {
    FILE* tilemap_f = CS_OpenFile(filename, "r");
    if (tilemap_f == NULL) {
        printf("HI file doesnt exist tilemap edition\n");
        exit(1);
    }

    char line[16];
    const int TILE_ARR_SIZE = (cfg->h / cfg->tile_size) * (cfg->w / cfg->tile_size);
    int* tiles = CS_Alloc(sizeof(int) * TILE_ARR_SIZE);
    int i = 0;
    while (fgets(line, sizeof(line), tilemap_f)) {
        line[strcspn(line, "\n")] = 0;
        tiles[i] = atoi(line);
        i++;
    }

    CS_CloseFile(tilemap_f);

    CS_TileMap* tilemap = CS_Alloc(sizeof(CS_TileMap));
    tilemap->row_size = cfg->h / cfg->tile_size;
    tilemap->tile_size = cfg->tile_size;
    tilemap->tiles = tiles;
    tilemap->tile_draw_pool_ids = CS_Alloc(sizeof(int) * TILE_ARR_SIZE);
    tilemap->tile_arr_size = TILE_ARR_SIZE;

    return tilemap;
}

void CS_DrawPoolLoadTilemap(
        CS_TileMap* tilemap,
        CS_TileSet* tileset,
        CS_DrawContext* draw_ctx,
        int window_w
) {

    SDL_FPoint tile_size = (SDL_FPoint) { tilemap->tile_size , tilemap->tile_size };
    for (int i = 0; i < tilemap->tile_arr_size; i++) {
        int x = i % tilemap->row_size;
        int y = i / tilemap->row_size;

        SDL_FPoint pos = (SDL_FPoint) {
            tilemap->tile_size * x,
            tilemap->tile_size * y
        };

        int id;
        if (tilemap->tiles[i]) {
            id = CS_DrawPoolInsertSprite(
                    draw_ctx,
                    tileset->textures[tilemap->tiles[i]],
                    pos, &tile_size, 1, 0
            );
        }

        tilemap->tile_draw_pool_ids[i] = id;
    }
}

void CS_DestroyTilemap(
        CS_TileMap* tilemap,
        CS_DrawContext* draw_ctx
) {
    for (int i = 0; i < tilemap->tile_arr_size; i++) {
        CS_DrawPoolRemove(draw_ctx, tilemap->tile_draw_pool_ids[i]);
    }

    free(tilemap->tiles);
    free(tilemap->tile_draw_pool_ids);
    free(tilemap);
}

#undef FULL_PATH_SIZE
#endif
