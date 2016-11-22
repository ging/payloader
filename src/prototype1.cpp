#include <stdio.h>
#include <libavutil/time.h>

#include "libs/InputReader.h"
#include "libs/OutputWriter.h"

int main(int argc, const char* argv[]) {

	const char *url = "file:/home/alvaro/Documentos/arqueopterix/payloader_test/extras/output.mp4";
	payloader::InputReader* reader = new payloader::InputReader(url);

	const char *outputurl = "file:/home/alvaro/Documentos/arqueopterix/payloader_test/extras/result.mp4";
	payloader::OutputWriter* writer = new payloader::OutputWriter(outputurl);
	writer->init();

	reader->setSink(writer);
	reader->init();
}