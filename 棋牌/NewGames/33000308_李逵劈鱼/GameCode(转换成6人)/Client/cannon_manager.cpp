
#include "StdAfx.h"
#include "cannon_manager.h"
#include "game_manager.h"
#include "pos_define.h"
#include "NewGameClient.h"

const float kRotateRadius = 10.f;
const float kRotateAngle = 10.f * M_PI / 180.f;

CannonManager::CannonManager() : spr_cannon_base_(NULL), spr_cannon_plate_(NULL), rotate_angle_(0.f) 
{
  hge_ = hgeCreate(HGE_VERSION);

  for (WORD i = 0; i < GAME_PLAYER; ++i) 
  {
    current_angle_[i] = kChairDefaultAngle[(CNewGameClient::GetMySelf())->SwitchChairID(i)];
    current_bullet_kind_[i] = /*BULLET_KIND_1_NORMAL*/BULLET_KIND_3_NORMAL;
    current_mulriple_[i] = 1000;
    fish_score_[i] = 0;
  }
}

CannonManager::~CannonManager() 
{
  hge_->Release();
}

void CannonManager::SetFishScore(WORD chair_id, SCORE swap_fish_score) {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return;
  fish_score_[chair_id] += swap_fish_score;
}

void CannonManager::ResetFishScore(WORD chair_id) {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return;
  fish_score_[chair_id] = 0;
}

SCORE CannonManager::GetFishScore(WORD chair_id) {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return 0;
  return fish_score_[chair_id];
}

bool CannonManager::LoadGameResource() {
  char file_name[MAX_PATH] = { 0 };

  hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
  spr_cannon_base_ = resource_manager->GetSprite("cannon_base");
  spr_cannon_plate_ = resource_manager->GetSprite("cannon_plate");
  spr_cannon_num_ = resource_manager->GetAnimation("cannon_num");

  for (WORD i = 0; i < GAME_PLAYER; ++i) {
    _snprintf(file_name, arraysize(file_name), "deco%d", i + 1);
    spr_cannon_deco_[i] = resource_manager->GetSprite(file_name);
  }

  for (int i = 0; i < BULLET_KIND_COUNT; ++i) {
    if (i >= BULLET_KIND_1_ION) {
      _snprintf(file_name, arraysize(file_name), "cannonIon%d", i - BULLET_KIND_1_ION + 1);
      spr_cannon_[i] = resource_manager->GetSprite(file_name);
    } else {
      _snprintf(file_name, arraysize(file_name), "cannonNormal%d", i + 1);
      spr_cannon_[i] = resource_manager->GetSprite(file_name);
    }
  }

  spr_score_box_ = resource_manager->GetSprite("score_box");
  spr_credit_num_ = resource_manager->GetAnimation("credit_num");
  spr_card_ion_ = resource_manager->GetSprite("card_ion");

  return true;
}

bool CannonManager::OnFrame(float delta_time) 
{
	rotate_angle_ += kRotateAngle;
	if (rotate_angle_ > M_PI * 2) {
		rotate_angle_ -= M_PI * 2;
	}
	return false;
}

