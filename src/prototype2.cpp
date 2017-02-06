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


	
	payloader::VideoCodecInfo mp4Info;
	mp4Info.enabled = true;
	mp4Info.codec = AV_CODEC_ID_MPEG4;
	mp4Info.width = 704;
	mp4Info.height = 396;
 	mp4Info.bitRate = 48000;

 	payloader::AudioCodecInfo audioInfo;
	audioInfo.enabled = true;
	audioInfo.codec = (AVCodecID)65536;

	payloader::VideoCodecInfo lheInfo;
	lheInfo.enabled = true;
	lheInfo.codec = AV_CODEC_ID_MLHE;
    lheInfo.width = 704;
    lheInfo.height = 396;
    lheInfo.bitRate = 48000;
	
	// 2a mp4 -> mp4
	// decoder->init(audioInfo, mp4Info);
	// encoder->init(audioInfo, mp4Info);
	// writer->init(audioInfo, mp4Info);

	// 2b mp4 -> lhe
	// decoder->init({}, mp4Info);
	// encoder->init({}, lheInfo);
	// writer->init({}, lheInfo);

	// 2c lhe -> mp4
	decoder->init({}, lheInfo);
	encoder->init({}, mp4Info);
	writer->init({}, mp4Info);
	
	reader->setSink(decoder);
	decoder->setSink(encoder);
	encoder->setSink(writer);
	reader->init();

}