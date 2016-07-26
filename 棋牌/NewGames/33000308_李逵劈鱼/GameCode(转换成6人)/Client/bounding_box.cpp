
#include "stdafx.h"
#include "bounding_box.h"
#include "math.h"

BoundingBox::BoundingBox() {
  x1_ = x2_ = x3_ = x4_ = y1_ = y2_ = y3_ = y4_ = 0.f;
}

BoundingBox::BoundingBox(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
  : x1_(x1), y1_(y1), x2_(x2), y2_(y2), x3_(x3), y3_(y3), x4_(x4), y4_(y4) {}

void BoundingBox::CreateBoundingBox(float box_width, float box_height, float center_x, float center_y, float angle) {
  box_width = box_width / 2;
  box_height = box_height / 2;
  float sint = sinf(angle);
  float cost = cosf(angle);
  float hot_x1 = -box_width;
  float hot_y1 = -box_height;
  float hot_x2 = box_width;
  float hot_y2 = box_height;
  x1_ = hot_x1 * cost - hot_y1 * sint + center_x;
  y1_ = hot_x1 * sint + hot_y1 * cost + center_y;
  x2_ = hot_x2 * cost - hot_y1 * sint + center_x;
  y2_ = hot_x2 * sint + hot_y1 * cost + center_y;
  x3_ = hot_x2 * cost - hot_y2 * sint + center_x;
  y3_ = hot_x2 * sint + hot_y2 * cost + center_y;
  x4_ = hot_x1 * cost - hot_y2 * sint + center_x;
  y4_ = hot_x1 * sint + hot_y2 * cost + center_y;
}

bool BoundingBox::ComputeCollision(float x, float y) {
  FPoint point[4] = { {x1_, y1_}, {x2_, y2_}, {x3_, y3_}, {x4_, y4_} };
  int i, j;
  bool inside = false;
  int count1 = 0, count2 = 0;

  for (i = 0, j = 4 - 1; i < 4; j = i++) {
    float value = (x - point[j].x) * (point[i].y - point[j].y) - (y - point[j].y) * (point[i].x - point[j].x);
    if (value > 0) {
      ++count1;
    } else if (value < 0) {
      ++count2;
    }
  }

  if (0 == count1 || 0 == count2) inside = true;

  return inside;
}

BoundingTriangle::BoundingTriangle() {
  x1_ = x2_ = x3_ = y1_ = y2_ = y3_ = 0.f;
}

BoundingTriangle::BoundingTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
  : x1_(x1), y1_(y1), x2_(x2), y2_(y2), x3_(x3), y3_(y3) {}

void BoundingTriangle::CreateBoundingTriangle(float side_length, float center_x, float center_y, float angle) {
  static const float kRotateRadian1 = 210 * M_PI / 180;
  static const float kRotateRadian2 = 30 * M_PI / 180;
  static const float kFactor = sqrtf(3.f) / 3;
  float radius = side_length * kFactor;
  x1_ = center_x + radius * cosf(angle - M_PI_2);
  y1_ = center_y + radius * sinf(angle - M_PI_2);
  x2_ = center_x + radius * cosf(angle - kRotateRadian1);
  y2_ = center_y + radius * sinf(angle - kRotateRadian1);
  x3_ = center_x + radius * cosf(angle + kRotateRadian2);
  y3_ = center_y + radius * sinf(angle + kRotateRadian2);
}

void BoundingTriangle::CreateBoundingTriangleWithRadius(float radius, float center_x, float center_y, float angle) {
  static const float kRotateRadian1 = 210 * M_PI / 180;
  static const float kRotateRadian2 = 30 * M_PI / 180;
  x1_ = center_x + radius * cosf(angle - M_PI_2);
  y1_ = center_y + radius * sinf(angle - M_PI_2);
  x2_ = center_x + radius * cosf(angle - kRotateRadian1);
  y2_ = center_y + radius * sinf(angle - kRotateRadian1);
  x3_ = center_x + radius * cosf(angle + kRotateRadian2);
  y3_ = center_y + radius * sinf(angle + kRotateRadian2);
}

bool BoundingTriangle::ComputeCollision(float x, float y) {
  FPoint point[3] = { {x1_, y1_}, {x2_, y2_}, {x3_, y3_} };
  int i, j;
  bool inside = false;
  int count1 = 0, count2 = 0;

  for (i = 0, j = 3 - 1; i < 3; j = i++) {
    float value = (x - point[j].x) * (point[i].y - point[j].y) - (y - point[j].y) * (point[i].x - point[j].x);
    if (value > 0) {
      ++count1;
    } else if (value < 0) {
      ++count2;
    }
  }

  if (0 == count1 || 0 == count2) inside = true;

  return inside;
}