#include "ImageDrawer.h"
#include <XnTypes.h>



ImageDrawer::ImageDrawer(xn::ImageGenerator &generator_)
  : generator(generator_)
{
  xn::ImageMetaData imageMD;
  generator.GetMetaData(imageMD);
  init(imageMD.XRes(), imageMD.YRes());
  texData = new GLubyte[getTextureX()*getTextureY()*3];
}

ImageDrawer::~ImageDrawer()
{
  delete[] texData;
}

void ImageDrawer::draw()
{
  update();
  TextureDrawer::draw();
}

void ImageDrawer::update()
{
  xn::ImageMetaData imageMD; 
  generator.GetMetaData(imageMD);
  
  const XnRGB24Pixel *src = imageMD.RGB24Data();
  
  for(int y = 0; y < getResolutionY(); y++){
    GLubyte *dst = &(texData[y * getTextureX() * 3]);
    memcpy(dst, &(src[y * getResolutionX()]), sizeof(XnRGB24Pixel) * getResolutionX());
  }
}

