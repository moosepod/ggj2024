#include "src/scenes/scene_game.h"
#include "src/pdantler/mooselib.h"
#include "src/pdantler/printf.h"

#define DEFAULT_BIRD_VELOCITY 5
#define DEBOUNCER_DELAY 5
#define SCREEN_WIDTH 400
#define AUDIENCE_MEMBER_COUNT 6
#define HECKLE_COUNT 11
#define JOKE_COUNT 15
#define AFTER_JOKE_WAIT_S 3.0
#define SPEECH_BUBBLE_LEN 50
#define INITIAL_HECKLE_S 2.0
#define SCREEN_BOUNDS                                                          \
  (MLIBRect) { 0, 0, 400, 240 }
#define INITIAL_HOOK_VELOCITY 2.0f
#define INITIAL_TOMATO_VELOCITY 5.0f
#define HOOK_VELOCITY_INCREMENT 0.5f
#define TOMATO_VELOCITY_INCREMENT 0.5f
#define HECKLE_WAIT_INCREMENT 0.05f
#define PLAYER_ANIMATION_FREQUENCY 3
#define JOKE_SOUND_COUNT 5
#define HECKLE_SOUND_COUNT 8

#define MAX_VELOCITY 15

// Uncomment to go into demo mode that doesn't randomize audience
//#define DEMO_MODE

typedef enum {
  STATE_JOKE,
  STATE_HECKLING,
  STATE_THROWING,
  STATE_PULLING,
  STATE_GAME_OVER
} GameState;

typedef struct {
  MLIBPoint speech_location;
  MLIBPoint throw_location;
  bool right;
  bool hook;
} AudienceMember;

typedef struct {
  MLIBSize size;
  MLIBPoint location;
  MLIBPoint start_location;
  MLIBRect hitbox;
  int velocity;
  long animation_index;
  bool mouth_open;
  bool flipped;
} Player;

typedef struct {
  MLIBFPoint location;
  MLIBFPoint velocity;
  bool active;
} Tomato;

typedef struct {
  MLIBFPoint location;
  MLIBFPoint velocity;
  MLIBSize size;
  bool active;
  bool flipped;
  bool pull_player;
} Hook;

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
  Hook hook;
  int audience_index;
  char *heckles[HECKLE_COUNT];
  char *jokes[JOKE_COUNT];
  int heckle_index;
  int joke_index;
  float change_state_time;
  MLIBPoint player_location_bookmark;
  GameState state;
  float current_hook_velocity;
  float current_tomato_velocity;
  float current_heckle_wait;
  int last_joke_text_index;
  int last_heckle_text_index;
  int last_heckle_sound_index;
  int last_joke_sound_index;
  int last_audience_index;
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
static void update_hook(GameContext *game, GameAssets *assets);
static void handle_movement(PlaydateAPI *pd, GameSceneContext *gsc,
                            PDButtons debounced_buttons, GameAssets *assets);

static void throw_tomato(GameContext *game, GameAssets *assets, MLIBPoint start,
                         MLIBPoint end, float velocity);
static void hide_tomato(GameContext *game, GameAssets *assets);
static void start_hook(GameContext *game, GameAssets *assets, MLIBPoint start,
                       MLIBPoint end, int velocity);
static void hide_hook(GameContext *game, GameAssets *assets);
static void kill_player(GameContext *game, GameAssets *assets);
static void reset_game(GameContext *game, GameAssets *assets);
static void start_game(GameContext *game, GameAssets *assets);
static void new_joke(GameContext *game, GameAssets *assets);
static void hide_joke(GameContext *game, GameAssets *assets);

static void play_random_joke_sound(GameContext *game, GameAssets *assets);
static void play_random_heckle_sound(GameContext *game, GameAssets *assets);

static int rand_int_range(int min, int max) {
  if (min == max) {
    return min;
  }

  return (rand() % (max - min)) + min;
}

static int rand_int_range_skip(int min, int max, int skip) {
  int v;
  do {
    v = rand_int_range(min, max);
  } while (v == skip);

  return v;
}

// Initializes the scene. Do not change the function name/signature.
void init_game(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;

  GameSceneContext *gsc = pd->system->realloc(NULL, sizeof(GameSceneContext));
  game->game_userdata = gsc;
  init_player(pd, gsc, assets);
  init_audience(pd, gsc, assets);
  init_speech_bubble(pd, gsc, assets);
  init_jokes(pd, gsc, assets);
  init_misc(pd, gsc, assets);
}

