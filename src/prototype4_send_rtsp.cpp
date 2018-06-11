#include <stdio.h>
#include <libavutil/time.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <string>
#include <sstream>


#include "libs/TcpServer.h"
#include "libs/logger.h"


using boost::asio::ip::tcp;

int main(int argc, const char* argv[]) {

	if (argc != 1) {
        printf("usage: ./prototype4_send_rtsp\nExample program to output media (InputReader.cpp) to UDP with url rtsp://138.4.7.72:8554/ej.\n");
        exit(1);
    }
   
   	payloader::TcpServer* server =  new payloader::TcpServer();

}
