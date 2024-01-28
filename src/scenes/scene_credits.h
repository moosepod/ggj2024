/** This file is managed by pdantler.

    Any changes you make to it will be overwritten next time pdantler is run.
*/

#include "../common.h"

#define CREDITS_TARGET_COUNT 0

typedef struct {

  LCDBitmap *credits_image;
  LCDSprite *credits_sprite;

  Target targets[CREDITS_TARGET_COUNT];
} CreditsAssets;

extern void init_credits(GameContext *game, CreditsAssets *assets);
extern void enter_credits(GameContext *game, CreditsAssets *assets);
extern void exit_credits(GameContext *game, CreditsAssets *assets);
extern void pause_credits(GameContext *game, CreditsAssets *assets);
extern void unpause_credits(GameContext *game, CreditsAssets *assets);
extern GameScene tick_credits(GameContext *game, CreditsAssets *assets,
                              PDButtons debounced_buttons, int delta);
