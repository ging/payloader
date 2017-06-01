#include <stdio.h>
#include <libavutil/time.h>

#include "libs/OutputWriter.h"
#include "libs/Unpackager.h"
#include "libs/Decoder.h"
#include "libs/Encoder.h"
#include "libs/Receiver.h"
#include "libs/Codecs.h"

int main(int argc, const char* argv[]) {

	if (argc != 2) {
        printf("usage: %s output_file\n"
               "Example program to input-output a media file from UDP.\n"
               "\n", argv[0]);
        exit(1);
    }

    const char *output_file = argv[1];

	payloader::Receiver* receiver = new payloader::Receiver(3001);
	payloader::Unpackager* unpackager = new payloader::Unpackager();
	payloader::Decoder* decoder = new payloader::Decoder();
	payloader::Encoder* encoder = new payloader::Encoder();
	payloader::OutputWriter* writer = new payloader::OutputWriter(output_file);

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
	unpackager->init();
	

	// 4a sin transcodificación
	// unpackager->setSink(writer);


	// 4b con transcodificación
	// encoder->init({}, mp4Info);
	// decoder->init({}, lheInfo);
	// unpackager->setSink(decoder);
	// decoder->setSink(encoder);
	// encoder->setSink(writer);

	// 4d a display
	decoder->init({}, lheInfo);
	unpackager->setSink(decoder);
	decoder->setSink(writer);
	

    // common
	receiver->setSink(unpackager);
	receiver->init();
}