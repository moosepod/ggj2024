/** This file is managed by pdantler.

    Any changes you make to it will be overwritten next time pdantler is run.
*/

#ifndef INCLUDE_COMMON
#define INCLUDE_COMMON
#include "pd_api.h"
#include "pdantler/pdlogger.h"
#include <stdbool.h>

#define MOOSELIB_PLAYDATE
#define PDANTLER_MAX_NAME_LEN 50

// Need to use playdate realloc functions here
typedef struct {
  void *(*realloc)(void *ptr,
                   size_t size); // ptr = NULL -> malloc, size = 0 -> free
} PlaydateMRLMemCtx;

#define MLIB_MALLOC(size, ctx)                                                 \
  (((PlaydateMRLMemCtx *)ctx)->realloc(NULL, size));
#define MLIB_REALLOC(ptr, size, ctx)                                           \
  (((PlaydateMRLMemCtx *)ctx)->realloc(ptr, size));
#define MLIB_FREE(ptr, ctx) (((PlaydateMRLMemCtx *)ctx)->realloc(ptr, 0));

#include "pdantler/mooselib.h"

#define DEFAULT_DEBOUNCE_DELAY 200

typedef struct {
  PDButtons last_buttons;
  unsigned int debounce_time;
  unsigned int delay;
} Debouncer;

typedef enum {
  NO_SCENE = 0,

  SCENE_SPLASH,

  SCENE_GAME

} GameScene;

typedef struct {
  char name[PDANTLER_MAX_NAME_LEN];
  MLIBRect rect;
} Target;

typedef enum { CHANNEL_MUSIC, CHANNEL_SFX_1, CHANNEL_SFX_2 } GameSampleChannel;

typedef struct {
  PlaydateAPI *pd;
  PlaydateMRLMemCtx *mem_ctx;
  GameScene current_scene;
  unsigned int last_tick_time;
  Debouncer debouncer;
  SamplePlayer *music_player;
  SamplePlayer *sfx_1_player;
  SamplePlayer *sfx_2_player; // Second player allows for layering sfx
  bool music_active;
  void *splash_assets;
  void *splash_userdata;
  void *game_assets;
  void *game_userdata;

  LCDFont *main_font;

} GameContext;

// Avoid using this in normal lifecycle flow. Appropriate for use
// in menu items
extern void pdantler_switch_to_scene(GameContext *game, GameScene new_scene);

extern void pdantler_channel_stop(GameContext *game, GameSampleChannel channel);
extern void pdantler_channel_start(GameContext *game, AudioSample *sample,
                                   GameSampleChannel channel);
extern void pdantler_channel_set_paused(GameContext *game,
                                        GameSampleChannel channel, bool paused);

#endif