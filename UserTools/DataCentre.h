#ifndef DataCentre_H
#define DataCentre_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <pthread.h>

#include "Tool.h"

class DataCentre: public Tool {


	public:

		DataCentre();
		bool Initialise(std::string configfile,DataModel &data);
		bool Execute();
		bool Finalise();

		static void* Handler(void* thrinfo);

		struct info
		{
			zmq::context_t *ctx;
			std::string id;
			std::string addr;
		};

	private:

		std::map<std::string, pthread_t*> mThread;
		std::map<std::string, info> mInfo;
		std::map<std::string, std::vector<std::string>::iterator> mLine;
		std::vector<std::string>::iterator it;

		zmq::socket_t *Tcp_sock, *In_sock;
		zmq::pollitem_t *polmain; 

		int verbose;
		std::string tcpport, inproc;
		std::string in_id, in_rep;

		bool done_f, final_f;

};


#endif
