
#include "StdAfx.h"
#include "imagebutton.h"

namespace fish {

ImageButton::ImageButton() : button_image_(NULL) {
  setFrameSize(0);
}

ImageButton::~ImageButton() {
  if (button_image_ != NULL) {
    delete button_image_;
    button_image_ = NULL;
  }
}

void ImageButton::setImage(const std::string& file_name) {
  button_image_ = gcn::Image::load(file_name);
  setWidth(button_image_->getWidth() / 4);
  setHeight(button_image_->getHeight());
}

void ImageButton::draw(gcn::Graphics* graphics) {
  if (button_image_ != NULL) {
    int btnWidth = button_image_->getWidth() / 4;
    int btnHeight = button_image_->getHeight();
    int srcX = 0;
    if (!isEnabled()) {
      srcX = 3;
    } else if (isPressed()) {
      srcX = 2;
    } else if (mHasMouse) {
      srcX = 1;
    }
    graphics->drawImage(button_image_, srcX * btnWidth, 0, 0, 0, btnWidth, btnHeight);
  }
}

} // namespace fish