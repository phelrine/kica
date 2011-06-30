#include "CSVData.h"
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <clx/table.h>
#include <clx/tokenizer.h>

using namespace std;
typedef list<pair<string, vector<XnSkeletonJointTransformation> *> > Column;

CSVData::CSVData(const char *filename)
{
  fstream input(filename);
  string header, line, joint, tmp;
  Column column;
  
  // parse header
  getline(input, header);
  replace(header.begin(), header.end(), ',', ' ');
  stringstream ss(header);
  ss >> joint;
  while(ss >> joint){
    cout << joint << endl;
    column.push_back(make_pair(joint, new vector<XnSkeletonJointTransformation>));
  }
  
  clx::char_separator<char> sep(',');
  clx::table<double> table(input, clx::create_tokenizer<double>(sep));
  for (size_t i = 0; i < table.size(); i++){
    vector<double>::iterator eit = table[i].begin();
    eit++; // skip frame column;
      
    Column::iterator cit;
    for(cit = column.begin(); cit != column.end(); cit++){
      XnSkeletonJointTransformation t;
      
      XnSkeletonJointPosition *pos = &t.position;
      pos->fConfidence = *eit++;
      pos->position.X = *eit++;
      pos->position.Y = *eit++;
      pos->position.Z = *eit++;

      XnSkeletonJointOrientation *ori = &t.orientation;
      ori->fConfidence = *eit++;
      for(int j=0; j < 9; j++) 
	ori->orientation.elements[j] = *eit++;
      
      cit->second->push_back(t);
    }
  }

  Column::iterator it;
  for(it = column.begin(); it != column.end(); it++){
    data[it->first] = it->second;
  }
}
