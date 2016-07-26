
#include "StdAfx.h"
#include "fish_manager.h"
#include "game_manager.h"
#include "bullet_manager.h"
#include "cannon_manager.h"
#include "coin_manager.h"
#include "bounding_box.h"
#include "sound_manager.h"
#include "math_aide.h"
#include "magic/mp_wrap.h"
#include "lock_fish_manager.h"
#include "NewGameClient.h"

HGE* FishManager::hge_ = NULL;

const int kFadeFactor = 5;
const float kOffset = 1.f;
const float kMaxOffset = 10.f;

ScoreAnimation::ScoreAnimation(const hgeAnimation& anim)
  : play_(false), return_(false), fade_(false), chair_id_(5), ani_score_(new hgeAnimation(anim)), pos_x_(0.f),
  pos_y_(0.f), offset_(-1.f), score_(0) {
}

ScoreAnimation::~ScoreAnimation() {
  SAFE_DELETE(ani_score_);
}

void ScoreAnimation::SetScoreInfo(WORD chair_id, SCORE score, float x, float y) {
  chair_id_ = chair_id;
  score_ = score;
  pos_x_ = x;
  pos_y_ = y;
}

void ScoreAnimation::Play() {
  play_ = true;
  fade_ = false;
  return_ = false;
  offset_ = -10000.f;
  ani_score_->SetColor(0xFFFFFFFF);
}

void ScoreAnimation::OnFrame(float delta_time) 
{
  if (!play_) return;

  if (offset_ == -10000.f) 
  {
    if (chair_id_ == 0 || chair_id_ == 1 || chair_id_ == 2 || chair_id_ == 7) 
	{
      offset_ = kOffset;
    } 
	else 
	{
      offset_ = -kOffset;
    }
  } else {
    if (offset_ == 0.f) {
      fade_ = true;
    } else {
      if (chair_id_ == 0 || chair_id_ == 1 || chair_id_ == 2 || chair_id_ == 7) {
        if (!return_) {
          offset_ += kOffset;
          if (offset_ >= kMaxOffset) return_ = true;
        } else {
          offset_ -= kOffset;
        }
      } else {
        if (!return_) {
          offset_ -= kOffset;
          if (offset_ <= -kMaxOffset) return_ = true;
        } else {
          offset_ += kOffset;
        }
      }
    }

    if (fade_) {
      DWORD color = ani_score_->GetColor();
      int alpha = GETA(color);
      alpha -= kFadeFactor;
      if (alpha <= 5) {
        color = 0x00FFFFFF;
        ani_score_->SetColor(color);
        fade_ = false;
        play_ = false;
      } else {
        color = SETA(color, alpha);
        ani_score_->SetColor(color);
      }
    }
  }
}

void ScoreAnimation::OnRender(float offset_x, float offset_y, float hscale, float vscale) 
{
  if (!play_) return;
  if (score_ <= 0) return;

  float num_width = ani_score_->GetWidth();
  float num_height = ani_score_->GetHeight();

  int num_count = 0;
  SCORE num_temp = score_;
  do {
    num_count++;
    num_temp /= 10;
  } while (num_temp > 0);
  num_temp = score_;

  float x = pos_x_, y = pos_y_;
  int iView_chair_id_ = (CNewGameClient::GetMySelf())->SwitchChairID(chair_id_);
  if (iView_chair_id_ == 0 || iView_chair_id_ == 1 || iView_chair_id_ == 2) x -= (num_count - 1) * num_width * hscale / 2;
  else if (iView_chair_id_ == 6) y -= (num_count - 1) * num_width * hscale / 2;
  else if (iView_chair_id_ == 7) y += (num_count - 1) * num_width * hscale / 2;
  else x += (num_count - 1) * num_width * hscale / 2;

  for (int i = 0; i < num_count; ++i) {
    ani_score_->SetFrame(static_cast<int>(num_temp % 10));
    if (iView_chair_id_ <= 2) {
      ani_score_->RenderEx(x + offset_x, y + offset_y + offset_, kChairDefaultAngle[iView_chair_id_], hscale, vscale);
      x += num_width * hscale;
    } else if (iView_chair_id_ == 6) {
      ani_score_->RenderEx(x + offset_x + offset_, y + offset_y, kChairDefaultAngle[iView_chair_id_], hscale, vscale);
      y += num_width * hscale;
    } else if (iView_chair_id_ == 7) {
      ani_score_->RenderEx(x + offset_x + offset_, y + offset_y, kChairDefaultAngle[iView_chair_id_], hscale, vscale);
      y -= num_width * hscale;
    } else {
      DWORD color = ani_score_->GetColor();
      color >>= 24;
      ani_score_->RenderEx(x + offset_x, y + offset_y + offset_, kChairDefaultAngle[iView_chair_id_], hscale, vscale);
      x -= num_width * hscale;
    }
    num_temp /= 10;
  }
}

Fish::Fish(FishKind fish_kind, 
	int fish_id, 
	int fish_multiple, 
	float fish_speed, 
	float bounding_box_width, 
	float bounding_box_height, 
	float hit_radius)
  : fish_kind_(fish_kind), active_(false), 
  valid_(false), fish_status_(FISH_INVALID), 
  fish_id_(fish_id), fish_multiple_(fish_multiple), 
  fish_speed_(fish_speed),bounding_box_width_(bounding_box_width), 
  bounding_box_height_(bounding_box_height), hit_radius_(hit_radius), 
  trace_type_(TRACE_LINEAR), trace_index_(0),
  stop_index_(0), stop_count_(0), current_stop_count_(0) 
{
	ani_fish_ = NULL;
	ani_fish_die_ = NULL;
	ani_score_ = NULL;

	hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
	char file_name[MAX_PATH] = { 0 };
	if (fish_kind <= FISH_KIND_24) 
	{
		_snprintf(file_name, arraysize(file_name), "fish%d", fish_kind + 1);
		ani_fish_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
		_snprintf(file_name, arraysize(file_name), "fish%d_die", fish_kind + 1);
		ani_fish_die_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
	} 
	else
	{
		ani_fish_ = NULL;
		ani_fish_die_ = NULL;
	}
	if (ani_fish_ != NULL) 
	{
		ani_fish_->Play();
	}

	if (fish_multiple <= 5) 
	{
		ani_score_ = new ScoreAnimation(*resource_manager->GetAnimation("score_num_small"));
	} 
	else 
	{
		ani_score_ = new ScoreAnimation(*resource_manager->GetAnimation("score_num"));
	}

#ifdef TEST
	hge_ = hgeCreate(HGE_VERSION);
#endif
}

Fish::~Fish() {
  SAFE_DELETE(ani_fish_);
  SAFE_DELETE(ani_fish_die_);
  SAFE_DELETE(ani_score_);
#ifdef TEST
  hge_->Release();
#endif
}

void Fish::SetFishStop(std::vector<FPointAngle>::size_type stop_index, std::vector<FPointAngle>::size_type stop_count) {
  stop_index_ = stop_index;
  stop_count_ = stop_count;
  current_stop_count_ = 0;
}

void Fish::CheckValid() 
{
	if (trace_index_ >= trace_vector_.size()) valid_ = false;
	if (FishManager::InsideScreen(trace_vector_[trace_index_])) 
	{
		valid_ = true;
	}
	else 
	{
		valid_ = false;
	}
}

bool Fish::GetCurPos(FPointAngle* fish_pos) {
  if (trace_index_ >= trace_vector_.size()) return false;
  *fish_pos = trace_vector_[trace_index_];
  return true;
}

bool Fish::GetCurPos(FPoint* fish_pos) {
  if (trace_index_ >= trace_vector_.size()) return false;
  fish_pos->x = trace_vector_[trace_index_].x;
  fish_pos->y = trace_vector_[trace_index_].y;
  return true;
}

