#include <XnOpenNI.h>
#include <XnCppWrapper.h>

#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <algorithm>

#include "ImageDrawer.h"
#include "DepthDrawer.h"
#include "SkeletonDrawer.h"
#include "Capture.h"

#define WIN_SIZE_X (800)
#define WIN_SIZE_Y (600)
#define WIN_TITLTE "KInect CApture"

xn::Context gContext;
xn::UserGenerator gUserGenerator;
xn::DepthGenerator gDepthGenerator;
char gPose[20] = "";

ImageDrawer *gImageDrawer;
DepthDrawer *gDepthDrawer;
SkeletonDrawer *gSkeletonDrawer;
Capture *capture;

void xn_call_and_check(XnStatus status, const char *message)
{
  if(status != XN_STATUS_OK){
    fprintf(stderr, "fail to %s: %s\n", message, xnGetStatusName(status));
    exit(status);
  }
}

void XN_CALLBACK_TYPE xn_onFoundUser
(xn::UserGenerator &user, XnUserID id, void *cookie)
{
  fprintf(stderr, "found user %d\n", id);
  if(gUserGenerator.GetSkeletonCap().NeedPoseForCalibration()){
    gUserGenerator.GetPoseDetectionCap().StartPoseDetection(gPose, id);
  }else{
    gUserGenerator.GetSkeletonCap().RequestCalibration(id, true);
  }
}

void XN_CALLBACK_TYPE xn_onPoseDetected
(xn::PoseDetectionCapability &cap, const XnChar* pose, XnUserID id, void *cookie)
{
  fprintf(stderr, "%s detected. strat calibration\n", pose);
  gUserGenerator.GetPoseDetectionCap().StopPoseDetection(id);
  gUserGenerator.GetSkeletonCap().RequestCalibration(id, TRUE);
}

void XN_CALLBACK_TYPE xn_onCalibrated
(xn::SkeletonCapability &skel, XnUserID id, XnBool isSuccess, void *cookie)
{
  if(isSuccess){
    gUserGenerator.GetSkeletonCap().StartTracking(id);
  }else{
    if(gUserGenerator.GetSkeletonCap().NeedPoseForCalibration()){
      gUserGenerator.GetPoseDetectionCap().StartPoseDetection(gPose, id);
    }else{
      gUserGenerator.GetSkeletonCap().RequestCalibration(id, true);
    }
  }
}

void XN_CALLBACK_TYPE xn_onLostUser
(xn::UserGenerator &user, XnUserID id, void *cookie)
{
  fprintf(stderr, "lost user %d\n", id);
  gUserGenerator.GetSkeletonCap().StopTracking(id);
}

void xn_init(char *argv)
{
  xn_call_and_check(gContext.InitFromXmlFile("../conf/SamplesConfig.xml"), "init from xml");
  
  xn::ImageGenerator image;
  xn_call_and_check(gContext.FindExistingNode(XN_NODE_TYPE_IMAGE, image), 
		    "find image node");
  xn_call_and_check(gContext.FindExistingNode(XN_NODE_TYPE_DEPTH, gDepthGenerator),
		    "find depth node");
  xn_call_and_check(gContext.FindExistingNode(XN_NODE_TYPE_USER, gUserGenerator),
		    "find user node");

  XnCallbackHandle userCB, poseCB, calibrationCB; 
  gUserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
  gUserGenerator.RegisterUserCallbacks(xn_onFoundUser, xn_onLostUser, NULL, userCB);
  gUserGenerator.GetSkeletonCap().RegisterCalibrationCallbacks(NULL, xn_onCalibrated, NULL, calibrationCB);
  gUserGenerator.GetSkeletonCap().GetCalibrationPose(gPose);
  gUserGenerator.GetPoseDetectionCap().RegisterToPoseCallbacks(xn_onPoseDetected, NULL, NULL, poseCB);
  

  gImageDrawer = new ImageDrawer(image);  
  gImageDrawer->setDrawRegion(WIN_SIZE_X/15, WIN_SIZE_Y/15, WIN_SIZE_X/3, WIN_SIZE_Y/3);
  
  gDepthDrawer = new DepthDrawer(gDepthGenerator, gUserGenerator);
  gDepthDrawer->setDrawRegion(0, 0, WIN_SIZE_X, WIN_SIZE_Y);
  
  gSkeletonDrawer = new SkeletonDrawer(gUserGenerator, gDepthGenerator);
  gSkeletonDrawer->setDrawRegion(0, 0, WIN_SIZE_X, WIN_SIZE_Y);
    
  capture  = new Capture(&gContext);
  capture->setFileName(argv);

  XnSkeletonJoint joints[] = {
    XN_SKEL_HEAD, XN_SKEL_NECK, XN_SKEL_TORSO, 
    XN_SKEL_LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_HAND,
    XN_SKEL_RIGHT_SHOULDER, XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_HAND,
    XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE, XN_SKEL_LEFT_FOOT,
    XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE, XN_SKEL_RIGHT_FOOT,
  };

  for(int i = 0; i < sizeof(joints)/sizeof(XnSkeletonJoint); i++){
    capture->addSkeletonJoint(joints[i]);
  }
}

