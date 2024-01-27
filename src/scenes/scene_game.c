#include "src/scenes/scene_game.h"
#include "src/pdantler/mooselib.h"
#include "src/pdantler/printf.h"

#define DEFAULT_BIRD_VELOCITY 5
#define DEBOUNCER_DELAY 5
#define SCREEN_WIDTH 400
#define AUDIENCE_MEMBER_COUNT 5
#define HECKLE_COUNT 7
#define JOKE_COUNT 8
#define SPEECH_BUBBLE_LEN 50
#define AFTER_JOKE_WAIT_S 2.0
#define HECKLE_S 2.0
#define AFTER_THROW_WAIT_S 2.0
#define SCREEN_BOUNDS                                                          \
  (MLIBRect) { 0, 0, 400, 240 }

typedef enum {
  STATE_JOKE,
  STATE_HECKLING,
  STATE_THROWING,
  STATE_GAME_OVER
} GameState;

typedef struct {
  MLIBPoint speech_location;
  MLIBPoint throw_location;
  bool right;
} AudienceMember;

typedef struct {
  MLIBSize size;
  MLIBPoint location;
  MLIBRect hitbox;
  int velocity;
} Player;

typedef struct {
  MLIBFPoint location;
  MLIBFPoint velocity;
  bool active;
} Tomato;

typedef struct {
  char line1[SPEECH_BUBBLE_LEN];
  MLIBSize size;
  bool flipped;
} SpeechBubble;

