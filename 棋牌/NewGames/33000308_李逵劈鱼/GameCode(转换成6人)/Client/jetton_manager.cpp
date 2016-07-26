
#include "stdafx.h"
#include "jetton_manager.h"
#include "game_manager.h"
#include "hgeresource.h"
#include "pos_define.h"
#include "NewGameClient.h"

const int kMaxShowJettonCount = 3;
const DWORD kJettonMoveInterval = 4000;
const float kJettonMoveOffset = 2.5f;
const int kMaxLayerCount = 62;
const SCORE kBaseScore = 10000;

JettonManager::JettonManager() 
{
	hge_ = hgeCreate(HGE_VERSION);
}

JettonManager::~JettonManager() 
{
	hge_->Release();
}

bool JettonManager::LoadGameResource() 
{
	hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
	spr_jetton_ = resource_manager->GetSprite("jetton");
	spr_jetton_num_ = resource_manager->GetAnimation("jetton_num");
	spr_bg_c1_ = resource_manager->GetSprite("jetton_bgc1");
	spr_bg_c2_ = resource_manager->GetSprite("jetton_bgc2");
	return true;
}

bool JettonManager::OnFrame(float delta_time) 
{
	//return false;
	float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
	float hscale = screen_width / kResolutionWidth;
	float move_offset = kJettonMoveOffset * hscale;
	DWORD now_tick = timeGetTime();
	std::deque<Jetton>::iterator iter;

	for (WORD i = 0; i < GAME_PLAYER; ++i) 
	{
		if(!(CNewGameClient::GetMySelf())->isPlayer(i))
			continue;

		if (jetton_queue_[i].size() == 0) 
			continue;
		FPoint offset = { 0.f, 0.f };
		Jetton& jetton = jetton_queue_[i].front();
		if (jetton.add_tick + kJettonMoveInterval < now_tick) 
		{
			jetton_queue_[i].pop_front();
			Jetton& jetton = jetton_queue_[i].front();
			jetton.add_tick = now_tick;
			for (iter = jetton_queue_[i].begin(); iter != jetton_queue_[i].end(); ++iter) 
			{
				Jetton& jetton = *iter;
				jetton.move = true;
			}
		}
		size_t size = 0;
		for (iter = jetton_queue_[i].begin(); iter != jetton_queue_[i].end(); ++iter) 
		{
			Jetton& jetton = *iter;
			if (jetton.layer < jetton.max_layer) 
			{
				jetton.layer += 1;
			} 
			else 
			{
				jetton.layer = jetton.max_layer;
			}
			FPoint pos = GetJettonPos(i, size++);
			if (jetton.move) 
			{
				switch (i) 
				{
				case 0:
				case 1:
				case 2:
					jetton.postion.x -= move_offset;
					if (jetton.postion.x <= pos.x) 
					{
						jetton.postion.x = pos.x;
						jetton.move = false;
					}
					break;
				case 6:
					jetton.postion.y -= move_offset;
					if (jetton.postion.y <= pos.y) 
					{
						jetton.postion.y = pos.y;
						jetton.move = false;
					}
					break;
				case 4:
				case 5:
				case 3:
					jetton.postion.x += move_offset;
					if (jetton.postion.x >= pos.x) 
					{
						jetton.postion.x = pos.x;
						jetton.move = false;
					}
					break;
				case 7:
					jetton.postion.y += move_offset;
					if (jetton.postion.y >= pos.y) 
					{
						jetton.postion.y = pos.y;
						jetton.move = false;
					}
					break;
				}
			}
		}
	}

	return false;
}

