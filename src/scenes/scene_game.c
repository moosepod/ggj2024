#include "src/scenes/scene_game.h"
#include "src/pdantler/mooselib.h"

#define DEFAULT_BIRD_VELOCITY 5
#define DEBOUNCER_DELAY 5

typedef struct {
  MLIBSize bird_size;
  int bird_velocity;
} GameSceneContext;

static void move_bird(PlaydateAPI *pd, GameAssets *assets, int x);

// Initializes the scene. Do not change the function name/signature.
void init_game(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;

  game->debouncer.delay = DEBOUNCER_DELAY;

  GameSceneContext *gsc = pd->system->realloc(NULL, sizeof(GameSceneContext));
  game->game_userdata = gsc;
  gsc->bird_velocity = DEFAULT_BIRD_VELOCITY;

  pd->graphics->getBitmapData(assets->bird_image, &gsc->bird_size.width,
                              &gsc->bird_size.height, NULL, NULL, NULL);
}

// Run once every game loop when scene is active. Do not change the function
// name/signature Return the scene to transfer to, or NO_SCENE if same scene
// should be used
GameScene tick_game(GameContext *game, GameAssets *assets,
                    PDButtons debounced_buttons, int delta) {

  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;
  if ((debounced_buttons & kButtonLeft)) {
    move_bird(game->pd, assets, -1 * gsc->bird_velocity);
  } else if ((debounced_buttons & kButtonRight)) {
    move_bird(game->pd, assets, gsc->bird_velocity);
  }

  return NO_SCENE;
}

void enter_game(GameContext *game, GameAssets *assets) {}

void exit_game(GameContext *game, GameAssets *assets) {}

void pause_game(GameContext *game, GameAssets *assets) {}

void unpause_game(GameContext *game, GameAssets *assets) {}

//
// Main function
//
static void move_bird(PlaydateAPI *pd, GameAssets *assets, int x) {
  pd->sprite->moveBy(assets->bird_sprite, x, 0);
}
