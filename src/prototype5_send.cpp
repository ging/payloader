#include <stdio.h>
#include <libavutil/time.h>

#include "libs/InputReader.h"
#include "libs/Packager.h"
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
	payloader::Sender* sender = new payloader::Sender("localhost", "3001");

	payloader::VideoCodecInfo videoInfo;
	videoInfo.enabled = true;
	videoInfo.codec = AV_CODEC_ID_MPEG4;
    videoInfo.width = 704;
    videoInfo.height = 396;
    videoInfo.bitRate = 48000;

	packager->init();
	sender->init();
	reader->setSink(packager);
	packager->setSink(sender);
	reader->init();
}