bool JettonManager::OnRender(float offset_x, float offset_y, float hscale, float vscale) 
{
	return false;
	try
	{
		float num_hotspot_x, num_hotspot_y;
		spr_jetton_num_->GetHotSpot(&num_hotspot_x, &num_hotspot_y);
		float layer_offset_x = 0.f, layer_offset_y = 0.f;
		static float jetton_width = spr_jetton_->GetWidth();
		static float jetton_height = spr_jetton_->GetHeight();
		static float cell_height = jetton_height / kMaxLayerCount;
		//std::deque<Jetton>::iterator iter;
		for (WORD i = 0; i < GAME_PLAYER; ++i) 
		{
			if(!(CNewGameClient::GetMySelf())->isPlayer(i))
				continue;
			int size = 0;
			for(int j=0; j<jetton_queue_[i].size(); j++)
			{
				Jetton& jetton = jetton_queue_[i].at(j);
				if (& jetton != NULL)
				{
					spr_jetton_->SetTextureRect(0.f, (kMaxLayerCount - jetton.layer - 1) * cell_height, jetton_width, jetton.layer * cell_height);
					spr_jetton_->SetHotSpot(jetton_width / 2, spr_jetton_->GetHeight());
					spr_jetton_->RenderEx(jetton.postion.x, jetton.postion.y, kChairDefaultAngle[i], hscale, vscale);
					if (jetton.layer >= jetton.max_layer) 
					{
						layer_offset_x = 0.f;
						layer_offset_y = 0.f;
						AdjustLayerOffset(i, (cell_height * jetton.layer + num_hotspot_y) * vscale, &layer_offset_x, &layer_offset_y);
						RenderJettonScore(i, jetton.color_index, jetton.score, jetton.postion.x + layer_offset_x, jetton.postion.y + layer_offset_y, kChairDefaultAngle[i], hscale, vscale);
					}
				}
			}
		}
		return false;
	}
	catch (...)
	{
		return false;
	}

}

void JettonManager::AddJetton(WORD chair_id, SCORE score) 
{
	assert(chair_id < GAME_PLAYER);
	if (chair_id >= GAME_PLAYER) return;
	if (score <= 0) return;

	Jetton jetton;
	memset(&jetton, 0, sizeof(jetton));
	jetton.postion = GetJettonPos(chair_id, jetton_queue_[chair_id].size());
	if (jetton_queue_[chair_id].size() == 0) 
	{
		jetton.add_tick = timeGetTime();
		jetton.color_index = 0;
	} 
	else 
	{
		jetton.color_index = (jetton_queue_[chair_id].back().color_index + 1) % 2;
	}
	jetton.layer = 0;
	jetton.max_layer = CalcLayerCount(chair_id, score);
	jetton.score = score;
	jetton_queue_[chair_id].push_back(jetton);
	if (jetton_queue_[chair_id].size() > kMaxShowJettonCount) 
	{
		jetton_queue_[chair_id].pop_front();
		Jetton& jetton = jetton_queue_[chair_id].front();
		jetton.add_tick = timeGetTime();
		std::deque<Jetton>::iterator iter;
		for (iter = jetton_queue_[chair_id].begin(); iter != jetton_queue_[chair_id].end(); ++iter) 
		{
			Jetton& jetton = *iter;
			jetton.move = true;
		}
	}
}

FPoint JettonManager::GetJettonPos(WORD chair_id, size_t size) 
{
	FPoint jetton_pos = { 0.f, 0.f };
	assert(chair_id < GAME_PLAYER);
	if (chair_id >= GAME_PLAYER) return jetton_pos;

	static float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
	static float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
	static float hscale = screen_width / kResolutionWidth;
	static float vscale = screen_height / kResolutionHeight;

	float jetton_hotspot_x, jetton_hotspot_y;
	spr_jetton_->GetHotSpot(&jetton_hotspot_x, &jetton_hotspot_y);

	if (chair_id <= 2) 
	{
		jetton_pos.x = (kPosStack[chair_id].x + jetton_hotspot_x * 2 * size) * hscale;
		jetton_pos.y = kPosStack[chair_id].y * vscale;
	} 
	else if (chair_id == 6) 
	{
		jetton_pos.x = kPosStack[chair_id].x * vscale;
		jetton_pos.y = (kPosStack[chair_id].y + jetton_hotspot_x * 2 * size) * hscale;
	} 
	else if (chair_id == 7) 
	{
		jetton_pos.x = kPosStack[chair_id].x * vscale;
		jetton_pos.y = (kPosStack[chair_id].y - jetton_hotspot_x * 2 * size) * hscale;
	}
	else 
	{
		jetton_pos.x = (kPosStack[chair_id].x - jetton_hotspot_x * 2 * size) * hscale;
		jetton_pos.y = kPosStack[chair_id].y * vscale;
	}

	return jetton_pos;
}