void gl_printString(void *font, const char *str)
{
  int i,l = strlen(str);

  for(i=0; i<l; i++)
    {
      glutBitmapCharacter(font,*str++);
    }
}

void gl_labelCapturingUser(XnUserID id)
{
  XnVector3D pos;
  gUserGenerator.GetCoM(id, pos);
  gDepthGenerator.ConvertRealWorldToProjective(1, &pos, &pos);
  glColor4f(0, 0, 0, 0);
  xn::DepthMetaData depthMD;
  gDepthGenerator.GetMetaData(depthMD);
  glRasterPos2i(pos.X/depthMD.XRes()*WIN_SIZE_X, 
		pos.Y/depthMD.YRes()*WIN_SIZE_Y);
  gl_printString(GLUT_BITMAP_HELVETICA_18, "Capturing");
  glColor4f(1.0, 1.0, 1.0, 1.0);
}

void gl_onDisplay()
{
  gContext.WaitAndUpdateAll();
  glClear(GL_COLOR_BUFFER_BIT);	

  gDepthDrawer->draw();
  gSkeletonDrawer->draw();
  gImageDrawer->draw();

  if(capture->isCapturing()){
    capture->captureSkeleton(gUserGenerator.GetSkeletonCap());
    gl_labelCapturingUser(capture->getCapturingUser());
  }
  
  glutSwapBuffers();
}

void gl_onKeyboard(unsigned char key, int x, int y)
{
  XnUInt16 nUser = 16;
  XnUserID users[16];
  
  switch(key){
  case 27:
    gContext.Shutdown();
    exit(1);
    break;
  case 'c':
    gUserGenerator.GetUsers(users, nUser);
    if(nUser == 0){
      fprintf(stderr, "cannot find user\n");
    }else if(capture->start(*std::min_element(users, &users[nUser]))){
      fprintf(stderr, "start recording\n");
    }
    break;
  case 's':
    if(capture->stop()){
      fprintf(stderr, "stop recording\n");
    }
    break;
  }
}

void gl_onIdle()
{
  glutPostRedisplay();
}

void gl_init(int *pargc, char **argv)
{
  glutInit(pargc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  
  glutInitWindowSize(WIN_SIZE_X, WIN_SIZE_Y);
  glutCreateWindow(WIN_TITLTE);
  glutSetCursor(GLUT_CURSOR_NONE);
  
  glutDisplayFunc(gl_onDisplay);
  glutKeyboardFunc(gl_onKeyboard);
  glutIdleFunc(gl_onIdle);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, WIN_SIZE_X, WIN_SIZE_Y, 0, -1.0, 1.0);       
}

int main(int argc, char *argv[])
{
  if(argc==2){
  gl_init(&argc, argv);

  xn_init(argv[1]);
  gContext.StartGeneratingAll();
  glutMainLoop();

  }

  return 0;
}
