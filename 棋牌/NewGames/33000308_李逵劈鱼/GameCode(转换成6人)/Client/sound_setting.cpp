
#include "StdAfx.h"
#include "sound_setting.h"
#include "gui_manager.h"

SoundSetting::SoundSetting()
  : hge_(hgeCreate(HGE_VERSION)),
    sound_setting_(NULL),
    back_music_(NULL),
    game_sound_(NULL),
    back_music_slider_(NULL),
    game_sound_slider_(NULL),
    btn_ok_(NULL),
    btn_cancel_(NULL),
    stream_volume_(100),
    effect_volume_(100) {
}

SoundSetting::~SoundSetting() 
{
  SAFE_DELETE(btn_ok_);
  SAFE_DELETE(btn_cancel_);
  SAFE_DELETE(back_music_slider_);
  SAFE_DELETE(game_sound_slider_);
  SAFE_DELETE(game_sound_);
  SAFE_DELETE(back_music_);
  SAFE_DELETE(sound_setting_);
  hge_->Release();
}

bool SoundSetting::Initialize() 
{
  stream_volume_ = hge_->System_GetState(HGE_STREAMVOLUME);
  effect_volume_ = hge_->System_GetState(HGE_FXVOLUME);

  gcn::Container* top = static_cast<gcn::Container*>(GuiManager::GetInstance().GetGui()->getTop());

  sound_setting_ = new fish::Window();
  sound_setting_->SetBackgroundImage("lkpy\\images\\gui\\setDlg.fish");
  sound_setting_->resizeToContent();
  sound_setting_->setPosition((hge_->System_GetState(HGE_SCREENWIDTH) - sound_setting_->getWidth()) / 2,
    (hge_->System_GetState(HGE_SCREENHEIGHT) - sound_setting_->getHeight()) / 2);

  back_music_ = new fish::CheckBox();
  back_music_->SetSelectedImage("lkpy\\images\\gui\\setDlgCheck1.fish");
  back_music_->setPosition(38, 106);
  back_music_->setSelected(true);
  sound_setting_->add(back_music_);

  game_sound_ = new fish::CheckBox();
  game_sound_->SetSelectedImage("lkpy\\images\\gui\\setDlgCheck1.fish");
  game_sound_->setPosition(38, 172);
  game_sound_->setSelected(true);
  sound_setting_->add(game_sound_);

  back_music_slider_ = new fish::Slider();
  back_music_slider_->SetSliderImage("lkpy\\images\\gui\\setDlgSlider.fish", "lkpy\\images\\gui\\SetDlgPitch.fish", "lkpy\\images\\gui\\setDlgThumb.fish");
  back_music_slider_->setPosition(38, 140);
  back_music_slider_->setValue(1.0);
  sound_setting_->add(back_music_slider_);

  game_sound_slider_ = new fish::Slider();
  game_sound_slider_->SetSliderImage("lkpy\\images\\gui\\setDlgSlider.fish", "lkpy\\images\\gui\\SetDlgPitch.fish", "lkpy\\images\\gui\\setDlgThumb.fish");
  game_sound_slider_->setPosition(38, 206);
  game_sound_slider_->setValue(1.0);
  sound_setting_->add(game_sound_slider_);

  btn_ok_ = new fish::ImageButton();
  btn_ok_->setImage("lkpy\\images\\gui\\BT_OK.fish");
  btn_ok_->setPosition(30, 280);
  btn_ok_->addActionListener(this);
  sound_setting_->add(btn_ok_);

  btn_cancel_ = new fish::ImageButton();
  btn_cancel_->setImage("lkpy\\images\\gui\\BT_CANCLE.fish");
  btn_cancel_->setPosition(205, 280);
  btn_cancel_->addActionListener(this);
  sound_setting_->add(btn_cancel_);

  sound_setting_->setVisible(false);
  top->add(sound_setting_);
  return true;
}

void SoundSetting::ShowSoundSetting(bool show) 
{
  sound_setting_->setVisible(show);
  hge_->System_SetState(HGE_HIDEMOUSE, !show);
}

void SoundSetting::action(const gcn::ActionEvent& actionEvent) 
{
  if (actionEvent.getSource() == btn_ok_) 
  {
    stream_volume_ = int(100 * back_music_slider_->getValue());
    effect_volume_ = int(100 * game_sound_slider_->getValue());

    if (back_music_->isSelected()) {
      hge_->System_SetState(HGE_STREAMVOLUME, stream_volume_);
      hge_->System_SetState(HGE_MUSVOLUME, stream_volume_);
    } else {
      hge_->System_SetState(HGE_STREAMVOLUME, 0);
      hge_->System_SetState(HGE_MUSVOLUME, 0);
    }

    if (game_sound_->isSelected()) {
      hge_->System_SetState(HGE_FXVOLUME, effect_volume_);
    } else {
      hge_->System_SetState(HGE_FXVOLUME, 0);
    }

    hge_->System_SetState(HGE_HIDEMOUSE, true);

    sound_setting_->setVisible(false);
  } 
  else if (actionEvent.getSource() == btn_cancel_) 
  {
    hge_->System_SetState(HGE_HIDEMOUSE, true);
    sound_setting_->setVisible(false);
  }
}
