#ifndef SENDER_RTSP_H_
#define SENDER_RTSP_H_

#include "Interfaces.h"

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

#include "logger.h"
#include <queue>


#include <thread>
#include <tchar.h>

extern "C"
{
    #include <libavcodec\avcodec.h>
    #include <libavformat\avformat.h>
    #include <libavformat\avio.h>
    #include <libswscale\swscale.h>
    #include <libavutil\time.h>
}

#pragma comment(lib,"libavformat/libavformat.a")
#pragma comment(lib,"libavcodec/libavcodec.a")
#pragma comment(lib,"libavutil/libavutil.a")
#pragma comment(lib,"libswscale/libswscale.a")
#pragma comment(lib,"x264.lib")
#pragma comment(lib,"libswresample/libswresample.a")

//using namespace std;


#define STREAM_DURATION   20
#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT   AV_PIX_FMT_YUV420P /* default pix_fmt */ //AV_PIX_FMT_NV12;
#define VIDEO_CODEC_ID CODEC_ID_H264

namespace payloader {

class sender_rtsp : public RtpReceiver {
    DECLARE_LOGGER();
	public:
	    Sender(const std::string& url, const std::string& port);
	    virtual ~Sender();
	    int init();
	    void receiveRtpPacket(unsigned char* inBuff, int buffSize);

	private:
		struct dataPacket{
		    char data[1500];
		    int length;
		    
		    AVFormatContext *outContext;
		    AVStream *video_st;
		    AVCodec *video_codec;
		    int ret = 0, frameCount = 0;

			/* video output */
			static AVFrame *frame;
			static AVPicture src_picture, dst_picture;

			static int video_is_eof;	

		};

		boost::scoped_ptr<boost::asio::ip::udp::socket> socket_;
		boost::scoped_ptr<boost::asio::ip::udp::resolver> resolver_;

		boost::scoped_ptr<boost::asio::ip::udp::resolver::query> query_;
		boost::asio::ip::udp::resolver::iterator iterator_;
		boost::asio::io_service io_service_;

		boost::thread send_Thread_;
		boost::condition_variable cond_;
		boost::mutex queueMutex_;
		std::queue<dataPacket> sendQueue_;
		bool sending_;

		void sendLoop();
		int sendData(char* buffer, int len);


};
}	// Namespace payloader
#endif // SENDER_RTSP_H_