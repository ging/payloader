#ifndef RTSPREADER_FROMDISK_H
#define RTSPREADER_FROMDISK_H

#include "Interfaces.h"

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#include "logger.h"
#include <queue>
#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/avconfig.h>
	#include <libavdevice/avdevice.h>
	#include <libavutil/mathematics.h>
	#include <libavutil/time.h>
}


namespace payloader {

#define DELIVERING_INTERVAL 1000

class RtspReader_fromDisk {
    DECLARE_LOGGER();
	public:
	    RtspReader_fromDisk(const std::string& url, const std::string& url1, const char *device);
	    virtual ~RtspReader_fromDisk();
	    int init();
	    void setSink( RtpReceiver* receiver);
	    void socketReciver();
	    

	private:
		AVFormatContext *ofmt_ctx;
	    AVFormatContext *ifmt_ctx;
	   	AVCodecContext *pCodecCtx;
	   	AVOutputFormat *ofmt;
	    std::string input_url_;
	    std::string output_url_;
	    const char *input_device_;
	    
	    bool reading_;
		RtpReceiver *sink_;
	    int video_stream_index_;
	    int audio_stream_index_;
	    int ret;
	    int64_t start_time;
	    AVPacket avpacket_;

	    void startReading();
	    void deliverLoop();
};
}	// Namespace payloader
#endif // RTSPREADER_FROMDISK_H