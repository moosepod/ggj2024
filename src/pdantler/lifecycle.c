/** This file is managed by pdantler.

    Any changes you make to it will be overwritten next time pdantler is run.
*/

#include "../common.h"
#include "src/scenes/scene_credits.h"
#include "src/scenes/scene_game.h"
#include "src/scenes/scene_splash.h"

static void init_game_splash(GameContext *game, SplashAssets *assets) {
  PlaydateAPI *pd = game->pd;
  const char *outerr = NULL;

  assets->splash_image =
      pd->graphics->loadBitmap("assets/sprites/splash.png", &outerr);
  if (outerr) {
    pdlogger_error("init_game_splash: error loading image splash. %s", outerr);
  }

  if (!assets->splash_image) {
    pdlogger_error("init_game_splash: image splash loaded as null.");
  }

  assets->splash_sprite = pd->sprite->newSprite();

  pd->sprite->setImage(assets->splash_sprite, assets->splash_image,
                       kBitmapUnflipped);
  pd->sprite->setZIndex(assets->splash_sprite, 1);

  pd->sprite->moveTo(assets->splash_sprite, 200, 120);
  pd->sprite->addSprite(assets->splash_sprite);
  // Sprite are always invisible when created in init. They will be
  // made visible during the scene enter/exit code
  pd->sprite->setVisible(assets->splash_sprite, false);
}

static void lifecycle_enter_splash(GameContext *game, SplashAssets *assets) {
  pdlogger_info("lifecycle.c: entering scene 'splash'");
  PlaydateAPI *pd = game->pd;

  pdlogger_info("lifecycle.c: .... showing sprite 'splash_sprite'");
  pd->sprite->setVisible(assets->splash_sprite, true);

  enter_splash(game, assets);
}

static void lifecycle_exit_splash(GameContext *game, SplashAssets *assets) {
  pdlogger_info("lifecycle.c: exiting scene 'splash'");
  PlaydateAPI *pd = game->pd;

  pd->sprite->setVisible(assets->splash_sprite, false);
  pdlogger_info("lifecycle.c: .... hiding sprite 'splash_sprite'");

  exit_splash(game, assets);
}

static void lifecycle_pause_splash(GameContext *game, SplashAssets *assets) {
  pause_splash(game, assets);
}

static void lifecycle_unpause_splash(GameContext *game, SplashAssets *assets) {
  unpause_splash(game, assets);
}

static void init_game_credits(GameContext *game, CreditsAssets *assets) {
  PlaydateAPI *pd = game->pd;
  const char *outerr = NULL;

  assets->credits_image =
      pd->graphics->loadBitmap("assets/sprites/credits_w_logo.png", &outerr);
  if (outerr) {
    pdlogger_error("init_game_credits: error loading image credits. %s",
                   outerr);
  }

  if (!assets->credits_image) {
    pdlogger_error("init_game_credits: image credits loaded as null.");
  }

  assets->credits_sprite = pd->sprite->newSprite();

  pd->sprite->setImage(assets->credits_sprite, assets->credits_image,
                       kBitmapUnflipped);
  pd->sprite->setZIndex(assets->credits_sprite, 1);

  pd->sprite->moveTo(assets->credits_sprite, 200, 120);
  pd->sprite->addSprite(assets->credits_sprite);
  // Sprite are always invisible when created in init. They will be
  // made visible during the scene enter/exit code
  pd->sprite->setVisible(assets->credits_sprite, false);
}

static void lifecycle_enter_credits(GameContext *game, CreditsAssets *assets) {
  pdlogger_info("lifecycle.c: entering scene 'credits'");
  PlaydateAPI *pd = game->pd;

  pdlogger_info("lifecycle.c: .... showing sprite 'credits_sprite'");
  pd->sprite->setVisible(assets->credits_sprite, true);

  enter_credits(game, assets);
}

