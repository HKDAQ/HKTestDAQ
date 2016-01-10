#ifndef DiscoveryCheck_H
#define DiscoveryCheck_H

#include <string>
#include <iostream>

#include "Tool.h"
#include "Store.h"
#include "zmq.hpp"

class DiscoveryCheck: public Tool {


 public:

  DiscoveryCheck();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  zmq::context_t* context;
  std::map<std::string,Store*> RemoteServices;



};


#endif
