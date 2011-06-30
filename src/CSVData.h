#ifndef CSVDATA_H_INCLUDED
#define CSVDATA_H_INCLUDED

#include <XnOpenNI.h>
#include <XnCppWrapper.h>

#include <map>
#include <vector>
#include <string>

class CSVData
{
  std::map<std::string, std::vector<XnSkeletonJointTransformation> *> data;

public:
  CSVData(const char *filename);
  virtual ~CSVData() {};
  
  XnSkeletonJointTransformation operator ()(const std::string joint, int n) const
  {
    return data.find(joint)->second->at(n);
  };
  
  const std::vector<XnSkeletonJointTransformation> *getColumn(std::string joint) const
  {
    return data.find(joint)->second;
  }

private:
};

#endif /* CSVDATA_H_INCLUDED */
