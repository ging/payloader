#include "OutputWriter.h"
#include <sys/time.h>

namespace payloader {

DEFINE_LOGGER(OutputWriter, "OutputWriter");

OutputWriter::OutputWriter(const std::string& url) : output_url_(url) {
	av_context_ = NULL;

}

OutputWriter::~OutputWriter() {

  if (av_context_ != NULL) {
      av_write_trailer(av_context_);
  }

  if (av_context_ != NULL) {
      avio_close(av_context_->pb);
      avformat_free_context(av_context_);
      av_context_ = NULL;
  }
}

int OutputWriter::init(){
	av_register_all();

	av_context_ = avformat_alloc_context();
	if (av_context_ == NULL) {
    	ELOG_ERROR("Error allocating memory for IO context");
    	return -1;
    }

	output_url_.copy(av_context_->filename, sizeof(av_context_->filename), 0);
	av_context_->oformat = av_guess_format(NULL,  av_context_->filename, NULL);
    if (!av_context_->oformat) {
     	ELOG_ERROR("Error guessing format %s", av_context_->filename);
     	return -1;
    }

    ELOG_DEBUG("Output format %s", av_context_->oformat->long_name);
	int res = avio_open2(&av_context_->pb, av_context_->filename, AVIO_FLAG_WRITE, NULL, NULL);
    
    if (res < 0) {
	    ELOG_ERROR("Error opening output file");
	    return -1;
	}

	AVCodec* videoCodec = avcodec_find_encoder(av_context_->oformat->video_codec);
    if (videoCodec == NULL) {
      ELOG_ERROR("Could not find video codec");
      return -1;
    }

    AVStream *video_stream_;
    video_stream_ = avformat_new_stream(av_context_, videoCodec);

   	video_stream_->time_base = (AVRational) { 1, 30 };
    
    video_stream_->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    video_stream_->codecpar->codec_id = av_context_->oformat->video_codec;
    video_stream_->codecpar->width = 640;
    video_stream_->codecpar->height = 480;
    video_stream_->codecpar->format = AV_PIX_FMT_YUV420P;

    av_context_->streams[1] = video_stream_;
    
    res = avformat_write_header(av_context_, NULL);

    if (res < 0) {
	    ELOG_ERROR("Error writing header");
	    return -1;
	}
 

    return 0;

}

void OutputWriter::receivePacket(AVPacket& packet, AVMediaType type) {

	ELOG_DEBUG("Received packet %d", type);

	if (type == AVMEDIA_TYPE_VIDEO)
		av_interleaved_write_frame(av_context_, &packet);

}



}	// Namespace payloader