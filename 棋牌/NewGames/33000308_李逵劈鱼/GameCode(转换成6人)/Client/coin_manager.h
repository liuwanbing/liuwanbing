
#ifndef COIN_MANAGER_H_
#define COIN_MANAGER_H_
#pragma once

#include <vector>

#include "hge.h"
#include "hgeanim.h"

class Coin {
 public:
  Coin();
  ~Coin();

  std::vector<FPoint>& trace_vector() { return trace_vector_; }
  std::vector<FPoint>::size_type trace_index() const { return trace_index_; }
  void set_trace_index(std::vector<FPoint>::size_type trace_index) { trace_index_ = trace_index; }
  float angle() const { return angle_; }
  void set_angle(float angle) { angle_ = angle; }

  bool OnFrame(float delta_time);
  bool OnRender(float offset_x, float offset_y, float hscale, float vscale);

 private:
  float angle_;
  std::vector<FPoint> trace_vector_;
  std::vector<FPoint>::size_type trace_index_;
  hgeAnimation* ani_coin_;
};

class CoinManager
{
 public:
  CoinManager();
  ~CoinManager();

  bool LoadGameResource();

  bool OnFrame(float delta_time);
  bool OnRender(float offset_x, float offset_y, float hscale, float vscale);

  void BuildCoin(const FPointAngle& fish_pos, const FPoint& cannon_pos, WORD chair_id, SCORE score);

 private:
  bool FreeCoin(Coin* coin);
  void FreeAllCoins();

 private:
  HGE* hge_;

  hgeAnimation* ani_coin1_;
  hgeAnimation* ani_coin2_;

  std::vector<Coin*> coin_vector_;
};

#endif // COIN_MANAGER_H_
