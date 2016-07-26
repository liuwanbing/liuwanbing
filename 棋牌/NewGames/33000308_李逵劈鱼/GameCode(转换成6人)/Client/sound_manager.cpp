
#include "stdafx.h"
#include "sound_manager.h"
#include "game_manager.h"
#include "hgeresource.h"
#include "NewGameClient.h"

SoundManager* SoundManager::sound_manager_ = NULL;

SoundManager::SoundManager() : hge_(hgeCreate(HGE_VERSION)), bg_music_(BGM_NORMAL_1), volume_(100) {}

SoundManager::~SoundManager() {
  hge_->Release();
}

SoundManager& SoundManager::GetInstance() {
  if (sound_manager_ == NULL) sound_manager_ = new SoundManager();
  return *sound_manager_;
}

void SoundManager::DelInstance() {
  SAFE_DELETE(sound_manager_);
}

bool SoundManager::LoadGameResource() 
{
	hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
	if (NULL == resource_manager)
	{
		return false;
	}

	//stream_bgm_[BGM_NORMAL_1] = resource_manager->GetStream("bgm_normal1");
	//if (stream_bgm_[BGM_NORMAL_1] == 0) 
	//	return false;

	//stream_bgm_[BGM_NORMAL_2] = resource_manager->GetStream("bgm_normal2");
	//if (stream_bgm_[BGM_NORMAL_2] == 0) 
	//	return false;

	//stream_bgm_[BGM_NORMAL_3] = resource_manager->GetStream("bgm_normal3");
	//if (stream_bgm_[BGM_NORMAL_3] == 0) 
	//	return false;

	//stream_bgm_[BGM_NORMAL_4] = resource_manager->GetStream("bgm_normal4");
	//if (stream_bgm_[BGM_NORMAL_4] == 0) 
	//	return false;

	game_effect_[CANNON_SWITCH] = resource_manager->GetEffect("effect_cannon_switch");
	if (game_effect_[CANNON_SWITCH] == 0) 
		return false;

	game_effect_[CASTING] = resource_manager->GetEffect("effect_casting");
	if (game_effect_[CASTING] == 0) 
		return false;

	game_effect_[CATCH] = resource_manager->GetEffect("effect_catch");
	if (game_effect_[CATCH] == 0) 
		return false;

	game_effect_[FIRE] = resource_manager->GetEffect("effect_fire");
	if (game_effect_[FIRE] == 0) 
		return false;

	game_effect_[SUPERARM] = resource_manager->GetEffect("effect_superarm");
	if (game_effect_[SUPERARM] == 0) 
		return false;

	game_effect_[BINGO] = resource_manager->GetEffect("effect_bingo");
	if (game_effect_[BINGO] == 0) 
		return false;

	game_effect_[WAVE] = resource_manager->GetEffect("effect_wave");
	if (game_effect_[WAVE] == 0) 
		return false;

	if (resource_manager->GetEffect("effect_fish10_1") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish10_2") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish11_1") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish11_2") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish12_1") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish12_2") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish13_1") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish13_2") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish14_1") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish14_2") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish15_1") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish15_2") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish16_1") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish16_2") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish17_1") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish17_2") == 0) 
		return false;
	if (resource_manager->GetEffect("effect_fish17_3") == 0) 
		return false;

	return true;
}

void SoundManager::PlayBackMusic() 
{
	//hge_->Stream_Play(stream_bgm_[bg_music_], true);
	(CNewGameClient::GetMySelf())->PlayBgSound(true,bg_music_);
	bg_music_ = static_cast<BgMusic>((bg_music_ + 1) % BGM_COUNT);
}

void SoundManager::StopBackMusic() {
  hge_->Channel_StopAll();
}

void SoundManager::PlayGameEffect(GameEffect game_effect) {
  hge_->Effect_Play(game_effect_[game_effect]);
}

void SoundManager::PlayFishEffect(FishKind fish_kind) {
  if (fish_kind >= FISH_KIND_10 && fish_kind <= FISH_KIND_17) {
    char file_name[MAX_PATH] = { 0 };
    if (fish_kind == FISH_KIND_17) {
      _snprintf(file_name, arraysize(file_name), "effect_fish%d_%d", fish_kind + 1, rand() % 3 + 1);
    } else {
      _snprintf(file_name, arraysize(file_name), "effect_fish%d_%d", fish_kind + 1, rand() % 2 + 1);
    }
    hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
    hge_->Effect_Play(resource_manager->GetEffect(file_name));
  }
}

void SoundManager::MuteSwitch() {
  if (volume_ == 0) {
    volume_ = 100;
    hge_->System_SetState(HGE_FXVOLUME, volume_);
    hge_->System_SetState(HGE_MUSVOLUME, volume_);
    hge_->System_SetState(HGE_STREAMVOLUME, volume_);
  } else {
    volume_ = 0;
    hge_->System_SetState(HGE_FXVOLUME, volume_);
    hge_->System_SetState(HGE_MUSVOLUME, volume_);
    hge_->System_SetState(HGE_STREAMVOLUME, volume_);
  }
}