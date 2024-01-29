/** This file is managed by pdantler.

    Any changes you make to it will be overwritten next time pdantler is run.
*/

#include "../common.h"

/** Use the settings on the debouncer to debounce any controls. This turns a
 * continuous button press into a discrete one */
PDButtons pdantler_debounce_input(PlaydateAPI *pd, Debouncer *debouncer) {
  PDButtons current;
  pd->system->getButtonState(&current, NULL, NULL);
  if (current == debouncer->last_buttons &&
      pd->system->getCurrentTimeMilliseconds() < debouncer->debounce_time) {
    // If button presses unchanged, act as if nothing is being pressed
    return 0;
  }
  debouncer->last_buttons = current;
  debouncer->debounce_time =
      pd->system->getCurrentTimeMilliseconds() + debouncer->delay;

  return current;
}

static SamplePlayer *pdantler_get_player(GameContext *game,
                                         GameSampleChannel channel) {
  switch (channel) {
  case CHANNEL_MUSIC:
    return game->music_player;
    break;
  case CHANNEL_SFX_1:
    return game->sfx_1_player;
    break;
  case CHANNEL_SFX_2:
    return game->sfx_2_player;
    break;
  };

  return NULL;
}

void pdantler_channel_start(GameContext *game, AudioSample *sample,
                            GameSampleChannel channel) {
  PlaydateAPI *pd = game->pd;
  SamplePlayer *player = pdantler_get_player(game, channel);
  if (channel == CHANNEL_MUSIC && !game->music_active) {
    return;
  }
  if (player && pd->sound->sampleplayer->isPlaying(player)) {
    pd->sound->sampleplayer->stop(player);
  }
  pd->sound->sampleplayer->setSample(player, sample);
  pd->sound->sampleplayer->play(player, 1, 1.0);
}

void pdantler_channel_stop(GameContext *game, GameSampleChannel channel) {
  PlaydateAPI *pd = game->pd;
  SamplePlayer *player = pdantler_get_player(game, channel);

  if (player && pd->sound->sampleplayer->isPlaying(player)) {
    pd->sound->sampleplayer->stop(player);
  }
}

void pdantler_channel_set_paused(GameContext *game, GameSampleChannel channel,
                                 bool paused) {
  PlaydateAPI *pd = game->pd;
  SamplePlayer *player = pdantler_get_player(game, channel);

  if (player) {
    pd->sound->sampleplayer->setPaused(player, paused);
  }
}
