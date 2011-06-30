#include "DepthDrawer.h"

DepthDrawer::DepthDrawer(xn::DepthGenerator depth)
  : depth(depth), hasUserNode(false)
{
  initTexture();
}

DepthDrawer::DepthDrawer(xn::DepthGenerator depth, xn::UserGenerator user)
  : depth(depth), user(user), hasUserNode(true)
{
  initTexture();
}

void DepthDrawer::initTexture()
{ 
  xn::DepthMetaData depthMD;
  depth.GetMetaData(depthMD);
  init(depthMD.XRes(), depthMD.YRes());
  texData = new GLubyte[getTextureX()*getTextureY()*3];
  depthHist = new float[depthMD.ZRes()];
}

DepthDrawer::~DepthDrawer()
{
  delete[] texData;
}

void DepthDrawer::draw()
{
  update();
  TextureDrawer::draw();
}

void DepthDrawer::update()
{
  int resX = getResolutionX();
  int resY = getResolutionY();
  int numberOfPoint = 0;
  xn::DepthMetaData depthMD;
  xn::SceneMetaData sceneMD;

  depth.GetMetaData(depthMD);
  if(hasUserNode){
    user.GetUserPixels(0, sceneMD);
  }
  
  int resZ = depthMD.ZRes();
  xnOSMemSet(depthHist, 0, sizeof(float) * resZ);
  
  for(int y = 0; y < resY; y++){
    for(int x = 0; x < resX; x++){
      if(depthMD(x, y) != 0){
	depthHist[depthMD(x, y)]++;
	numberOfPoint++;
      }
    }
  }

  for(int i = 1; i < resZ; i++){
    depthHist[i] += depthHist[i-1];
  }
  
  for(int i = 0; i < resZ; i++){
    depthHist[i] = (1.0 - depthHist[i]/numberOfPoint) * 255;
  }

  for(int y = 0; y < resY; y++){
    for(int x = 0; x < resX; x++){
      int p = depthHist[depthMD(x, y)];
      GLubyte *dst = &texData[(y*getTextureX() + x) * 3];
      XnUserID userID = 0;
      
      if(hasUserNode ){
	userID = sceneMD(x, y);
      }
      if(userID != 0){
	for(int i = 0; i < 3; i++){
	  dst[i] = userColor[userID % 6][i] * p;
	}
      }else{
	xnOSMemSet(dst, p, sizeof(GLubyte)*3);
      }
    }
  }
}

float DepthDrawer::userColor[6][3] = {
  {1, 0, 0},
  {0, 1, 0},
  {0, 0, 1},
  {1, 1, 0},
  {1, 0, 1},
  {0, 1, 1},
};  
