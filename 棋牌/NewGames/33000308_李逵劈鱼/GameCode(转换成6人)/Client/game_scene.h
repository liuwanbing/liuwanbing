
#ifndef GAME_SCENE_H_
#define GAME_SCENE_H_
#pragma once

#include "hge.h"
#include "hgesprite.h"
#include "hgeanim.h"

enum SceneStyle {
  SCENE_STYLE_1 = 0,
  SCENE_STYLE_2,
  SCENE_STYLE_3,

  SCENE_STYLE_COUNT
};

class Water;

class GameScene {
 public:
  GameScene();
  ~GameScene();

 public:
  void SetSceneStyle(SceneStyle scene_style) { scene_style_ = scene_style; }
  void SetSwitchSceneStyle(/*SceneStyle scene_style*/);
  bool IsSwitchingScene();
  bool OnFrame(float delta_time);
  bool OnRender(float offset_x, float offset_y, float hscale, float vscale, bool switch_scene = false);
  bool LoadGameResource();

 private:
  FPoint GetFramePos(WORD chair_id);

 private:
  static const int kSwitchSceneFPS = 30;

 private:
  HGE* hge_;

  hgeSprite* spr_bg_[SCENE_STYLE_COUNT];
  Water* water_;
  hgeAnimation* ani_wave_;

  SceneStyle scene_style_;
  SceneStyle scene_switching_;

  float since_last_frame_;
  float scene_current_pos;
};

#endif // GAME_SCENE_H_
