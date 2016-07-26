
#ifndef FISH_SLIDER_H_
#define FISH_SLIDER_H_
#pragma once

#include <string>

#include "guichan/widgets/slider.hpp"
#include "guichan/graphics.hpp"
#include "guichan/image.hpp"

namespace fish {

class Slider : public gcn::Slider {
public:
  Slider();
  virtual ~Slider();

  void SetSliderImage(const std::string& slider_file_name, const std::string& pich_file_name, const std::string& thumb_file_name);

  virtual void draw(gcn::Graphics* graphics);

protected:
  gcn::Image* slider_image_;
  gcn::Image* ptch_image_;
  gcn::Image* thumb_image_;
};

}  // namespace fish

#endif  // FISH_SLIDER_H_