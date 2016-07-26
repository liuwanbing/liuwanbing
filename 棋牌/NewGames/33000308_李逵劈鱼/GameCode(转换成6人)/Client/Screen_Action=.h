
#ifndef SCREEN_ACTION_H_
#define SCREEN_ACTION_H_
#pragma once
//游戏界面按钮响应类
#include "hge.h"

#include "guichan.hpp"
#include "guichan_hge/widgets/Window.h"
#include "guichan_hge/widgets/checkbox.h"
#include "guichan_hge/widgets/slider.h"
#include "guichan_hge/widgets/imagebutton.h"

class GameSrceenAction : public gcn::ActionListener
{
 public:
  GameSrceenAction();
  virtual ~GameSrceenAction();

  bool Initialize();

  void ShowSoundSetting(bool show);

 private:
  void action(const gcn::ActionEvent& actionEvent);

 private:
  HGE* hge_;
  fish::Window* Screen_Action_Window;
  fish::ImageButton* Btn_Min_;
  fish::ImageButton* Btn_Close_;

};

#endif  // SOUND_SETTING_H_
