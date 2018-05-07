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
#include <sys/time.h>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>




extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/avconfig.h>
	#include <libavdevice/avdevice.h>
	#include <libavutil/mathematics.h>
	#include <libavutil/time.h>
}

#define RTSP_BUFFER_SIZE       10000    // for incoming requests, and outgoing responses
#define RTSP_PARAM_STRING_MAX  200  

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
    	
  		boost::thread sendStart_Thread_;
  		long int PortGetter();
  		bool Done();
  		long int puerto;
  		
  		//boost::condition_variable cond_;
		char   SDPBuf[1024];
	    

	private:
		char   response[1024];
		void writeResponse();
		void PortSetter();
		void SenderStart();

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