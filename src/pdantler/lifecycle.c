/** This file is managed by pdantler.

    Any changes you make to it will be overwritten next time pdantler is run.
*/

#include "../common.h"
#include "src/scenes/scene_game.h"

static void init_game_game(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  const char *outerr = NULL;

  assets->background_image =
      pd->graphics->loadBitmap("assets/background.png", &outerr);
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

  assets->bird_image = pd->graphics->loadBitmap("assets/bird.png", &outerr);
  if (outerr) {
    pdlogger_error("init_game_game: error loading image bird. %s", outerr);
  }

  if (!assets->bird_image) {
    pdlogger_error("init_game_game: image bird loaded as null.");
  }

  assets->bird_sprite = pd->sprite->newSprite();

  pd->sprite->setImage(assets->bird_sprite, assets->bird_image,
                       kBitmapUnflipped);
  pd->sprite->setZIndex(assets->bird_sprite, 2);

  pd->sprite->moveTo(assets->bird_sprite, 200, 120);
  pd->sprite->addSprite(assets->bird_sprite);
  // Sprite are always invisible when created in init. They will be
  // made visible during the scene enter/exit code
  pd->sprite->setVisible(assets->bird_sprite, false);
}

static void lifecycle_enter_game(GameContext *game, GameAssets *assets) {
  pdlogger_info("lifecycle.c: entering scene 'game'");
  PlaydateAPI *pd = game->pd;

  pdlogger_info("lifecycle.c: .... showing sprite 'background_sprite'");
  pd->sprite->setVisible(assets->background_sprite, true);

  pdlogger_info("lifecycle.c: .... showing sprite 'bird_sprite'");
  pd->sprite->setVisible(assets->bird_sprite, true);

  enter_game(game, assets);
}

static void lifecycle_exit_game(GameContext *game, GameAssets *assets) {
  pdlogger_info("lifecycle.c: exiting scene 'game'");
  PlaydateAPI *pd = game->pd;

  pd->sprite->setVisible(assets->background_sprite, false);
  pdlogger_info("lifecycle.c: .... hiding sprite 'background_sprite'");

  pd->sprite->setVisible(assets->bird_sprite, false);
  pdlogger_info("lifecycle.c: .... hiding sprite 'bird_sprite'");

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

  case SCENE_GAME:
    lifecycle_exit_game(game, (GameAssets *)game->game_assets);
    break;
  case NO_SCENE:
    // If not currently on a scene, do nothing
    break;
  }

  game->current_scene = scene;

  switch (game->current_scene) {

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

  game->game_assets = MLIB_CALLOC(1, sizeof(GameAssets), game->mem_ctx);
  init_game_game(game, (GameAssets *)game->game_assets);

  // This should change
  // to being on demand and having some kind of loading process
  // (which also handles the sprite loads from init_game)
  init_game(game, (GameAssets *)game->game_assets);

  init_sounds(game);

  pdantler_switch_to_scene(game, SCENE_GAME);
}