#include "Capture.h"

int Capture::count = 0;
char inputName[256];

Capture::Capture(xn::Context *ctx) 
  : state(CAPTURE_STOP), context(ctx), joints()
{
}

bool Capture::setFileName(char *name_)
{

  strcpy(inputName,name_);

  return true;
}
bool Capture::start(XnUserID user_)
{
  if(state == CAPTURE_START)
    return false;

  state = CAPTURE_START;  
  
  XnChar filename[256];
  recorder = new xn::Recorder;
  recorder->Create(*context);
  sprintf(filename, "%s-%d.oni", inputName, count);
  recorder->SetDestination(XN_RECORD_MEDIUM_FILE, filename);
  
  sprintf(filename, "%s-%d.csv", inputName, count);
  out = fopen(filename, "w");
  printf("start\n");
  user = user_;
  frame = 0;
  
  xn::ImageGenerator image;
  context->FindExistingNode(XN_NODE_TYPE_IMAGE, image);
  recorder->AddNodeToRecording(image);
  
  xn::DepthGenerator depth;
  context->FindExistingNode(XN_NODE_TYPE_DEPTH, depth);
  recorder->AddNodeToRecording(depth);
  
  writeHeader();
  return true;
}

bool Capture::stop()
{
  if(state == CAPTURE_STOP)
    return false;
  
  state = CAPTURE_STOP;  
  fclose(out);
  recorder->Release();
  delete recorder;
  
  count++;
  return true;
}

void Capture::addSkeletonJoint(XnSkeletonJoint joint)
{
  if(state == CAPTURE_STOP){
    joints.insert(joint);
  }
}

typedef std::set<XnSkeletonJoint>::iterator JointIterator;
void Capture::captureSkeleton(const xn::SkeletonCapability &skeleton)
{
  if(state == CAPTURE_STOP)
    return;
  
  if(skeleton.IsTracking(user)){
    fprintf(out, "%d,", frame);
    frame++;

    JointIterator it;
    for(it = joints.begin(); it != joints.end(); it++){
      XnSkeletonJointTransformation trans;
      skeleton.GetSkeletonJoint(user, *it, trans);
      writeJoint(trans);
    }
    putc('\n', out);
  }else{
    fprintf(stderr, "stop capture: %d\n", user);
    stop();
  }
}

void Capture::writeHeader()
{
  fprintf(out, "Frame,");
  
  JointIterator it;
  for(it = joints.begin(); it != joints.end(); it++){
    fprintf(out, "%s", getSkeletonJointName(*it));
    for(int i = 0; i < 14; i++){
      putc(',', out);
    }
  }
  putc('\n', out);
}

const char *Capture::getSkeletonJointName(XnSkeletonJoint joint) const
{
  static const char *name[] = { 
    "", "HEAD", "NECK", "TORSO", "WAIST",
    "LEFT_COLLAR", "LEFT_SHOULDER", "LEFT_ELBOW", "LEFT_WRIST", 
    "LEFT_HAND", "LEFT_FINGERTIP",
    "RIGHT_COLLAR", "RIGHT_SHOULDER", "RIGHT_ELBOW", "RIGHT_WRIST", 
    "RIGHT_HAND", "RIGHT_FINGERTIP",
    "LEFT_HIP", "LEFT_KNEE", "LEFT_ANKLE", "LEFT_FOOT",
    "RIGHT_HIP", "RIGHT_KNEE", "RIGHT_ANKLE", "RIGHT_FOOT"
  };
  
  return name[joint];
}

void Capture::writeJoint(const XnSkeletonJointTransformation &trans)
{
  XnVector3D v = trans.position.position;
  XnMatrix3X3 ori = trans.orientation.orientation;
  
  // position
  fprintf(out, "%f,%f,%f,%f,", trans.position.fConfidence, v.X, v.Y, v.Z);

  // orientation
  fprintf(out, "%f,", trans.orientation.fConfidence);
  for(int i = 0; i < 9; i++){
    fprintf(out, "%f,", ori.elements[i]);
  }
}
