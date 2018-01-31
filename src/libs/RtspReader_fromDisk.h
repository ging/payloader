#ifndef RTSPREADER_FROMDISK_H
#define RTSPREADER_FROMDISK_H

#include "Interfaces.h"

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#include "logger.h"
#include <queue>

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/avconfig.h>
	#include <libavdevice/avdevice.h>
}


namespace payloader {

#define DELIVERING_INTERVAL 1000

class RtspReader_fromDisk {
    DECLARE_LOGGER();
	public:
	    RtspReader_fromDisk(const std::string& url, const char *device);
	    virtual ~RtspReader_fromDisk();
	    int init();
	    void setSink( RtpReceiver* receiver);

	private:
		AVFormatContext *ofmt_ctx;
	    AVFormatContext *ifmt_ctx;
	   	AVCodecContext *pCodecCtx;
	    std::string input_url_;
	    const char *input_device_;
	    std::queue<AVPacket> packet_queue_;
	    bool reading_;
		RtpReceiver* sink_;
	    int video_stream_index_;
	    int audio_stream_index_;


	    boost::mutex queue_mutex_;
		boost::thread deliver_thread_;

	    void startReading();
	    void deliverLoop();
};
}	// Namespace payloader
#endif // RTSPREADER_FROMDISK_H