
#ifndef JETTON_MANAGER_H_
#define JETTON_MANAGER_H_
#pragma once

#include <deque>

#include "hge.h"
#include "hgesprite.h"
#include "hgeanim.h"

struct Jetton 
{
	bool move;
	FPoint postion;
	DWORD add_tick;
	int layer;
	int max_layer;
	int color_index;
	SCORE score;
};

class JettonManager 
{
public:
	JettonManager();
	~JettonManager();

	bool LoadGameResource();

	bool OnFrame(float delta_time);
	bool OnRender(float offset_x, float offset_y, float hscale, float vscale);

	void AddJetton(WORD chair_id, SCORE score);

private:
	FPoint	GetJettonPos(WORD chair_id, size_t size);
	void	AdjustLayerOffset(WORD chair_id, float offset, float* layer_offset_x, float* layer_offset_y);
	void	RenderJettonScore(WORD chair_id, int index, SCORE score, float x, float y, float rot, float hscale, float vscale);
	int		CalcLayerCount(WORD chair_id, SCORE score);

private:
	HGE* hge_;

	hgeSprite* spr_jetton_;
	hgeAnimation* spr_jetton_num_;
	hgeSprite* spr_bg_c1_;
	hgeSprite* spr_bg_c2_;

	std::deque<Jetton> jetton_queue_[GAME_PLAYER];
};

#endif // JETTON_MANAGER_H_