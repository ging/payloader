#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>
#include <libavutil/time.h>

#include "libs/ExternalInput.h"
#include "libs/rtp/RtpSink.h"

int main(int argc, const char* argv[]) {

	const char *url = "file:/home/alvaro/Documentos/arqueopterix/payloader_test/extras/video.avi";
	// const char *url = "file:/home/alvaro/Documentos/arqueopterix/payloader_test/extras/output.mp4";
	// const char *url = "file:/home/alvaro/Documentos/arqueopterix/payloader_test/extras/test_flv.flv";
	erizo::RtpSink* rtp = new erizo::RtpSink("localhost", "3011");
	erizo::ExternalInput* ei = new erizo::ExternalInput(url);
	ei->setAudioSink(rtp);
	ei->setVideoSink(rtp);
    ei-> init();

}