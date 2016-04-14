#include "DataCentre.h"

DataCentre::DataCentre():Tool(){}

bool DataCentre::Initialise(std::string configfile, DataModel &data){

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_variables.Get("verbose", verbose);
	m_variables.Get("tcpport", tcpport);
	m_variables.Get("inproc", inproc);

	tcpport = "tcp://*:" + tcpport;
	Tcp_sock = new zmq::socket_t(*(m_data->context), ZMQ_ROUTER);
	Tcp_sock->setsockopt(ZMQ_SNDTIMEO, 10000);
	Tcp_sock->bind(tcpport.c_str());

	std::string inport = "inproc://" + inproc;
	In_sock = new zmq::socket_t(*(m_data->context), ZMQ_ROUTER);
	In_sock->setsockopt(ZMQ_SNDTIMEO, 10000);
	In_sock->setsockopt(ZMQ_RCVTIMEO, 10000);
	In_sock->bind(inport.c_str());

	polmain = new zmq::pollitem_t[2];
	zmq::pollitem_t pol0 = {*Tcp_sock, 0, ZMQ_POLLIN, 0};
	polmain[0] = pol0;
	zmq::pollitem_t pol1 = {*In_sock, 0, ZMQ_POLLIN, 0};
	polmain[1] = pol1;

	it = m_data->vData.begin();
	std::cout << "vData size " << m_data->vData.size() << std::endl;

	done_f = false;
	final_f = false;

	return true;
}


bool DataCentre::Execute(){

	if (!done_f)
	{
		std::cout << "# of threads = " << mThread.size() << std::endl;
		std::cout << "it is @ " << it - m_data->vData.begin() << std::endl;
		zmq::poll(polmain, 2, 0);
	
		zmq::message_t idabak;
			
		if (polmain[0].revents & ZMQ_POLLIN)
		{
			zmq::message_t id;
			zmq::message_t msg;
			Tcp_sock->recv(&id);
			Tcp_sock->recv(&msg);
	
			std::string lid = std::string(static_cast<char*>(id.data()), id.size());
			std::string lmsg = std::string(static_cast<char*>(msg.data()));
			std::cout << "TCP: " << lid << "\t" << lmsg << std::endl;
			if (mThread.count(lid) == 0)
			{
				zmq::message_t open(5);
				snprintf((char*) open.data(), 5, "OPEN");
				Tcp_sock->send(id, ZMQ_SNDMORE);
				Tcp_sock->send(open);
	
				info tmp;
				tmp.ctx = m_data->context;
				tmp.id = lid;
				tmp.addr = inproc; 
		
				mInfo[lid] = tmp;
				mThread[lid] = new pthread_t;
				int cc = pthread_create(mThread[lid], NULL, &Handler, &(mInfo[lid]));
				mLine[lid] = it;
				if (cc) Log("thread spawned\n", verbose, 1);
			}
			else
			{
				std::cout << "REPLY back " << std::endl;
				zmq::message_t warn(5);
				snprintf((char*) warn.data(), 5, "WARN");
				In_sock->send(id, ZMQ_SNDMORE);
				In_sock->send(warn);
			}
		}
	
		if (polmain[1].revents & ZMQ_POLLIN)
		{
			zmq::message_t id;
			zmq::message_t rep;
			In_sock->recv(&id);
			if (In_sock->recv(&rep))
			{		
				in_id = std::string(static_cast<char*>(id.data()), id.size());
				in_rep = std::string(static_cast<char*>(rep.data()));
				std::cout << "INP: " << in_id << "\t" << in_rep << std::endl;
				if (in_rep == "0") 
					m_data->vRes.at(mLine[in_id] - m_data->vData.begin()) = 0;
				else if (in_rep == "1") 
					m_data->vRes.at(mLine[in_id] - m_data->vData.begin()) = 1;
				else if (in_rep == "CANC")
				{
					if (mInfo.count(in_id)) mInfo.erase(in_id);
					if (mThread.count(in_id)) mThread.erase(in_id);
				}
				else Log("Uknown result\n", verbose, 2);
	
				if (it != m_data->vData.end())
				{
					zmq::message_t data((*it).length()+1);
					snprintf((char*) data.data(), (*it).length()+1, (*it).c_str());
					In_sock->send(id, ZMQ_SNDMORE);
					In_sock->send(data);
					mLine[in_id] = it++;
				}
				else
				{	
					zmq::message_t kill(5);
					snprintf((char*) kill.data(), 5, "QUIT");
					In_sock->send(id, ZMQ_SNDMORE);
					In_sock->send(kill);
	
					if (mInfo.count(in_id))
					{
						pthread_join(*(mThread[in_id]), NULL);
						mInfo.erase(in_id);
					}
					if ((mInfo.size() == 0) && (mThread.size() == 0))
						done_f = true;
				}
		}
	}

	usleep(1000);

	}
	return true;
}