bool Fish::OnFrame(float delta_time, bool lock) 
{
	if (!active()) 
	{
		return false;
	}
	if (trace_index_ >= trace_vector_.size()) 
	{
		return true;
	}
	if (fish_status() == FISH_INVALID) 
	{
		trace_index_ = 0;
		fish_status_ = FISH_ALIVE;
		ani_fish_->Update(delta_time);
		CheckValid();
		return false;
	}
	if (fish_status_ == FISH_ALIVE) 
	{
		if (stop_count_ > 0 && trace_index_ == stop_index_ && current_stop_count_ < stop_count_) 
		{
			++current_stop_count_;
			if (current_stop_count_ >= stop_count_) SetFishStop(0, 0);
		} 
		else if (!lock) 
		{
			++trace_index_;
		}
		if (trace_index_ >= trace_vector_.size()) 
		{
			return true;
		}
		ani_fish_->Update(delta_time);
	} 
	else 
	{
		if (!ani_fish_die_->IsPlaying()) 
		{
			return true;
		} 
		else 
		{
			ani_fish_die_->Update(delta_time);
			ani_score_->OnFrame(delta_time);
		}
	}
	CheckValid();
	return false;
}

bool Fish::OnRender(float offset_x, float offset_y, float hscale, float vscale) 
{
	if (!active()) 
	{
		return false;
	}

	if (trace_index_ >= trace_vector_.size())
	{
		return true;
	}
	FPointAngle& fish_trace = trace_vector_[trace_index_];

	if (fish_status_ == FISH_DIED) 
	{
		ani_fish_die_->RenderEx(offset_x + fish_trace.x, offset_y + fish_trace.y, fish_trace.angle, hscale, vscale);
		ani_score_->OnRender(offset_x, offset_y, hscale, vscale);
	} 
	else 
	{
		ani_fish_->RenderEx(offset_x + fish_trace.x, offset_y + fish_trace.y, fish_trace.angle, hscale, vscale);
#ifdef TEST
		float radius_w = bounding_box_width_ / 2;
		float radius_h = bounding_box_height_ / 2;
		float sint = sinf(fish_trace.angle);
		float cost = cosf(fish_trace.angle);
		float x1 = -radius_w;
		float y1 = -radius_h;
		float x2 = radius_w;
		float y2 = radius_h;
		float pos_x1 = x1 * cost - y1 * sint + offset_x + fish_trace.x;
		float pos_y1 = x1 * sint + y1 * cost + offset_y + fish_trace.y;
		float pos_x2 = x2 * cost - y1 * sint + offset_x + fish_trace.x;
		float pos_y2 = x2 * sint + y1 * cost + offset_y + fish_trace.y;
		float pos_x3 = x2 * cost - y2 * sint + offset_x + fish_trace.x;
		float pos_y3 = x2 * sint + y2 * cost + offset_y + fish_trace.y;
		float pos_x4 = x1 * cost - y2 * sint + offset_x + fish_trace.x;
		float pos_y4 = x1 * sint + y2 * cost + offset_y + fish_trace.y;
		hge_->Gfx_RenderLine(pos_x1, pos_y1, pos_x2, pos_y2, 0xffffffff);
		hge_->Gfx_RenderLine(pos_x2, pos_y2, pos_x3, pos_y3, 0xffffffff);
		hge_->Gfx_RenderLine(pos_x3, pos_y3, pos_x4, pos_y4, 0xffffffff);
		hge_->Gfx_RenderLine(pos_x4, pos_y4, pos_x1, pos_y1, 0xffffffff);
		{
			float x0, y0, x1, y1, x2, y2;
			if (fish_kind_ == FISH_KIND_14 || fish_kind_ == FISH_KIND_15 || fish_kind_ == FISH_KIND_16 || fish_kind_ == FISH_KIND_18) 
			{
				x0 = fish_trace.x;
				y0 = fish_trace.y;
				x1 = fish_trace.x - hit_radius_ * cosf(fish_trace.angle - M_PI);
				y1 = fish_trace.y - hit_radius_ * sinf(fish_trace.angle - M_PI);
				x2 = fish_trace.x - hit_radius_ * cosf(fish_trace.angle);
				y2 = fish_trace.y - hit_radius_ * sinf(fish_trace.angle);
			} 
			else 
			{
				BoundingTriangle triangle;
				triangle.CreateBoundingTriangleWithRadius(hit_radius_, fish_trace.x, fish_trace.y, fish_trace.angle);
				x0 = triangle.x1_;
				y0 = triangle.y1_;
				x1 = triangle.x2_;
				y1 = triangle.y2_;
				x2 = triangle.x3_;
				y2 = triangle.y3_;
			}
			RenderCircle(x0, y0, 1.f, 36, 0xFFFF00FF);
			RenderCircle(x1, y1, 1.f, 36, 0xFFFF00FF);
			RenderCircle(x2, y2, 1.f, 36, 0xFFFF00FF);
		}
#endif
	}
	return false;
}

bool Fish::BulletHitTest(Bullet* bullet) {
  if (!active_) return false;
  if (!valid_) return false;
  if (fish_status_ != FISH_ALIVE) return false;
  if (trace_index_ >= trace_vector_.size()) return false;
  if (bullet->lock_fishid() > 0 && bullet->lock_fishid() != fish_id_) return false;

  FPoint point = bullet->GetCurPos();
  BoundingBox box;
  box.CreateBoundingBox(bounding_box_width_, bounding_box_height_, trace_vector_[trace_index_].x, trace_vector_[trace_index_].y, trace_vector_[trace_index_].angle);
  if (box.ComputeCollision(point.x, point.y)) 
  {
    if (fish_kind() == FISH_KIND_21 && bullet->bullet_mulriple() == GameManager::GetInstance().max_bullet_multiple()) 
	{
      WORD me_chair_id = (CNewGameClient::GetMySelf())->m_iMyDeskStation;
      if (bullet->firer_chair_id() == me_chair_id || bullet->android_chairid() == me_chair_id) 
	  {
        GameManager::GetInstance().SendHitFishLK(bullet->firer_chair_id(), fish_id_);
      }
    }
    return true;
  }
  return false;
}

bool Fish::NetHitTest(Bullet* bullet) {
  if (!active_) return false;
  if (!valid_) return false;
  if (fish_status_ != FISH_ALIVE) return false;
  if (trace_index_ >= trace_vector_.size()) return false;
  if (bullet->lock_fishid() > 0 && bullet->lock_fishid() != fish_id_) return false;

  //// 鱼身上的3个点 有2个在网内就算打中
  //FPoint point = bullet->GetCurPos();
  //FPointAngle& fish_trace = trace_vector_[trace_index_];
  //float x0, y0, x1, y1, x2, y2;
  //if (fish_kind_ == FISH_KIND_14 || fish_kind_ == FISH_KIND_15 || fish_kind_ == FISH_KIND_16 || fish_kind_ == FISH_KIND_17 || fish_kind_ == FISH_KIND_18) {
  //  x0 = fish_trace.x;
  //  y0 = fish_trace.y;
  //  x1 = fish_trace.x - hit_radius_ * cosf(fish_trace.angle - M_PI);
  //  y1 = fish_trace.y - hit_radius_ * sinf(fish_trace.angle - M_PI);
  //  x2 = fish_trace.x - hit_radius_ * cosf(fish_trace.angle);
  //  y2 = fish_trace.y - hit_radius_ * sinf(fish_trace.angle);
  //} else {
  //  BoundingTriangle triangle;
  //  triangle.CreateBoundingTriangleWithRadius(hit_radius_, fish_trace.x, fish_trace.y, fish_trace.angle);
  //  x0 = triangle.x1_;
  //  y0 = triangle.y1_;
  //  x1 = triangle.x2_;
  //  y1 = triangle.y2_;
  //  x2 = triangle.x3_;
  //  y2 = triangle.y3_;
  //}
  //int count = 0;
  //float distance = MathAide::CalcDistance(point.x, point.y, x0, y0);
  //if (distance <= bullet->net_radius()) ++count;
  //distance = MathAide::CalcDistance(point.x, point.y, x1, y1);
  //if (distance <= bullet->net_radius()) ++count;
  //distance = MathAide::CalcDistance(point.x, point.y, x2, y2);
  //if (distance <= bullet->net_radius()) ++count;
  //if (count >= 2) return true;

  // 碰撞就算打中了
  FPoint point = bullet->GetCurPos();
  BoundingBox box;
  box.CreateBoundingBox(bounding_box_width_, bounding_box_height_, trace_vector_[trace_index_].x, trace_vector_[trace_index_].y, trace_vector_[trace_index_].angle);
  if (box.ComputeCollision(point.x, point.y)) {
    return true;
  }

  return false;
}

