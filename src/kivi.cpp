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
Drawer *gDrawOrder[2];

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
    gDrawOrder[0] = gDepthDrawer;
    gDrawOrder[1] = gImageDrawer;
    winMode = DEPTH_WINDOW;
    break;
  case IMAGE_WINDOW:
    gDrawOrder[0] = gImageDrawer;
    gDrawOrder[1] = gDepthDrawer;
    winMode = IMAGE_WINDOW;
    break;
  }
  
  if(gDrawOrder[0]){
    gDrawOrder[0]->setDrawRegion(0, 0, WIN_SIZE_X, WIN_SIZE_Y);
  }
  if(gDrawOrder[1]){
    gDrawOrder[1]->setDrawRegion(WIN_SIZE_X/15, WIN_SIZE_Y/15, WIN_SIZE_X/3, WIN_SIZE_Y/3);
  }
}

void xn_init()
{
  char filename[256];
  sprintf(filename, "%s", argv);
  xn_call_and_check(gContext.Init(), "init context");
  xn_call_and_check(gContext.OpenFileRecording(filename), "set global mirror mode.");
  
  gContext.FindExistingNode(XN_NODE_TYPE_PLAYER, gPlayer);
  gPlayer.SetRepeat(false);
  
  XnStatus rc = XN_STATUS_OK;
  xn::ImageGenerator image;
  rc = gContext.FindExistingNode(XN_NODE_TYPE_IMAGE, image);
  gImageDrawer = (rc == XN_STATUS_OK)? new ImageDrawer(image) : NULL;
  
  xn::DepthGenerator depth;
  rc = gContext.FindExistingNode(XN_NODE_TYPE_DEPTH, depth);
  gDepthDrawer = (rc == XN_STATUS_OK)? new DepthDrawer(depth) : NULL;
  
  setMainWindow(IMAGE_WINDOW);
}

void gl_onDisplay()
{
  gContext.WaitAndUpdateAll();
  glClear(GL_COLOR_BUFFER_BIT);	

  for(int i = 0; i < 2; i++){
    if(gDrawOrder[i] != NULL){
      gDrawOrder[i]->draw();
    }
  }
  
  glutSwapBuffers();
}

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
  if(argc==2){
  gl_init(&argc, argv);

  xn_init(argv[1]);
  glutMainLoop();

  }
  return 0;
}

