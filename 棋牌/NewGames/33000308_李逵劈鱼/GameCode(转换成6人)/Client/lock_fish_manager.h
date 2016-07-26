
#ifndef LOCK_FISH_MANAGER_H_
#define LOCK_FISH_MANAGER_H_
#pragma once

#include <vector>

#include "hge.h"
#include "hgesprite.h"
#include "hgeanim.h"

class LockFishManager {
 public:
  LockFishManager();
  ~LockFishManager();

  bool LoadGameResource();

  bool OnFrame(float delta_time);
  bool OnRender(float offset_x, float offset_y, float hscale, float vscale);

  void SetLockFishID(WORD chair_id, int lock_fish_id) { lock_fish_id_[chair_id] = lock_fish_id; }
  int GetLockFishID(WORD chair_id) const { return lock_fish_id_[chair_id]; }
  void SetLockFishKind(WORD chair_id, FishKind lock_fish_kind) { lock_fish_kind_[chair_id] = lock_fish_kind; }
  FishKind GetLockFishKind(WORD chair_id) const { return lock_fish_kind_[chair_id];}

  void UpdateLockTrace(WORD chair_id, float fish_pos_x, float fish_pos_y);
  void ClearLockTrace(WORD chair_id);
  FPoint LockPos(WORD chair_id);

 private:

 private:
  hgeSprite* spr_lock_flag_[GAME_PLAYER];
  hgeSprite* spr_lock_line_;
  hgeAnimation* ani_lock_flag_;

  float rotate_angle_;

  int lock_fish_id_[GAME_PLAYER];
  FishKind lock_fish_kind_[GAME_PLAYER];
  typedef std::vector<FPoint> FPointVector;
  FPointVector lock_line_trace_[GAME_PLAYER];
};

#endif // LOCK_FISH_MANAGER_H_
