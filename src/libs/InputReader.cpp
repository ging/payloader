#include "InputReader.h"
#include <sys/time.h>

namespace payloader {

DEFINE_LOGGER(InputReader, "InputReader");

InputReader::InputReader(const std::string& url) : input_url_(url) {
    ELOG_DEBUG("Creating source reader to %s", url.c_str());
    av_context_ = NULL;
    reading_ = false;
    sink_ = NULL;
}

InputReader::~InputReader() {
	deliver_thread_.join();
	avformat_close_input(&av_context_);
}

int InputReader::init(){

	av_register_all();
	av_context_ = avformat_alloc_context();
	
	char errbuff[500];

	ELOG_DEBUG("Opening source %s", input_url_.c_str());
	int res = avformat_open_input(&av_context_, input_url_.c_str(), NULL, NULL);
	ELOG_DEBUG("Opening source result %d", res);
	if(res != 0){
		av_strerror(res, (char*)(&errbuff), 500);
		ELOG_ERROR("Error opening source %s", errbuff);
		return res;
    }

    ELOG_DEBUG("Finding stream info");
    res = avformat_find_stream_info(av_context_,NULL);
    ELOG_DEBUG("Finding stream info result %d", res);
    if(res < 0){
      av_strerror(res, (char*)(&errbuff), 500);
      ELOG_ERROR("Error finding stream info %s", errbuff);
      return res;
    }

    video_stream_index = av_find_best_stream(av_context_, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
  	if (video_stream_index < 0)
    	ELOG_WARN("No Video stream found");

    audio_stream_index = av_find_best_stream(av_context_, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
  	if (audio_stream_index < 0)
    	ELOG_WARN("No Audio stream found");

    ELOG_DEBUG("Video stream index %d, Audio Stream index %d", video_stream_index, audio_stream_index);

    deliver_thread_ = boost::thread(&InputReader::deliverLoop, this);

    this->startReading();

    return true;

}

void InputReader::setSink(PacketReceiver* receiver) {
	sink_ = receiver;
}


void InputReader::startReading() {

	AVPacket avpacket_;
	avpacket_.data = NULL;

	reading_ = true;

	while (av_read_frame(av_context_, &avpacket_) >= 0) {

		AVBufferRef buff;
		avpacket_.buf = &buff;
		ELOG_DEBUG("Readed packet %d", avpacket_.size);

		queue_mutex_.lock();
        packet_queue_.push(avpacket_);
        queue_mutex_.unlock();

	}

	ELOG_DEBUG("Ended source reading");
	reading_ = false;
	av_packet_unref(&avpacket_);
}

void InputReader::deliverLoop() {
	while (reading_ == true) {
		queue_mutex_.lock();
		if (packet_queue_.size() > 0) {
			ELOG_DEBUG("Delivering packet %d", packet_queue_.front().size);

			AVMediaType type = AVMEDIA_TYPE_UNKNOWN;
			if (packet_queue_.front().stream_index == video_stream_index)
				type = AVMEDIA_TYPE_VIDEO;
			else if (packet_queue_.front().stream_index == audio_stream_index)
				type = AVMEDIA_TYPE_AUDIO;

		  	if (sink_ != NULL) {
				sink_->receivePacket(packet_queue_.front(), type);
			}

		  	packet_queue_.pop();
		  	queue_mutex_.unlock();
		} else {
		  	queue_mutex_.unlock();
		  	usleep(DELIVERING_INTERVAL);
		}
	}
}

}	// Namespace payloader