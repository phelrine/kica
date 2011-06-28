#ifndef DEPTHDRAWER_H_INCLUDED
#define DEPTHDRAWER_H_INCLUDED

#include <XnOpenNI.h>
#include <XnCppWrapper.h>

#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "TextureDrawer.h"

class DepthDrawer : public TextureDrawer
{
  xn::DepthGenerator depth;
  xn::UserGenerator user;
  bool hasUserNode;
  GLubyte *texData;
  float *depthHist;
  static float userColor[6][3];

public:
  DepthDrawer(xn::DepthGenerator depth);
  DepthDrawer(xn::DepthGenerator depth, xn::UserGenerator user);
  virtual ~DepthDrawer();  

  virtual void draw();
  const GLubyte *getTextureData() const {return texData;}

private:
  void initTexture();
  void update();
};

#endif /* DEPTHDRAWER_H_INCLUDED */
