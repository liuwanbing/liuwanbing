
#ifndef WATER_H_
#define WATER_H_
#pragma once

#include "hge.h"
#include "hgesprite.h"

class Water
{
 public:
  Water();
  ~Water();

  bool OnFrame(float delta_time);
  bool OnRender(float offset_x, float offset_y, float hscale, float vscale);
  bool LoadGameResource();

 private:
  static const int kWaterFPS = 16;
  static const int kWaterFrames = 16;

 private:
  HGE* hge_;

  hgeSprite* spr_water_;
  HTEXTURE tex_water_[kWaterFrames];

  int current_water_frame_;
  float since_last_frame_;
};

#endif // WATER_H_