void Fish::CatchFish(WORD chair_id, SCORE score) 
{
	fish_status_ = FISH_DIED;
	ani_fish_die_->SetFrame(0);
	ani_fish_die_->Play();
	ani_score_->SetScoreInfo(chair_id, score, trace_vector_[trace_index_].x, trace_vector_[trace_index_].y);
	ani_score_->Play();
	SoundManager::GetInstance().PlayGameEffect(CATCH);
	SoundManager::GetInstance().PlayFishEffect(fish_kind_);
	MP_Manager& MP=MP_Manager::GetInstance();
	MP_Emitter* emitter = NULL;
	if (fish_kind_ >= FISH_KIND_18 && fish_kind_ <= FISH_KIND_21) 
	{
		emitter = MP.GetEmitterByName("一网打尽爆炸");
	} 
	else if (fish_kind_ >= FISH_KIND_25) 
	{
		emitter = MP.GetEmitterByName("海啸来袭爆炸");
	} 
	else if (fish_kind_ == FISH_KIND_23 || fish_kind_ == FISH_KIND_24) 
	{
		emitter = MP.GetEmitterByName("鱼雷爆炸");
	} 
	else if (fish_kind_ == FISH_KIND_22) 
	{
		emitter = MP.GetEmitterByName("定海");
	}

	if (emitter != NULL) 
	{
		MP_POSITION pos;
		emitter->GetPosition(pos);
		FPointAngle& fish_trace = trace_vector_[trace_index_];
		pos.x = fish_trace.x;
		pos.y = fish_trace.y;
		emitter->Move(pos);
		emitter->SetState(MAGIC_STATE_STOP);
		emitter->SetState(MAGIC_STATE_UPDATE);
		GameManager::GetInstance().ShakeScreen();
		SoundManager::GetInstance().PlayGameEffect(SUPERARM);
	}
}

#ifdef TEST
void Fish::RenderRect(float x1, float y1, float x2, float y2, DWORD color) {
  hge_->Gfx_RenderLine(x1, y1, x2, y1, color);
  hge_->Gfx_RenderLine(x2, y1, x2, y2, color);
  hge_->Gfx_RenderLine(x2, y2, x1, y2, color);
  hge_->Gfx_RenderLine(x1, y2, x1, y1, color);
}

void Fish::RenderCircle(float center_x, float center_y, float radius, int segments, DWORD color) {
  float each_angle = 2 * M_PI / segments;
  float x1, y1;
  float x2 = radius, y2 = 0.f;
  for (float a = 0.f; a <= (2 * M_PI + each_angle); a += each_angle) {
    x1 = x2;
    y1 = y2;
    x2 = radius * cosf(a);
    y2 = radius * sinf(a);
    hge_->Gfx_RenderLine(x1 + center_x, y1 + center_y, x2 + center_x, y2 + center_y, color);
  }
}
#endif

FishLK::FishLK(int fish_id, int fish_multiple, float fish_speed, float bounding_box_width, float bounding_box_height, float hit_radius)
  : Fish(FISH_KIND_21, fish_id, fish_multiple, fish_speed, bounding_box_width, bounding_box_height, hit_radius) {
  spr_mulriple_ = new hgeAnimation(*GameManager::GetInstance().GetResourceManager()->GetAnimation("score_num"));
}

FishLK::~FishLK() {
  SAFE_DELETE(spr_mulriple_);
}

bool FishLK::OnFrame(float delta_time, bool lock) {
  if (!active()) return false;
  if (trace_index_ >= trace_vector_.size()) return true;

  if (fish_status() == FISH_INVALID) {
    trace_index_ = 0;
    fish_status_ = FISH_ALIVE;
    ani_fish_->Update(delta_time);
    CheckValid();
    return false;
  }

  if (fish_status_ == FISH_ALIVE) {
    if (!lock) ++trace_index_;
    if (trace_index_ >= trace_vector_.size()) return true;
    ani_fish_->Update(delta_time);
  } else {
    if (!ani_fish_die_->IsPlaying()) {
      return true;
    } else {
      ani_fish_die_->Update(delta_time);
      ani_score_->OnFrame(delta_time);
    }
  }
  CheckValid();

  return false;
}

bool FishLK::OnRender(float offset_x, float offset_y, float hscale, float vscale) {
  if (!active()) return false;
  if (trace_index_ >= trace_vector_.size()) return true;

  FPointAngle& fish_trace = trace_vector_[trace_index_];
  if (fish_status_ == FISH_DIED) {
    ani_fish_die_->RenderEx(offset_x + fish_trace.x, offset_y + fish_trace.y, fish_trace.angle, hscale, vscale);
    ani_score_->OnRender(offset_x, offset_y, hscale, vscale);
  } else {
    ani_fish_->RenderEx(offset_x + fish_trace.x, offset_y + fish_trace.y, fish_trace.angle, hscale, vscale);
    RenderMulriple(offset_x + fish_trace.x, offset_y + fish_trace.y, fish_trace.angle, hscale, vscale);
#ifdef TEST
    BoundingBox box;
    box.CreateBoundingBox(bounding_box_width_, bounding_box_height_, fish_trace.x, fish_trace.y, fish_trace.angle);
    hge_->Gfx_RenderLine(box.x1_, box.y1_, box.x2_, box.y2_, 0xffffffff);
    hge_->Gfx_RenderLine(box.x2_, box.y2_, box.x3_, box.y3_, 0xffffffff);
    hge_->Gfx_RenderLine(box.x3_, box.y3_, box.x4_, box.y4_, 0xffffffff);
    hge_->Gfx_RenderLine(box.x4_, box.y4_, box.x1_, box.y1_, 0xffffffff);
    {
      BoundingTriangle triangle;
      triangle.CreateBoundingTriangleWithRadius(hit_radius_, fish_trace.x, fish_trace.y, fish_trace.angle);
      RenderCircle(triangle.x1_, triangle.y1_, 1.f, 36, 0xFFFF00FF);
      RenderCircle(triangle.x2_, triangle.y2_, 1.f, 36, 0xFFFF00FF);
      RenderCircle(triangle.x3_, triangle.y3_, 1.f, 36, 0xFFFF00FF);
    }
#endif
  }

  return false;
}