// Run once every game loop when scene is active. Do not change the function
// name/signature Return the scene to transfer to, or NO_SCENE if same scene
// should be used
GameScene tick_game(GameContext *game, GameAssets *assets,
                    PDButtons debounced_buttons, int delta) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;

  switch (gsc->state) {
  case STATE_GAME_OVER:
    if ((debounced_buttons & kButtonA) || (debounced_buttons & kButtonB)) {
      start_game(game, assets);
    }
    break;
  case STATE_HECKLING:
  case STATE_THROWING:
  case STATE_JOKE:
    handle_movement(pd, gsc, debounced_buttons, assets);
  // Fallthrough to default deliberate here
  default:
    update_tomato(game, assets);
    update_hook(game, assets);
    update_state(game, assets);
  }

  return NO_SCENE;
}

void enter_game(GameContext *game, GameAssets *assets) {
  game->debouncer.delay = DEBOUNCER_DELAY;

  reset_game(game, assets);
}

void exit_game(GameContext *game, GameAssets *assets) {}

void pause_game(GameContext *game, GameAssets *assets) {}

void unpause_game(GameContext *game, GameAssets *assets) {}

//
// Main function
//
static void move_player(PlaydateAPI *pd, Player *player, GameAssets *assets,
                        int x) {

  int new_x =
      MLIB_CLAMP_TO_RANGE(player->location.x + x, player->size.width / 2,
                          SCREEN_WIDTH - player->size.width / 2);

  if (new_x != player->location.x) {
    player->flipped = new_x < player->location.x;
    pd->sprite->setImageFlip(assets->bird_sprite, player->flipped);
    player->location.x = new_x;
    pd->sprite->moveTo(assets->bird_sprite, player->location.x,
                       player->location.y);
  }
}

static void init_player(PlaydateAPI *pd, GameSceneContext *gsc,
                        GameAssets *assets) {
  gsc->player.velocity = DEFAULT_BIRD_VELOCITY;

  float x, y;
  pd->sprite->getPosition(assets->bird_sprite, &x, &y);
  gsc->player.location.x = x;
  gsc->player.location.y = y;
  gsc->player.start_location.x = x;
  gsc->player.start_location.y = y;
  gsc->player.flipped = false;

  gsc->player.hitbox = MLIBRECT_CREATE(16, 16, 80, 50);

  pd->graphics->getBitmapData(assets->bird_image, &gsc->player.size.width,
                              &gsc->player.size.height, NULL, NULL, NULL);
}

static void init_misc(PlaydateAPI *pd, GameSceneContext *gsc,
                      GameAssets *assets) {
  srand(pd->system->getCurrentTimeMilliseconds());

  gsc->tomato.active = false;
  gsc->hook.active = false;

  pd->graphics->getBitmapData(assets->hook_image, &gsc->hook.size.width,
                              &gsc->hook.size.height, NULL, NULL, NULL);
}

static void init_speech_bubble(PlaydateAPI *pd, GameSceneContext *gsc,
                               GameAssets *assets) {
  pd->graphics->getBitmapData(
      assets->speech_bubble_base_image, &gsc->speech_bubble.size.width,
      &gsc->speech_bubble.size.height, NULL, NULL, NULL);

  gsc->heckle_index = 0;
  gsc->heckles[gsc->heckle_index++] = "Get off the stage!";
  gsc->heckles[gsc->heckle_index++] = "Booooo!";
  gsc->heckles[gsc->heckle_index++] = "Get new material!";
  gsc->heckles[gsc->heckle_index++] = "You suck!";
  gsc->heckles[gsc->heckle_index++] = "Go home!";
  gsc->heckles[gsc->heckle_index++] = "Get a job!";
  gsc->heckles[gsc->heckle_index++] = "You're not funny!";
  gsc->heckles[gsc->heckle_index++] = "I want a refund!";
  gsc->heckles[gsc->heckle_index++] = "You call this funny?";
  gsc->heckles[gsc->heckle_index++] = "What is this?";
  gsc->heckles[gsc->heckle_index++] = "Next comic!";
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
  gsc->jokes[gsc->joke_index++] =
      "Spring is here! I got so excited I wet my plants!";
  gsc->jokes[gsc->joke_index++] = "A pony with a cough is just a little horse.";
  gsc->jokes[gsc->joke_index++] =
      "I used to hate facial hair, but it grew on me.";
  gsc->jokes[gsc->joke_index++] =
      "I tell dad jokes but have no kids. I'm a faux pa.";
  gsc->jokes[gsc->joke_index++] =
      "What is a funny mountain called? Hill-arious!";
  gsc->jokes[gsc->joke_index++] =
      "Why do we drink water? Because we can't eat it.";
  gsc->jokes[gsc->joke_index++] =
      "What do you call a factory that makes okay products? Satisfactory.";

  gsc->joke_index = 0;
}

