#ifndef CAPTURE_H_INCLUDED
#define CAPTURE_H_INCLUDED

#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <set>

class Capture
{
  enum {
    CAPTURE_START, 
    CAPTURE_STOP
  } state;
  xn::Context *context;
  std::set<XnSkeletonJoint> joints;
  xn::Recorder *recorder;
  static int count;
  FILE *out;
  XnUserID user;
  int frame;
  
public:
  Capture(xn::Context *context);
  virtual ~Capture() {};
  
  bool start(XnUserID user);
  bool stop();
  void addSkeletonJoint(XnSkeletonJoint joint);
  void captureSkeleton(const xn::SkeletonCapability &skeleton);
  XnUserID getCapturingUser() const {return user;}
  bool isCapturing() const {return state == CAPTURE_START;}
  
 private:
  void writeHeader();
  void writeJoint(const XnSkeletonJointTransformation &trans);
  const char *getSkeletonJointName(XnSkeletonJoint joint) const;
};

#endif /* CAPTURE_H_INCLUDED */

