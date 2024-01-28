/** This file is managed by pdantler.

    Any changes you make to it will be overwritten next time pdantler is run.
*/

#include "../common.h"

#define SPLASH_TARGET_COUNT 0

typedef struct {

  LCDBitmap *splash_image;
  LCDSprite *splash_sprite;

  Target targets[SPLASH_TARGET_COUNT];
} SplashAssets;

extern void init_splash(GameContext *game, SplashAssets *assets);
extern void enter_splash(GameContext *game, SplashAssets *assets);
extern void exit_splash(GameContext *game, SplashAssets *assets);
extern void pause_splash(GameContext *game, SplashAssets *assets);
extern void unpause_splash(GameContext *game, SplashAssets *assets);
extern GameScene tick_splash(GameContext *game, SplashAssets *assets,
                             PDButtons debounced_buttons, int delta);
