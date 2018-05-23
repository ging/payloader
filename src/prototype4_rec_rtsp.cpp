#include <stdio.h>
#include <libavutil/time.h>

#include "libs/OutputWriter.h"
#include "libs/RtspReader.h"
#include "libs/Unpackager.h"
#include "libs/Decoder.h"
#include "libs/Encoder.h"
#include "libs/Receiver.h"
#include "libs/Codecs.h"


int main(int argc, const char* argv[]) {

if (argc != 2) {
        printf("usage: %s/ej/1\n"
               "Example...\n"
               "\n", argv[0]);
        exit(1);
    }
    
	const char *input_file = argv[1];
	const char *output_file = "NULL";

	
	payloader::Decoder* decoder = new payloader::Decoder();
	//payloader::Encoder* encoder = new payloader::Encoder();
	payloader::OutputWriter* writer = new payloader::OutputWriter(output_file);
	payloader::RtspReader* receiver_rtsp = new payloader::RtspReader(input_file,NULL);

	payloader::VideoCodecInfo mp4Info;
	mp4Info.enabled = true;
	mp4Info.codec = AV_CODEC_ID_MPEG4;
    mp4Info.width = 704;
    mp4Info.height = 396;
    mp4Info.bitRate = 48000;

    payloader::VideoCodecInfo lheInfo;
	lheInfo.enabled = true;
	lheInfo.codec = AV_CODEC_ID_MLHE;
    lheInfo.width = 704;
    lheInfo.height = 396;
    lheInfo.bitRate = 48000;

    // common
	writer->init({}, mp4Info);
	decoder->setSink(writer);

	receiver_rtsp->setSink(decoder);
	receiver_rtsp->init();
}