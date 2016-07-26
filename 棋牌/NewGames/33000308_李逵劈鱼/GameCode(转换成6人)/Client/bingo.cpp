
#include "stdafx.h"
#include "bingo.h"
#include "game_manager.h"
#include "hgeresource.h"
#include "cannon_manager.h"
#include "sound_manager.h"
#include "NewGameClient.h"

const float kFadeTime = 5.f;
const int kFadeFactor = 10;
const float kRotateRadian = 0.1f;

Bingo::Bingo() {
	for (WORD i = 0; i < GAME_PLAYER; ++i) {
		delta_time_[i] = 0.f;
		fish_score_[i] = 0;
		spr_bingo_num_[i] = NULL;
		ani_bingo_[i] = NULL;
		rotate_[i] = 0.f;
		rotate_factor[i] = 1;
	}
}

Bingo::~Bingo() {
	for (WORD i = 0; i < GAME_PLAYER; ++i) {
		SAFE_DELETE(spr_bingo_num_[i]);
		SAFE_DELETE(ani_bingo_[i]);
	}
}

bool Bingo::LoadGameResource() {
	hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
	hgeAnimation* spr_bingo_num = resource_manager->GetAnimation("bingo_num");
	hgeAnimation* ani_bingo = resource_manager->GetAnimation("bingo");
	for (WORD i = 0; i < GAME_PLAYER; ++i) {
		spr_bingo_num_[i] = new hgeAnimation(*spr_bingo_num);
		ani_bingo_[i] = new hgeAnimation(*ani_bingo);
		ani_bingo_[i]->Play();
	}

	return true;
}

void Bingo::SetBingoInfo(WORD chair_id, SCORE fish_score) {
	assert(chair_id < GAME_PLAYER);
	if (chair_id >= GAME_PLAYER) return;
	//assert(fish_score > 0);
	if (fish_score <= 0) return;

	fish_score_[chair_id] = fish_score;
	spr_bingo_num_[chair_id]->SetColor(0xFFFFFFFF);
	ani_bingo_[chair_id]->SetColor(0xFFFFFFFF);
	delta_time_[chair_id] = 0.f;
	rotate_[chair_id] = 0.f;
	rotate_factor[chair_id] = 1;
	SoundManager::GetInstance().PlayGameEffect(BINGO);
}

bool Bingo::OnFrame(float delta_time) 
{
	for (WORD i = 0; i < GAME_PLAYER; ++i) 
	{
		if (fish_score_[i] == 0) continue;
		ani_bingo_[i]->Update(delta_time);
		delta_time_[i] += delta_time;
		rotate_[(CNewGameClient::GetMySelf())->SwitchChairID(i)] += kRotateRadian * rotate_factor[i];
		if (rotate_[(CNewGameClient::GetMySelf())->SwitchChairID(i)] >= M_PI_4) 
		{
			rotate_factor[i] = -1;
		} 
		else if (rotate_[(CNewGameClient::GetMySelf())->SwitchChairID(i)] <= -M_PI_4) 
		{
			rotate_factor[i] = 1;
		}
		if (delta_time_[i] >= kFadeTime) {
			DWORD color = ani_bingo_[i]->GetColor();
			int alpha = GETA(color);
			alpha -= kFadeFactor;
			if (alpha <= 5) {
				color = 0x00FFFFFF;
				ani_bingo_[i]->SetColor(color);
				spr_bingo_num_[i]->SetColor(color);
				fish_score_[i] = 0;
			} else {
				color = SETA(color, alpha);
				ani_bingo_[i]->SetColor(color);
				spr_bingo_num_[i]->SetColor(color);
			}
		}
	}

	return false;
}

bool Bingo::OnRender(float offset_x, float offset_y, float hscale, float vscale) 
{
	float scale = vscale * 0.5030f;
	float scaleh = hscale * 0.5874f;
	for (WORD i = 0; i < GAME_PLAYER; ++i) 
	{
		if (fish_score_[i] == 0) continue;
		FPoint pos = GetBingoPos((CNewGameClient::GetMySelf())->SwitchChairID(i), hscale, vscale);
		ani_bingo_[i]->RenderEx(pos.x, pos.y, kChairDefaultAngle[(CNewGameClient::GetMySelf())->SwitchChairID(i)], scaleh, scaleh);
		RenderNum((CNewGameClient::GetMySelf())->SwitchChairID(i), (int)fish_score_[i], pos.x, pos.y, kChairDefaultAngle[(CNewGameClient::GetMySelf())->SwitchChairID(i)], scaleh, scale);
	}

	return false;
}

