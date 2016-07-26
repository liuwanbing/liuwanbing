
#ifndef FISH_MANAGER_H_
#define FISH_MANAGER_H_
#pragma once

#include <vector>

#include "hge.h"
#include "hgeanim.h"

class Bullet;

enum FishStatus {
  FISH_INVALID = 0,
  FISH_ALIVE,
  FISH_DIED
};

class ScoreAnimation {
 public:
  ScoreAnimation(const hgeAnimation& anim);
  ~ScoreAnimation();

  void SetScoreInfo(WORD chair_id, SCORE score, float x, float y);
  void Play();
  void OnFrame(float delta_time);
  void OnRender(float offset_x, float offset_y, float hscale, float vscale);
  WORD chair_id() const { return chair_id_; }

 private:
  bool play_;
  bool return_;
  bool fade_;
  WORD chair_id_;
  hgeAnimation* ani_score_;
  float pos_x_;
  float pos_y_;
  float offset_;
  SCORE score_;
};

class Fish 
{
public:
	Fish(FishKind fish_kind, int fish_id, int fish_multiple, float fish_speed, float bounding_box_width, float bounding_box_height, float hit_radius);
	virtual ~Fish();

	void set_active(bool active) { active_ = active; }
	bool active() const { return active_; }
	FishKind fish_kind() const { return fish_kind_; }
	FishStatus fish_status() const { return fish_status_; }
	void set_fish_id(int fish_id) { fish_id_ = fish_id; }
	void set_fish_mulriple(int mul) { fish_multiple_ = mul; }
	int fish_id() const { return fish_id_; }
	float fish_speed() const { return fish_speed_; }
	void set_trace_type(TraceType trace_type) { trace_type_ = trace_type; }
	TraceType trace_type() const { return trace_type_; }
	vector<FPointAngle>& trace_vector() { return trace_vector_; }
	std::vector<FPointAngle>::size_type trace_index() const { return trace_index_; }
	void set_trace_index(std::vector<FPointAngle>::size_type trace_index) { trace_index_ = trace_index; }
	void SetFishStop(std::vector<FPointAngle>::size_type stop_index, std::vector<FPointAngle>::size_type stop_count);
	void CheckValid();
	bool GetCurPos(FPointAngle* fish_pos);
	bool GetCurPos(FPoint* fish_pos);

	virtual bool OnFrame(float delta_time, bool lock);
	virtual bool OnRender(float offset_x, float offset_y, float hscale, float vscale);

	virtual bool BulletHitTest(Bullet* bullet);
	virtual bool NetHitTest(Bullet* bullet);
	virtual void CatchFish(WORD chair_id, SCORE score);

protected:
#ifdef TEST
	HGE* hge_;
	void RenderRect(float x1, float y1, float x2, float y2, DWORD color = 0xFFFFFFFF);
	void RenderCircle(float center_x, float center_y, float radius, int segments, DWORD color);
#endif
	FishKind fish_kind_;
	bool active_;
	bool valid_;
public:
	FishStatus fish_status_;
protected:
	int fish_id_;
	int fish_multiple_;
	float fish_speed_;
	float bounding_box_width_;
	float bounding_box_height_;
	float hit_radius_;

	hgeAnimation* ani_fish_;
	hgeAnimation* ani_fish_die_;
	ScoreAnimation* ani_score_;

	TraceType trace_type_;
	std::vector<FPointAngle> trace_vector_;
	std::vector<FPointAngle>::size_type trace_index_;

	std::vector<FPointAngle>::size_type stop_index_;
	std::vector<FPointAngle>::size_type stop_count_;
	std::vector<FPointAngle>::size_type current_stop_count_;
};

class FishLK : public Fish {
 public:
  FishLK(int fish_id, int fish_multiple, float fish_speed, float bounding_box_width, float bounding_box_height, float hit_radius);
  virtual ~FishLK();

