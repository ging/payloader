#include <stdio.h>
#include <libavutil/time.h>

#include "libs/InputReader.h"
#include "libs/RtspReader_fromDisk.h"
#include "libs/Packager.h"
#include "libs/Decoder.h"
#include "libs/Encoder.h"
#include "libs/Sender.h"
#include "libs/SenderRtsp.h"
#include "libs/Codecs.h"

int main(int argc, const char* argv[]) {

	if (argc != 3) {
        printf("usage: %s input\n device"
               "Example program to input-output media to UDP.\n"
               "\n", argv[0]);
        exit(1);
    }

    // file: myvideo.avi
    // camera: /dev/video0 vfwcap
    // desktop :0.0 x11grab
    const char *input = argv[1];
    const char *device = argv[2];

	payloader::RtspReader_fromDisk* reader = new payloader::RtspReader_fromDisk(input, device);
	//payloader::Packager* packager = new payloader::Packager();
	//payloader::Decoder* decoder = new payloader::Decoder();
	// payloader::Encoder* encoder = new payloader::Encoder();
	// payloader::Sender* sender = new payloader::Sender("localhost", "3001");
	payloader::SenderRtsp* sender_rtsp = new payloader::SenderRtsp();


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
	//packager->init();
	sender->init(NULL);


	// 4a sin transcodificación
	   reader->setSink(sender_rtsp);


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
	//packager->setSink(sender);
	reader->init();
}