void FishLK::RenderMulriple(float x, float y, float rot, float hscale, float vscale) {
  float num_width = spr_mulriple_->GetWidth();
  float num_height = spr_mulriple_->GetHeight();

  int num_count = 0;
  int num_temp = fish_multiple_;
  do {
    num_count++;
    num_temp /= 10;
  } while (num_temp > 0);
  num_temp = fish_multiple_;

  float cell_width = num_width * hscale;
  float radius = (num_count - 1) * cell_width / 2;
  float dx, dy;

  float excursion = vscale * (ani_fish_->GetHeight() / 2);
  FPoint center;
  center.x = x + sinf(rot) * excursion;
  center.y = y - cosf(rot) * excursion;

  for (int i = 0; i < num_count; ++i) {
    spr_mulriple_->SetFrame(num_temp % 10);
    if (radius < 0.f) {
      dx = center.x + radius * cosf(rot);
      dy = center.y + radius * sinf(rot);
    } else {
      dx = center.x - radius * cosf(rot - M_PI);
      dy = center.y - radius * sinf(rot - M_PI);
    }
    spr_mulriple_->RenderEx(dx, dy, rot, hscale, vscale);
    radius -= cell_width;
    num_temp /= 10;
  }
}

Fish24::Fish24(int fish_id, int fish_multiple, float fish_speed, float bounding_box_width, float bounding_box_height, float hit_radius)
    : Fish(FISH_KIND_24, fish_id, fish_multiple, fish_speed, bounding_box_width, bounding_box_height, hit_radius) {
  init_speed_ = ani_fish_->GetSpeed();
  init_speed_die_ = ani_fish_die_->GetSpeed();
  switch_texture_ = false;
}

Fish24::~Fish24() {
}

bool Fish24::OnFrame(float delta_time, bool lock) {
  if (!active()) return false;
  if (trace_index_ >= trace_vector_.size()) return true;

  if (fish_status() == FISH_INVALID) {
    trace_index_ = 0;
    fish_status_ = FISH_ALIVE;
    float fps = ani_fish_->GetSpeed() + init_speed_;
    ani_fish_->SetSpeed(fps);
    CheckValid();
    return false;
  }

  if (fish_status_ == FISH_ALIVE) {
    if (!lock) ++trace_index_;
    if (trace_index_ >= trace_vector_.size()) return true;
    float fps = ani_fish_->GetSpeed() + init_speed_;
    ani_fish_->SetSpeed(fps);
  } else {
    if (ani_fish_die_->GetSpeed() > init_speed_die_ * ani_fish_die_->GetFrames()) {
      return true;
    } else if (!switch_texture_ && ani_fish_die_->GetSpeed() >= init_speed_die_ * ani_fish_die_->GetFrames() / 2) {
      int frames = ani_fish_die_->GetFrames();
      ani_fish_die_->SetFrames(2);
      ani_fish_die_->SetFrame(1);
      ani_fish_die_->SetFrames(frames);
      switch_texture_ = true;
      float fps = ani_fish_die_->GetSpeed() + init_speed_die_;
      ani_fish_die_->SetSpeed(fps);
      ani_score_->OnFrame(delta_time);
    } else {
      float fps = ani_fish_die_->GetSpeed() + init_speed_die_;
      ani_fish_die_->SetSpeed(fps);
      ani_score_->OnFrame(delta_time);
    }
  }

  CheckValid();

  return false;
}

bool Fish24::OnRender(float offset_x, float offset_y, float hscale, float vscale) {
  if (!active()) 
	  return false;
  if (trace_index_ >= trace_vector_.size()) 
	  return true;

  FPointAngle& fish_trace = trace_vector_[trace_index_];
  if (fish_status_ == FISH_DIED) 
  {
    ani_fish_die_->RenderEx(offset_x + fish_trace.x, offset_y + fish_trace.y, ani_fish_die_->GetSpeed(), hscale, hscale);
    ani_score_->OnRender(offset_x, offset_y, hscale, vscale);
  } 
  else 
  {
    ani_fish_->RenderEx(offset_x + fish_trace.x, offset_y + fish_trace.y, ani_fish_->GetSpeed(), hscale, hscale);
  }

#ifdef TEST
  RenderCircle(fish_trace.x, fish_trace.y, bounding_box_width_ / 2, 36, 0xffffffff);
#endif

  return false;
}

bool Fish24::BulletHitTest(Bullet* bullet) {
  if (!active_) return false;
  if (!valid_) return false;
  if (fish_status_ != FISH_ALIVE) return false;
  if (trace_index_ >= trace_vector_.size()) return false;
  if (bullet->lock_fishid() > 0 && bullet->lock_fishid() != fish_id_) return false;

  FPoint point = bullet->GetCurPos();
  float distance = MathAide::CalcDistance(point.x, point.y, trace_vector_[trace_index_].x, trace_vector_[trace_index_].y);
  if (distance <= bounding_box_width_ / 2) return true;

  return false;
}

bool Fish24::NetHitTest(Bullet* bullet) {
  if (!active_) return false;
  if (!valid_) return false;
  if (fish_status_ != FISH_ALIVE) return false;
  if (trace_index_ >= trace_vector_.size()) return false;
  if (bullet->lock_fishid() > 0 && bullet->lock_fishid() != fish_id_) return false;

  FPoint point = bullet->GetCurPos();
  float distance = MathAide::CalcDistance(point.x, point.y, trace_vector_[trace_index_].x, trace_vector_[trace_index_].y);
  if (distance <= bounding_box_width_ / 2) return true;

  return false;
}

FishSanSi::FishSanSi(FishKind fish_kind, int fish_id, int fish_multiple, float fish_speed, float bounding_box_width, float bounding_box_height, float hit_radius)
  : Fish(fish_kind, fish_id, fish_multiple, fish_speed, bounding_box_width, bounding_box_height, hit_radius) {
  hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
  ani_fish_ = new hgeAnimation(*resource_manager->GetAnimation("dish"));
  ani_fish_die_ = new hgeAnimation(*resource_manager->GetAnimation("dish_die"));
  assert(fish_kind >= FISH_KIND_25 && fish_kind <= FISH_KIND_30);
  char file_name[MAX_PATH] = { 0 };
  if (fish_kind == FISH_KIND_25) {
    _snprintf(file_name, arraysize(file_name), "fish%d", FISH_KIND_4 + 1);
    ani_fish_ex_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
    _snprintf(file_name, arraysize(file_name), "fish%d_die", FISH_KIND_4 + 1);
    ani_fish_ex_die_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
    ani_fish_ex_->Play();
  } else if (fish_kind == FISH_KIND_26) {
    _snprintf(file_name, arraysize(file_name), "fish%d", FISH_KIND_5 + 1);
    ani_fish_ex_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
    _snprintf(file_name, arraysize(file_name), "fish%d_die", FISH_KIND_5 + 1);
    ani_fish_ex_die_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
    ani_fish_ex_->Play();
  } else if (fish_kind == FISH_KIND_27) {
    _snprintf(file_name, arraysize(file_name), "fish%d", FISH_KIND_7 + 1);
    ani_fish_ex_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
    _snprintf(file_name, arraysize(file_name), "fish%d_die", FISH_KIND_7 + 1);
    ani_fish_ex_die_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
    ani_fish_ex_->Play();
  } else if (fish_kind == FISH_KIND_28) {
    _snprintf(file_name, arraysize(file_name), "fish%d", FISH_KIND_6 + 1);
    ani_fish_ex_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
    _snprintf(file_name, arraysize(file_name), "fish%d_die", FISH_KIND_6 + 1);
    ani_fish_ex_die_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
    ani_fish_ex_->Play();
  } else if (fish_kind == FISH_KIND_29) {
    _snprintf(file_name, arraysize(file_name), "fish%d", FISH_KIND_8 + 1);
    ani_fish_ex_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
    _snprintf(file_name, arraysize(file_name), "fish%d_die", FISH_KIND_8 + 1);
    ani_fish_ex_die_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
    ani_fish_ex_->Play();
  } else {
    _snprintf(file_name, arraysize(file_name), "fish%d", FISH_KIND_10 + 1);
    ani_fish_ex_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
    _snprintf(file_name, arraysize(file_name), "fish%d_die", FISH_KIND_10 + 1);
    ani_fish_ex_die_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
    ani_fish_ex_->Play();
  }

  init_speed_ = ani_fish_->GetSpeed();
  init_speed_die_ = ani_fish_die_->GetSpeed();
  ani_fish_->Play();
}

