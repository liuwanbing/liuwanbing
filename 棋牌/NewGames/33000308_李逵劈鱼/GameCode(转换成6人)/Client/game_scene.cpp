
#include "stdafx.h"
#include "game_scene.h"
#include "water.h"
#include "game_manager.h"
#include "sound_manager.h"

GameScene::GameScene() : scene_style_(SCENE_STYLE_1), scene_switching_(SCENE_STYLE_COUNT), since_last_frame_(-1.0f),
                         scene_current_pos(0.0f) {
  hge_ = hgeCreate(HGE_VERSION);
  water_ = new Water;
}

GameScene::~GameScene() {
  SAFE_DELETE(water_);

  hge_->Release();
}

void GameScene::SetSwitchSceneStyle(/*SceneStyle scene_style*/) {
  scene_switching_ = static_cast<SceneStyle>((scene_style_ + 1) % SCENE_STYLE_COUNT);
  ani_wave_->SetFrame(0);
  ani_wave_->Play();
  SoundManager::GetInstance().StopBackMusic();
  SoundManager::GetInstance().PlayGameEffect(WAVE);
}

bool GameScene::IsSwitchingScene() {
  return scene_switching_ != SCENE_STYLE_COUNT;
}

bool GameScene::OnFrame(float delta_time) 
{
	if (scene_switching_ != SCENE_STYLE_COUNT) 
	{
		OutputDebugString("dwjlkpy1::SCENE_STYLE_COUNT--0");
		if (since_last_frame_ == -1.0f) since_last_frame_ = 0.0f;
		else since_last_frame_ += delta_time;

		static const float kSpeed = 1.0f / kSwitchSceneFPS;
		float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
		OutputDebugString("dwjlkpy1::SCENE_STYLE_COUNT--1");
		while (since_last_frame_ >= kSpeed) 
		{
			since_last_frame_ -= kSpeed;
			scene_current_pos += screen_width / (kSwitchSceneFPS * 6);
			if (scene_current_pos >= screen_width) 
			{
				scene_style_ = scene_switching_;
				scene_switching_ = SCENE_STYLE_COUNT;
				ani_wave_->Stop();
				scene_current_pos = 0.0f;
				since_last_frame_ = -1.0f;
				OutputDebugString("dwjlkpy1::SCENE_STYLE_COUNT--2");
				return true;
			}
		}
	}

	water_->OnFrame(delta_time);

	if (scene_switching_ != SCENE_STYLE_COUNT) 
	{
		ani_wave_->Update(delta_time);
	}

	return false;
}

bool GameScene::OnRender(float offset_x, float offset_y, float hscale, float vscale, bool switch_scene) 
{
	float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
	float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));

	if (switch_scene) 
	{
		if (scene_switching_ != SCENE_STYLE_COUNT) 
		{
			spr_bg_[scene_switching_]->RenderEx(offset_x + screen_width - scene_current_pos, offset_y + 0.f, 0.f, hscale, vscale);
			water_->OnRender(offset_x, offset_y, hscale, vscale);
		}

		if (scene_switching_ != SCENE_STYLE_COUNT) 
		{
			ani_wave_->RenderEx(offset_x + screen_width - scene_current_pos - 80.0f * hscale, offset_y + 0.0f, 0.0f, hscale, vscale);
		}
	} 
	else 
	{
		spr_bg_[scene_style_]->RenderEx(offset_x + 0.0f, offset_y + 0.f, 0.f, hscale, vscale);

		water_->OnRender(offset_x, offset_y, hscale, vscale);
	}

  return false;
}

bool GameScene::LoadGameResource() 
{
	char file_name[MAX_PATH] = { 0 };
	hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
	if (NULL == resource_manager)
	{
		return false;
	}

	for (int i = 0; i < SCENE_STYLE_COUNT; ++i) 
	{
		_snprintf(file_name, arraysize(file_name), "scene_bg%d", i + 1);
		hgeSprite* spr_bg = resource_manager->GetSprite(file_name);//创建场景精灵
		if (NULL == spr_bg)
		{
			return false;
		}
		spr_bg_[i] = spr_bg;
	}

	if (!water_->LoadGameResource())//加载水资源
	{
		return false;
	}

	ani_wave_ = resource_manager->GetAnimation("scene_wave");

	return true;
}