#ifndef SimpleRemoteCom_H
#define SimpleRemoteCom_H

#include <string>
#include <iostream>

#include "Tool.h"

#include "SocketCom.h"

class SimpleRemoteCom: public Tool {


 public:

  SimpleRemoteCom();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  bool *m_newbuff;
  char *m_buffer;

  SocketCom *client;

};


#endif
