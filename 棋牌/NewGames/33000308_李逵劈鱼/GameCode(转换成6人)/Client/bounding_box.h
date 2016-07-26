
#ifndef BOUNDING_BOX_H_
#define BOUNDING_BOX_H_
#pragma once

/*
(x1,y1)------(x2,y2)
   |            |
   |            |
(x4,y4)------(x3,y3)
*/

class BoundingBox {
 public:
  BoundingBox();
  BoundingBox(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);

  void CreateBoundingBox(float box_width, float box_height, float center_x, float center_y, float angle);
  bool ComputeCollision(float x, float y);

  float x1_, y1_, x2_,  y2_, x3_, y3_, x4_, y4_;
};

/*
      (x1,y1)

(x2,y2)     (x3,y3)
*/
class BoundingTriangle {
 public:
  BoundingTriangle();
  BoundingTriangle(float x1, float y1, float x2, float y2, float x3, float y3);

  void CreateBoundingTriangle(float side_length, float center_x, float center_y, float angle);
  void CreateBoundingTriangleWithRadius(float radius, float center_x, float center_y, float angle);
  bool ComputeCollision(float x, float y);

  float x1_, y1_, x2_,  y2_, x3_, y3_;
};

#endif // BOUNDING_BOX_H_