static void init_audience(PlaydateAPI *pd, GameSceneContext *gsc,
                          GameAssets *assets) {

  gsc->audience[0].speech_location = MLIBPOINT_CREATE(45, 200);
  gsc->audience[0].throw_location = MLIBPOINT_CREATE(69, 178);
  gsc->audience[0].right = false;
  gsc->audience[0].hook = false;

  gsc->audience[1].speech_location = MLIBPOINT_CREATE(130, 200);
  gsc->audience[1].throw_location = MLIBPOINT_CREATE(178, 168);
  gsc->audience[1].right = false;
  gsc->audience[1].hook = false;

  gsc->audience[2].speech_location = MLIBPOINT_CREATE(200, 200);
  gsc->audience[2].throw_location = MLIBPOINT_CREATE(199, 187);
  gsc->audience[2].right = false;
  gsc->audience[2].hook = false;

  gsc->audience[3].speech_location = MLIBPOINT_CREATE(128, 200);
  gsc->audience[3].throw_location = MLIBPOINT_CREATE(275, 175);
  gsc->audience[3].right = true;
  gsc->audience[3].hook = false;

  gsc->audience[4].speech_location = MLIBPOINT_CREATE(190, 200);
  gsc->audience[4].throw_location = MLIBPOINT_CREATE(312, 140);
  gsc->audience[4].right = true;
  gsc->audience[4].hook = false;

  gsc->audience[5].speech_location = MLIBPOINT_CREATE(250, 75);
  gsc->audience[5].throw_location =
      MLIBPOINT_CREATE(550, gsc->player.location.y);
  gsc->audience[5].right = true;
  gsc->audience[5].hook = true;

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
  pd->sprite->markDirty(assets->speech_bubble_sprite);
  pd->sprite->setOpaque(assets->speech_bubble_sprite, false);
  pd->sprite->moveTo(assets->speech_bubble_sprite,
                     p.x + gsc->speech_bubble.size.width / 2,
                     p.y - gsc->speech_bubble.size.height / 2);

  play_random_heckle_sound(game, assets);
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

  if (!pd->system->isCrankDocked()) {
    float dx = pd->system->getCrankChange();
    int velocity = MLIB_CLAMP_TO_RANGE((int)abs(dx), 0, MAX_VELOCITY);
    if (velocity) {
      if (dx < 0) {
        move_player(pd, &gsc->player, assets, -1 * velocity);
      } else {
        move_player(pd, &gsc->player, assets, velocity);
      }
    }
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

static void update_hook(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;
  if (gsc->hook.active) {
    gsc->hook.location.x += gsc->hook.velocity.x;

    MLIBPoint p =
        MLIBPOINT_CREATE(gsc->hook.location.x - (gsc->hook.size.width / 2),
                         gsc->hook.location.y);

    PDRect bounds = pd->sprite->getBounds(assets->bird_sprite);
    if (MLIB_POINT_IN_RECT(p, MLIBRECT_CREATE(bounds.x + gsc->player.hitbox.x,
                                              bounds.y + gsc->player.hitbox.y,
                                              gsc->player.hitbox.width,
                                              gsc->player.hitbox.height))) {
      gsc->hook.velocity.x *= -1;
      gsc->hook.location.x += gsc->hook.velocity.x;
      gsc->hook.pull_player = true;
      gsc->state = STATE_PULLING;
      pd->sprite->setVisible(assets->hooked_crow_sprite, true);
      pd->sprite->setVisible(assets->hook_sprite, false);
      pd->sprite->setVisible(assets->bird_sprite, false);
      pd->sprite->moveTo(assets->hooked_crow_sprite, (int)gsc->hook.location.x,
                         (int)gsc->hook.location.y);
    } else if (MLIB_POINT_IN_RECT(
                   p, assets->targets[TARGET_GAME_STAGE_CENTER].rect)) {
      gsc->hook.velocity.x *= -2;
      gsc->hook.location.x += gsc->hook.velocity.x;
      gsc->hook.pull_player = false;
      pd->sprite->moveTo(assets->hook_sprite, (int)gsc->hook.location.x,
                         (int)gsc->hook.location.y);
    } else if (MLIB_POINT_IN_RECT(
                   p, assets->targets[TARGET_GAME_STAGE_RIGHT].rect)) {
      hide_hook(game, assets);
      if (gsc->hook.pull_player) {
        kill_player(game, assets);
      } else {
        new_joke(game, assets);
      }
    } else {
      if (gsc->hook.pull_player) {
        pd->sprite->moveTo(assets->hooked_crow_sprite,
                           (int)gsc->hook.location.x,
                           (int)gsc->hook.location.y);
      } else {
        pd->sprite->moveTo(assets->hook_sprite, (int)gsc->hook.location.x,
                           (int)gsc->hook.location.y);
      }
    }
  }
}

static void update_state(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;

  switch (gsc->state) {
  case STATE_JOKE:
    if (gsc->player.animation_index++ % PLAYER_ANIMATION_FREQUENCY == 0) {
      gsc->player.mouth_open = !gsc->player.mouth_open;
      if (gsc->player.mouth_open) {
        pd->sprite->setImage(assets->bird_sprite, assets->bird_mouth_open_image,
                             gsc->player.flipped);
      } else {
        pd->sprite->setImage(assets->bird_sprite, assets->bird_image,
                             gsc->player.flipped);
      }
    }
    if (pd->system->getElapsedTime() > gsc->change_state_time) {
#ifdef DEMO_MODE
      gsc->audience_index = MLIB_CLAMP_TO_RANGE_MOD(gsc->audience_index + 1, 0,
                                                    AUDIENCE_MEMBER_COUNT - 1);
#endif
#ifndef DEMO_MODE
      gsc->audience_index = rand_int_range_skip(0, AUDIENCE_MEMBER_COUNT,
                                                gsc->last_audience_index);
      gsc->last_audience_index = gsc->audience_index;
#endif
      if (gsc->audience[gsc->audience_index].hook) {
        gsc->heckle_index = 0;
      } else {
        gsc->heckle_index =
            rand_int_range_skip(0, HECKLE_COUNT, gsc->last_heckle_text_index);
        gsc->last_heckle_text_index = gsc->heckle_index;
      }

      show_speech(game, assets,
                  gsc->audience[gsc->audience_index].speech_location,
                  gsc->heckles[gsc->heckle_index],
                  !gsc->audience[gsc->audience_index].right);

      gsc->change_state_time = gsc->current_heckle_wait;
      gsc->state = STATE_HECKLING;
      pd->system->resetElapsedTime();
      gsc->player_location_bookmark = gsc->player.location;
      hide_joke(game, assets);
    }
    break;
  case STATE_HECKLING:
    if (pd->system->getElapsedTime() > gsc->change_state_time) {
      hide_speech(game, assets);

      if (gsc->audience[gsc->audience_index].hook) {
        start_hook(game, assets,
                   gsc->audience[gsc->audience_index].throw_location,
                   gsc->player_location_bookmark, gsc->current_hook_velocity);
      } else {
        throw_tomato(
            game, assets, gsc->audience[gsc->audience_index].throw_location,
            gsc->player_location_bookmark, gsc->current_tomato_velocity);
      }
    }
    break;
  default:
    break;
  }
}

static void throw_tomato(GameContext *game, GameAssets *assets, MLIBPoint start,
                         MLIBPoint end, float velocity) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;

  gsc->tomato.active = true;
  gsc->tomato.location = MLIBFPOINT_CREATE(start.x, start.y);
  MLIBSize s = MLIBSIZE_CREATE(end.x - start.x, end.y - start.y);
  float m = sqrt(s.width * s.width + s.height * s.height);
  MLIBFPoint v = MLIBFPOINT_CREATE((float)s.width / m, (float)s.height / m);
  gsc->tomato.velocity = MLIBFPOINT_CREATE(velocity * v.x, velocity * v.y);
  pd->sprite->moveTo(assets->tomato_sprite, start.x, start.y);
  pd->sprite->setVisible(assets->tomato_sprite, true);

  gsc->current_tomato_velocity += TOMATO_VELOCITY_INCREMENT;
  gsc->state = STATE_THROWING;
}

static void hide_tomato(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;
  gsc->tomato.active = false;
  pd->sprite->setVisible(assets->tomato_sprite, false);
}

static void start_hook(GameContext *game, GameAssets *assets, MLIBPoint start,
                       MLIBPoint end, int velocity) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;

  gsc->hook.active = true;
  gsc->hook.pull_player = false;
  gsc->hook.location = MLIBFPOINT_CREATE(start.x, start.y);
  gsc->hook.velocity = MLIBFPOINT_CREATE(-1 * velocity, 0);
  pd->sprite->moveTo(assets->hook_sprite, start.x, start.y);
  pd->sprite->setVisible(assets->hook_sprite, true);
  gsc->current_hook_velocity += HOOK_VELOCITY_INCREMENT;
  gsc->state = STATE_THROWING;
}

