
#ifndef FISH_WINDOW_H_
#define FISH_WINDOW_H_
#pragma once

#include <string>

#include "guichan/widgets/window.hpp"

namespace fish {

class Window : public gcn::Window {
 public:
  Window();
  virtual ~Window();

  virtual void SetBackgroundImage(const std::string& file_name);

  virtual void resizeToContent();

  virtual gcn::Rectangle getChildrenArea();

  virtual void draw(gcn::Graphics* graphics);

  virtual void mousePressed(gcn::MouseEvent& mouseEvent);

  virtual void mouseDragged(gcn::MouseEvent& mouseEvent);

  virtual void mouseReleased(gcn::MouseEvent& mouseEvent);

 protected:
   gcn::Image* background_image_;
};

}  // namespace fish

#endif  // FISH_WINDOW_H_
