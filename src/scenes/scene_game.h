/** This file is managed by pdantler.

    Any changes you make to it will be overwritten next time pdantler is run.
*/

#include "../common.h"

typedef struct {

  LCDBitmap *background_image;
  LCDSprite *background_sprite;

  LCDBitmap *bird_image;
  LCDSprite *bird_sprite;

  LCDBitmap *speech_bubble_image;
  LCDSprite *speech_bubble_sprite;

  LCDBitmap *speech_bubble_base_image;

} GameAssets;

extern void init_game(GameContext *game, GameAssets *assets);
extern void enter_game(GameContext *game, GameAssets *assets);
extern void exit_game(GameContext *game, GameAssets *assets);
extern void pause_game(GameContext *game, GameAssets *assets);
extern void unpause_game(GameContext *game, GameAssets *assets);
extern GameScene tick_game(GameContext *game, GameAssets *assets,
                           PDButtons debounced_buttons, int delta);
