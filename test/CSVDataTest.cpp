#include <iostream>
#include "CSVData.h"

int main(int argc, char *argv[])
{
  CSVData csv("skeleton-0.csv");
  const std::vector<XnSkeletonJointTransformation> *col = csv.getColumn("HEAD");
  std::vector<XnSkeletonJointTransformation>::const_iterator it;

  for(it = col->begin(); it != col->end(); it++){
    std::cout << (*it).position.position.X << std::endl;
  }

  return 0;
}
