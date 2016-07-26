
#include "stdafx.h"
#include "gui_manager.h"

GuiManager* GuiManager::gui_manager_ = NULL;

GuiManager::GuiManager()
  : gui_(NULL),
    graphics_(NULL),
    input_(NULL),
    image_loader_(NULL),
    container_(NULL),
    font_(NULL)  {}

GuiManager::~GuiManager() {
  SAFE_DELETE(font_);
  SAFE_DELETE(image_loader_);
  SAFE_DELETE(input_);
  SAFE_DELETE(graphics_);
  SAFE_DELETE(gui_);
}

GuiManager& GuiManager::GetInstance() {
  if (gui_manager_ == NULL) gui_manager_ = new GuiManager();
  return *gui_manager_;
}

GuiManager* GuiManager::GetpGuiManager() {
	return gui_manager_;
}

void GuiManager::DelInstance() {
  SAFE_DELETE(gui_manager_);
}

bool GuiManager::Initialize(int screen_width, int screen_height) 
{
  image_loader_ = new gcn::HGEImageLoader();
  gcn::Image::setImageLoader(image_loader_);

  graphics_ = new gcn::HGEGraphics();
  input_ = new gcn::HGEInput();
  gui_ = new gcn::Gui();
  gui_->setGraphics(graphics_);
  gui_->setInput(input_);

  container_ = new gcn::Container();
  container_->setOpaque(false);
  container_->setFrameSize(0);
  container_->setDimension(gcn::Rectangle(0, 0, screen_width, screen_height));
  gui_->setTop(container_);

  char font_dir[MAX_PATH] = { 0 };
  GetWindowsDirectoryA(font_dir, arraysize(font_dir));
  strcat(font_dir, "\\fonts\\SIMSUN.TTC");
  font_ = new gcn::HGEImageFont(font_dir, 12);
  gcn::Widget::setGlobalFont(font_);

  return true;
}