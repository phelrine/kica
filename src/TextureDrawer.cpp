#include "TextureDrawer.h"

void TextureDrawer::init(int x, int y)
{
  resX = x;
  resY = y;
  
  texX = calcClosestPowerOfTwo(resX);
  texY = calcClosestPowerOfTwo(resY);
  
  glGenTextures(1, &texID);
  texData = new GLubyte[texX * texY * 3];

  for(int i = 0; i < 8; i++){
    texCoords[i] = 0;
  }
  texCoords[3] = texCoords[5] = (float)resY/texY;
  texCoords[4] = texCoords[6] = (float)resX/texX;
}

void TextureDrawer::draw()
{
  glEnable(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, texX, texY, 0, GL_RGB, GL_UNSIGNED_BYTE,
	       getTextureData());
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
  drawRect();
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void TextureDrawer::setDrawRegion
(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY)
{
  drawCoords[0] = drawCoords[2] = topLeftX;
  drawCoords[1] = drawCoords[7] = topLeftY;
  drawCoords[4] = drawCoords[6] = bottomRightX;
  drawCoords[3] = drawCoords[5] = bottomRightY;
}

void TextureDrawer::drawRect()
{
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, drawCoords);
  glDrawArrays(GL_POLYGON, 0, 4);
  glDisableClientState(GL_VERTEX_ARRAY);
}

int TextureDrawer::calcClosestPowerOfTwo(int n) const
{
  int m = 1;
  while(m < n) m <<= 1;
  return m;
}
