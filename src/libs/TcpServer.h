#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "TcpConnection.h"
#include "Interfaces.h"
#include "logger.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <stdio.h>

using namespace std;
using namespace boost::system;

namespace payloader {

class TcpServer : public PortReceiver {
    DECLARE_LOGGER();
	public:
		TcpServer(boost::asio::io_service& io_service);
		virtual ~TcpServer();
		long int getterData();
		void tomaPuerto(long int puerto);
		
		char* data;
	    long int puerto;
	    pthread_t mithread;
	   
	   
		tcp::acceptor acceptor_;

	private:
		void start_accept();
		void handle_accept(TcpConnection::pointer new_connection, const boost::system::error_code& error);

};
}	// Namespace payloader
#endif // TCPSERVER_H