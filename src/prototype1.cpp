#include <stdio.h>
#include <libavutil/time.h>

#include "libs/InputReader.h"
#include "libs/OutputWriter.h"

int main(int argc, const char* argv[]) {

	if (argc != 3) {
        printf("usage: %s input_file output_file\n"
               "Example program to input-output a media file.\n"
               "\n", argv[0]);
        exit(1);
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

	payloader::InputReader* reader = new payloader::InputReader(input_file);
	payloader::OutputWriter* writer = new payloader::OutputWriter(output_file);

	payloader::VideoCodecInfo videoInfo;
	videoInfo.enabled = true;
	videoInfo.codec = AV_CODEC_ID_MPEG4;
    videoInfo.width = 704;
    videoInfo.height = 396;
    videoInfo.bitRate = 48000;

    payloader::AudioCodecInfo audioInfo;
	audioInfo.enabled = true;
	audioInfo.codec = (AVCodecID)65536;
	
	writer->init(audioInfo, videoInfo);
	reader->setSink(writer);
	reader->init();
}