bool CannonManager::OnRender(float offset_x, float offset_y, float hscale, float vscale) 
{
	FPoint cannon_pos;
	static float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
	static float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
	for (WORD i = 0; i < GAME_PLAYER; ++i) 
	{
		if(!(CNewGameClient::GetMySelf())->isPlayer(i))
			continue;
		cannon_pos = GetCannonPos((CNewGameClient::GetMySelf())->SwitchChairID(i));
		spr_cannon_base_->RenderEx(cannon_pos.x, cannon_pos.y, kChairDefaultAngle[(CNewGameClient::GetMySelf())->SwitchChairID(i)], hscale, vscale);
		spr_cannon_deco_[i]->RenderEx(kPosDeco[(CNewGameClient::GetMySelf())->SwitchChairID(i)].x * hscale, kPosDeco[(CNewGameClient::GetMySelf())->SwitchChairID(i)].y * vscale, kChairDefaultAngle[(CNewGameClient::GetMySelf())->SwitchChairID(i)], hscale, vscale);
		spr_cannon_[current_bullet_kind_[i]]->RenderEx(kPosGun[(CNewGameClient::GetMySelf())->SwitchChairID(i)].x * hscale, kPosGun[(CNewGameClient::GetMySelf())->SwitchChairID(i)].y * vscale, current_angle_[i], hscale, vscale);
		spr_cannon_plate_->RenderEx(kPosSeat[(CNewGameClient::GetMySelf())->SwitchChairID(i)].x * hscale, kPosSeat[(CNewGameClient::GetMySelf())->SwitchChairID(i)].y * vscale, kChairDefaultAngle[(CNewGameClient::GetMySelf())->SwitchChairID(i)], hscale, vscale);
		RenderCannonNum((CNewGameClient::GetMySelf())->SwitchChairID(i), current_mulriple_[i], kPosCannonMul[(CNewGameClient::GetMySelf())->SwitchChairID(i)].x * hscale,kPosCannonMul[(CNewGameClient::GetMySelf())->SwitchChairID(i)].y * vscale,kChairDefaultAngle[(CNewGameClient::GetMySelf())->SwitchChairID(i)], hscale, vscale);

		spr_score_box_->RenderEx(kPosScoreBox[(CNewGameClient::GetMySelf())->SwitchChairID(i)].x * hscale, kPosScoreBox[(CNewGameClient::GetMySelf())->SwitchChairID(i)].y * vscale, kChairDefaultAngle[(CNewGameClient::GetMySelf())->SwitchChairID(i)], hscale, vscale);
		RenderCreditNum((CNewGameClient::GetMySelf())->SwitchChairID(i), fish_score_[i], kPosScore[(CNewGameClient::GetMySelf())->SwitchChairID(i)].x * hscale,  kPosScore[(CNewGameClient::GetMySelf())->SwitchChairID(i)].y * vscale, kChairDefaultAngle[(CNewGameClient::GetMySelf())->SwitchChairID(i)], hscale, vscale);
		if (current_bullet_kind_[i] >= BULLET_KIND_1_ION) 
		{
			float rotate_x = (kPosCardIon[(CNewGameClient::GetMySelf())->SwitchChairID(i)].x + kRotateRadius * cosf(rotate_angle_)) * hscale;
			float rotate_y = (kPosCardIon[(CNewGameClient::GetMySelf())->SwitchChairID(i)].y + kRotateRadius * sinf(rotate_angle_)) * vscale;
			spr_card_ion_->RenderEx(rotate_x, rotate_y, kChairDefaultAngle[(CNewGameClient::GetMySelf())->SwitchChairID(i)], hscale, vscale);
		}
	}

	return false;
}

void CannonManager::SetCurrentAngle(WORD chair_id, float angle)
{
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return;

  int iView = (CNewGameClient::GetMySelf())->SwitchChairID(chair_id);
  float fAngle = 0.0f;
  if (iView <= 2)
  {
	  //fAngle = angle % (float)(2*M_PI);
	  if (angle > M_PI_2 && angle <= M_PI + M_PI_2)
	  {

	  }
	  else
	  {
		  angle -= M_PI;
	  }
  }
  else
  {
	  if (angle > M_PI_2 && angle <= M_PI + M_PI_2)
	  {
		  angle +=  M_PI;
	  }
  }

  current_angle_[chair_id] = angle;
}

void CannonManager::SetCannonMulriple(WORD chair_id, int mulriple) 
{
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return;

  current_mulriple_[chair_id] = mulriple;
}

float CannonManager::GetCurrentAngle(WORD chair_id) const {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return 0.f;

  return current_angle_[chair_id];
}

