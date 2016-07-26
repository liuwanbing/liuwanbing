
#ifndef BULLET_MANAGER_H_
#define BULLET_MANAGER_H_
#pragma once

#include <vector>

#include "hge.h"
#include "hgeanim.h"

enum BulletStatus {
  BULLET_INVALID = 0,
  BULLET_FLYING,
  BULLET_CASTING_NET
};

class Bullet {
 public:
  Bullet(BulletKind bullet_kind, int bullet_id, int bullet_mulriple, WORD firer_chair_id, float net_radius);
  ~Bullet();

  BulletStatus bullet_status() const { return bullet_status_; }
  int bullet_id() const { return bullet_id_; }
  BulletKind bullet_kind() const { return bullet_kind_; }
  int bullet_mulriple() const { return bullet_mulriple_; }
  WORD firer_chair_id() const { return firer_chair_id_; }
  std::vector<FPoint>& trace_vector() { return trace_vector_; }
  std::vector<FPoint>::size_type trace_index() const { return trace_index_; }
  void set_trace_index(std::vector<FPoint>::size_type trace_index) { trace_index_ = trace_index; }
  float net_radius() const { return net_radius_; }
  float speed() const { return speed_; }
  void set_speed(float speed) { speed_ = speed; }
  float angle() const { return angle_; }
  void set_angle(float angle) { angle_ = angle; }
  void CastingNet();
  FPoint GetCurPos();
  bool IsValid();
  void set_android_chairid(WORD android_chairid) { android_chairid_ = android_chairid; }
  WORD android_chairid() const { return android_chairid_; }
  void set_lock_fishid(int fish_id) { lock_fishid_ = fish_id; }
  int lock_fishid() const { return lock_fishid_; }

  bool OnFrame(float delta_time);
  bool OnRender(float offset_x, float offset_y, float hscale, float vscale);

 private:
#ifdef TEST
  HGE* hge_;
  void RenderCircle(float center_x, float center_y, float radius, int segments, DWORD color);
#endif
  BulletKind bullet_kind_;
  BulletStatus bullet_status_;
  int bullet_id_;
  int bullet_mulriple_;
  WORD firer_chair_id_;
  WORD android_chairid_;
  int lock_fishid_;

  float net_radius_;
  float speed_;
  float angle_;
  std::vector<FPoint> trace_vector_;
  std::vector<FPoint>::size_type trace_index_;

  hgeAnimation* ani_bullet_;
  hgeAnimation* ani_net_;
};

class BulletManager
{
 public:
  BulletManager();
  ~BulletManager();

  //整理数据sdp
  void TidyData();

  bool LoadGameResource();

  bool OnFrame(float delta_time);
  bool OnRender(float offset_x, float offset_y, float hscale, float vscale);

  bool FreeBullet(Bullet* bullet);
  void FreeAllBullet();

  int Fire(float src_x_pos, float src_y_pos, float angle, BulletKind bullet_kind, int bullet_id, int bullet_mulriple, WORD firer_chair_id,
    float bullet_speed, float net_radius, WORD android_chairid,  int lock_fishid);

 private:
  void ReboundBullet(Bullet* bullet);
  void GetTargetPoint(WORD firer_chair_id, float src_x_pos, float src_y_pos, float angle, float& target_x_pos, float& target_y_pos);

 private:
  HGE* hge_;

  std::vector<Bullet*> bullet_vector_;
};

#endif // BULLET_MANAGER_H_
