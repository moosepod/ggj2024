#include "src/scenes/scene_game.h"
#include "src/pdantler/mooselib.h"
#include "src/pdantler/printf.h"

#define DEFAULT_BIRD_VELOCITY 5
#define DEBOUNCER_DELAY 5
#define SCREEN_WIDTH 400
#define AUDIENCE_MEMBER_COUNT 7
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
  bool flipped;
} SpeechBubble;

typedef struct {
  Player player;
  SpeechBubble speech_bubble;
  int audience_index;
  AudienceMember audience[AUDIENCE_MEMBER_COUNT];
} GameSceneContext;

static void move_player(PlaydateAPI *pd, Player *player, GameAssets *assets,
                        int x);
static void init_player(PlaydateAPI *pd, GameSceneContext *gsc,
                        GameAssets *assets);
static void init_audience(PlaydateAPI *pd, GameSceneContext *gsc,
                          GameAssets *assets);
static void init_speech_bubble(PlaydateAPI *pd, GameSceneContext *gsc,
                               GameAssets *assets);
static void show_speech(GameContext *game, GameAssets *assets, MLIBPoint p,
                        char *text, bool flipped);

// Initializes the scene. Do not change the function name/signature.
void init_game(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;

  game->debouncer.delay = DEBOUNCER_DELAY;

  GameSceneContext *gsc = pd->system->realloc(NULL, sizeof(GameSceneContext));
  game->game_userdata = gsc;
  init_player(pd, gsc, assets);
  init_audience(pd, gsc, assets);
  init_speech_bubble(pd, gsc, assets);

  show_speech(game, assets, gsc->audience[gsc->audience_index].location,
              "Get new material!", !gsc->audience[gsc->audience_index].right);
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
  } else if ((debounced_buttons & kButtonA)) {
    gsc->audience_index = MLIB_CLAMP_TO_RANGE_MOD(gsc->audience_index + 1, 0,
                                                  AUDIENCE_MEMBER_COUNT - 1);
    show_speech(game, assets, gsc->audience[gsc->audience_index].location,
                "Get new material!", !gsc->audience[gsc->audience_index].right);
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
  gsc->audience[0].right = false;

  gsc->audience[1].location = MLIBPOINT_CREATE(100, 200);
  gsc->audience[1].right = false;

  gsc->audience[2].location = MLIBPOINT_CREATE(157, 200);
  gsc->audience[2].right = false;

  gsc->audience[3].location = MLIBPOINT_CREATE(221, 200);
  gsc->audience[3].right = false;

  gsc->audience[4].location = MLIBPOINT_CREATE(128, 200);
  gsc->audience[4].right = true;

  gsc->audience[5].location = MLIBPOINT_CREATE(180, 200);
  gsc->audience[5].right = true;

  gsc->audience[6].location = MLIBPOINT_CREATE(240, 200);
  gsc->audience[6].right = true;

  gsc->audience_index = 0;
}

// Text will be copied and can be deallocated
static void show_speech(GameContext *game, GameAssets *assets, MLIBPoint p,
                        char *text, bool flipped) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;
  snprintf(gsc->speech_bubble.line1, SPEECH_BUBBLE_LEN, text);
  pd->graphics->pushContext(assets->speech_bubble_image);
  pd->graphics->fillRect(0, 0, gsc->speech_bubble.size.width,
                         gsc->speech_bubble.size.height, kColorWhite);

  if (flipped) {
    pd->graphics->drawBitmap(assets->speech_bubble_base_image, 0, 0,
                             kBitmapUnflipped);
  } else {
    pd->graphics->drawBitmap(assets->speech_bubble_base_image, 0, 0,
                             kBitmapFlippedX);
  }

  pd->graphics->setFont(game->main_font);
  int tracking = pd->graphics->getTextTracking();
  int width = pd->graphics->getTextWidth(
      game->main_font, gsc->speech_bubble.line1,
      strlen(gsc->speech_bubble.line1), kASCIIEncoding, tracking);

  pd->graphics->drawText(gsc->speech_bubble.line1,
                         strlen(gsc->speech_bubble.line1), kASCIIEncoding,
                         (gsc->speech_bubble.size.width - width) / 2, 15);
  pd->graphics->popContext();

  pd->sprite->setVisible(assets->speech_bubble_sprite, true);
  pd->sprite->moveTo(assets->speech_bubble_sprite,
                     p.x + gsc->speech_bubble.size.width / 2,
                     p.y - gsc->speech_bubble.size.height / 2);
}
