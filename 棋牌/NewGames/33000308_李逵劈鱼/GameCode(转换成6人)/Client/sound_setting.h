
#ifndef SOUND_SETTING_H_
#define SOUND_SETTING_H_
#pragma once

#include "hge.h"

#include "guichan.hpp"
#include "guichan_hge/widgets/Window.h"
#include "guichan_hge/widgets/checkbox.h"
#include "guichan_hge/widgets/slider.h"
#include "guichan_hge/widgets/imagebutton.h"

class SoundSetting : public gcn::ActionListener
{
 public:
  SoundSetting();
  virtual ~SoundSetting();

  bool Initialize();

  void ShowSoundSetting(bool show);

 private:
  void action(const gcn::ActionEvent& actionEvent);

 private:
  HGE* hge_;
  fish::Window* sound_setting_;
  fish::CheckBox* back_music_;
  fish::CheckBox* game_sound_;
  fish::Slider* back_music_slider_;
  fish::Slider* game_sound_slider_;
  fish::ImageButton* btn_ok_;
  fish::ImageButton* btn_cancel_;

  int stream_volume_;
  int effect_volume_;
};

#endif  // SOUND_SETTING_H_
