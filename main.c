#include <SDL2/SDL.h>
#include "src/engine/engine.c"
#include "src/game.c"

int main(void) {
    CS_Game* game = malloc(sizeof(CS_Game));
    game_ready(game);

    while (game->is_open) {
        CS_Update(game, game->window_ctx.display_mode.refresh_rate);
        CS_Draw(game);
    }

    free(game);

    return 0;
}