FishSanSi::~FishSanSi() {
  SAFE_DELETE(ani_fish_ex_);
  SAFE_DELETE(ani_fish_ex_die_);
}

bool FishSanSi::OnFrame(float delta_time, bool lock) {
  if (!active()) return false;
  if (trace_index_ >= trace_vector_.size()) return true;

  if (fish_status() == FISH_INVALID) {
    trace_index_ = 0;
    fish_status_ = FISH_ALIVE;
    float fps = ani_fish_->GetSpeed() + init_speed_;
    ani_fish_->SetSpeed(fps);
    ani_fish_ex_->Update(delta_time);
    CheckValid();
    return false;
  }

  if (fish_status_ == FISH_ALIVE) {
    if (!lock) ++trace_index_;
    if (trace_index_ >= trace_vector_.size()) return true;
    float fps = ani_fish_->GetSpeed() + init_speed_;
    ani_fish_->SetSpeed(fps);
    ani_fish_ex_->Update(delta_time);
  } else {
    if (ani_fish_die_->GetSpeed() > init_speed_die_ * ani_fish_die_->GetFrames()) {
      return true;
    } else {
      float fps = ani_fish_die_->GetSpeed() + init_speed_die_;
      ani_fish_die_->SetSpeed(fps);
      ani_fish_ex_die_->Update(delta_time);
      ani_score_->OnFrame(delta_time);
    }
  }
  CheckValid();

  return false;
}

bool FishSanSi::OnRender(float offset_x, float offset_y, float hscale, float vscale) {
  if (!active()) return false;
  if (trace_index_ >= trace_vector_.size()) return true;

  int fish_count = 3;
  if (fish_kind_ >= FISH_KIND_28 && fish_kind_ <= FISH_KIND_30) fish_count = 4;
  FPointAngle& fish_trace = trace_vector_[trace_index_];
  float x0 = offset_x + fish_trace.x;
  float y0 = offset_y + fish_trace.y;
  float dx, dy;

  if (fish_status_ == FISH_DIED) {
    float cell_width = ani_fish_die_->GetWidth() * hscale;
    float radius = (fish_count - 1) * cell_width / 2;
    for (int i = 0; i < fish_count; ++i) {
      if (radius < 0.f) {
        dx = x0 + radius * cosf(fish_trace.angle);
        dy = y0 + radius * sinf(fish_trace.angle);
      } else {
        dx = x0 - radius * cosf(fish_trace.angle - M_PI);
        dy = y0 - radius * sinf(fish_trace.angle - M_PI);
      }
      ani_fish_die_->RenderEx(dx, dy, -ani_fish_die_->GetSpeed(), hscale, hscale);
      ani_fish_ex_die_->RenderEx(dx, dy, fish_trace.angle, hscale, vscale);
      radius -= cell_width;
    }
    ani_score_->OnRender(x0, y0, hscale, vscale);
  } else {
    float cell_width = ani_fish_->GetWidth() * hscale;
    float radius = (fish_count - 1) * cell_width / 2;
    for (int i = 0; i < fish_count; ++i) {
      if (radius < 0.f) {
        dx = x0 + radius * cosf(fish_trace.angle);
        dy = y0 + radius * sinf(fish_trace.angle);
      } else {
        dx = x0 - radius * cosf(fish_trace.angle - M_PI);
        dy = y0 - radius * sinf(fish_trace.angle - M_PI);
      }
      ani_fish_->RenderEx(dx, dy, -ani_fish_->GetSpeed(), hscale, hscale);
      ani_fish_ex_->RenderEx(dx, dy, fish_trace.angle, hscale, vscale);
      radius -= cell_width;
    }
#ifdef TEST
    BoundingBox box;
    box.CreateBoundingBox(bounding_box_width_, bounding_box_height_, fish_trace.x, fish_trace.y, fish_trace.angle);
    hge_->Gfx_RenderLine(box.x1_, box.y1_, box.x2_, box.y2_, 0xffffffff);
    hge_->Gfx_RenderLine(box.x2_, box.y2_, box.x3_, box.y3_, 0xffffffff);
    hge_->Gfx_RenderLine(box.x3_, box.y3_, box.x4_, box.y4_, 0xffffffff);
    hge_->Gfx_RenderLine(box.x4_, box.y4_, box.x1_, box.y1_, 0xffffffff);
    {
      BoundingTriangle triangle;
      triangle.CreateBoundingTriangleWithRadius(hit_radius_, fish_trace.x, fish_trace.y, fish_trace.angle);
      RenderCircle(triangle.x1_, triangle.y1_, 1.f, 36, 0xFFFF00FF);
      RenderCircle(triangle.x2_, triangle.y2_, 1.f, 36, 0xFFFF00FF);
      RenderCircle(triangle.x3_, triangle.y3_, 1.f, 36, 0xFFFF00FF);
    }
#endif
  }

  return false;
}

void FishSanSi::CatchFish(WORD chair_id, SCORE score) {
  Fish::CatchFish(chair_id, score);
  ani_fish_ex_->SetFrame(0);
  ani_fish_ex_->Play();
}

FishKing::FishKing(FishKind fish_kind, int fish_id, int fish_multiple, float fish_speed, float bounding_box_width, float bounding_box_height, float hit_radius)
  : Fish(FishKind(fish_kind - FISH_KIND_31), fish_id, fish_multiple, fish_speed, bounding_box_width, bounding_box_height, hit_radius) {
  assert(fish_kind >= FISH_KIND_31 && fish_kind <= FISH_KIND_40);
  fish_kind_ = fish_kind;
  hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
  ani_fish_ex_ = new hgeAnimation(*resource_manager->GetAnimation("halo"));
  ani_fish_ex_die_ = new hgeAnimation(*resource_manager->GetAnimation("halo_die"));
  init_speed_ = ani_fish_ex_->GetSpeed();
  init_speed_die_ = ani_fish_ex_die_->GetSpeed();
}

FishKing::~FishKing() {
  SAFE_DELETE(ani_fish_ex_);
  SAFE_DELETE(ani_fish_ex_die_);
}

bool FishKing::OnFrame(float delta_time, bool lock) {
  if (!active()) return false;
  if (trace_index_ >= trace_vector_.size()) return true;

  if (fish_status() == FISH_INVALID) {
    trace_index_ = 0;
    fish_status_ = FISH_ALIVE;
    float fps = ani_fish_ex_->GetSpeed() + init_speed_;
    ani_fish_ex_->SetSpeed(fps);
    ani_fish_->Update(delta_time);
    CheckValid();
    return false;
  }

  if (fish_status_ == FISH_ALIVE) {
    if (!lock) ++trace_index_;
    if (trace_index_ >= trace_vector_.size()) return true;
    float fps = ani_fish_ex_->GetSpeed() + init_speed_;
    ani_fish_ex_->SetSpeed(fps);
    ani_fish_->Update(delta_time);
  } else {
    if (ani_fish_ex_die_->GetSpeed() > init_speed_die_ * ani_fish_ex_die_->GetFrames()) {
      return true;
    } else {
      float fps = ani_fish_ex_die_->GetSpeed() + init_speed_die_;
      ani_fish_ex_die_->SetSpeed(fps);
      ani_fish_die_->Update(delta_time);
      ani_score_->OnFrame(delta_time);
    }
  }
  CheckValid();

  return false;
}

