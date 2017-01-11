#include <stdio.h>
#include <libavutil/time.h>

#include "libs/InputReader.h"
#include "libs/OutputWriter.h"
#include "libs/Decoder.h"
#include "libs/Encoder.h"
#include "libs/Codecs.h"

int main(int argc, const char* argv[]) {

	if (argc != 3) {
        printf("usage: %s input_file output_file\n"
               "Example program to input-output a media file with transcoding.\n"
               "\n", argv[0]);
        exit(1);
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

	payloader::InputReader* reader = new payloader::InputReader(input_file);
	payloader::Decoder* decoder = new payloader::Decoder();
	payloader::Encoder* encoder = new payloader::Encoder();
	payloader::OutputWriter* writer = new payloader::OutputWriter(output_file);

	payloader::VideoCodecInfo videoInfo;
	videoInfo.enabled = true;
	videoInfo.codec = (AVCodecID)13;
    videoInfo.width = 704;
    videoInfo.height = 396;
    videoInfo.bitRate = 48000;

    payloader::AudioCodecInfo audioInfo;
	audioInfo.enabled = true;
	audioInfo.codec = (AVCodecID)65536;
	
	decoder->init(audioInfo, videoInfo);
	encoder->init(audioInfo, videoInfo);
	writer->init(audioInfo, videoInfo);
	reader->setSink(decoder);
	decoder->setSink(encoder);
	encoder->setSink(writer);
	reader->init();
}