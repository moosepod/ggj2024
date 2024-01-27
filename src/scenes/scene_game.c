#include "src/scenes/scene_game.h"
#include "src/pdantler/mooselib.h"
#include "src/pdantler/printf.h"

#define DEFAULT_BIRD_VELOCITY 5
#define DEBOUNCER_DELAY 5
#define SCREEN_WIDTH 400
#define AUDIENCE_MEMBER_COUNT 5
#define HECKLE_COUNT 7
#define SPEECH_BUBBLE_LEN 50
#define AFTER_HECKLE_WAIT_S 2.0
#define AFTER_THROW_WAIT_S 2.0

typedef enum {
  STATE_WAIT_TO_HECKLE,
  STATE_HECKLE,
  STATE_WAIT_TO_THROW,
  STATE_THROW
} GameState;

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
  AudienceMember audience[AUDIENCE_MEMBER_COUNT];
  int audience_index;
  char *heckles[HECKLE_COUNT];
  int heckle_index;
  float change_state_time;
  GameState state;
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
static void hide_speech(GameContext *game, GameAssets *assets);

// Initializes the scene. Do not change the function name/signature.
void init_game(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;

  game->debouncer.delay = DEBOUNCER_DELAY;

  GameSceneContext *gsc = pd->system->realloc(NULL, sizeof(GameSceneContext));
  game->game_userdata = gsc;
  init_player(pd, gsc, assets);
  init_audience(pd, gsc, assets);
  init_speech_bubble(pd, gsc, assets);

  gsc->state = STATE_WAIT_TO_HECKLE;
  gsc->change_state_time = AFTER_THROW_WAIT_S;
  pd->system->resetElapsedTime();
}

// Run once every game loop when scene is active. Do not change the function
// name/signature Return the scene to transfer to, or NO_SCENE if same scene
// should be used
GameScene tick_game(GameContext *game, GameAssets *assets,
                    PDButtons debounced_buttons, int delta) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;
  if ((debounced_buttons & kButtonLeft)) {
    move_player(game->pd, &gsc->player, assets, -1 * gsc->player.velocity);
  } else if ((debounced_buttons & kButtonRight)) {
    move_player(game->pd, &gsc->player, assets, gsc->player.velocity);
  }

  switch (gsc->state) {
  case STATE_WAIT_TO_HECKLE:
    if (pd->system->getElapsedTime() > gsc->change_state_time) {
      show_speech(game, assets, gsc->audience[gsc->audience_index].location,
                  gsc->heckles[gsc->heckle_index],
                  !gsc->audience[gsc->audience_index].right);
      gsc->audience_index = MLIB_CLAMP_TO_RANGE_MOD(gsc->audience_index + 1, 0,
                                                    AUDIENCE_MEMBER_COUNT - 1);
      gsc->heckle_index =
          MLIB_CLAMP_TO_RANGE_MOD(gsc->heckle_index + 1, 0, HECKLE_COUNT - 1);

      gsc->change_state_time = AFTER_HECKLE_WAIT_S;
      gsc->state = STATE_WAIT_TO_THROW;
      pd->system->resetElapsedTime();
    };
    break;
  case STATE_WAIT_TO_THROW:
    if (pd->system->getElapsedTime() > gsc->change_state_time) {
      gsc->change_state_time = AFTER_THROW_WAIT_S;
      pd->system->resetElapsedTime();
      hide_speech(game, assets);
      gsc->state = STATE_WAIT_TO_HECKLE;
    }
    break;
  default:
    break;
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
  pd->sprite->setImageFlip(assets->bird_sprite, x < 0);
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

  gsc->heckle_index = 0;
  gsc->heckles[gsc->heckle_index++] = "Booooo!";
  gsc->heckles[gsc->heckle_index++] = "Get new material!";
  gsc->heckles[gsc->heckle_index++] = "You suck!";
  gsc->heckles[gsc->heckle_index++] = "Go home!";
  gsc->heckles[gsc->heckle_index++] = "Get a job!";
  gsc->heckles[gsc->heckle_index++] = "You're not funny!";
  gsc->heckles[gsc->heckle_index++] = "I want a refund!";
  gsc->heckle_index = 0;
}

static void init_audience(PlaydateAPI *pd, GameSceneContext *gsc,
                          GameAssets *assets) {

  gsc->audience[0].location = MLIBPOINT_CREATE(45, 200);
  gsc->audience[0].right = false;

  gsc->audience[1].location = MLIBPOINT_CREATE(130, 200);
  gsc->audience[1].right = false;

  gsc->audience[2].location = MLIBPOINT_CREATE(200, 200);
  gsc->audience[2].right = false;

  gsc->audience[3].location = MLIBPOINT_CREATE(128, 200);
  gsc->audience[3].right = true;

  gsc->audience[4].location = MLIBPOINT_CREATE(190, 200);
  gsc->audience[4].right = true;

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

static void hide_speech(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  pd->sprite->setVisible(assets->speech_bubble_sprite, false);
}