static void lifecycle_exit_credits(GameContext *game, CreditsAssets *assets) {
  pdlogger_info("lifecycle.c: exiting scene 'credits'");
  PlaydateAPI *pd = game->pd;

  pd->sprite->setVisible(assets->credits_sprite, false);
  pdlogger_info("lifecycle.c: .... hiding sprite 'credits_sprite'");

  exit_credits(game, assets);
}

static void lifecycle_pause_credits(GameContext *game, CreditsAssets *assets) {
  pause_credits(game, assets);
}

static void lifecycle_unpause_credits(GameContext *game,
                                      CreditsAssets *assets) {
  unpause_credits(game, assets);
}

static void init_game_game(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  const char *outerr = NULL;

  assets->speech_bubble_base_image =
      pd->graphics->loadBitmap("assets/sprites/speech_bubble.png", &outerr);
  if (outerr) {
    pdlogger_error("init_game_game: error loading image speech_bubble_base. %s",
                   outerr);
  }

  assets->joke_bubble_base_image = pd->graphics->loadBitmap(
      "assets/sprites/large_speech_bubble.png", &outerr);
  if (outerr) {
    pdlogger_error("init_game_game: error loading image joke_bubble_base. %s",
                   outerr);
  }

  assets->speech_bubble_mask_image = pd->graphics->loadBitmap(
      "assets/sprites/speech_bubble_mask.png", &outerr);
  if (outerr) {
    pdlogger_error("init_game_game: error loading image speech_bubble_mask. %s",
                   outerr);
  }

  assets->bird_mouth_open_image =
      pd->graphics->loadBitmap("assets/sprites/bird3.png", &outerr);
  if (outerr) {
    pdlogger_error("init_game_game: error loading image bird_mouth_open. %s",
                   outerr);
  }

  assets->background_image =
      pd->graphics->loadBitmap("assets/sprites/background2.png", &outerr);
  if (outerr) {
    pdlogger_error("init_game_game: error loading image background. %s",
                   outerr);
  }

  if (!assets->background_image) {
    pdlogger_error("init_game_game: image background loaded as null.");
  }

  assets->background_sprite = pd->sprite->newSprite();

  pd->sprite->setImage(assets->background_sprite, assets->background_image,
                       kBitmapUnflipped);
  pd->sprite->setZIndex(assets->background_sprite, 1);

  pd->sprite->moveTo(assets->background_sprite, 200, 120);
  pd->sprite->addSprite(assets->background_sprite);
  // Sprite are always invisible when created in init. They will be
  // made visible during the scene enter/exit code
  pd->sprite->setVisible(assets->background_sprite, false);

  assets->bird_image =
      pd->graphics->loadBitmap("assets/sprites/bird2.png", &outerr);
  if (outerr) {
    pdlogger_error("init_game_game: error loading image bird. %s", outerr);
  }

  if (!assets->bird_image) {
    pdlogger_error("init_game_game: image bird loaded as null.");
  }

  assets->bird_sprite = pd->sprite->newSprite();

  pd->sprite->setImage(assets->bird_sprite, assets->bird_image,
                       kBitmapUnflipped);
  pd->sprite->setZIndex(assets->bird_sprite, 5);

  pd->sprite->moveTo(assets->bird_sprite, 200, 100);
  pd->sprite->addSprite(assets->bird_sprite);
  // Sprite are always invisible when created in init. They will be
  // made visible during the scene enter/exit code
  pd->sprite->setVisible(assets->bird_sprite, false);

  assets->audience_image =
      pd->graphics->loadBitmap("assets/sprites/foreground_birds.png", &outerr);
  if (outerr) {
    pdlogger_error("init_game_game: error loading image audience. %s", outerr);
  }

  if (!assets->audience_image) {
    pdlogger_error("init_game_game: image audience loaded as null.");
  }

  assets->audience_sprite = pd->sprite->newSprite();

  pd->sprite->setImage(assets->audience_sprite, assets->audience_image,
                       kBitmapUnflipped);
  pd->sprite->setZIndex(assets->audience_sprite, 3);

  pd->sprite->moveTo(assets->audience_sprite, 200, 120);
  pd->sprite->addSprite(assets->audience_sprite);
  // Sprite are always invisible when created in init. They will be
  // made visible during the scene enter/exit code
  pd->sprite->setVisible(assets->audience_sprite, false);

  assets->tomato_image =
      pd->graphics->loadBitmap("assets/sprites/tomato.png", &outerr);
  if (outerr) {
    pdlogger_error("init_game_game: error loading image tomato. %s", outerr);
  }

  if (!assets->tomato_image) {
    pdlogger_error("init_game_game: image tomato loaded as null.");
  }

  assets->tomato_sprite = pd->sprite->newSprite();

  pd->sprite->setImage(assets->tomato_sprite, assets->tomato_image,
                       kBitmapUnflipped);
  pd->sprite->setZIndex(assets->tomato_sprite, 5);

  pd->sprite->moveTo(assets->tomato_sprite, 50, 50);
  pd->sprite->addSprite(assets->tomato_sprite);
  // Sprite are always invisible when created in init. They will be
  // made visible during the scene enter/exit code
  pd->sprite->setVisible(assets->tomato_sprite, false);

  assets->hook_image =
      pd->graphics->loadBitmap("assets/sprites/hook.png", &outerr);
  if (outerr) {
    pdlogger_error("init_game_game: error loading image hook. %s", outerr);
  }

  if (!assets->hook_image) {
    pdlogger_error("init_game_game: image hook loaded as null.");
  }

  assets->hook_sprite = pd->sprite->newSprite();

  pd->sprite->setImage(assets->hook_sprite, assets->hook_image,
                       kBitmapUnflipped);
  pd->sprite->setZIndex(assets->hook_sprite, 5);

  pd->sprite->moveTo(assets->hook_sprite, 50, 50);
  pd->sprite->addSprite(assets->hook_sprite);
  // Sprite are always invisible when created in init. They will be
  // made visible during the scene enter/exit code
  pd->sprite->setVisible(assets->hook_sprite, false);

  assets->hooked_crow_image =
      pd->graphics->loadBitmap("assets/sprites/hooked_crow.png", &outerr);
  if (outerr) {
    pdlogger_error("init_game_game: error loading image hooked_crow. %s",
                   outerr);
  }

  if (!assets->hooked_crow_image) {
    pdlogger_error("init_game_game: image hooked_crow loaded as null.");
  }

  assets->hooked_crow_sprite = pd->sprite->newSprite();

  pd->sprite->setImage(assets->hooked_crow_sprite, assets->hooked_crow_image,
                       kBitmapUnflipped);
  pd->sprite->setZIndex(assets->hooked_crow_sprite, 5);

  pd->sprite->moveTo(assets->hooked_crow_sprite, 50, 50);
  pd->sprite->addSprite(assets->hooked_crow_sprite);
  // Sprite are always invisible when created in init. They will be
  // made visible during the scene enter/exit code
  pd->sprite->setVisible(assets->hooked_crow_sprite, false);

  assets->speech_bubble_image = pd->graphics->newBitmap(144, 55, kColorWhite);

  if (!assets->speech_bubble_image) {
    pdlogger_error("init_game_game: image speech_bubble loaded as null.");
  }

  assets->speech_bubble_sprite = pd->sprite->newSprite();

  pd->sprite->setImage(assets->speech_bubble_sprite,
                       assets->speech_bubble_image, kBitmapUnflipped);
  pd->sprite->setZIndex(assets->speech_bubble_sprite, 4);

  pd->sprite->moveTo(assets->speech_bubble_sprite, 200, 120);
  pd->sprite->addSprite(assets->speech_bubble_sprite);
  // Sprite are always invisible when created in init. They will be
  // made visible during the scene enter/exit code
  pd->sprite->setVisible(assets->speech_bubble_sprite, false);

  assets->joke_bubble_image = pd->graphics->newBitmap(384, 38, kColorWhite);

  if (!assets->joke_bubble_image) {
    pdlogger_error("init_game_game: image joke_bubble loaded as null.");
  }

  assets->joke_bubble_sprite = pd->sprite->newSprite();

  pd->sprite->setImage(assets->joke_bubble_sprite, assets->joke_bubble_image,
                       kBitmapUnflipped);
  pd->sprite->setZIndex(assets->joke_bubble_sprite, 2);

  pd->sprite->moveTo(assets->joke_bubble_sprite, 198, 30);
  pd->sprite->addSprite(assets->joke_bubble_sprite);
  // Sprite are always invisible when created in init. They will be
  // made visible during the scene enter/exit code
  pd->sprite->setVisible(assets->joke_bubble_sprite, false);

  assets->start_text_image =
      pd->graphics->loadBitmap("assets/sprites/start_text.png", &outerr);
  if (outerr) {
    pdlogger_error("init_game_game: error loading image start_text. %s",
                   outerr);
  }

  if (!assets->start_text_image) {
    pdlogger_error("init_game_game: image start_text loaded as null.");
  }

  assets->start_text_sprite = pd->sprite->newSprite();

  pd->sprite->setImage(assets->start_text_sprite, assets->start_text_image,
                       kBitmapUnflipped);
  pd->sprite->setZIndex(assets->start_text_sprite, 5);

  pd->sprite->moveTo(assets->start_text_sprite, 200, 30);
  pd->sprite->addSprite(assets->start_text_sprite);
  // Sprite are always invisible when created in init. They will be
  // made visible during the scene enter/exit code
  pd->sprite->setVisible(assets->start_text_sprite, false);

  pdlogger_info(
      "init_game_game: loading sample joke1 at assets/sounds/CrowJoke1.wav");
  assets->joke1_sample = pd->sound->sample->load("assets/sounds/CrowJoke1.wav");
  if (!assets->joke1_sample) {
    pdlogger_error("init_game_game: sample joke1 loaded as null.");
  }
  pdlogger_info(
      "init_game_game: loading sample joke2 at assets/sounds/CrowJoke2.wav");
  assets->joke2_sample = pd->sound->sample->load("assets/sounds/CrowJoke2.wav");
  if (!assets->joke2_sample) {
    pdlogger_error("init_game_game: sample joke2 loaded as null.");
  }
  pdlogger_info(
      "init_game_game: loading sample joke3 at assets/sounds/CrowJoke3.wav");
  assets->joke3_sample = pd->sound->sample->load("assets/sounds/CrowJoke3.wav");
  if (!assets->joke3_sample) {
    pdlogger_error("init_game_game: sample joke3 loaded as null.");
  }
  pdlogger_info(
      "init_game_game: loading sample joke4 at assets/sounds/CrowJoke4.wav");
  assets->joke4_sample = pd->sound->sample->load("assets/sounds/CrowJoke4.wav");
  if (!assets->joke4_sample) {
    pdlogger_error("init_game_game: sample joke4 loaded as null.");
  }
  pdlogger_info(
      "init_game_game: loading sample joke5 at assets/sounds/CrowJoke5.wav");
  assets->joke5_sample = pd->sound->sample->load("assets/sounds/CrowJoke5.wav");
  if (!assets->joke5_sample) {
    pdlogger_error("init_game_game: sample joke5 loaded as null.");
  }
  pdlogger_info(
      "init_game_game: loading sample heckle1 at assets/sounds/Peckle1.wav");
  assets->heckle1_sample = pd->sound->sample->load("assets/sounds/Peckle1.wav");
  if (!assets->heckle1_sample) {
    pdlogger_error("init_game_game: sample heckle1 loaded as null.");
  }
  pdlogger_info(
      "init_game_game: loading sample heckle2 at assets/sounds/Peckle2.wav");
  assets->heckle2_sample = pd->sound->sample->load("assets/sounds/Peckle2.wav");
  if (!assets->heckle2_sample) {
    pdlogger_error("init_game_game: sample heckle2 loaded as null.");
  }
  pdlogger_info(
      "init_game_game: loading sample heckle3 at assets/sounds/Peckle3.wav");
  assets->heckle3_sample = pd->sound->sample->load("assets/sounds/Peckle3.wav");
  if (!assets->heckle3_sample) {
    pdlogger_error("init_game_game: sample heckle3 loaded as null.");
  }
  pdlogger_info(
      "init_game_game: loading sample heckle4 at assets/sounds/Peckle4.wav");
  assets->heckle4_sample = pd->sound->sample->load("assets/sounds/Peckle4.wav");
  if (!assets->heckle4_sample) {
    pdlogger_error("init_game_game: sample heckle4 loaded as null.");
  }
  pdlogger_info(
      "init_game_game: loading sample heckle5 at assets/sounds/Peckle5.wav");
  assets->heckle5_sample = pd->sound->sample->load("assets/sounds/Peckle5.wav");
  if (!assets->heckle5_sample) {
    pdlogger_error("init_game_game: sample heckle5 loaded as null.");
  }
  pdlogger_info(
      "init_game_game: loading sample heckle6 at assets/sounds/Peckle6.wav");
  assets->heckle6_sample = pd->sound->sample->load("assets/sounds/Peckle6.wav");
  if (!assets->heckle6_sample) {
    pdlogger_error("init_game_game: sample heckle6 loaded as null.");
  }
  pdlogger_info(
      "init_game_game: loading sample heckle7 at assets/sounds/Peckle7.wav");
  assets->heckle7_sample = pd->sound->sample->load("assets/sounds/Peckle7.wav");
  if (!assets->heckle7_sample) {
    pdlogger_error("init_game_game: sample heckle7 loaded as null.");
  }
  pdlogger_info(
      "init_game_game: loading sample heckle8 at assets/sounds/Peckle8.wav");
  assets->heckle8_sample = pd->sound->sample->load("assets/sounds/Peckle8.wav");
  if (!assets->heckle8_sample) {
    pdlogger_error("init_game_game: sample heckle8 loaded as null.");
  }

  assets->targets[TARGET_GAME_STAGE_CENTER].rect =
      MLIBRECT_CREATE(180, 90, 40, 20);

  assets->targets[TARGET_GAME_STAGE_RIGHT].rect =
      MLIBRECT_CREATE(450, 90, 40, 20);
}