FPoint CannonManager::GetCannonPos(WORD chair_id,int iRealChairId, FPoint* muzzle_pos) 
{
	assert(chair_id < GAME_PLAYER);
	static float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
	static float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
	static float hscale = screen_width / kResolutionWidth;
	static float vscale = screen_height / kResolutionHeight;
	FPoint cannon_pos = { kPosBoard[chair_id].x * hscale, kPosBoard[chair_id].y * vscale };

	if (muzzle_pos != NULL) 
	{
		GetMuzzlePosExcusion(iRealChairId, muzzle_pos);
	}

	return cannon_pos;
}

void CannonManager::Switch(WORD chair_id, BulletKind bullet_kind) {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return;

  current_bullet_kind_[chair_id] = bullet_kind;
}

BulletKind CannonManager::GetCurrentBulletKind(WORD chair_id) {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return BULLET_KIND_2_NORMAL;

  return current_bullet_kind_[chair_id];
}

void CannonManager::Fire(WORD chair_id, BulletKind bullet_kind) {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return;

  current_bullet_kind_[chair_id] = bullet_kind;
}

void CannonManager::RenderCannonNum(WORD chair_id, int num, float x, float y, float rot, float hscale, float vscale) {
  if (num < 0) num = -num;

  float num_width = spr_cannon_num_->GetWidth();
  float num_height = spr_cannon_num_->GetHeight();

  int num_count = 0;
  int num_temp = num;
  do {
    num_count++;
    num_temp /= 10;
  } while (num_temp > 0);

  if (chair_id == 0 || chair_id == 1 || chair_id == 2) x -= (num_count - 1) * num_width * hscale / 2;
  else if (chair_id == 6) y -= (num_count - 1) * num_width * hscale / 2;
  else if (chair_id == 7) y += (num_count - 1) * num_width * hscale / 2;
  else x += (num_count - 1) * num_width * hscale / 2;

  for (int i = 0; i < num_count; ++i) {
    spr_cannon_num_->SetFrame(num % 10);
    spr_cannon_num_->RenderEx(x, y, rot, hscale, vscale);
    if (chair_id <= 2) x += num_width * hscale;
    else if (chair_id == 6) y += num_width * hscale;
    else if (chair_id == 7) y -= num_width * hscale;
    else x -= num_width * hscale;
    num /= 10;
  }
}

