#ifndef SKELETONDRAWER_H_INCLUDED
#define SKELETONDRAWER_H_INCLUDED

#include <XnOpenNI.h>
#include <XnCppWrapper.h>

#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Drawer.h"

class SkeletonDrawer : public Drawer
{
  xn::UserGenerator userGenerator;
  xn::DepthGenerator depthGenerator;
  
  float x, y;
  float width, height;

public:
  SkeletonDrawer(xn::UserGenerator &userGenerator, xn::DepthGenerator &depthGenerator);
  virtual ~SkeletonDrawer() {};
  virtual void draw();
  virtual void setDrawRegion(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY);

private:
  void drawLimb(XnUserID id, XnSkeletonJoint start, XnSkeletonJoint end);
};

#endif /* SKELETONDRAWER_H_INCLUDED */
