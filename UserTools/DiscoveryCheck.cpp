#include "DiscoveryCheck.h"

DiscoveryCheck::DiscoveryCheck():Tool(){}


bool DiscoveryCheck::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  context = m_data->context;
  
  

  return true;
}


bool DiscoveryCheck::Execute(){

  sleep(1);
  zmq::socket_t Ireceive (*context, ZMQ_DEALER);
  Ireceive.connect("inproc://ServiceDiscovery");

  zmq::message_t send(256);
  std::string command="All 0";
  snprintf ((char *) send.data(), 256 , "%s" ,command.c_str()) ;
  Ireceive.send(send);	    
  
  zmq::message_t sizem;
  Ireceive.recv(&sizem);
  
  std::istringstream iss(static_cast<char*>(sizem.data()));
  int size;
  iss>>size;

  std::cout<<"discovered services = "<<size<<std::endl<<std::endl;

  RemoteServices.clear();

  for(int i=0;i<size;i++){

 Store *service = new Store;
  
 

  zmq::message_t servicem;
  Ireceive.recv(&servicem);
  
  std::istringstream ss(static_cast<char*>(servicem.data()));
  service->JsonPaser(ss.str()); 

  std::string uuid;
  service->Get("uuid",uuid);
  RemoteServices[uuid]=service;

  }



for (std::map<std::string,Store*>::iterator it=RemoteServices.begin(); it!=RemoteServices.end(); ++it){
  std::string output;
  *(it->second)>> output;
  std::cout<<output<<std::endl<<std::endl;;

 }

  return true;
}


bool DiscoveryCheck::Finalise(){

  return true;
}
