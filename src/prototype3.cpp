#include <stdio.h>
#include <libavutil/time.h>

#include "libs/InputReader.h"
#include "libs/OutputWriter.h"
#include "libs/Decoder.h"
#include "libs/Encoder.h"
#include "libs/Packager.h"
#include "libs/Unpackager.h"
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
	payloader::Packager* packager = new payloader::Packager();
	payloader::Decoder* decoder1 = new payloader::Decoder();
	payloader::Encoder* encoder1 = new payloader::Encoder();
	payloader::Decoder* decoder2 = new payloader::Decoder();
	payloader::Encoder* encoder2 = new payloader::Encoder();
	payloader::Unpackager* unpackager = new payloader::Unpackager();
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

    packager->init();
	unpackager->init();

	// 3a mp4 -> mp4
	// writer->init({}, mp4Info);
	// reader->setSink(packager);
	// packager->setSink(unpackager);
	// unpackager->setSink(writer);

	// 3b lhe -> lhe
	// writer->init({}, lheInfo);
	// reader->setSink(packager);
	// packager->setSink(unpackager);
	// unpackager->setSink(writer);

	// 3c mp4 -> mp4 (con transcoding)
	writer->init({}, mp4Info);
	encoder2->init({}, mp4Info);
	decoder2->init({}, lheInfo);
	encoder1->init({}, lheInfo);
	decoder1->init({}, mp4Info);
	reader->setSink(decoder1);
	decoder1->setSink(encoder1);
	encoder1->setSink(packager);
	packager->setSink(unpackager);
	unpackager->setSink(decoder2);
	decoder2->setSink(encoder2);
	encoder2->setSink(writer);

	
	reader->init();
}