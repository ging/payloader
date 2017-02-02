#include <stdio.h>
#include <libavutil/time.h>

#include "libs/OutputWriter.h"
#include "libs/Unpackager.h"
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
	payloader::OutputWriter* writer = new payloader::OutputWriter(output_file);

	payloader::VideoCodecInfo videoInfo;
	videoInfo.enabled = true;
	videoInfo.codec = AV_CODEC_ID_MPEG4;
    videoInfo.width = 704;
    videoInfo.height = 396;
    videoInfo.bitRate = 48000;

	writer->init({}, videoInfo);
	unpackager->init();
	unpackager->setSink(writer);
	receiver->setSink(unpackager);
	receiver->init();
}