void CannonManager::GetMuzzlePosExcusion(WORD iRealChairId, FPoint* muzzle_pos) 
{
	if (muzzle_pos == NULL) return;
	//muzzle_pos->x = 1000;
	//muzzle_pos->y = 1000;
	static float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
	static float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
	static float hscale = screen_width / kResolutionWidth;
	static float vscale = screen_height / kResolutionHeight;

	float hotx, muzzle_excursion;
	spr_cannon_[current_bullet_kind_[iRealChairId]]->GetHotSpot(&hotx, &muzzle_excursion);
	muzzle_excursion *= vscale;
	char file_name[MAX_PATH] = { 0 };
	if (current_bullet_kind_[iRealChairId] <= BULLET_KIND_4_NORMAL) {
		_snprintf(file_name, arraysize(file_name), "bullet1_norm%d", current_bullet_kind_[iRealChairId] + 1);
	} else {
		_snprintf(file_name, arraysize(file_name), "bullet%d_ion", current_bullet_kind_[iRealChairId] - BULLET_KIND_1_ION + 1);
	}
	hgeAnimation* ani = GameManager::GetInstance().GetResourceManager()->GetAnimation(file_name);
	if (ani != NULL) {
		float hoty;
		ani->GetHotSpot(&hotx, &hoty);
		muzzle_excursion += hoty * vscale;
	}


	float angle = current_angle_[iRealChairId];
	int chair_id = (CNewGameClient::GetMySelf())->SwitchChairID(iRealChairId);
	if (chair_id == 0 || chair_id == 1 || chair_id == 2) 
	{
		if (angle < M_PI) 
		{
			muzzle_pos->x = kPosGun[(chair_id)].x * hscale + sinf(M_PI - angle) * muzzle_excursion;
			muzzle_pos->y = kPosGun[chair_id].y * vscale + cosf(M_PI - angle) * muzzle_excursion;
		} 
		else 
		{
			muzzle_pos->x = kPosGun[chair_id].x * hscale - sinf(angle - M_PI) * muzzle_excursion;
			muzzle_pos->y = kPosGun[chair_id].y * vscale + cosf(angle - M_PI) * muzzle_excursion;
		}
	}
	else if (chair_id == 6) 
	{
		if (angle > (M_PI + M_PI_2)) 
		{
			muzzle_pos->x = kPosGun[chair_id].x * hscale - cosf(angle - M_PI - M_PI_2) * muzzle_excursion;
			muzzle_pos->y = kPosGun[chair_id].y * vscale - sinf(angle - M_PI - M_PI_2) * muzzle_excursion;
		} 
		else 
		{
			muzzle_pos->x = kPosGun[chair_id].x * hscale - cosf(M_PI + M_PI_2 - angle) * muzzle_excursion;
			muzzle_pos->y = kPosGun[chair_id].y * vscale + sinf(M_PI + M_PI_2 - angle) * muzzle_excursion;
		}
	} 
	else if (chair_id == 5 || chair_id == 3 || chair_id == 4) 
	{
		if (angle == M_PI / 2) 
		{
			muzzle_pos->x = kPosGun[chair_id].x * hscale + muzzle_excursion;
			muzzle_pos->y = kPosGun[chair_id].y * vscale;
		} 
		else if (angle > M_PI * 2) 
		{
			muzzle_pos->x = kPosGun[chair_id].x * hscale + sinf(angle - M_PI * 2) * muzzle_excursion;
			muzzle_pos->y = kPosGun[chair_id].y * vscale - cosf(angle - M_PI * 2) * muzzle_excursion;
		} 
		else 
		{
			muzzle_pos->x = kPosGun[chair_id].x * hscale - sinf(2 * M_PI - angle) * muzzle_excursion;
			muzzle_pos->y = kPosGun[chair_id].y * vscale - cosf(2 * M_PI - angle) * muzzle_excursion;
		}
	} 
	else 
	{
		if (angle == M_PI * 2) 
		{
			muzzle_pos->x = kPosGun[chair_id].x * hscale;
			muzzle_pos->y = kPosGun[chair_id].y * vscale - muzzle_excursion;
		} 
		else if (angle > M_PI * 2) 
		{
			muzzle_pos->x = kPosGun[chair_id].x * hscale + cosf(M_PI * 2 + M_PI_2 - angle) * muzzle_excursion;
			muzzle_pos->y = kPosGun[chair_id].y * vscale - sinf(M_PI * 2 + M_PI_2 - angle) * muzzle_excursion;
		} 
		else 
		{
			muzzle_pos->x = kPosGun[chair_id].x * hscale + cosf(angle - M_PI_2) * muzzle_excursion;
			muzzle_pos->y = kPosGun[chair_id].y * vscale + sinf(angle - M_PI_2) * muzzle_excursion;
		}
	}
}

void CannonManager::RenderCreditNum(WORD chair_id, SCORE num, float x, float y, float rot, float hscale, float vscale) {
  if (num < 0) return;

  static const int kMaxShowNumCount = 7;

  float num_width = spr_credit_num_->GetWidth();
  float num_height = spr_credit_num_->GetHeight();

  int num_count = 0;
  SCORE num_temp = num;
  do {
    num_count++;
    num_temp /= 10;
  } while (num_temp > 0);
  if (num_count > kMaxShowNumCount) {
    hscale *= (float)(kMaxShowNumCount) / (float)(num_count);
  }

  for (int i = 0; i < num_count; ++i) {
    spr_credit_num_->SetFrame(static_cast<int>(num % 10));
    spr_credit_num_->RenderEx(x, y, rot, hscale, vscale);
    if (chair_id == 0 || chair_id == 1 || chair_id == 2) x += num_width * hscale;
    else if (chair_id == 6) y += num_width * hscale;
    else if (chair_id == 7) y -= num_width * hscale;
    else x -= num_width * hscale;
    num /= 10;
  }
}
