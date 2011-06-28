#ifndef IMAGEDRAWER_H_INCLUDED
#define IMAGEDRAWER_H_INCLUDED

#include <XnOpenNI.h>
#include <XnCppWrapper.h>

#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "TextureDrawer.h"

class ImageDrawer : public TextureDrawer
{
  xn::ImageGenerator generator;
  GLubyte *texData;

public:
  ImageDrawer(xn::ImageGenerator &generator_);
  virtual ~ImageDrawer();
  const GLubyte *getTextureData() const {return texData;};
  virtual void draw();

private:
  void update();  
};

#endif /* IMAGEDRAWER_H_INCLUDED */
