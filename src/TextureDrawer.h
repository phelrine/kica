#ifndef TEXTUREDRAWER_H_INCLUDED
#define TEXTUREDRAWER_H_INCLUDED

#include <XnOpenNI.h>
#include <XnCppWrapper.h>

#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Drawer.h"

class TextureDrawer : public Drawer
{
  int resX, resY;
  int texX, texY;
  GLuint texID;
  GLubyte *texData;
  float texCoords[8];
  float drawCoords[8];

public:
  virtual ~TextureDrawer() {};
  void init(int resX, int resY);

  int getResolutionX() const {return resX;};  
  int getResolutionY() const {return resY;};
  int getTextureX() const {return texX;};
  int getTextureY() const {return texY;};
  
  virtual void draw();
  virtual void setDrawRegion(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY);
  virtual const GLubyte *getTextureData() const = 0;
  
private:
  int calcClosestPowerOfTwo(int n) const;
  void drawRect();
};

#endif /* TEXTUREDRAWER_H_INCLUDED */
