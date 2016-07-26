
#ifndef SOUND_MANAGER_H_
#define SOUND_MANAGER_H_
#pragma once

#include "hge.h"

enum BgMusic {
  BGM_NORMAL_1 = 0,
  BGM_NORMAL_2,
  BGM_NORMAL_3,
  BGM_NORMAL_4,

  BGM_COUNT
};

enum GameEffect {
  CANNON_SWITCH = 0,
  CASTING,
  CATCH,
  FIRE,
  SUPERARM,
  BINGO,
  WAVE,

  GAME_EFFECT_COUNT
};

class SoundManager {
 public:
  ~SoundManager();

  static SoundManager& GetInstance();
  static void DelInstance();

  bool LoadGameResource();

  void PlayBackMusic();
  void StopBackMusic();
  void PlayGameEffect(GameEffect game_effect);
  void PlayFishEffect(FishKind fish_kind);

  void MuteSwitch();

 private:
  SoundManager();
  SoundManager(const SoundManager&);
  void operator=(const SoundManager&);

 private:
  static SoundManager* sound_manager_;

  HGE* hge_;

  //HSTREAM stream_bgm_[BGM_COUNT];//sdp20140627由于声音资源加密，无解密工具，且需要修改声音，在此屏蔽功能，在游戏客户端主类中实现
  HEFFECT game_effect_[GAME_EFFECT_COUNT];

  BgMusic bg_music_;

  int volume_;
};

#endif // SOUND_MANAGER_H_