static void hide_hook(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;
  gsc->hook.active = false;
  pd->sprite->setVisible(assets->hook_sprite, false);
  pd->sprite->setVisible(assets->hooked_crow_sprite, false);
}

static void kill_player(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;
  pd->sprite->setImageFlip(assets->bird_sprite, kBitmapFlippedY);

  reset_game(game, assets);
}

static void reset_game(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;

  gsc->state = STATE_GAME_OVER;
  pd->sprite->setVisible(assets->start_text_sprite, true);
}

static void start_game(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;

  gsc->player.location.x = gsc->player.start_location.x;
  gsc->player.location.y = gsc->player.start_location.y;
  gsc->player.animation_index = 0;
  gsc->player.mouth_open = false;
  pd->sprite->setImage(assets->bird_sprite, assets->bird_image,
                       kBitmapUnflipped);
  pd->sprite->moveTo(assets->bird_sprite, gsc->player.location.x,
                     gsc->player.location.y);
  pd->sprite->setImageFlip(assets->bird_sprite, kBitmapUnflipped);
  pd->sprite->setVisible(assets->start_text_sprite, false);
  pd->sprite->setVisible(assets->bird_sprite, true);

  gsc->current_hook_velocity = INITIAL_HOOK_VELOCITY;
  gsc->current_tomato_velocity = INITIAL_TOMATO_VELOCITY;
  gsc->current_heckle_wait = INITIAL_HECKLE_S;

  gsc->last_joke_text_index = -1;
  gsc->last_heckle_text_index = -1;
  gsc->last_heckle_sound_index = -1;
  gsc->last_joke_sound_index = -1;
  gsc->last_audience_index = -1;

  new_joke(game, assets);
}

