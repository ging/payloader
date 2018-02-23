#ifndef SENDER_RTSP_H_
#define SENDER_RTSP_H_

#include "Interfaces.h"

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

#include "logger.h"
#include <queue>

#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <thread>
//#include <tchar.h>

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/avconfig.h>
	#include <libavdevice/avdevice.h>
	#include <libavutil/mathematics.h>
	#include <libavutil/time.h>
}

// #pragma comment(lib,"libavformat/libavformat.a")
// #pragma comment(lib,"libavcodec/libavcodec.a")
// #pragma comment(lib,"libavutil/libavutil.a")
// #pragma comment(lib,"libswscale/libswscale.a")
// #pragma comment(lib,"x264.lib")
// #pragma comment(lib,"libswresample/libswresample.a")

using namespace std;


#define STREAM_DURATION   20
#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT   AV_PIX_FMT_YUV420P /* default pix_fmt */ //AV_PIX_FMT_NV12;
#define VIDEO_CODEC_ID CODEC_ID_H264

namespace payloader {

class SenderRtsp : public RtpReceiver {
    DECLARE_LOGGER();
	public:
	  	SenderRtsp();
	    virtual ~SenderRtsp();
	    int init();
	    void receiveRtpPacket(unsigned char* inBuff, int buffSize);
		void sendPacket(AVPacket packet, int video_stream_index_, AVFormatContext *ifmt_ctx, AVFormatContext *ofmt_ctx, int64_t start_time, AVMediaType type);
		//int write_video_frame(AVFormatContext *oc, int frameCount, AVPacket *pkt);

	private:
	
		    char data[1500];
		    int length;
		    
		   
	   		AVStream *in_stream, *out_stream;
		  
		    AVStream *video_st;
		    AVCodec *video_codec;
		    AVStream *video_stream_;
		    AVOutputFormat *outputfmt;


		    int ret, frameCount, frame_index;
		    std::string output_url_;
		    const char *url;
		    


			/* video output */
			static AVFrame *frame;
			static AVPicture src_picture, dst_picture;

			static int video_is_eof;	

	



};
}	// Namespace payloader
#endif // SENDER_RTSP_H_