bool FishKing::OnRender(float offset_x, float offset_y, float hscale, float vscale) {
  if (!active()) return false;
  if (trace_index_ >= trace_vector_.size()) return true;

  FPointAngle& fish_trace = trace_vector_[trace_index_];

  if (fish_status_ == FISH_DIED) {
    ani_fish_ex_die_->RenderEx(fish_trace.x, fish_trace.y, ani_fish_ex_die_->GetSpeed(), hscale, hscale);
    ani_fish_die_->RenderEx(fish_trace.x, fish_trace.y, fish_trace.angle, hscale, vscale);
    ani_score_->OnRender(offset_x, offset_y, hscale, vscale);
  } else {
    ani_fish_ex_->RenderEx(fish_trace.x, fish_trace.y, ani_fish_ex_->GetSpeed(), hscale, hscale);
    ani_fish_->RenderEx(fish_trace.x, fish_trace.y, fish_trace.angle, hscale, vscale);
#ifdef TEST
    {
      BoundingTriangle triangle;
      triangle.CreateBoundingTriangleWithRadius(hit_radius_, fish_trace.x, fish_trace.y, fish_trace.angle);
      RenderCircle(triangle.x1_, triangle.y1_, 1.f, 36, 0xFFFF00FF);
      RenderCircle(triangle.x2_, triangle.y2_, 1.f, 36, 0xFFFF00FF);
      RenderCircle(triangle.x3_, triangle.y3_, 1.f, 36, 0xFFFF00FF);
    }
#endif
  }

  return false;
}

bool FishKing::BulletHitTest(Bullet* bullet) {
  if (!active_) return false;
  if (!valid_) return false;
  if (fish_status_ != FISH_ALIVE) return false;
  if (trace_index_ >= trace_vector_.size()) return false;
  if (bullet->lock_fishid() > 0 && bullet->lock_fishid() != fish_id_) return false;

  FPoint point = bullet->GetCurPos();
  float distance = MathAide::CalcDistance(point.x, point.y, trace_vector_[trace_index_].x, trace_vector_[trace_index_].y);
  if (distance <= bounding_box_width_ / 2) return true;

  return false;
}

bool FishKing::NetHitTest(Bullet* bullet) {
  if (!active_) return false;
  if (!valid_) return false;
  if (fish_status_ != FISH_ALIVE) return false;
  if (trace_index_ >= trace_vector_.size()) return false;
  if (bullet->lock_fishid() > 0 && bullet->lock_fishid() != fish_id_) return false;

  FPoint point = bullet->GetCurPos();
  float distance = MathAide::CalcDistance(point.x, point.y, trace_vector_[trace_index_].x, trace_vector_[trace_index_].y);
  if (distance <= bounding_box_width_ / 2) return true;

  return false;
}

FishManager::FishManager() {
  hge_ = hgeCreate(HGE_VERSION);
  fish_vector_size_ = 0;
}

FishManager::~FishManager() 
{
  /*std::vector<Fish*>::iterator iter;
  for (iter = fish_vector_.begin(); iter != fish_vector_.end(); ++iter) 
  {
	  if (*iter != NULL)
	  {
		  SAFE_DELETE(*iter);
	  }
  }*/

  Fish *pfish=NULL;

  for (int i=0; i<fish_vector_.size(); i++)
  {
	  pfish = fish_vector_.at(i);
	  if (pfish != NULL)
	  {
		  SAFE_DELETE(pfish);
	  }
  }

  fish_vector_.clear();

  hge_->Release();
}

bool FishManager::LoadGameResource() {
  char file_name[MAX_PATH] = { 0 };

  hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();

  for (int i = 0; i <= FISH_KIND_24; ++i) 
  {
    _snprintf(file_name, arraysize(file_name), "fish%d", i + 1);
    resource_manager->GetAnimation(file_name);

    _snprintf(file_name, arraysize(file_name), "fish%d_die", i + 1);
    resource_manager->GetAnimation(file_name);
  }

  resource_manager->GetAnimation("dish");
  resource_manager->GetAnimation("dish_die");
  resource_manager->GetAnimation("halo");
  resource_manager->GetAnimation("halo_die");

  spr_score_num_ = resource_manager->GetAnimation("score_num");
  spr_score_num_small_ = resource_manager->GetAnimation("score_num_small");

  return true;
}

//整理数据sdp
void FishManager::TidyData()
{
	Fish* fish = NULL;
	std::vector<Fish*> fish_vector_2;
	fish_vector_2.clear();
	for (int i=0; i<fish_vector_.size(); i++)
	{
		fish = fish_vector_.at(i);
		if (fish == NULL)
			continue;

		if (fish->fish_kind() >= FISH_KIND_COUNT || fish->fish_kind() < 0)
			continue;

		fish_vector_2.push_back(fish);
	}

	fish_vector_.clear();
	for (int i=0; i<fish_vector_2.size(); i++)
	{
		fish = fish_vector_2.at(i);
		if (fish == NULL)
			continue;

		fish_vector_.push_back(fish);
	}
}

bool FishManager::OnFrame(float delta_time, bool lock) 
{
	LockFishManager* lock_fish_manager = GameManager::GetInstance().GetLockFishManager();
	Fish* fish = NULL;
	bool switch_lock = false;
	WORD me_chair_id = (CNewGameClient::GetMySelf())->m_iMyDeskStation;
	
	CString sdp;
	int inum=0;
	//整理数据
	TidyData();

	for (int i=0; i<fish_vector_.size(); i++)
	{
		fish = fish_vector_.at(i);
		if (fish == NULL)
		{
			hge_->System_Log("FishManager::OnFrame fish == NULL 异常!!!");
			continue;
		}
		inum++;

		if (fish->OnFrame(delta_time, lock) && fish_vector_size_ == 0) 
		{
			fish_vector_.at(i) = NULL;
		} 
		else
		{
			for (WORD i = 0; i < GAME_PLAYER; ++i) 
			{
				if (lock_fish_manager->GetLockFishID(i) == fish->fish_id()) 
				{
					FPointAngle& point_angle = fish->trace_vector()[fish->trace_index()];
					if (!InsideScreen(point_angle) || fish->fish_status() == FISH_DIED) 
					{
						lock_fish_manager->ClearLockTrace(i);
						if (i == me_chair_id) switch_lock = true;
					} 
					else 
					{
						lock_fish_manager->UpdateLockTrace(i, point_angle.x, point_angle.y);
					}
				}
			}
		}
	}

	//整理数据
	TidyData();

	if (switch_lock) 
	{
		FishKind lock_fish_kind;
		lock_fish_manager->SetLockFishID(me_chair_id, LockFish(&lock_fish_kind));
		lock_fish_manager->SetLockFishKind(me_chair_id, lock_fish_kind);
	}

	return false;
}

bool FishManager::OnRender(float offset_x, float offset_y, float hscale, float vscale) 
{
	std::vector<Fish*>::iterator iter;
	Fish* fish = NULL;

	CString sdp;
	int inum=0;

	//整理数据
	TidyData();

	for(int i=0; i<fish_vector_.size(); i++)
	{
		fish = fish_vector_.at(i);

		if (fish == NULL)
		{
			hge_->System_Log("FishManager::OnRender fish == NULL 异常!!!");
			continue;
		}

		fish->OnRender(offset_x, offset_y, hscale, vscale);

	}
  return false;
}

