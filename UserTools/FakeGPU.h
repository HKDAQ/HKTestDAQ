#ifndef FakeGPU_H
#define FakeGPU_H

#include <string>
#include <iostream>

#include "Tool.h"

class FakeGPU: public Tool {


	public:

		FakeGPU();
		bool Initialise(std::string configfile,DataModel &data);
		bool Execute();
		bool Finalise();


	private:

		std::string test(std::string input);
		zmq::socket_t *Exe_sock, *Thr_sock;
		zmq::pollitem_t *polin;

		int verbose;
		std::string tcpport, idport;
};


#endif
