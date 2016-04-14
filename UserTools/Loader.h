#ifndef Loader_H
#define Loader_H

#include <string>
#include <iostream>
#include <fstream>

#include "Tool.h"

class Loader: public Tool {


 public:

  Loader();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  std::string infile, outfile, emp;




};


#endif
