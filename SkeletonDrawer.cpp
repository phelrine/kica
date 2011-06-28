#include "SkeletonDrawer.h"
#include <algorithm>

SkeletonDrawer::SkeletonDrawer
(xn::UserGenerator &user, xn::DepthGenerator &depth)
  : userGenerator(user), depthGenerator(depth)
{
  
}

void SkeletonDrawer::setDrawRegion
(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY)
{
  x = topLeftX;
  y = topLeftY;
  width = bottomRightX - topLeftX;
  height = bottomRightY - topLeftY;
}

void SkeletonDrawer::draw()
{
  XnUInt16 numberOfUser = 16;
  XnUserID users[16];
  userGenerator.GetUsers(users, numberOfUser);
  
  XnUserID user = *std::min_element(users, &(users[numberOfUser]));
  if(userGenerator.GetSkeletonCap().IsTracking(user)){
    glBegin(GL_LINES);
    glColor4f(1, 0, 0, 0);
    drawLimb(user, XN_SKEL_HEAD, XN_SKEL_NECK);
  
    drawLimb(user, XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER);
    drawLimb(user, XN_SKEL_LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW);
    drawLimb(user, XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_HAND);
    drawLimb(user, XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER);
    drawLimb(user, XN_SKEL_RIGHT_SHOULDER, XN_SKEL_RIGHT_ELBOW);
    drawLimb(user, XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_HAND);

    drawLimb(user, XN_SKEL_LEFT_SHOULDER, XN_SKEL_TORSO);
    drawLimb(user, XN_SKEL_RIGHT_SHOULDER, XN_SKEL_TORSO);

    drawLimb(user, XN_SKEL_TORSO, XN_SKEL_LEFT_HIP);
    drawLimb(user, XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE);
    drawLimb(user, XN_SKEL_LEFT_KNEE, XN_SKEL_LEFT_FOOT);

    drawLimb(user, XN_SKEL_TORSO, XN_SKEL_RIGHT_HIP);
    drawLimb(user, XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE);
    drawLimb(user, XN_SKEL_RIGHT_KNEE, XN_SKEL_RIGHT_FOOT);

    drawLimb(user, XN_SKEL_LEFT_HIP, XN_SKEL_RIGHT_HIP);
    glColor4f(1, 1, 1, 1);
    glEnd();
  }
}

void SkeletonDrawer::drawLimb
(XnUserID id, XnSkeletonJoint j1, XnSkeletonJoint j2)
{
  XnSkeletonJointPosition p1, p2;
  userGenerator.GetSkeletonCap().GetSkeletonJointPosition(id, j1, p1);
  userGenerator.GetSkeletonCap().GetSkeletonJointPosition(id, j2, p2);

  if (p1.fConfidence < 0.1 || p2.fConfidence < 0.1){
    return;
  }
  
  XnPoint3D points[2];
  points[0] = p1.position;
  points[1] = p2.position;
  depthGenerator.ConvertRealWorldToProjective(2, points, points);

  xn::DepthMetaData md;
  depthGenerator.GetMetaData(md);

  for(int i = 0; i < 2; i++){
    points[i].X = x + points[i].X / md.XRes() * width;
    points[i].Y = y + points[i].Y / md.YRes() * height;
  }
  
  glVertex3f(points[0].X, points[0].Y, 0);
  glVertex3f(points[1].X, points[1].Y, 0);
}
