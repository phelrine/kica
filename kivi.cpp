#include <XnOpenNI.h>
#include <XnCppWrapper.h>

#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "ImageDrawer.h"
#include "DepthDrawer.h"
#include "SkeletonDrawer.h"
#include "Capture.h"

#define WIN_SIZE_X (800)
#define WIN_SIZE_Y (600)
#define WIN_TITLTE "KInect VIewer"

xn::Context gContext;
xn::Player gPlayer;
ImageDrawer *gImageDrawer;
DepthDrawer *gDepthDrawer;
SkeletonDrawer *g_skeletonDrawer;

void xn_call_and_check(XnStatus status, const char *message)
{
  if(status != XN_STATUS_OK){
    fprintf(stderr, "fail to %s: %s\n", message, xnGetStatusName(status));
    exit(status);
  }
}

typedef enum{ DEPTH_WINDOW, IMAGE_WINDOW } WindowMode;
WindowMode winMode = DEPTH_WINDOW;

void setMainWindow(WindowMode mode)
{
  switch(mode){
  case DEPTH_WINDOW:
    gDepthDrawer->setDrawRegion(0, 0, WIN_SIZE_X, WIN_SIZE_Y);
    gImageDrawer->setDrawRegion(WIN_SIZE_X/15, WIN_SIZE_Y/15, WIN_SIZE_X/3, WIN_SIZE_Y/3);
    winMode = DEPTH_WINDOW;
    break;
  case IMAGE_WINDOW:
    gDepthDrawer->setDrawRegion(WIN_SIZE_X/15, WIN_SIZE_Y/15, WIN_SIZE_X/3, WIN_SIZE_Y/3);
    gImageDrawer->setDrawRegion(0, 0, WIN_SIZE_X, WIN_SIZE_Y);
    winMode = IMAGE_WINDOW;
    break;
  }
}

void xn_init()
{
  char filename[256];
  sprintf(filename, "%s.oni", "image-0");
  xn_call_and_check(gContext.Init(), "init context");
  xn_call_and_check(gContext.OpenFileRecording(filename), "set global mirror mode.");
  
  gContext.FindExistingNode(XN_NODE_TYPE_PLAYER, gPlayer);
  gPlayer.SetRepeat(false);

  xn::ImageGenerator image;
  gContext.FindExistingNode(XN_NODE_TYPE_IMAGE, image);
  gImageDrawer = new ImageDrawer(image);  
  xn::DepthGenerator depth;
  gContext.FindExistingNode(XN_NODE_TYPE_DEPTH, depth);
  gDepthDrawer = new DepthDrawer(depth);
  setMainWindow(IMAGE_WINDOW);
}

void gl_onDisplay()
{
  gContext.WaitAndUpdateAll();
  glClear(GL_COLOR_BUFFER_BIT);	

  switch(winMode){
  case DEPTH_WINDOW: 
    gDepthDrawer->draw();  
    gImageDrawer->draw();
    break;
  case IMAGE_WINDOW:
    gImageDrawer->draw();
    gDepthDrawer->draw();  
    break;
  }

  glutSwapBuffers();
}

int count = 0;
void gl_onKeyboard(unsigned char key, int x, int y)
{
  switch(key){
  case 27:
    gContext.Shutdown();
    exit(1);
    break;
  case 't':
    setMainWindow((winMode == DEPTH_WINDOW)? IMAGE_WINDOW : DEPTH_WINDOW);
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
  gl_init(&argc, argv);
  xn_init();

  glutMainLoop();

  return 0;
}