static void lifecycle_enter_game(GameContext *game, GameAssets *assets) {
  pdlogger_info("lifecycle.c: entering scene 'game'");
  PlaydateAPI *pd = game->pd;

  pdlogger_info("lifecycle.c: .... showing sprite 'background_sprite'");
  pd->sprite->setVisible(assets->background_sprite, true);

  pdlogger_info("lifecycle.c: .... showing sprite 'bird_sprite'");
  pd->sprite->setVisible(assets->bird_sprite, true);

  pdlogger_info("lifecycle.c: .... showing sprite 'audience_sprite'");
  pd->sprite->setVisible(assets->audience_sprite, true);

  enter_game(game, assets);
}

static void lifecycle_exit_game(GameContext *game, GameAssets *assets) {
  pdlogger_info("lifecycle.c: exiting scene 'game'");
  PlaydateAPI *pd = game->pd;

  pd->sprite->setVisible(assets->background_sprite, false);
  pdlogger_info("lifecycle.c: .... hiding sprite 'background_sprite'");

  pd->sprite->setVisible(assets->bird_sprite, false);
  pdlogger_info("lifecycle.c: .... hiding sprite 'bird_sprite'");

  pd->sprite->setVisible(assets->audience_sprite, false);
  pdlogger_info("lifecycle.c: .... hiding sprite 'audience_sprite'");

  pd->sprite->setVisible(assets->tomato_sprite, false);
  pdlogger_info("lifecycle.c: .... hiding sprite 'tomato_sprite'");

  pd->sprite->setVisible(assets->hook_sprite, false);
  pdlogger_info("lifecycle.c: .... hiding sprite 'hook_sprite'");

  pd->sprite->setVisible(assets->hooked_crow_sprite, false);
  pdlogger_info("lifecycle.c: .... hiding sprite 'hooked_crow_sprite'");

  pd->sprite->setVisible(assets->speech_bubble_sprite, false);
  pdlogger_info("lifecycle.c: .... hiding sprite 'speech_bubble_sprite'");

  pd->sprite->setVisible(assets->joke_bubble_sprite, false);
  pdlogger_info("lifecycle.c: .... hiding sprite 'joke_bubble_sprite'");

  pd->sprite->setVisible(assets->start_text_sprite, false);
  pdlogger_info("lifecycle.c: .... hiding sprite 'start_text_sprite'");

  exit_game(game, assets);
}