static void new_joke(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;
  gsc->state = STATE_JOKE;
  gsc->change_state_time = AFTER_JOKE_WAIT_S;
  pd->system->resetElapsedTime();
  gsc->joke_index =
      rand_int_range_skip(0, JOKE_COUNT, gsc->last_joke_text_index);
  gsc->last_joke_text_index = gsc->joke_index;
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
  gsc->current_heckle_wait -= HECKLE_WAIT_INCREMENT;
  play_random_joke_sound(game, assets);
}

static void hide_joke(GameContext *game, GameAssets *assets) {
  PlaydateAPI *pd = game->pd;
  pd->sprite->setVisible(assets->joke_bubble_sprite, false);
}

static void play_random_joke_sound(GameContext *game, GameAssets *assets) {
  AudioSample *sample;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;

  int joke_index =
      rand_int_range_skip(0, JOKE_SOUND_COUNT, gsc->last_joke_sound_index);
  gsc->last_joke_sound_index = joke_index;

  switch (joke_index) {
  case 0:
    sample = assets->joke1_sample;
    break;
  case 1:
    sample = assets->joke2_sample;
    break;
  case 2:
    sample = assets->joke3_sample;
    break;
  case 3:
    sample = assets->joke4_sample;
    break;
  case 4:
    sample = assets->joke5_sample;
    break;
  default:
    sample = NULL;
  }

  pdantler_channel_start(game, sample, CHANNEL_SFX_1);
}

static void play_random_heckle_sound(GameContext *game, GameAssets *assets) {
  AudioSample *sample;
  GameSceneContext *gsc = (GameSceneContext *)game->game_userdata;

  int heckle_index =
      rand_int_range_skip(0, HECKLE_SOUND_COUNT, gsc->last_heckle_sound_index);
  gsc->last_heckle_sound_index = heckle_index;

  switch (heckle_index) {
  case 0:
    sample = assets->heckle1_sample;
    break;
  case 1:
    sample = assets->heckle2_sample;
    break;
  case 2:
    sample = assets->heckle3_sample;
    break;
  case 3:
    sample = assets->heckle4_sample;
    break;
  case 4:
    sample = assets->heckle5_sample;
    break;
  case 5:
    sample = assets->heckle6_sample;
    break;
  case 6:
    sample = assets->heckle7_sample;
    break;
  case 7:
    sample = assets->heckle8_sample;
    break;
  default:
    sample = NULL;
  }

  pdantler_channel_start(game, sample, CHANNEL_SFX_1);
}
