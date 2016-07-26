
#ifndef GUI_MANAGER_H_
#define GUI_MANAGER_H_
#pragma once

#include "guichan.hpp"
#include "guichan_hge/hgegraphics.hpp"
#include "guichan_hge/hgeimage.hpp"
#include "guichan_hge/hgeimagefont.hpp"
#include "guichan_hge/hgeimageloader.hpp"
#include "guichan_hge/hgeinput.hpp"

class GuiManager {
 public:
  ~GuiManager();

  static GuiManager& GetInstance();
  static GuiManager* GetpGuiManager();
  static void DelInstance();

  bool Initialize(int screen_width, int screen_height);

  gcn::Gui* GetGui() const { return gui_; }

 private:
  GuiManager();
  GuiManager(const GuiManager&);
  void operator=(const GuiManager&);

 private:
  static GuiManager* gui_manager_;

  gcn::Gui* gui_;
  gcn::HGEGraphics* graphics_;
  gcn::HGEInput* input_;
  gcn::HGEImageLoader* image_loader_;
  gcn::Container* container_;
  gcn::HGEImageFont* font_;
};

#endif  // GUI_MANAGER_H_