static void lifecycle_pause_game(GameContext *game, GameAssets *assets) {
  pause_game(game, assets);
}

static void lifecycle_unpause_game(GameContext *game, GameAssets *assets) {
  unpause_game(game, assets);
}

static void init_fonts(GameContext *game) {
  PlaydateAPI *pd = game->pd;
  const char *outerr = NULL;

  pdlogger_info("lifecycle.c: .... loading font 'main_font'");
  game->main_font =
      pd->graphics->loadFont("assets/fonts/font-pedallica", &outerr);
  if (outerr) {
    pdlogger_error("init_game_fonts: error loading font main. %s", outerr);
  }
}

static void init_sounds(GameContext *game) {
  PlaydateAPI *pd = game->pd;
  game->music_active = true;
  game->music_player = pd->sound->sampleplayer->newPlayer();
  game->sfx_1_player = pd->sound->sampleplayer->newPlayer();
  game->sfx_2_player = pd->sound->sampleplayer->newPlayer();
}

void pdantler_switch_to_scene(GameContext *game, GameScene scene) {
  pdlogger_info("lifecycle.c: switching to scene %d", scene);
  if (scene == game->current_scene) {
    pdlogger_info("pdantler_switch_to_scene: ignored request to switch to "
                  "current scene.");
    return;
  }

  switch (game->current_scene) {

  case SCENE_SPLASH:
    lifecycle_exit_splash(game, (SplashAssets *)game->splash_assets);
    break;
  case SCENE_CREDITS:
    lifecycle_exit_credits(game, (CreditsAssets *)game->credits_assets);
    break;
  case SCENE_GAME:
    lifecycle_exit_game(game, (GameAssets *)game->game_assets);
    break;
  case NO_SCENE:
    // If not currently on a scene, do nothing
    break;
  }

  game->current_scene = scene;

  switch (game->current_scene) {

  case SCENE_SPLASH:
    lifecycle_enter_splash(game, (SplashAssets *)game->splash_assets);
    break;
  case SCENE_CREDITS:
    lifecycle_enter_credits(game, (CreditsAssets *)game->credits_assets);
    break;
  case SCENE_GAME:
    lifecycle_enter_game(game, (GameAssets *)game->game_assets);
    break;
  default:
    pdlogger_error("pdantler_switch_to_scene: unhandled scene on enter.");
  }
}

void lifecycle_init_game(GameContext *game) {
  game->debouncer.delay = DEFAULT_DEBOUNCE_DELAY;

  init_fonts(game);

  game->splash_assets = MLIB_CALLOC(1, sizeof(SplashAssets), game->mem_ctx);
  init_game_splash(game, (SplashAssets *)game->splash_assets);

  game->credits_assets = MLIB_CALLOC(1, sizeof(CreditsAssets), game->mem_ctx);
  init_game_credits(game, (CreditsAssets *)game->credits_assets);

  game->game_assets = MLIB_CALLOC(1, sizeof(GameAssets), game->mem_ctx);
  init_game_game(game, (GameAssets *)game->game_assets);

  // This should change
  // to being on demand and having some kind of loading process
  // (which also handles the sprite loads from init_game)
  init_splash(game, (SplashAssets *)game->splash_assets);
  init_credits(game, (CreditsAssets *)game->credits_assets);
  init_game(game, (GameAssets *)game->game_assets);

  init_sounds(game);

  pdantler_switch_to_scene(game, SCENE_SPLASH);
}