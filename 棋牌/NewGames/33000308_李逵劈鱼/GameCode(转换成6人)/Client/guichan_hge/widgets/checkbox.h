
#ifndef FISH_CHECK_BOX_H_
#define FISH_CHECK_BOX_H_
#pragma once

#include <string>

#include "guichan/widgets/checkbox.hpp"
#include "guichan/graphics.hpp"
#include "guichan/image.hpp"

namespace fish {

class CheckBox : public gcn::CheckBox {
public:
  CheckBox();
  virtual ~CheckBox();

  virtual void SetSelectedImage(const std::string& file_name);

  // Òþ²Ø¸¸ÀàadjustSize
  void adjustSize();

  virtual void draw(gcn::Graphics* graphics);

protected:
  gcn::Image* selected_image_;
};

}  // namespace fish

#endif  // FISH_CHECK_BOX_H_