Fish* FishManager::ActiveFish(FishKind fish_kind, int fish_id, int fish_multiple, float fish_speed,
                              float bounding_box_width, float bounding_box_height, float hit_radius)
{
	//CATCH_BEGIN //调试开始
	try {	
		Fish* fish = NULL;
		if (fish_kind == FISH_KIND_22)
		{
			OutputDebugString("dwjlkpy1::创建鱼王");
		}
		if (fish_kind == FISH_KIND_21) 
		{
			fish = new FishLK(fish_id, fish_multiple, fish_speed, bounding_box_width, bounding_box_height, hit_radius);
		} 
		else if (fish_kind == FISH_KIND_24) 
		{
			fish = new Fish24(fish_id, fish_multiple, fish_speed, bounding_box_width, bounding_box_height, hit_radius);
		} 
		else if (fish_kind >= FISH_KIND_25 && fish_kind <= FISH_KIND_30) 
		{
			fish = new FishSanSi(fish_kind, fish_id, fish_multiple, fish_speed, bounding_box_width, bounding_box_height, hit_radius);
		} 
		else if (fish_kind >= FISH_KIND_31 && fish_kind <= FISH_KIND_40)
		{
			fish = new FishKing(fish_kind, fish_id, fish_multiple, fish_speed, bounding_box_width, bounding_box_height, hit_radius);
		} 
		else  
		{
			fish = new Fish(fish_kind, fish_id, fish_multiple, fish_speed, bounding_box_width, bounding_box_height, hit_radius);
		}

		if(fish != NULL)
			fish_vector_.push_back(fish);

		return fish;

	} 
	catch(...)	
	{
		OutputDebugString("dwjlkpy1::创建鱼儿错误");
		return NULL;
		//char szErr[50]; 
		//sprintf_s(szErr, 50, "error fun=%s line=%d", __FUNCTION__, __LINE__);	
		//MessageBox(NULL, szErr, szErr, 0);
	}
	return NULL;
	//CATCH_END //调试结束
}

bool FishManager::FreeFish(Fish* fish) 
{
  std::vector<Fish*>::iterator iter;
  for (iter = fish_vector_.begin(); iter != fish_vector_.end(); ++iter) 
  {
    if (fish == *iter) 
	{
      fish_vector_.erase(iter);
      SAFE_DELETE(fish);
      return true;
    }
  }
  assert(!"FreeFish Failed");
  return false;
}

void FishManager::FreeAllFish() {
  std::vector<Fish*>::iterator iter;
  for (iter = fish_vector_.begin(); iter != fish_vector_.end(); ++iter) {
    SAFE_DELETE(*iter);
  }
  fish_vector_.clear();
}

Fish* FishManager::GetFish(int fish_id) {
	//CATCH_BEGIN //调试开始
	try {
		Fish* fish = NULL;
		for (int i=0; i<fish_vector_.size(); i++)
		{
			fish = fish_vector_.at(i);
			if (fish == NULL)
				continue;
			if (!fish->active()) continue;
			if (fish->fish_id() == fish_id) return fish;
		}
		//assert(!"GetFish Failed");
	} catch(...)
	{
		return NULL;
	}
	//CATCH_END //调试结束

	return NULL;
}

void FishManager::SceneSwitchIterator() {
  fish_vector_size_ = fish_vector_.size();
}

void FishManager::FreeSceneSwitchFish() {
  for (std::vector<Fish*>::size_type i = 0; i < fish_vector_size_; ++i) 
  {
    SAFE_DELETE(fish_vector_[i]);
  }
  fish_vector_.erase(fish_vector_.begin(), fish_vector_.begin() + fish_vector_size_);
  fish_vector_size_ = 0;

  std::vector<Fish*>::iterator iter;
  Fish* fish = NULL;
  for (int i=0; i<fish_vector_.size(); i++)
  {
	  fish = fish_vector_.at(i);
	  if (fish == NULL)
		  continue;
    fish->set_active(true);
  }
}

int FishManager::GetFishKindCount(FishKind fish_kind) {
  assert(fish_kind < FISH_KIND_COUNT);
  if (fish_kind >= FISH_KIND_COUNT)
    return 0;

  int fish_kind_count = 0;
  Fish* fish = NULL;
  for (int i=0; i<fish_vector_.size(); i++)
  {
	  fish = fish_vector_.at(i);
	  if (fish == NULL)
		  continue;
    if (fish->fish_kind() == fish_kind)
      ++fish_kind_count;
  }
  return fish_kind_count;
}

bool FishManager::BulletHitTest(Bullet* bullet) {

  Fish* fish = NULL;
  for (int i=0; i<fish_vector_.size(); i++)
  {
	  fish = fish_vector_.at(i);
	  if (fish == NULL)
		  continue;
    if (fish->BulletHitTest(bullet)) return true;
  }
  return false;
}

bool FishManager::NetHitTest(Bullet* bullet) {
  WORD me_chair_id = (CNewGameClient::GetMySelf())->m_iMyDeskStation;
 // if (bullet->firer_chair_id() != me_chair_id && bullet->android_chairid() != me_chair_id) //屏蔽 sdp20140508//通过真人发来的消息帮助机器人打到鱼
	//  return false;
  Fish* fish = NULL;
  int catch_count = 0;
  for (int i=0; i<fish_vector_.size(); i++)
  {
	  fish = fish_vector_.at(i);
	  if (fish == NULL)
		  continue;

    if (fish->NetHitTest(bullet)) 
	{
      GameManager::GetInstance().SendCatchFish(fish->fish_id(), bullet->firer_chair_id(), bullet->bullet_id(), bullet->bullet_kind(), bullet->bullet_mulriple());
      if ((++catch_count) >= kMaxCatchFishCount) 
		  return true;
    }
  }
  return false;
}

void FishManager::CatchFish(WORD chair_id, int fish_id, SCORE score) 
{
  assert(chair_id < GAME_PLAYER);
  Fish* fish = GetFish(fish_id);
  if (fish == NULL) 
  {
    //assert(!"CatchFish: not found fish");
    return;
  }

  fish->CatchFish(chair_id, score);

  FPoint cannon_pos = GameManager::GetInstance().GetCannonManager()->GetCannonPos((CNewGameClient::GetMySelf())->SwitchChairID(chair_id));
  std::vector<FPointAngle>& trace_vector = fish->trace_vector();
  FPointAngle& point_angle = trace_vector[fish->trace_index()];
  GameManager::GetInstance().GetCoinManager()->BuildCoin(point_angle, cannon_pos, chair_id, score);

  if (fish->fish_kind() == FISH_KIND_22) 
  {
    GameManager::GetInstance().set_lock(true);
  }
}

