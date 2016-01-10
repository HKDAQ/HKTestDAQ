#include "SimpleRemoteCom.h"

SimpleRemoteCom::SimpleRemoteCom():Tool(){}


bool SimpleRemoteCom::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;


  m_buffer =new char[256];
  m_newbuff=new bool;


  client= new SocketCom(true,"hepdaq002",24001,m_buffer,m_newbuff);






  return true;
}


bool SimpleRemoteCom::Execute(){

  client->Connect();

  std::string tmp;
  bool running=true;
  
  while (running){
    std::cout<<"type command to send (type \"close\" to quit) "<<std::endl;
    std::cin>>tmp;
    std::cout<<"Sending Command = "<<tmp<<std::endl;
    if (tmp=="close") running=false;
    else{
      strcpy(m_buffer,tmp.c_str());
      client->Send();
    }
    
  }

  return true;
}


bool SimpleRemoteCom::Finalise(){

  client->Disconnect();
  return true;
}
