
#include "StdAfx.h"
#include "water.h"
#include "game_manager.h"

Water::Water() : current_water_frame_(0), since_last_frame_(-1.f) 
{
  hge_ = hgeCreate(HGE_VERSION);
}

Water::~Water() {
  hge_->Release();
}

bool Water::OnFrame(float delta_time) {
  if (since_last_frame_ == -1.0f) since_last_frame_ = 0.0f;
  else since_last_frame_ += delta_time;

  static const float kSpeed = 1.0f / kWaterFPS;
  while (since_last_frame_ >= kSpeed) {
    since_last_frame_ -= kSpeed;
    current_water_frame_ = (++current_water_frame_) % kWaterFrames;
    spr_water_->SetTexture(tex_water_[current_water_frame_]);
  }
  return false;
}

bool Water::OnRender(float offset_x, float offset_y, float hscale, float vscale) {
  spr_water_->RenderStretch(offset_x + 0.f, offset_y + 0.f, static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH)), static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT)));

  return false;
}

bool Water::LoadGameResource() 
{
	char file_name[MAX_PATH] = { 0 };

	hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
	if (NULL == resource_manager)
	{
		return false;
	}

	for (int i = 0; i < kWaterFrames; ++i) 
	{
		_snprintf(file_name, arraysize(file_name), "scene_water%d", i + 1);
		HTEXTURE tex_water = resource_manager->GetTexture(file_name, 1);//加载水资源
		if (!tex_water)
		{
			return false;
		}
		tex_water_[i] = tex_water;
	}
	hgeSprite* spr_water = resource_manager->GetSprite("scene_water");//创建水精灵
	if (NULL == spr_water)
	{
		return false;
	}

	spr_water_ = spr_water;

	return true;
}
