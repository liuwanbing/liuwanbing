
#include "StdAfx.h"
#include "bullet_manager.h"
#include "math_aide.h"
#include "math.h"
#include "game_manager.h"
#include "fish_manager.h"
#include "cannon_manager.h"
#include "coin_manager.h"
#include "sound_manager.h"
#include "lock_fish_manager.h"
#include "NewGameClient.h"
#ifdef TEST
#include "bounding_box.h"
#endif

Bullet::Bullet(BulletKind bullet_kind, int bullet_id, int bullet_mulriple, WORD firer_chair_id, float net_radius)
  : bullet_kind_(bullet_kind), bullet_status_(BULLET_INVALID), bullet_id_(bullet_id), bullet_mulriple_(bullet_mulriple),
	firer_chair_id_(firer_chair_id),android_chairid_(INVALID_CHAIR), lock_fishid_(0), net_radius_(net_radius), speed_(0.f),
  angle_(0.f), trace_index_(0) {
  hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
  char file_name[MAX_PATH] = { 0 };
  if (bullet_kind_ >= BULLET_KIND_1_ION) {
    _snprintf(file_name, arraysize(file_name), "bullet%d_ion", bullet_kind_ - BULLET_KIND_1_ION + 1);
    ani_bullet_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
  } else {
    _snprintf(file_name, arraysize(file_name), "bullet%d_norm%d", bullet_kind_ + 1, firer_chair_id + 1);
    ani_bullet_ = new hgeAnimation(*resource_manager->GetAnimation(file_name));
  }
  int net_idx = bullet_kind_ % 4;
  if (net_idx == 3) 
  {
    ani_net_ = new hgeAnimation(*resource_manager->GetAnimation("net4"));
  } 
  else if (net_idx == 2) 
  {
    ani_net_ = new hgeAnimation(*resource_manager->GetAnimation("net3"));
  } 
  else 
  {
    ani_net_ = new hgeAnimation(*resource_manager->GetAnimation("net2"));
  }
  ani_bullet_->Play();

#ifdef TEST
  hge_ = hgeCreate(HGE_VERSION);
#endif
}

Bullet::~Bullet() {
  SAFE_DELETE(ani_bullet_);
  SAFE_DELETE(ani_net_);
#ifdef TEST
  hge_->Release();
#endif
}

void Bullet::CastingNet() {
  bullet_status_ = BULLET_CASTING_NET;
  if (ani_net_ != NULL) {
    ani_net_->SetFrame(0);
    ani_net_->Play();
  }
  SoundManager::GetInstance().PlayGameEffect(CASTING);
}

FPoint Bullet::GetCurPos() 
{
  if (trace_index_ >= trace_vector_.size()) 
  {
    FPoint point = { 0.f, 0.f };
    return point;
  }
  return trace_vector_[trace_index_];
}

bool Bullet::IsValid() 
{
  return trace_index_ < trace_vector_.size();
}



bool Bullet::OnFrame(float delta_time) 
{
	if (trace_index_ >= trace_vector_.size()) 
	{
		return true;
	}

	if (bullet_status_ == BULLET_INVALID) 
	{
		trace_index_ = 0;
		bullet_status_ = BULLET_FLYING;
		ani_bullet_->Update(delta_time);
		return false;
	}

	if (bullet_status_ == BULLET_FLYING) 
	{
		++trace_index_;
		if (trace_index_ >= trace_vector_.size()) 
		{
			return true;
		}
		ani_bullet_->Update(delta_time);
	} 
	else 
	{
		if (!ani_net_->IsPlaying()) 
		{
			return true;
		} 
		else 
		{
			ani_net_->Update(delta_time);
		}
	}
	return false;
}

bool Bullet::OnRender(float offset_x, float offset_y, float hscale, float vscale) 
{
  if (trace_index_ >= trace_vector_.size()) return true;

  FPoint& bullet_trace = trace_vector_[trace_index_];

  if (bullet_status_ == BULLET_CASTING_NET) 
  {
    ani_net_->RenderEx(offset_x + bullet_trace.x, offset_y + bullet_trace.y, angle_, hscale, vscale);
#ifdef TEST
    RenderCircle(bullet_trace.x, bullet_trace.y, net_radius_, 36, 0xffff0000);
#endif
  }
  else 
  {
    ani_bullet_->RenderEx(offset_x + bullet_trace.x, offset_y + bullet_trace.y, angle_, hscale, vscale);
  }
  return false;
}