bool DataCentre::Finalise(){

	if (!final_f)
	{
		delete In_sock;
		In_sock = 0;
		delete Tcp_sock;
		Tcp_sock = 0;
		delete polmain;
	
		mThread.clear();
		mInfo.clear();
		mLine.clear();
	}
	final_f = true;
	
	return true;
}

void* DataCentre::Handler(void* thrinfo)
{
	struct info *linfo = static_cast<info*>(thrinfo);
	std::string text = "CQ ";
	std::string tcpss = "tcp://localhost:" + (*linfo).id;
	std::string inss = "inproc://" + (*linfo).addr;
	std::cout << "Thread " << (*linfo).id << " spawned " << std::endl;

	zmq::socket_t Exe_hand(*((*linfo).ctx), ZMQ_DEALER);
	zmq::socket_t Tcp_hand(*((*linfo).ctx), ZMQ_DEALER);

	Exe_hand.setsockopt(ZMQ_IDENTITY, (*linfo).id.c_str(), (*linfo).id.length());
	Exe_hand.setsockopt(ZMQ_SNDTIMEO, 10000);
	Exe_hand.setsockopt(ZMQ_RCVTIMEO, 10000);
	Exe_hand.connect(inss.c_str());

	Tcp_hand.setsockopt(ZMQ_SNDTIMEO, 1000);
	Tcp_hand.setsockopt(ZMQ_RCVTIMEO, 1000);
	Tcp_hand.connect(tcpss.c_str());

	zmq::pollitem_t pollist [] =
	{
		{Exe_hand, 0, ZMQ_POLLIN, 0},
		{Exe_hand, 0, ZMQ_POLLOUT, 0},
//		{Tcp_hand, 0, ZMQ_POLLIN, 0},
//		{Tcp_hand, 0, ZMQ_POLLOUT, 0},
	};

	bool cond = true;
	std::string data = "";
	std::string repl = "";
	while (cond)
	{
		zmq::poll(&pollist[0], 2, -1);

		if (pollist[0].revents & ZMQ_POLLIN)	//listen for Server -> get data
		{
			zmq::message_t msgin;
			Exe_hand.recv(&msgin);
			repl = std::string(static_cast<char*>(msgin.data()));
			if (repl == "QUIT") cond = false;
		}

		if (pollist[1].revents & ZMQ_POLLOUT)	//tell Server to be done -> Submit result
		{
			zmq::message_t req(repl.length());
			snprintf((char*) req.data(), repl.length()+1, repl.c_str());
			if (Exe_hand.send(req))
			{
				zmq::message_t rep;
				if (Exe_hand.recv(&rep))
				{	
					data = std::string(static_cast<char*>(rep.data()));
					if(data=="QUIT")cond=false;
					if (Tcp_hand.send(rep))
					{
						zmq::message_t res;
						if (Tcp_hand.recv(&res))
						{
							repl = std::string(static_cast<char*>(res.data()));
						}
						else cond = false;
					}
					else cond = false;
				}
//				else cond = false;
			}
		}
		usleep(10000);

	}

	zmq::message_t canc(5);
	snprintf((char*) canc.data(), 5, "CANC");
	Exe_hand.send(canc);

	sleep(1);
	std::cout << "DEAD " << (*linfo).id << std::endl;
	pthread_exit(NULL);
}
