#ifndef DRAWER_H_INCLUDED
#define DRAWER_H_INCLUDED

class Drawer
{
public:
  virtual void setDrawRegion(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY) = 0;
  virtual void draw() = 0;
};

#endif /* DRAWER_H_INCLUDED */
