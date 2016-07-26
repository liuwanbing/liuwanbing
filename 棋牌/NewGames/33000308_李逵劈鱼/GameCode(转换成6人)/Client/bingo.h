
#ifndef BINGO_H_
#define BINGO_H_
#pragma once

#include "hgeanim.h"

class Bingo {
 public:
  Bingo();
  ~Bingo();

  bool LoadGameResource();
  void SetBingoInfo(WORD chair_id, SCORE fish_score);

  bool OnFrame(float delta_time);
  bool OnRender(float offset_x, float offset_y, float hscale, float vscale);

 private:
  FPoint GetBingoPos(WORD chair_id, float hscale, float vscale);
  void RenderNum(WORD chair_id, int num, float x, float y, float rot, float hscale, float vscale);

 private:
  hgeAnimation* spr_bingo_num_[GAME_PLAYER];
  hgeAnimation* ani_bingo_[GAME_PLAYER];
  float delta_time_[GAME_PLAYER];
  SCORE fish_score_[GAME_PLAYER];
  float rotate_[GAME_PLAYER];
  int rotate_factor[GAME_PLAYER];
};

#endif  // BINGO_H_