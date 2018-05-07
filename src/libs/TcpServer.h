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

using namespace std;

namespace payloader {

class TcpServer : public PortReceiver {
    DECLARE_LOGGER();
	public:
		TcpServer(boost::asio::io_service& io_service);
		virtual ~TcpServer();
		long int getterData();
		void tomaPuerto(long int puerto);
	
		char* data;
	    long int puerto = 0;
	    pthread_t mithread;
	    //boost::array<char, 256> buf; // We use a boost::array to hold the received data. 


	private:
		void start_accept();
		void handle_accept(TcpConnection::pointer new_connection, const boost::system::error_code& error);

};
}	// Namespace payloader
#endif // TCPSERVER_H