FPoint Bingo::GetBingoPos(WORD chair_id, float hscale, float vscale) 
{
	static const float kExcursion = 20.f;
	FPoint ret_pos;
	FPoint cannon_pos = GameManager::GetInstance().GetCannonManager()->GetCannonPos((chair_id));
	float hotspotx, hotspoty;
	ani_bingo_[chair_id]->GetHotSpot(&hotspotx, &hotspoty);
	if (chair_id == 0 || chair_id == 1|| chair_id == 2) {
		ret_pos.x = cannon_pos.x;
		ret_pos.y = cannon_pos.y + hotspoty * vscale + kExcursion * vscale;
	} else if (chair_id == 6) {
		ret_pos.x = cannon_pos.x - hotspoty * vscale - kExcursion * vscale;
		ret_pos.y = cannon_pos.y;
	} else if (chair_id == 7) {
		ret_pos.x = cannon_pos.x + hotspoty * vscale + kExcursion * vscale;
		ret_pos.y = cannon_pos.y;
	} else {
		ret_pos.x = cannon_pos.x;
		ret_pos.y = cannon_pos.y - hotspoty * vscale - kExcursion * vscale;
	}
	return ret_pos;
}

void Bingo::RenderNum(WORD chair_id, int num, float x, float y, float rot, float hscale, float vscale) {
	if (num == 0) return;
	if (num < 0) num = -num;
	static const int kMaxShowNumCount = 6;

	float num_width = spr_bingo_num_[chair_id]->GetWidth();
	float num_height = spr_bingo_num_[chair_id]->GetHeight();
	FPoint center;

	int num_count = 0;
	int num_temp = num;
	do {
		num_count++;
		num_temp /= 10;
	} while (num_temp > 0);
	if (num_count > kMaxShowNumCount) {
		hscale *= (float)(kMaxShowNumCount) / (float)(num_count);
	}

	static const float kExcursion = 10.f;

	if (chair_id == 0 || chair_id == 1|| chair_id == 2) {
		y -= kExcursion;
	} else if (chair_id == 6) {
		x += kExcursion;
	} else if (chair_id == 7) {
		x -= kExcursion;
	} else {
		y += kExcursion;
	}
	center.x = x;
	center.y = y;

	float cell_width = num_width * hscale;
	float radius = (num_count - 1) * cell_width / 2;
	float dx, dy;

	for (int i = 0; i < num_count; ++i) {
		spr_bingo_num_[chair_id]->SetFrame(num % 10);
		if (radius < 0.f) {
			if (chair_id == 0 || chair_id == 1 || chair_id == 2) {
				dx = center.x + radius * cosf(rotate_[chair_id] - M_PI);
				dy = center.y + radius * sinf(rotate_[chair_id] - M_PI);
			} else if (chair_id == 6) {
				dx = center.x + radius * cosf(rotate_[chair_id] + kChairDefaultAngle[chair_id]);
				dy = center.y + radius * sinf(rotate_[chair_id] + kChairDefaultAngle[chair_id]);
			} else if (chair_id == 7) {
				dx = center.x + radius * cosf(rotate_[chair_id] + kChairDefaultAngle[chair_id]);
				dy = center.y + radius * sinf(rotate_[chair_id] + kChairDefaultAngle[chair_id]);
			} else {
				dx = center.x + radius * cosf(rotate_[chair_id]);
				dy = center.y + radius * sinf(rotate_[chair_id]);
			}
		} else {
			if (chair_id == 0 || chair_id == 1 || chair_id == 2) {
				dx = center.x - radius * cosf(rotate_[chair_id]);
				dy = center.y - radius * sinf(rotate_[chair_id]);
			} else if (chair_id == 6) {
				dx = center.x + radius * cosf(rotate_[chair_id] + kChairDefaultAngle[chair_id]);
				dy = center.y + radius * sinf(rotate_[chair_id] + kChairDefaultAngle[chair_id]);
			} else if (chair_id == 7) {
				dx = center.x + radius * cosf(rotate_[chair_id] + kChairDefaultAngle[chair_id]);
				dy = center.y + radius * sinf(rotate_[chair_id] + kChairDefaultAngle[chair_id]);
			} else {
				dx = center.x - radius * cosf(rotate_[chair_id] - M_PI);
				dy = center.y - radius * sinf(rotate_[chair_id] - M_PI);
			}
		}
		spr_bingo_num_[chair_id]->RenderEx(dx, dy, rotate_[chair_id] + kChairDefaultAngle[chair_id], hscale, vscale);
		radius -= cell_width;
		num /= 10;
	}
}