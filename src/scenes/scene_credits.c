#include "src/scenes/scene_credits.h"

#define CREDITS_DEBOUNCE_DELAY 1000

// Initializes the scene. Do not change the function name/signature.
void init_credits(GameContext *game, CreditsAssets *assets) {
  PlaydateAPI *pd = game->pd;
}

// Run once every game loop when scene is active. Do not change the function
// name/signature Return the scene to transfer to, or NO_SCENE if same scene
// should be used
GameScene tick_credits(GameContext *game, CreditsAssets *assets,
                       PDButtons debounced_buttons, int delta) {
  if ((debounced_buttons & kButtonA) || (debounced_buttons & kButtonB)) {
    return SCENE_SPLASH;
  }

  return NO_SCENE;
}

void enter_credits(GameContext *game, CreditsAssets *assets) {
  game->debouncer.delay = CREDITS_DEBOUNCE_DELAY;
}

void exit_credits(GameContext *game, CreditsAssets *assets) {}

void pause_credits(GameContext *game, CreditsAssets *assets) {}

void unpause_credits(GameContext *game, CreditsAssets *assets) {}
