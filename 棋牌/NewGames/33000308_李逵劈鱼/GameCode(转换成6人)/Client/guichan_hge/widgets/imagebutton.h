
#ifndef FISH_IMAGE_BUTTON_H_
#define FISH_IMAGE_BUTTON_H_
#pragma once

#include <string>

#include "guichan/widgets/button.hpp"
#include "guichan/graphics.hpp"
#include "guichan/image.hpp"

namespace fish {

class ImageButton : public gcn::Button {
public:
  ImageButton();
  virtual ~ImageButton();

  void setImage(const std::string& filename);

  virtual void draw(gcn::Graphics* graphics);

protected:
  gcn::Image* button_image_;
};

}  // namespace fish

#endif  // FISH_CHECK_BOX_H_