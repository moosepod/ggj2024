#include "src/scenes/scene_game.h"
#include "src/pdantler/mooselib.h"

#define DEFAULT_BIRD_VELOCITY 5
#define DEBOUNCER_DELAY 5
#define SCREEN_WIDTH 400

typedef struct {
  MLIBSize size;
  MLIBPoint location;
  int velocity;
} Player;

typedef struct {
  Player player;
} GameSceneContext;

static void move_player(PlaydateAPI *pd, Player *player, GameAssets *assets,
                        int x);

// Initializes the scene. Do not change the function name/signature.
void init_game(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;

  game->debouncer.delay = DEBOUNCER_DELAY;

  GameSceneContext *gsc = pd->system->realloc(NULL, sizeof(GameSceneContext));
  game->game_userdata = gsc;
  gsc->player.velocity = DEFAULT_BIRD_VELOCITY;

  float x, y;
  pd->sprite->getPosition(assets->bird_sprite, &x, &y);
  gsc->player.location.x = x;
  gsc->player.location.y = y;

  pd->graphics->getBitmapData(assets->bird_image, &gsc->player.size.width,
                              &gsc->player.size.height, NULL, NULL, NULL);
}

// Run once every game loop when scene is active. Do not change the function
// name/signature Return the scene to transfer to, or NO_SCENE if same scene
// should be used
GameScene tick_game(GameContext *game, GameAssets *assets,
                    PDButtons debounced_buttons, int delta) {

  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;
  if ((debounced_buttons & kButtonLeft)) {
    move_player(game->pd, &gsc->player, assets, -1 * gsc->player.velocity);
  } else if ((debounced_buttons & kButtonRight)) {
    move_player(game->pd, &gsc->player, assets, gsc->player.velocity);
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
static void move_player(PlaydateAPI *pd, Player *player, GameAssets *assets,
                        int x) {
  player->location.x =
      MLIB_CLAMP_TO_RANGE(player->location.x + x, player->size.width / 2,
                          SCREEN_WIDTH - player->size.width / 2);
  pd->sprite->moveTo(assets->bird_sprite, player->location.x,
                     player->location.y);
}
