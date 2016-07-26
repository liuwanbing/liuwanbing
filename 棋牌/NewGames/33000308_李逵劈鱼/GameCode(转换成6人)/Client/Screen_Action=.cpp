
#include "StdAfx.h"
#include "Screen_Action.h"
#include "gui_manager.h"

GameSrceenAction::GameSrceenAction()
  : hge_(hgeCreate(HGE_VERSION)),
    Screen_Action_Window(NULL),
    Btn_Min_(NULL),
    Btn_Close_(NULL){
}

GameSrceenAction::~GameSrceenAction() {
  SAFE_DELETE(Btn_Min_);
  SAFE_DELETE(Btn_Close_);
  SAFE_DELETE(Screen_Action_Window);
  hge_->Release();
}

bool GameSrceenAction::Initialize() 
{

  gcn::Container* top = static_cast<gcn::Container*>(GuiManager::GetInstance().GetGui()->getTop());

  Screen_Action_Window = new fish::Window();
  Screen_Action_Window->SetBackgroundImage("lkpy\\images\\Frame\\game_title_bg.fish");
  Screen_Action_Window->resizeToContent();
  Screen_Action_Window->setPosition(0,0);



  Btn_Min_ = new fish::ImageButton();
  Btn_Min_->setImage("lkpy\\images\\Frame\\min_window.fish");
  Btn_Min_->setPosition(0, 0);
  Btn_Min_->addActionListener(this);
  Screen_Action_Window->add(Btn_Min_);

  Btn_Close_ = new fish::ImageButton();
  Btn_Close_->setImage("lkpy\\images\\Frame\\close_window.fish");
  Btn_Close_->setPosition(100, 0);
  Btn_Close_->addActionListener(this);
  Screen_Action_Window->add(Btn_Close_);

  Screen_Action_Window->setVisible(true);
  top->add(Screen_Action_Window);
  return true;
}

void GameSrceenAction::ShowSoundSetting(bool show) 
{
  Screen_Action_Window->setVisible(show);
  hge_->System_SetState(HGE_HIDEMOUSE, !show);
}

void GameSrceenAction::action(const gcn::ActionEvent& actionEvent) 
{
  if (actionEvent.getSource() == Btn_Min_) 
  {

	  OutputDebugString("dwjlkpy::最小化");

 
  } 
  else if (actionEvent.getSource() == Btn_Close_) 
  {
	   OutputDebugString("dwjlkpy::关闭");
  }
}
