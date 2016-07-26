
#include "StdAfx.h"
#include "slider.h"

namespace fish {

Slider::Slider() : slider_image_(NULL), ptch_image_(NULL), thumb_image_(NULL) {
  setFrameSize(0);
}

Slider::~Slider() {
  if (slider_image_ != NULL) {
    delete slider_image_;
    slider_image_ = NULL;
  }

  if (ptch_image_ != NULL) {
    delete ptch_image_;
    ptch_image_ = NULL;
  }

  if (thumb_image_ != NULL) {
    delete thumb_image_;
    thumb_image_ = NULL;
  }
}

void Slider::SetSliderImage(const std::string& slider_file_name, const std::string& pich_file_name, const std::string& thumb_file_name) {
  slider_image_ = gcn::Image::load(slider_file_name);
  ptch_image_ = gcn::Image::load(pich_file_name);
  thumb_image_ = gcn::Image::load(thumb_file_name);
  if (slider_image_) {
    setWidth(slider_image_->getWidth());
    setHeight(slider_image_->getHeight());
  }
  if (thumb_image_) {
    setHeight(thumb_image_->getHeight());
    setMarkerLength(thumb_image_->getWidth());
  }
}

void Slider::draw(gcn::Graphics* graphics) {
  // 只做了水平方向的处理
  int v = getMarkerPosition();

  if (slider_image_ != NULL) {
    graphics->drawImage(slider_image_, 0, (getHeight() - slider_image_->getHeight()) / 2);
  }

  if (ptch_image_ != NULL) {
    graphics->drawImage(ptch_image_, 0, 0, 0, (getHeight() - ptch_image_->getHeight()) / 2, v, ptch_image_->getHeight());
  }

  if (thumb_image_) {
    graphics->drawImage(thumb_image_, v, 0);
  }
}

} // namespace fish