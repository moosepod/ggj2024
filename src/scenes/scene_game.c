#include "src/scenes/scene_game.h"
#include "src/pdantler/mooselib.h"

#define DEFAULT_BIRD_VELOCITY 5
#define DEBOUNCER_DELAY 5
#define SCREEN_WIDTH 400
#define AUDIENCE_MEMBER_COUNT 1
#define SPEECH_BUBBLE_LEN 50

typedef struct {
  MLIBPoint location;
  bool right;
} AudienceMember;

typedef struct {
  MLIBSize size;
  MLIBPoint location;
  int velocity;
} Player;

typedef struct {
  char line1[SPEECH_BUBBLE_LEN];
  MLIBSize size;
} SpeechBubble;

typedef struct {
  Player player;
  AudienceMember audience[AUDIENCE_MEMBER_COUNT];
  SpeechBubble speech_bubble;
} GameSceneContext;

static void move_player(PlaydateAPI *pd, Player *player, GameAssets *assets,
                        int x);
static void init_player(PlaydateAPI *pd, GameSceneContext *gsc,
                        GameAssets *assets);
static void init_audience(PlaydateAPI *pd, GameSceneContext *gsc,
                          GameAssets *assets);
static void init_speech_bubble(PlaydateAPI *pd, GameSceneContext *gsc,
                               GameAssets *assets);
static void show_speech(PlaydateAPI *pd, GameSceneContext *gsc,
                        GameAssets *assets, MLIBPoint p, char *text);

// Initializes the scene. Do not change the function name/signature.
void init_game(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;

  game->debouncer.delay = DEBOUNCER_DELAY;

  GameSceneContext *gsc = pd->system->realloc(NULL, sizeof(GameSceneContext));
  game->game_userdata = gsc;
  init_player(pd, gsc, assets);
  init_audience(pd, gsc, assets);
  init_speech_bubble(pd, gsc, assets);

  show_speech(pd, gsc, assets, gsc->audience[0].location, "You suck!");
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

static void init_player(PlaydateAPI *pd, GameSceneContext *gsc,
                        GameAssets *assets) {
  gsc->player.velocity = DEFAULT_BIRD_VELOCITY;

  float x, y;
  pd->sprite->getPosition(assets->bird_sprite, &x, &y);
  gsc->player.location.x = x;
  gsc->player.location.y = y;

  pd->graphics->getBitmapData(assets->bird_image, &gsc->player.size.width,
                              &gsc->player.size.height, NULL, NULL, NULL);
}

static void init_speech_bubble(PlaydateAPI *pd, GameSceneContext *gsc,
                               GameAssets *assets) {
  pd->graphics->getBitmapData(
      assets->speech_bubble_base_image, &gsc->speech_bubble.size.width,
      &gsc->speech_bubble.size.height, NULL, NULL, NULL);
}

static void init_audience(PlaydateAPI *pd, GameSceneContext *gsc,
                          GameAssets *assets) {

  gsc->audience[0].location = MLIBPOINT_CREATE(45, 200);
}

// Text will be copied and can be deallocated
static void show_speech(PlaydateAPI *pd, GameSceneContext *gsc,
                        GameAssets *assets, MLIBPoint p, char *text) {
  pd->graphics->pushContext(assets->speech_bubble_image);
  pd->graphics->drawBitmap(assets->speech_bubble_base_image, 0, 0,
                           kBitmapUnflipped);
  pd->graphics->popContext();

  pd->sprite->setVisible(assets->speech_bubble_sprite, true);
  pd->sprite->moveTo(assets->speech_bubble_sprite,
                     p.x + gsc->speech_bubble.size.width / 2,
                     p.y - gsc->speech_bubble.size.height / 2);
}
