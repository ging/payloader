#include <stdio.h>
#include <libavutil/time.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <string>
#include <sstream>

#include "libs/RtspReader_fromDisk.h"
#include "libs/TcpServer.h"
#include "libs/logger.h"


using boost::asio::ip::tcp;

/*
void serverLauncher() {
   try{
        // We need to create a server object to accept incoming client connections.
        boost::asio::io_service io_service;
        // The io_service object provides I/O services, such as sockets, 
        // that the server object will use.
       	payloader::TcpServer* server =  new payloader::TcpServer(io_service);
        // Run the io_service object to perform asynchronous operations.
        io_service.run();
        //ELOG_DEBUG("Escuchando... en el 8554");
        printf("Escuchando... en el 8554\n");
        
    }catch (std::exception& e){
        printf("Saliengo de fuera, ALGO HA PETADO\n");
        std::cerr << e.what() << std::endl;
    }
}*/
int main(int argc, const char* argv[]) {

	if (argc != 1) {
        printf("usage: ./prototype4_send_rtsp\nExample program to output media (InputReader.cpp) to UDP with url rtsp://138.4.7.72:8554/ej.\n");
        exit(1);
    }
    // file: myvideo.avi
    // camera: /dev/video0 vfwcap
    // desktop :0.0 x11grab
   /* const char *input = argv[1];
    const char *device = NULL;
    const char *output = NULL;
    output = "rtsp://138.4.7.72:8554/ej";//rtsp://138.4.7.72:8554/ej*/
   
	//payloader::RtspReader_fromDisk* sessionCreator = new payloader::RtspReader_fromDisk(input, output, device);//sacar el contexto necerario para el sdp
   	payloader::TcpServer* server =  new payloader::TcpServer();

}
