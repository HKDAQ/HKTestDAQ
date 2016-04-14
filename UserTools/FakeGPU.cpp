#include "FakeGPU.h"

FakeGPU::FakeGPU():Tool(){}


bool FakeGPU::Initialise(std::string configfile, DataModel &data){

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_variables.Get("verbose", verbose);
	m_variables.Get("tcpport", tcpport);
	m_variables.Get("idport", idport);

	tcpport = "tcp://localhost:" + tcpport;
	Exe_sock = new zmq::socket_t(*(m_data->context), ZMQ_DEALER);
	std::cout << "connect " << tcpport << std::endl;
	Exe_sock->setsockopt(ZMQ_IDENTITY, idport.c_str(), idport.length());
//	Exe_sock->setsockopt(ZMQ_SNDTIMEO, 1000);
	Exe_sock->connect(tcpport.c_str());

	std::string port = "tcp://*:" + idport;
	std::cout << "bind " << port << std::endl;
	Thr_sock = new zmq::socket_t(*(m_data->context), ZMQ_DEALER);
//	Thr_sock->setsockopt(ZMQ_SNDTIMEO, 1000);
	Thr_sock->setsockopt(ZMQ_RCVTIMEO, 1000);
	Thr_sock->bind(port.c_str());

	zmq::pollitem_t pollist [] =
	{
		{*Exe_sock, 0, ZMQ_POLLOUT, 0},
		{*Exe_sock, 0, ZMQ_POLLIN, 0},
	};

	bool cond = true;
	while (cond)
	{
		zmq::poll(pollist, 2, 1000);

		if (pollist[0].revents & ZMQ_POLLOUT)	//tell Server to be done -> Submit result
		{
			zmq::message_t req(idport.length() + 1);
			snprintf((char*) req.data(), idport.length() + 1, idport.c_str());
			if (Exe_sock->send(req))
				std::cout << "request sent" << std::endl;
			usleep(500000);
		}

		if (pollist[1].revents & ZMQ_POLLIN)	//listen for Server -> get data
		{
			zmq::message_t msgin;
			Exe_sock->recv(&msgin);
			std::string repl = std::string(static_cast<char*>(msgin.data()));
			if (repl == "OPEN") cond = false;
		}

	}

	return true;
}


bool FakeGPU::Execute(){

	zmq::message_t msgin;
	if (Thr_sock->recv(&msgin))
	{
		std::string msg = std::string(static_cast<char*>(msgin.data()));
		std::cout << "GPU <= " << msg << std::endl;
		zmq::message_t rep(5);
		snprintf((char*) rep.data(), 5, test(msg).c_str());
		std::cout << "RES = " << test(msg) << std::endl;
		Thr_sock->send(rep);
	}

	return true;
}


bool FakeGPU::Finalise(){

	delete Exe_sock;
	Exe_sock = 0;
	delete Thr_sock;
	Thr_sock = 0;

	return true;
}


std::string FakeGPU::test(std::string input)
{
	if (input == "Hello World!") return "1";	
	else return "0";	
}
