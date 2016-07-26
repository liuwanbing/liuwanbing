
#include "StdAfx.h"
#include "checkbox.h"

namespace fish {

CheckBox::CheckBox() : selected_image_(NULL) {
}

CheckBox::~CheckBox() {
  if (selected_image_ != NULL) {
    delete selected_image_;
    selected_image_ = NULL;
  }
}

void CheckBox::SetSelectedImage(const std::string& file_name) {
  selected_image_ = gcn::Image::load(file_name);
  adjustSize();
}

void CheckBox::adjustSize() {
  if (selected_image_ != NULL) {
    setWidth(selected_image_->getWidth() / 2);
    setHeight(selected_image_->getHeight());
  }
}

void CheckBox::draw(gcn::Graphics* graphics) {
  if (selected_image_ != NULL) {
    if (mSelected) {
      graphics->drawImage(selected_image_, selected_image_->getWidth() / 2, 0, 0, 0, selected_image_->getWidth() / 2, selected_image_->getHeight());
    } else {
      graphics->drawImage(selected_image_, 0, 0, 0, 0, selected_image_->getWidth() / 2, selected_image_->getHeight());
    }
  }
}

} // namespace fish