  virtual bool OnFrame(float delta_time, bool lock);
  virtual bool OnRender(float offset_x, float offset_y, float hscale, float vscale);

 private:
  void RenderMulriple(float x, float y, float rot, float hscale, float vscale);

 private:
  hgeAnimation* spr_mulriple_;
};

class Fish24 : public Fish {
 public:
  Fish24(int fish_id, int fish_multiple, float fish_speed, float bounding_box_width, float bounding_box_height, float hit_radius);
  virtual ~Fish24();

  virtual bool OnFrame(float delta_time, bool lock);
  virtual bool OnRender(float offset_x, float offset_y, float hscale, float vscale);

  virtual bool BulletHitTest(Bullet* bullet);
  virtual bool NetHitTest(Bullet* bullet);

 private:
  float init_speed_;
  float init_speed_die_;
  bool switch_texture_;
};

// 大三元 大四喜
class FishSanSi : public Fish {
 public:
  FishSanSi(FishKind fish_kind, int fish_id, int fish_multiple, float fish_speed, float bounding_box_width, float bounding_box_height, float hit_radius);
  virtual ~FishSanSi();

  virtual bool OnFrame(float delta_time, bool lock);
  virtual bool OnRender(float offset_x, float offset_y, float hscale, float vscale);

  virtual void CatchFish(WORD chair_id, SCORE score);

 private:
   float init_speed_;
   float init_speed_die_;
   hgeAnimation* ani_fish_ex_;
   hgeAnimation* ani_fish_ex_die_;
};

class FishKing : public Fish {
 public:
  FishKing(FishKind fish_kind, int fish_id, int fish_multiple, float fish_speed, float bounding_box_width, float bounding_box_height, float hit_radius);
  virtual ~FishKing();

  virtual bool OnFrame(float delta_time, bool lock);
  virtual bool OnRender(float offset_x, float offset_y, float hscale, float vscale);

  virtual bool BulletHitTest(Bullet* bullet);
  virtual bool NetHitTest(Bullet* bullet);

 private:
  float init_speed_;
  float init_speed_die_;
  hgeAnimation* ani_fish_ex_;
  hgeAnimation* ani_fish_ex_die_;
};

class FishManager {
 public:
  FishManager();
  ~FishManager();

  bool LoadGameResource();

  bool OnFrame(float delta_time, bool lock);
  bool OnRender(float offset_x, float offset_y, float hscale, float vscale);

  Fish* ActiveFish(FishKind fish_kind, int fish_id, int fish_multiple, float fish_speed, float bounding_box_width, float bounding_box_height, float hit_radius);
  bool FreeFish(Fish* fish);
  void FreeAllFish();
  Fish* GetFish(int fish_id);
  void SceneSwitchIterator();
  void FreeSceneSwitchFish();
  int GetFishKindCount(FishKind fish_kind);

  bool BulletHitTest(Bullet* bullet);
  bool NetHitTest(Bullet* bullet);

  void CatchFish(WORD chair_id, int fish_id, SCORE score);
  void CatchSweepFish(WORD chair_id, int fish_id);
  void CatchSweepFishResult(WORD chair_id, int fish_id, SCORE score, int* catch_fish_id, int catch_fish_count);
  void HitFishLK(WORD chair_id, int fish_id, int fish_mulriple);

  int LockFish(FishKind* lock_fish_kind = NULL, int last_lock_fish_id = 0, FishKind last_fish_kind = FISH_KIND_COUNT);
  bool LockFishReachPos(int lock_fishid, size_t size, FPointAngle* pos);

 public:
  static bool InsideScreen(const FPointAngle& pos);

  //整理数据sdp
  void TidyData();

 private:
  static HGE* hge_;

  std::vector<Fish*> fish_vector_;
  std::vector<Fish*>::size_type fish_vector_size_;

  hgeAnimation* spr_score_num_;
  hgeAnimation* spr_score_num_small_;
};

#endif // FISH_MANAGER_H_
