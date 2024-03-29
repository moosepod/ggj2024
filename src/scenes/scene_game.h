/** This file is managed by pdantler.

    Any changes you make to it will be overwritten next time pdantler is run.
*/

#include "../common.h"

#define GAME_TARGET_COUNT 2

typedef enum {
  TARGET_GAME_STAGE_CENTER,
  TARGET_GAME_STAGE_RIGHT

} GameTargets;

typedef struct {

  LCDBitmap *background_image;
  LCDSprite *background_sprite;

  LCDBitmap *bird_image;
  LCDSprite *bird_sprite;

  LCDBitmap *audience_image;
  LCDSprite *audience_sprite;

  LCDBitmap *tomato_image;
  LCDSprite *tomato_sprite;

  LCDBitmap *hook_image;
  LCDSprite *hook_sprite;

  LCDBitmap *hooked_crow_image;
  LCDSprite *hooked_crow_sprite;

  LCDBitmap *speech_bubble_image;
  LCDSprite *speech_bubble_sprite;

  LCDBitmap *joke_bubble_image;
  LCDSprite *joke_bubble_sprite;

  LCDBitmap *start_text_image;
  LCDSprite *start_text_sprite;

  LCDBitmap *speech_bubble_base_image;

  LCDBitmap *joke_bubble_base_image;

  LCDBitmap *speech_bubble_mask_image;

  LCDBitmap *bird_mouth_open_image;

  AudioSample *joke1_sample;
  AudioSample *joke2_sample;
  AudioSample *joke3_sample;
  AudioSample *joke4_sample;
  AudioSample *joke5_sample;
  AudioSample *heckle1_sample;
  AudioSample *heckle2_sample;
  AudioSample *heckle3_sample;
  AudioSample *heckle4_sample;
  AudioSample *heckle5_sample;
  AudioSample *heckle6_sample;
  AudioSample *heckle7_sample;
  AudioSample *heckle8_sample;
  Target targets[GAME_TARGET_COUNT];
} GameAssets;

extern void init_game(GameContext *game, GameAssets *assets);
extern void enter_game(GameContext *game, GameAssets *assets);
extern void exit_game(GameContext *game, GameAssets *assets);
extern void pause_game(GameContext *game, GameAssets *assets);
extern void unpause_game(GameContext *game, GameAssets *assets);
extern GameScene tick_game(GameContext *game, GameAssets *assets,
                           PDButtons debounced_buttons, int delta);