typedef struct {
  Player player;
  SpeechBubble speech_bubble;
  AudienceMember audience[AUDIENCE_MEMBER_COUNT];
  Tomato tomato;
  int audience_index;
  char *heckles[HECKLE_COUNT];
  char *jokes[JOKE_COUNT];
  int heckle_index;
  int joke_index;
  float change_state_time;
  MLIBPoint player_location_bookmark;
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
static void init_jokes(PlaydateAPI *pd, GameSceneContext *gsc,
                       GameAssets *assets);
static void init_misc(PlaydateAPI *pd, GameSceneContext *gsc,
                      GameAssets *assets);
static void show_speech(GameContext *game, GameAssets *assets, MLIBPoint p,
                        char *text, bool flipped);
static void hide_speech(GameContext *game, GameAssets *assets);
static void update_state(GameContext *game, GameAssets *assets);
static void update_tomato(GameContext *game, GameAssets *assets);
static void handle_movement(PlaydateAPI *pd, GameSceneContext *gsc,
                            PDButtons debounced_buttons, GameAssets *assets);

static void throw_tomato(GameContext *game, GameAssets *assets, MLIBPoint start,
                         MLIBPoint end, int steps);
static void hide_tomato(GameContext *game, GameAssets *assets);
static void kill_player(GameContext *game, GameAssets *assets);
static void reset_game(GameContext *game, GameAssets *assets);
static void new_joke(GameContext *game, GameAssets *assets);
static void hide_joke(GameContext *game, GameAssets *assets);

static int rand_int_range(int min, int max) {
  return (rand() % (max - min)) + min;
}

// Initializes the scene. Do not change the function name/signature.
void init_game(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;

  game->debouncer.delay = DEBOUNCER_DELAY;

  GameSceneContext *gsc = pd->system->realloc(NULL, sizeof(GameSceneContext));
  game->game_userdata = gsc;
  init_player(pd, gsc, assets);
  init_audience(pd, gsc, assets);
  init_speech_bubble(pd, gsc, assets);
  init_jokes(pd, gsc, assets);

  reset_game(game, assets);
}

// Run once every game loop when scene is active. Do not change the function
// name/signature Return the scene to transfer to, or NO_SCENE if same scene
// should be used
GameScene tick_game(GameContext *game, GameAssets *assets,
                    PDButtons debounced_buttons, int delta) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;

  if (gsc->state == STATE_GAME_OVER) {
    if ((debounced_buttons & kButtonA) || (debounced_buttons & kButtonB)) {
      reset_game(game, assets);
    }
  } else {
    handle_movement(pd, gsc, debounced_buttons, assets);
    update_tomato(game, assets);
    update_state(game, assets);
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

  gsc->player.hitbox = MLIBRECT_CREATE(16, 16, 80, 50);

  pd->graphics->getBitmapData(assets->bird_image, &gsc->player.size.width,
                              &gsc->player.size.height, NULL, NULL, NULL);
}

static void init_misc(PlaydateAPI *pd, GameSceneContext *gsc,
                      GameAssets *assets) {
  srand(pd->system->getCurrentTimeMilliseconds());

  gsc->tomato.active = false;

  int width, height;
  pd->graphics->getBitmapData(assets->tomato_image, &width, &height, NULL, NULL,
                              NULL);
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

static void init_jokes(PlaydateAPI *pd, GameSceneContext *gsc,
                       GameAssets *assets) {
  gsc->joke_index = 0;
  gsc->jokes[gsc->joke_index++] = "Where do fruits go on vacation?"
                                  "Pear-is!";
  gsc->jokes[gsc->joke_index++] =
      "I'm afraid for the calendar. Its days are numbered!";
  gsc->jokes[gsc->joke_index++] =
      "I only know 25 letters of the alphabet. I don't know y.";
  gsc->jokes[gsc->joke_index++] =
      "Why couldn't the bicycle stand up by itself? It was two tired.";
  gsc->jokes[gsc->joke_index++] =
      "What do you call cheese that isn't yours? Nacho cheese.";
  gsc->jokes[gsc->joke_index++] =
      "Why did the chicken cross the road? To get to the other side.";
  gsc->jokes[gsc->joke_index++] =
      "Shout out to my fingers. I can count on all of them.";
  gsc->jokes[gsc->joke_index++] =
      "I made a pencil with two erasers. It was pointless.";
  gsc->joke_index = 0;
}

static void init_audience(PlaydateAPI *pd, GameSceneContext *gsc,
                          GameAssets *assets) {

  gsc->audience[0].speech_location = MLIBPOINT_CREATE(45, 200);
  gsc->audience[0].throw_location = MLIBPOINT_CREATE(69, 178);
  gsc->audience[0].right = false;

  gsc->audience[1].speech_location = MLIBPOINT_CREATE(130, 200);
  gsc->audience[1].throw_location = MLIBPOINT_CREATE(178, 168);
  gsc->audience[1].right = false;

  gsc->audience[2].speech_location = MLIBPOINT_CREATE(200, 200);
  gsc->audience[2].throw_location = MLIBPOINT_CREATE(199, 187);
  gsc->audience[2].right = false;

  gsc->audience[3].speech_location = MLIBPOINT_CREATE(128, 200);
  gsc->audience[3].throw_location = MLIBPOINT_CREATE(275, 175);
  gsc->audience[3].right = true;

  gsc->audience[4].speech_location = MLIBPOINT_CREATE(190, 200);
  gsc->audience[4].throw_location = MLIBPOINT_CREATE(312, 140);
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

static void handle_movement(PlaydateAPI *pd, GameSceneContext *gsc,
                            PDButtons debounced_buttons, GameAssets *assets) {
  if ((debounced_buttons & kButtonLeft)) {
    move_player(pd, &gsc->player, assets, -1 * gsc->player.velocity);
  } else if ((debounced_buttons & kButtonRight)) {
    move_player(pd, &gsc->player, assets, gsc->player.velocity);
  }
}

static void update_tomato(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;
  if (gsc->tomato.active) {
    gsc->tomato.location.x += gsc->tomato.velocity.x;
    gsc->tomato.location.y += gsc->tomato.velocity.y;

    MLIBPoint p =
        MLIBPOINT_CREATE(gsc->tomato.location.x, gsc->tomato.location.y);
    PDRect bounds = pd->sprite->getBounds(assets->bird_sprite);

    if (MLIB_POINT_IN_RECT(p, MLIBRECT_CREATE(bounds.x + gsc->player.hitbox.x,
                                              bounds.y + gsc->player.hitbox.y,
                                              gsc->player.hitbox.width,
                                              gsc->player.hitbox.height))) {
      hide_tomato(game, assets);
      kill_player(game, assets);
    } else if (MLIB_POINT_IN_RECT(p, SCREEN_BOUNDS)) {
      pd->sprite->moveTo(assets->tomato_sprite, (int)gsc->tomato.location.x,
                         (int)gsc->tomato.location.y);
    } else {
      new_joke(game, assets);
      hide_tomato(game, assets);
    }
  }
}
static void update_state(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;

  switch (gsc->state) {
  case STATE_JOKE:
    if (pd->system->getElapsedTime() > gsc->change_state_time) {
      gsc->audience_index = rand_int_range(0, AUDIENCE_MEMBER_COUNT);
      gsc->heckle_index = rand_int_range(0, HECKLE_COUNT);

      show_speech(game, assets,
                  gsc->audience[gsc->audience_index].speech_location,
                  gsc->heckles[gsc->heckle_index],
                  !gsc->audience[gsc->audience_index].right);

      gsc->change_state_time = HECKLE_S;
      gsc->state = STATE_HECKLING;
      pd->system->resetElapsedTime();
      gsc->player_location_bookmark = gsc->player.location;
      hide_joke(game, assets);
    }
    break;
  case STATE_HECKLING:
    if (pd->system->getElapsedTime() > gsc->change_state_time) {
      hide_speech(game, assets);

      throw_tomato(game, assets,
                   gsc->audience[gsc->audience_index].throw_location,
                   gsc->player_location_bookmark, 20);
    }
    break;
  default:
    break;
  }
}

static void throw_tomato(GameContext *game, GameAssets *assets, MLIBPoint start,
                         MLIBPoint end, int steps) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;

  gsc->tomato.active = true;
  gsc->tomato.location = MLIBFPOINT_CREATE(start.x, start.y);
  gsc->tomato.velocity =
      MLIBFPOINT_CREATE((end.x - start.x) / steps, (end.y - start.y) / steps);
  pd->sprite->moveTo(assets->tomato_sprite, start.x, start.y);
  pd->sprite->setVisible(assets->tomato_sprite, true);

  gsc->state = STATE_THROWING;
}

static void hide_tomato(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;
  gsc->tomato.active = false;
  pd->sprite->setVisible(assets->tomato_sprite, false);
}

static void kill_player(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;
  gsc->state = STATE_GAME_OVER;
  pd->sprite->setImageFlip(assets->bird_sprite, kBitmapFlippedY);
}

static void reset_game(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;

  new_joke(game, assets);

  pd->sprite->setImageFlip(assets->bird_sprite, kBitmapUnflipped);
}

static void new_joke(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;
  gsc->state = STATE_JOKE;
  gsc->change_state_time = AFTER_JOKE_WAIT_S;
  pd->system->resetElapsedTime();
  gsc->joke_index = rand_int_range(0, JOKE_COUNT);
  char *joke = gsc->jokes[gsc->joke_index];
  pd->graphics->pushContext(assets->joke_bubble_image);
  pd->graphics->fillRect(0, 0, 400, 240, kColorWhite);
  pd->graphics->drawBitmap(assets->joke_bubble_base_image, 0, 0,
                           kBitmapUnflipped);
  pd->graphics->setFont(game->main_font);
  int tracking = pd->graphics->getTextTracking();
  int width = pd->graphics->getTextWidth(game->main_font, joke, strlen(joke),
                                         kASCIIEncoding, tracking);
  pd->graphics->drawText(joke, strlen(joke), kASCIIEncoding, (384 - width) / 2,
                         10);
  pd->graphics->popContext();
  pd->sprite->setVisible(assets->joke_bubble_sprite, true);
}

static void hide_joke(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  pd->sprite->setVisible(assets->joke_bubble_sprite, false);
}
