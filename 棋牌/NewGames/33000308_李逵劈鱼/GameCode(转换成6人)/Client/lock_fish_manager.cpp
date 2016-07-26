
#include "stdafx.h"
#include "lock_fish_manager.h"
#include "game_manager.h"
#include "hgeresource.h"
#include "cannon_manager.h"
#include "math_aide.h"
#include "pos_define.h"
#include "NewGameClient.h"


const float kLockLineInterval = 64.f;

const float kRotateRadius = 10.f;
const float kRotateAngle = 10.f * M_PI / 180.f;

LockFishManager::LockFishManager() : rotate_angle_(0.f) {
  for (WORD i = 0; i < GAME_PLAYER; ++i) {
    lock_fish_id_[i] = 0;
    lock_fish_kind_[i] = FISH_KIND_COUNT;
  }
}

LockFishManager::~LockFishManager() {
}

bool LockFishManager::LoadGameResource() 
{
  hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
  if (NULL == resource_manager)
  {
	  return false;
  }
  ani_lock_flag_ = resource_manager->GetAnimation("lock_flag");
  if (NULL == ani_lock_flag_)
  {
	  return false;
  }

  spr_lock_line_ = resource_manager->GetSprite("lock_line");
  if (NULL == spr_lock_line_)
  {
	  return false;
  }

  char file_name[MAX_PATH] = { 0 };
  for (WORD i = 0; i < GAME_PLAYER; ++i) 
  {
    _snprintf(file_name, arraysize(file_name), "lock_flag_%d", i + 1);
    spr_lock_flag_[i] = resource_manager->GetSprite(file_name);
	if (NULL == spr_lock_flag_[i])
	{
		return false;
	}

  }

  return true;
}

bool LockFishManager::OnFrame(float delta_time) 
{
	CannonManager* cannon_manager = GameManager::GetInstance().GetCannonManager();
	for (WORD i = 0; i < GAME_PLAYER; ++i) 
	{
		if (lock_fish_id_[i] == 0) continue;
		FPoint cannon_pos = cannon_manager->GetCannonPos((CNewGameClient::GetMySelf())->SwitchChairID(i));
		FPoint mouse_pos = LockPos(i);
		float angle = MathAide::CalcAngle(mouse_pos.x, mouse_pos.y, cannon_pos.x, cannon_pos.y);
		cannon_manager->SetCurrentAngle(i, angle);
	}

	rotate_angle_ += kRotateAngle;
	if (rotate_angle_ > M_PI * 2) 
	{
		rotate_angle_ -= M_PI * 2;
	}

  return false;
}

bool LockFishManager::OnRender(float offset_x, float offset_y, float hscale, float vscale)
{
	for (WORD i = 0; i < GAME_PLAYER; ++i) {
		if (lock_fish_id_[i] == 0) continue;
		if (lock_line_trace_[i].size() == 0) continue;
		float rotate_x = (kPosLockFlag[(CNewGameClient::GetMySelf())->SwitchChairID(i)].x + kRotateRadius * cosf(rotate_angle_)) * hscale;
		float rotate_y = (kPosLockFlag[(CNewGameClient::GetMySelf())->SwitchChairID(i)].y + kRotateRadius * sinf(rotate_angle_)) * vscale;
		ani_lock_flag_->SetFrame(lock_fish_kind_[i]);
		ani_lock_flag_->RenderEx(rotate_x, rotate_y, kChairDefaultAngle[(CNewGameClient::GetMySelf())->SwitchChairID(i)], hscale, vscale);
		for (FPointVector::size_type j = 0; j < lock_line_trace_[i].size(); ++j) 
		{
			if (j == lock_line_trace_[i].size() - 1)
			{
				spr_lock_flag_[i]->RenderEx(offset_x + lock_line_trace_[i][j].x, offset_y + lock_line_trace_[i][j].y,
					kChairDefaultAngle[(CNewGameClient::GetMySelf())->SwitchChairID(i)], hscale, hscale);
			} 
			else
			{
				spr_lock_line_->RenderEx(offset_x + lock_line_trace_[i][j].x, offset_y + lock_line_trace_[i][j].y,
					kChairDefaultAngle[(CNewGameClient::GetMySelf())->SwitchChairID(i)], hscale, hscale);
			}
		}
	}

  return false;
}

void LockFishManager::UpdateLockTrace(WORD chair_id, float fish_pos_x, float fish_pos_y) {
  lock_line_trace_[chair_id].clear();

  FPoint cannon_pos = GameManager::GetInstance().GetCannonManager()->GetCannonPos((CNewGameClient::GetMySelf())->SwitchChairID(chair_id));
  float x[2], y[2];
  x[0] = cannon_pos.x;
  y[0] = cannon_pos.y;
  x[1] = fish_pos_x;
  y[1] = fish_pos_y;
  MathAide::BuildLinear(x, y, 2, lock_line_trace_[chair_id], kLockLineInterval);
}

void LockFishManager::ClearLockTrace(WORD chair_id) {
  lock_line_trace_[chair_id].clear();
  lock_fish_id_[chair_id] = 0;
  lock_fish_kind_[chair_id] = FISH_KIND_COUNT;
}

FPoint LockFishManager::LockPos(WORD chair_id) {
  if (lock_line_trace_[chair_id].size() == 0) {
    FPoint pos = { 0.f, 0.f };
    return pos;
  } else {
    return lock_line_trace_[chair_id].back();
  }
}
