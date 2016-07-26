
#include "StdAfx.h"
#include "window.h"
#include "guichan/graphics.hpp"
#include "guichan/mouseinput.hpp"
#include "guichan/image.hpp"

namespace fish {

Window::Window() : background_image_(NULL) {
  setFrameSize(0);
}

Window::~Window() {
  if (background_image_ != NULL) {
    delete background_image_;
    background_image_ = NULL;
  }
}

void Window::SetBackgroundImage(const std::string& file_name) {
  background_image_ = gcn::Image::load(file_name);
}

void Window::resizeToContent() {
  WidgetListIterator it;

  int w = 0, h = 0;
  if (background_image_ != NULL) {
    w = background_image_->getWidth();
    h = background_image_->getHeight();
  } else {
    for (it = mWidgets.begin(); it != mWidgets.end(); it++) {
      if ((*it)->getX() + (*it)->getWidth() > w) {
        w = (*it)->getX() + (*it)->getWidth();
      }

      if ((*it)->getY() + (*it)->getHeight() > h) {
        h = (*it)->getY() + (*it)->getHeight();
      }
    }
  }

  setSize(w, h);
}

gcn::Rectangle Window::getChildrenArea() {
  return gcn::Rectangle(0, 0,
    getWidth(), getHeight());
}

void Window::draw(gcn::Graphics* graphics) {
  if (background_image_ != NULL) {
    graphics->drawImage(background_image_, 0, 0);
  }

  drawChildren(graphics);
}

void Window::mousePressed(gcn::MouseEvent& mouseEvent) {
  if (mouseEvent.getSource() != this) {
    return;
  }

  if (getParent() != NULL) {
    getParent()->moveToTop(this);
  }

  mDragOffsetX = mouseEvent.getX();
  mDragOffsetY = mouseEvent.getY();

  mMoved = true;
}

void Window::mouseDragged(gcn::MouseEvent& mouseEvent) {
  if (mouseEvent.isConsumed() || mouseEvent.getSource() != this) {
    return;
  }

  if (isMovable() && mMoved) {
    setPosition(mouseEvent.getX() - mDragOffsetX + getX(),
      mouseEvent.getY() - mDragOffsetY + getY());
  }

  mouseEvent.consume();
}

void Window::mouseReleased(gcn::MouseEvent& mouseEvent) {
  mMoved = false;
}

} // namespace fish