void FishManager::CatchSweepFish(WORD chair_id, int fish_id) {
  Fish* fish = GetFish(fish_id);
  if (fish == NULL) {
    //assert(!"CatchFish: not found fish");
    return;
  }

  CMD_C_CatchSweepFish catch_sweep_fish;
  memset(&catch_sweep_fish, 0, sizeof(catch_sweep_fish));
  catch_sweep_fish.fish_id = fish_id;
  catch_sweep_fish.chair_id = chair_id;
  if (fish->fish_kind() == FISH_KIND_23) {
    std::vector<FPointAngle>& trace_vector = fish->trace_vector();
    FPointAngle& point_angle = trace_vector[fish->trace_index()];
    BoundingBox box;
    box.CreateBoundingBox(GameManager::GetInstance().bomb_range_width(), GameManager::GetInstance().bomb_range_height(), point_angle.x, point_angle.y, point_angle.angle);

    Fish* fish_temp = NULL;
	for (int i=0; i<fish_vector_.size(); i++)
	{
		fish_temp = fish_vector_.at(i);
		if (fish_temp == NULL)
			continue;
      if (fish_temp == fish) continue;
      if (!fish_temp->active()) continue;
      FPointAngle& fish_pos = fish_temp->trace_vector()[fish_temp->trace_index()];
      if (InsideScreen(fish_pos) && box.ComputeCollision(fish_pos.x, fish_pos.y)) {
        catch_sweep_fish.catch_fish_id[catch_sweep_fish.catch_fish_count++] = fish_temp->fish_id();
        if (catch_sweep_fish.catch_fish_count >= arraysize(catch_sweep_fish.catch_fish_id)) break;
      }
    }
    GameManager::GetInstance().SendCatchSweepFish(&catch_sweep_fish);
  } else if (fish->fish_kind()  == FISH_KIND_24) {
    Fish* fish_temp = NULL;
	for (int i=0; i<fish_vector_.size(); i++)
	{
		fish_temp = fish_vector_.at(i);
		if (fish_temp == NULL)
			continue;
      if (fish_temp == fish) continue;
      if (!fish_temp->active()) continue;
      FPointAngle& fish_pos = fish_temp->trace_vector()[fish_temp->trace_index()];
      if (InsideScreen(fish_pos)) {
        catch_sweep_fish.catch_fish_id[catch_sweep_fish.catch_fish_count++] = fish_temp->fish_id();
        if (catch_sweep_fish.catch_fish_count >= arraysize(catch_sweep_fish.catch_fish_id)) break;
      }
    }
    GameManager::GetInstance().SendCatchSweepFish(&catch_sweep_fish);
  } else if (fish->fish_kind() >= FISH_KIND_31 && fish->fish_kind() <= FISH_KIND_40) {
    std::vector<Fish*>::iterator iter;
    Fish* fish_temp = NULL;
    FishKind fish_kind = FishKind(fish->fish_kind() - FISH_KIND_31);
	for (int i=0; i<fish_vector_.size(); i++)
	{
		fish_temp = fish_vector_.at(i);
		if (fish_temp == NULL)
			continue;
      if (fish_temp == fish) continue;
      if (!fish_temp->active()) continue;
      if (fish_temp->fish_kind() != fish_kind) continue;
      FPointAngle& fish_pos = fish_temp->trace_vector()[fish_temp->trace_index()];
      if (InsideScreen(fish_pos)) {
        catch_sweep_fish.catch_fish_id[catch_sweep_fish.catch_fish_count++] = fish_temp->fish_id();
        if (catch_sweep_fish.catch_fish_count >= arraysize(catch_sweep_fish.catch_fish_id)) break;
      }
    }
    GameManager::GetInstance().SendCatchSweepFish(&catch_sweep_fish);
  }
}

void FishManager::CatchSweepFishResult(WORD chair_id, int fish_id, SCORE score, int* catch_fish_id, int catch_fish_count) {
  Fish* fish = GetFish(fish_id);
  if (fish != NULL) {
    assert(fish->fish_kind() == FISH_KIND_23 || fish->fish_kind() == FISH_KIND_24 || (fish->fish_kind() >= FISH_KIND_31 && fish->fish_kind() <= FISH_KIND_40));
    fish->CatchFish(chair_id, score);

    FPoint cannon_pos = GameManager::GetInstance().GetCannonManager()->GetCannonPos((CNewGameClient::GetMySelf())->SwitchChairID(chair_id));
    std::vector<FPointAngle>& trace_vector = fish->trace_vector();
    FPointAngle& point_angle = trace_vector[fish->trace_index()];
    GameManager::GetInstance().GetCoinManager()->BuildCoin(point_angle, cannon_pos, chair_id, score);
  }

  for (int i = 0; i < catch_fish_count; ++i) {
    fish = GetFish(catch_fish_id[i]);
    if (fish == NULL) continue;
    fish->CatchFish(chair_id, 0);
  }
}

void FishManager::HitFishLK(WORD chair_id, int fish_id, int fish_mulriple) 
{
	Fish* fish = NULL;
	if (fish_vector_.size() == 0)
	{
		return;
	}
	for (int i=0; i<fish_vector_.size(); i++)
	{
		/*fish = *iter;*/
		fish = fish_vector_.at(i);
		if (fish == NULL)
			continue;
		if (fish->fish_id() == fish_id) 
		{
			break;
		}
	}
	if (fish == NULL) 
		return;

	//assert(fish->fish_kind() == FISH_KIND_21);
	if (fish->fish_kind() != FISH_KIND_21) 
		return;
	fish->set_fish_mulriple(fish_mulriple);
}

int FishManager::LockFish(FishKind* lock_fish_kind, int last_lock_fish_id, FishKind last_fish_kind) {
  Fish* fish = NULL;
  std::vector<Fish*>::iterator iter;
  FishKind next_fish_kind = last_fish_kind;
  if (next_fish_kind != FISH_KIND_COUNT) {
    bool exist_fish_kind[FISH_KIND_COUNT];
    memset(exist_fish_kind, 0, sizeof(exist_fish_kind));
	for (int i=0; i<fish_vector_.size(); i++)
	{
		fish = fish_vector_.at(i);
		if (fish == NULL)
			continue;
      if (!fish->active()) continue;
      if (fish->fish_status() == FISH_DIED) continue;
      if (fish->fish_id() == last_lock_fish_id) continue;
      if (fish->fish_kind() < FISH_KIND_16) continue;
      FPointAngle& point_angle = fish->trace_vector()[fish->trace_index()];
      if (!InsideScreen(point_angle)) continue;
      exist_fish_kind[fish->fish_kind()] = true;
    }
    for (int i = FISH_KIND_16; i < FISH_KIND_COUNT; ++i) {
      next_fish_kind = static_cast<FishKind>((next_fish_kind + 1) % FISH_KIND_COUNT);
      if (next_fish_kind < FISH_KIND_16) next_fish_kind = FISH_KIND_16;
      if (exist_fish_kind[next_fish_kind]) break;
    }
  }
  
  for (int i=0; i<fish_vector_.size(); i++)
  {
	  fish = fish_vector_.at(i);
	  if (fish == NULL)
		  continue;
    if (!fish->active()) continue;
    if (fish->fish_status() == FISH_DIED) continue;
    if (fish->fish_id() == last_lock_fish_id) continue;
    if (fish->fish_kind() < FISH_KIND_16) continue;
    FPointAngle& point_angle = fish->trace_vector()[fish->trace_index()];
    if (!InsideScreen(point_angle)) continue;
    if (next_fish_kind == FISH_KIND_COUNT || next_fish_kind == fish->fish_kind()) {
      if (lock_fish_kind != NULL) *lock_fish_kind = fish->fish_kind();
      return fish->fish_id();
    }
  }

  if (last_lock_fish_id > 0) {
    if (lock_fish_kind != NULL) *lock_fish_kind = last_fish_kind;
    return last_lock_fish_id;
  } else {
    if (lock_fish_kind != NULL) *lock_fish_kind = FISH_KIND_COUNT;
    return 0;
  }
}

bool FishManager::LockFishReachPos(int lock_fishid, size_t size, FPointAngle* pos) {
  Fish* fish = NULL;
  for (int i=0; i<fish_vector_.size(); i++)
  {
	  fish = fish_vector_.at(i);
	  if (fish == NULL)
		  continue;
    if (!fish->active()) continue;
    if (fish->fish_id() == lock_fishid) {
      if (size + fish->trace_index() < fish->trace_vector().size()) {
        FPointAngle& point_angle = fish->trace_vector()[fish->trace_index() + size];
        pos->x = point_angle.x;
        pos->y = point_angle.y;
        pos->angle = point_angle.angle;
      } else {
        FPointAngle& point_angle = fish->trace_vector().back();
        pos->x = point_angle.x;
        pos->y = point_angle.y;
        pos->angle = point_angle.angle;
      }
      return true;
    }
  }

  return false;
}

bool FishManager::InsideScreen(const FPointAngle& pos) {
  float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
  float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
  if (pos.x > 0.f && pos.x < screen_width && pos.y > 0.f && pos.y < screen_height) return true;
  return false;
}