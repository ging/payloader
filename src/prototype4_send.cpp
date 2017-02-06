#include <stdio.h>
#include <libavutil/time.h>

#include "libs/InputReader.h"
#include "libs/Packager.h"
#include "libs/Decoder.h"
#include "libs/Encoder.h"
#include "libs/Sender.h"
#include "libs/Codecs.h"

int main(int argc, const char* argv[]) {

	if (argc != 2) {
        printf("usage: %s input_file\n"
               "Example program to input-output a media file to UDP.\n"
               "\n", argv[0]);
        exit(1);
    }

    const char *input_file = argv[1];

	payloader::InputReader* reader = new payloader::InputReader(input_file);
	payloader::Packager* packager = new payloader::Packager();
	payloader::Decoder* decoder = new payloader::Decoder();
	payloader::Encoder* encoder = new payloader::Encoder();
	payloader::Sender* sender = new payloader::Sender("localhost", "3001");

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
	packager->init();
	sender->init();


	// 4a sin transcodificación
	// reader->setSink(packager);


	// 4b con transcodificación
	encoder->init({}, lheInfo);
	decoder->init({}, mp4Info);
	reader->setSink(decoder);
	decoder->setSink(encoder);
	encoder->setSink(packager);


	// common
	packager->setSink(sender);
	reader->init();
}