void JettonManager::AdjustLayerOffset(WORD chair_id, float offset, float* layer_offset_x, float* layer_offset_y) 
{
	assert(chair_id < GAME_PLAYER);
	if (chair_id >= GAME_PLAYER) return;

	switch (chair_id) 
	{
	case 0:
	case 1:
	case 2:
		*layer_offset_x = 0.f;
		*layer_offset_y = offset;
		break;
	case 6:
		*layer_offset_x = -offset;
		*layer_offset_y = 0.f;
		break;
	case 4:
	case 5:
	case 3:
		*layer_offset_x = 0.f;
		*layer_offset_y = -offset;
		break;
	case 7:
		*layer_offset_x = offset;
		*layer_offset_y = 0.f;
		break;
	}
}

void JettonManager::RenderJettonScore(WORD chair_id, int index, SCORE num, float x, float y, float rot, float hscale, float vscale) 
{
	if (num < 0) num = -num;

	float num_width = spr_jetton_num_->GetWidth();
	float num_height = spr_jetton_num_->GetHeight();

	int num_count = 0;
	SCORE num_temp = num;
	do {
		num_count++;
		num_temp /= 10;
	} while (num_temp > 0);

	float ex_width = num_count * num_width / spr_bg_c1_->GetWidth();
	if (index == 0) {
		spr_bg_c1_->RenderEx(x, y, kChairDefaultAngle[chair_id], ex_width, vscale);
	} else {
		spr_bg_c2_->RenderEx(x, y, kChairDefaultAngle[chair_id], ex_width, vscale);
	}

	if (chair_id == 0 || chair_id == 1 || chair_id == 2) x -= (num_count - 1) * num_width * hscale / 2;
	else if (chair_id == 6) y -= (num_count - 1) * num_width * hscale / 2;
	else if (chair_id == 7) y += (num_count - 1) * num_width * hscale / 2;
	else x += (num_count - 1) * num_width * hscale / 2;

	for (int i = 0; i < num_count; ++i) {
		spr_jetton_num_->SetFrame(static_cast<int>(num % 10));
		spr_jetton_num_->RenderEx(x, y, rot, hscale, vscale);
		if (chair_id == 0 || chair_id == 1 || chair_id == 2) x += num_width * hscale;
		else if (chair_id == 6) y += num_width * hscale;
		else if (chair_id == 7) y -= num_width * hscale;
		else x -= num_width * hscale;
		num /= 10;
	}
}

int JettonManager::CalcLayerCount(WORD chair_id, SCORE score) 
{
	if (jetton_queue_[chair_id].size() == 0)
	{
		return min(kMaxLayerCount, max(1, int(score / kBaseScore)));
	} 
	else 
	{
		std::deque<Jetton>::iterator iter;
		int max_layer = 1;
		SCORE max_score = 0;
		for (iter = jetton_queue_[chair_id].begin(); iter != jetton_queue_[chair_id].end(); ++iter) 
		{
			Jetton& jetton = *iter;
			if (jetton.score > max_score) 
			{
				max_layer = jetton.max_layer;
				max_score = jetton.score;
			}
		}

		int ret_layer = min(kMaxLayerCount, max(1, int(score * max_layer / max_score)));
		if (score > max_score)
		{
			max_layer = min(kMaxLayerCount, max(1, int(score / kBaseScore)));
			max_score = score;
			ret_layer = max_layer;
		}
		for (iter = jetton_queue_[chair_id].begin(); iter != jetton_queue_[chair_id].end(); ++iter) 
		{
			Jetton& jetton = *iter;
			jetton.layer = min(kMaxLayerCount, max(1, int(jetton.score * max_layer / max_score)));
		}

		return ret_layer;
	}
}