#include <stdio.h>
#include <libavutil/time.h>
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <sstream>

#include "libs/RtspReader_fromDisk.h"
#include "libs/Decoder.h"
#include "libs/Encoder.h"
#include "libs/Receiver.h"
#include "libs/Sender.h"
#include "libs/Codecs.h"
#include "libs/InputReader.h"
#include "libs/Packager.h"


int main(int argc, const char* argv[]) {

	if (argc != 2) {
        printf("usage: %s input\n device"
               "Example program to input-output(fijo) media to UDP.\n"
               "\n", argv[0]);
        exit(1);
    }

    // file: myvideo.avi
    // camera: /dev/video0 vfwcap
    // desktop :0.0 x11grab
    const char *input = argv[1];
    //const char *destine = argv[2];
    const char *device = NULL;
    const char *output = NULL;
    output = "rtsp://138.4.7.72:8554/ej";//rtsp://138.4.7.72:8554/ej
   	//output = "rtp://localhost/desktop/ej/";//rtsp://138.4.7.72:8554/ej
   	//output = "prueba_a_disco.avi";

   
	
	payloader::RtspReader_fromDisk* sessionCreator = new payloader::RtspReader_fromDisk(input, output, device);

	if(sessionCreator->Done()){
		long int port = sessionCreator->PortGetter();
		
		printf("PUERTO ASIGNADO = %ld\n",port);
		std::string str = std::to_string(port);
		printf("PUERTO STRING = %s\n",str.c_str());


		payloader::InputReader* reader = new payloader::InputReader(input, device);
		payloader::Packager* packager = new payloader::Packager();

		payloader::Sender* sender_rtsp = new payloader::Sender("localhost", std::to_string(port));


		payloader::VideoCodecInfo mp4Info;
		mp4Info.enabled = true;
		mp4Info.codec = AV_CODEC_ID_MPEG4;
	    mp4Info.width = 704;
	    mp4Info.height = 396;
	    mp4Info.bitRate = 48000;

	    payloader::VideoCodecInfo mjpegInfo;
		mjpegInfo.enabled = true;
		mjpegInfo.codec = AV_CODEC_ID_MJPEG;
	    mjpegInfo.width = 640;
	    mjpegInfo.height = 480;
	    mjpegInfo.bitRate = 4800;

	    payloader::VideoCodecInfo rv32Info;
		rv32Info.enabled = true;
		rv32Info.codec = AV_CODEC_ID_RAWVIDEO;
	    rv32Info.width = 704;
	    rv32Info.height = 396;
	    rv32Info.bitRate = 4800;
	    rv32Info.pix_fmt = AV_PIX_FMT_YUV420P;

	    payloader::VideoCodecInfo lheInfo;
		lheInfo.enabled = true;
		lheInfo.codec = AV_CODEC_ID_MLHE;
	    lheInfo.width = 1680;
	    lheInfo.height = 1050;
	    lheInfo.bitRate = 48000;

	    // common
		packager->init();
		//receiver->setSink();
		//receiver->init();

		sender_rtsp->init();

		// 4a sin transcodificación
		reader->setSink(packager);
		   

		// 4b con transcodificación
		// encoder->init({}, lheInfo);
		// decoder->init({}, mp4Info);
		// reader->setSink(decoder);
		// decoder->setSink(encoder);
		// encoder->setSink(packager);

		// 4c desde webcam MJPEG
		// encoder->init({}, lheInfo);
		// decoder->init({}, mjpegInfo);
		// reader->setSink(decoder);
		// decoder->setSink(encoder);
		// encoder->setSink(packager);

		// 4e desde desktop
		// encoder->init({}, lheInfo);
		// decoder->init({}, rv32Info);
		// reader->setSink(decoder);
		// decoder->setSink(encoder);
		// encoder->setSink(packager);


		//Rtsp: InputReader -> sender_rtsp
		

		// common
		packager->setSink(sender_rtsp);
		reader->init();


}

}
