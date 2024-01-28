/** This file is managed by pdantler.

    Any changes you make to it will be overwritten next time pdantler is run.
*/

#include "../build.h"
#include "../common.h"

#include "src/scenes/scene_game.h"

extern void lifecycle_init_game(GameContext *context);
extern PDButtons pdantler_debounce_input(PlaydateAPI *pd, Debouncer *debouncer);

static int update(void *userdata);

void pd_appender(const char *p_entry, void *p_udata) {
  PlaydateAPI *pd = (PlaydateAPI *)p_udata;
  pd->system->logToConsole(p_entry);
}

// Called by Playdate whenever there is a system event. Currently used soley for
// initialization.
int eventHandler(PlaydateAPI *pd, PDSystemEvent event, uint32_t arg) {
  (void)arg; // arg is currently only used for event = kEventKeyPressed

  if (event == kEventInit) {
    pdlogger_register_appender(&pd_appender, pd);
    pdlogger_info("eventHandler: Starting Pecklers build number %s",
                  BUILD_NUMBER);

    PlaydateMRLMemCtx *mem_ctx =
        pd->system->realloc(NULL, sizeof(PlaydateMRLMemCtx));
    mem_ctx->realloc = pd->system->realloc;

    GameContext *game = MLIB_CALLOC(1, sizeof(GameContext), mem_ctx);
    game->pd = pd;
    game->mem_ctx = mem_ctx;
    game->current_scene = NO_SCENE;
    game->last_tick_time = pd->system->getCurrentTimeMilliseconds();
    lifecycle_init_game(game);

    // Setting update callback will skip Lua callback and stay in C
    pd->system->setUpdateCallback(update, game);
  }

  return 0;
}

static int update(void *userdata) {
  GameContext *game = (GameContext *)userdata;
  PlaydateAPI *pd = game->pd;

  unsigned int delta =
      pd->system->getCurrentTimeMilliseconds() - game->last_tick_time;
  game->last_tick_time = pd->system->getCurrentTimeMilliseconds();

  PDButtons debounced_buttons = pdantler_debounce_input(pd, &game->debouncer);

  GameScene next_scene = NO_SCENE;

  switch (game->current_scene) {

  case SCENE_GAME:
    next_scene = tick_game(game, (GameAssets *)game->game_assets,
                           debounced_buttons, delta);
    break;
  default:
    pdlogger_error("update called with unhandled scene");
  }

  if (next_scene != NO_SCENE && next_scene != game->current_scene) {
    pdantler_switch_to_scene(game, next_scene);
  }

  pd->sprite->updateAndDrawSprites();

  return 1;
}