#ifdef TEST
void Bullet::RenderCircle(float center_x, float center_y, float radius, int segments, DWORD color) {
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

BulletManager::BulletManager() {
  hge_ = hgeCreate(HGE_VERSION);
}

BulletManager::~BulletManager() {
  std::vector<Bullet*>::iterator iter;
  for (iter = bullet_vector_.begin(); iter != bullet_vector_.end(); ++iter) {
    SAFE_DELETE(*iter);
  }
  bullet_vector_.clear();

  hge_->Release();
}

bool BulletManager::LoadGameResource() {
  char file_name[MAX_PATH] = { 0 };

  hgeAnimation* anim = NULL;
  for (int i = 0; i <= BULLET_KIND_4_NORMAL; ++i) {
    _snprintf(file_name, arraysize(file_name), "bullet%d_ion", i + 1);
    anim = GameManager::GetInstance().GetResourceManager()->GetAnimation(file_name);
    for (WORD j = 0; j < GAME_PLAYER; ++j) {
      _snprintf(file_name, arraysize(file_name), "bullet%d_norm%d", i + 1, j + 1);
      anim = GameManager::GetInstance().GetResourceManager()->GetAnimation(file_name);
    }
  }

  GameManager::GetInstance().GetResourceManager()->GetAnimation("net2");
  GameManager::GetInstance().GetResourceManager()->GetAnimation("net3");
  GameManager::GetInstance().GetResourceManager()->GetAnimation("net4");

  return true;
}

//整理数据sdp
void BulletManager::TidyData()
{
	Bullet* bullet = NULL;
	std::vector<Bullet*> bullet_vector_2;
	bullet_vector_2.clear();
	for (int i=0; i<bullet_vector_.size(); i++)
	{
		bullet = bullet_vector_.at(i);
		if (bullet == NULL)
			continue;

		//子弹类型
		if (bullet->bullet_kind() < BULLET_KIND_1_NORMAL || bullet->bullet_kind() >= BULLET_KIND_COUNT)
			continue;
		//发射子弹玩家
		if (!(CNewGameClient::GetMySelf())->isPlayer(bullet->firer_chair_id()))
			continue;

		bullet_vector_2.push_back(bullet);
	}

	bullet_vector_.clear();
	for (int i=0; i<bullet_vector_2.size(); i++)
	{
		bullet = bullet_vector_2.at(i);
		if (bullet == NULL)
			continue;

		bullet_vector_.push_back(bullet);
	}
}

bool BulletManager::OnFrame(float delta_time) 
{
	FishManager* fish_manager = GameManager::GetInstance().GetFishManager();

	//整理数据
	TidyData();

	Bullet* bullet = NULL;
	for (int i = 0; i < bullet_vector_.size();++i) 
	{
		bullet = bullet_vector_.at(i);
		if (bullet == NULL)
			continue;
		if (bullet->OnFrame(delta_time)) 
		{
			if (bullet->bullet_status() == BULLET_FLYING) 
			{
				ReboundBullet(bullet);
			} 
			else 
			{
				bullet_vector_.at(i) = NULL;
			}
		} 
		else 
		{
			if (bullet->bullet_status() == BULLET_FLYING && fish_manager->BulletHitTest(bullet)) 
			{
				bullet->CastingNet();
				fish_manager->NetHitTest(bullet);
			}
		}
	}

	TidyData();
	return false;
}

bool BulletManager::OnRender(float offset_x, float offset_y, float hscale, float vscale) 
{
	TidyData();
	Bullet* bullet = NULL;
	for (int i = 0; i < bullet_vector_.size(); ++i) 
	{
		bullet = bullet_vector_.at(i);
		if (bullet == NULL)
			continue;
		bullet->OnRender(offset_x, offset_y, hscale, vscale);
	}
	return false;
}

bool BulletManager::FreeBullet(Bullet* bullet) {
  std::vector<Bullet*>::iterator iter;
  for (iter = bullet_vector_.begin(); iter != bullet_vector_.end(); ++iter) {
    if (bullet == *iter) {
      bullet_vector_.erase(iter);
      SAFE_DELETE(bullet);
      return true;
    }
  }

  assert(!"FreeBullet Failed");
  return false;
}

void BulletManager::FreeAllBullet() {
  std::vector<Bullet*>::iterator iter;
  for (iter = bullet_vector_.begin(); iter != bullet_vector_.end(); ++iter) {
    SAFE_DELETE(*iter);
  }
  bullet_vector_.clear();
}

int BulletManager::Fire(float src_x_pos, float src_y_pos, float angle, BulletKind bullet_kind,int bullet_id, int bullet_mulriple, WORD firer_chair_id, float bullet_speed,float net_radius, WORD android_chairid,int lock_fishid) 
{
	assert(firer_chair_id < GAME_PLAYER);
	if (firer_chair_id >= GAME_PLAYER) 
		return 0;

	Bullet* bullet = new Bullet(bullet_kind, bullet_id, bullet_mulriple, firer_chair_id, net_radius);
	bullet->set_android_chairid(android_chairid);
	bullet->set_lock_fishid(lock_fishid);

	float init_x_pos[2] = { src_x_pos };
	float init_y_pos[2] = { src_y_pos };
	bool lock_success = false;

	do 
	{
		if (lock_fishid <= 0) 
			break;
		if (GameManager::GetInstance().lock()) 
			break;
		bullet->trace_vector().clear();
		LockFishManager* lock_fish = GameManager::GetInstance().GetLockFishManager();
		FishManager* fish_manager = GameManager::GetInstance().GetFishManager();
		FPoint lock_pos = lock_fish->LockPos(firer_chair_id);
		init_x_pos[1] = lock_pos.x;
		init_y_pos[1] = lock_pos.y;
		MathAide::BuildLinear(init_x_pos, init_y_pos, 2, bullet->trace_vector(), bullet_speed);

		FPointAngle fish_reach_pos;
		bool success = fish_manager->LockFishReachPos(lock_fishid, bullet->trace_vector().size(), &fish_reach_pos);
		if (!success) 
			break;

		//FPoint& center = bullet->trace_vector()[bullet->trace_vector().size() / 2];
		FPoint& center = bullet->trace_vector().back();

		float bezier_x[3] = { src_x_pos, center.x, fish_reach_pos.x };
		float bezier_y[3] = { src_y_pos, center.y, fish_reach_pos.y };
		MathAide::BuildBezierFast(bezier_x, bezier_y, 3, bullet->trace_vector(), bullet_speed);

		std::vector<FPoint> fish_move_vector;
		if (FishManager::InsideScreen(fish_reach_pos)) 
		{
			init_x_pos[0] = fish_reach_pos.x;
			init_y_pos[0] = fish_reach_pos.y;
			GetTargetPoint((CNewGameClient::GetMySelf())->SwitchChairID(firer_chair_id), init_x_pos[0], init_y_pos[0], angle, init_x_pos[1], init_y_pos[1]);
			MathAide::BuildLinear(init_x_pos, init_y_pos, 2, fish_move_vector, bullet_speed);
			bullet->trace_vector().pop_back();
			std::copy(fish_move_vector.begin(), fish_move_vector.end(), std::back_inserter(bullet->trace_vector()));
		} 
		else 
		{
			FPointAngle temp_pos_angle;
			for (std::vector<FPoint>::reverse_iterator iter = bullet->trace_vector().rbegin(); iter != bullet->trace_vector().rend(); ++iter) {
				FPoint& pos = *iter;
				temp_pos_angle.x = pos.x;
				temp_pos_angle.y = pos.y;
				temp_pos_angle.angle = angle;
				if (FishManager::InsideScreen(temp_pos_angle)) 
				{
					init_x_pos[0] = pos.x;
					init_y_pos[0] = pos.y;
					bullet->trace_vector().erase(iter.base(), bullet->trace_vector().end());
					break;
				}
			}
			GetTargetPoint((CNewGameClient::GetMySelf())->SwitchChairID(firer_chair_id), init_x_pos[0], init_y_pos[0], angle, init_x_pos[1], init_y_pos[1]);
			MathAide::BuildLinear(init_x_pos, init_y_pos, 2, fish_move_vector, bullet_speed);
			//bullet->trace_vector().pop_back();
			std::copy(fish_move_vector.begin(), fish_move_vector.end(), std::back_inserter(bullet->trace_vector()));
		}

		lock_success = true;
	} while (0);

	if (!lock_success) 
	{
		init_x_pos[0] =  src_x_pos;
		init_y_pos[0] =  src_y_pos;
		GetTargetPoint((CNewGameClient::GetMySelf())->SwitchChairID(firer_chair_id), src_x_pos, src_y_pos, angle, init_x_pos[1], init_y_pos[1]);
		MathAide::BuildLinear(init_x_pos, init_y_pos, 2, bullet->trace_vector(), bullet_speed);
	}

	bullet->set_speed(bullet_speed);
	bullet->set_angle(angle);
	bullet->set_trace_index(0);
	bullet_vector_.push_back(bullet);

	return bullet_id;
}

void BulletManager::ReboundBullet(Bullet* bullet) 
{
  bullet->set_lock_fishid(0);
  FPoint& point_back = bullet->trace_vector().back();;
  FPoint& point_front = bullet->trace_vector().front();
  float init_x_pos[2] = { point_back.x };
  float init_y_pos[2] = { point_back.y };

  float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
  float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
  WORD chair_id;
  float angle;
  if (init_x_pos[0] == screen_width && init_y_pos[0] > 0.f && init_y_pos[0] < screen_height) 
  {
	  chair_id = 6;
	  if (point_front.y > point_back.y) {
		  angle = MathAide::CalcAngle(point_front.x, point_front.y - (point_front.y - point_back.y) * 2, point_back.x, point_back.y);
	  }
	  else {
		  angle = MathAide::CalcAngle(point_front.x, point_front.y + (point_back.y - point_front.y) * 2, point_back.x, point_back.y);
	  }
  }
  else if (init_y_pos[0] == screen_height) 
  {
	  chair_id = 5;
	  if (point_front.x > point_back.x) 
	  {
		  angle = MathAide::CalcAngle(point_front.x - (point_front.x - point_back.x) * 2, point_front.y, point_back.x, point_back.y);
	  }
	  else 
	  {
		  angle = MathAide::CalcAngle(point_front.x + (point_back.x - point_front.x) * 2, point_front.y, point_back.x, point_back.y);
	  }
  } 
  else if (init_x_pos[0] == 0.f && init_y_pos[0] > 0.f && init_y_pos[0] < screen_height) 
  {
	  chair_id = 7;
	  if (point_front.y > point_back.y) 
	  {
		  angle = MathAide::CalcAngle(point_front.x, point_front.y - (point_front.y - point_back.y) * 2, point_back.x, point_back.y);
	  }
	  else 
	  {
		  angle = MathAide::CalcAngle(point_front.x, point_front.y + (point_back.y - point_front.y) * 2, point_back.x, point_back.y);
	  }
  } 
  else 
  {
	  chair_id = 0;
	  if (point_front.x > point_back.x) 
	  {
		  angle = MathAide::CalcAngle(point_front.x - (point_front.x - point_back.x) * 2, point_front.y, point_back.x, point_back.y);
	  }
	  else
	  {
		  angle = MathAide::CalcAngle(point_front.x + (point_back.x - point_front.x) * 2, point_front.y, point_back.x, point_back.y);
	  }
  }

  GetTargetPoint(chair_id, init_x_pos[0], init_y_pos[0], angle, init_x_pos[1], init_y_pos[1]);

  MathAide::BuildLinear(init_x_pos, init_y_pos, 2, bullet->trace_vector(), bullet->speed());
  bullet->set_angle(angle);
  bullet->set_trace_index(0);
}

void BulletManager::GetTargetPoint(WORD firer_chair_id, float src_x_pos, float src_y_pos, float angle, float& target_x_pos, float& target_y_pos) 
{
  assert(firer_chair_id < GAME_PLAYER);
  // 数学不好 有更好的求目标点方法？

  float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
  float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
  float hscale = screen_width / kResolutionWidth;
  float vscale = screen_height / kResolutionHeight;

  if (firer_chair_id == 0 || firer_chair_id == 1 || firer_chair_id == 2) 
  {
    if (angle < M_PI) 
	{
      float temp_angle = MathAide::CalcAngle(screen_width, screen_height, src_x_pos, src_y_pos);
      if (temp_angle > angle) 
	  {
        target_x_pos = screen_width;
        target_y_pos = src_y_pos + tanf(angle - M_PI_2) * (screen_width - src_x_pos);
      } 
	  else 
	  {
        target_x_pos = src_x_pos + (screen_height - src_y_pos) * tanf(M_PI - angle);
        target_y_pos = screen_height;
      }
    } else {
      float temp_angle = MathAide::CalcAngle(0.f, screen_height, src_x_pos, src_y_pos);
      if (temp_angle > angle) {
        target_x_pos = src_x_pos - (screen_height - src_y_pos) * tanf(angle - M_PI);
        target_y_pos = screen_height;
      } else {
        target_x_pos = 0.f;
        target_y_pos = src_y_pos + tanf(M_PI + M_PI_2 - angle) * src_x_pos;
      }
    }
  }
  else if (firer_chair_id == 6) {
    if (angle > (M_PI + M_PI_2)) {
      float temp_angle = MathAide::CalcAngle(0.f, 0.f, src_x_pos, src_y_pos);
      if (temp_angle > angle) {
        target_x_pos = 0.f;
        target_y_pos = src_y_pos - tanf(angle - M_PI - M_PI_2) * (src_x_pos);
      } else {
        target_x_pos = src_x_pos - src_y_pos * tanf(2 * M_PI - angle);
        target_y_pos = 0.f;
      }
    } else {
      float temp_angle = MathAide::CalcAngle(0.f, screen_height, src_x_pos, src_y_pos);
      if (temp_angle > angle) {
        target_x_pos = src_x_pos - (screen_height - src_y_pos) * tanf(angle - M_PI);
        target_y_pos = screen_height;
      } else {
        target_x_pos = 0.f;
        target_y_pos = src_y_pos + tanf(M_PI + M_PI_2 - angle) * src_x_pos;
      }
    }
  } else if (firer_chair_id == 5|| firer_chair_id == 3 || firer_chair_id == 4) {
    if (angle == M_PI / 2) {
      target_x_pos = screen_width;
      target_y_pos = src_y_pos;
    } else if (angle > M_PI * 2) {
      float temp_angle = MathAide::CalcAngle(screen_width, 0.f, src_x_pos, src_y_pos);
      if (temp_angle > angle) {
        target_x_pos = src_x_pos + src_y_pos * tanf(angle - M_PI * 2);
        target_y_pos = 0.f;
      } else {
        target_x_pos = screen_width;
        target_y_pos = src_y_pos - (screen_width - src_x_pos) * tanf(M_PI_2 + M_PI * 2 - angle);
      }
    } else {
      float temp_angle = MathAide::CalcAngle(0.f, 0.f, src_x_pos, src_y_pos);
      if (temp_angle > angle) {
        target_x_pos = 0.f;
        target_y_pos = src_y_pos - tanf(angle - M_PI - M_PI_2) * src_x_pos;
      } else {
        target_x_pos = src_x_pos - src_y_pos * tanf(2 * M_PI - angle);
        target_y_pos = 0.f;
      }
    }
  } else {
    if (angle == M_PI * 2) {
      target_x_pos = src_x_pos;
      target_y_pos = 0.f;
    } else if (angle > M_PI * 2) {
      float temp_angle = MathAide::CalcAngle(screen_width, 0.f, src_x_pos, src_y_pos);
      if (temp_angle > angle) {
        target_x_pos = src_x_pos + src_y_pos * tanf(angle - M_PI * 2);
        target_y_pos = 0.f;
      } else {
        target_x_pos = screen_width;
        target_y_pos = src_y_pos - tanf(M_PI * 2 + M_PI_2 - angle) * (screen_width - src_x_pos);
      }
    } else {
      float temp_angle = MathAide::CalcAngle(screen_width, screen_height, src_x_pos, src_y_pos);
      if (temp_angle > angle) {
        target_x_pos = screen_width;
        target_y_pos = src_y_pos + tanf(angle - M_PI_2) * (screen_width - src_x_pos);
      } else {
        target_x_pos = src_x_pos + (screen_height - src_y_pos) * tanf(M_PI - angle);
        target_y_pos = screen_height;
      }
    }
  }
}
