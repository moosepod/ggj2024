#include "src/scenes/scene_splash.h"

// Initializes the scene. Do not change the function name/signature.
void init_splash(GameContext *game, SplashAssets *assets) {
  PlaydateAPI *pd = game->pd;
}

// Run once every game loop when scene is active. Do not change the function
// name/signature Return the scene to transfer to, or NO_SCENE if same scene
// should be used
GameScene tick_splash(GameContext *game, SplashAssets *assets,
                      PDButtons debounced_buttons, int delta) {

  if (debounced_buttons & kButtonA) {
    return SCENE_GAME;
  }
  return NO_SCENE;
}

void enter_splash(GameContext *game, SplashAssets *assets) {}

void exit_splash(GameContext *game, SplashAssets *assets) {}

void pause_splash(GameContext *game, SplashAssets *assets) {}

void unpause_splash(GameContext *game, SplashAssets *assets) {}
