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

// supported command types
enum RTSP_CMD_TYPES
{
    RTSP_OPTIONS,
    RTSP_DESCRIBE,
    RTSP_SETUP,
    RTSP_PLAY,
    RTSP_TEARDOWN,
    RTSP_UNKNOWN
};

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
    	bool ParseRtspRequest(char const * aRequest, unsigned aRequestSize);
	    char const * DateHeader();
 		RTSP_CMD_TYPES Handle_RtspRequest(char const * aRequest, unsigned aRequestSize);
  		

	    

	private:

		char   response[1024];
		void writeResponse();
		void Handle_RtspOPTION();
		void Handle_RtspDESCRIBE();


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

		    // global session state parameters
	    int            m_RtspSessionID;
	    int         m_RtspClient;                              // RTSP socket of that session
	    int            m_StreamID;                                // number of simulated stream of that session
	    u_short        m_ClientRTPPort;                           // client port for UDP based RTP transport
	    u_short        m_ClientRTCPPort;                          // client port for UDP based RTCP transport  
	    bool           m_TcpTransport;                            // if Tcp based streaming was activated
	   // CStreamer    * m_Streamer;                                // the UDP or TCP streamer of that session

	    // parameters of the last received RTSP request

	    RTSP_CMD_TYPES m_RtspCmdType;                             // command type (if any) of the current request
	    char           m_URLPreSuffix[RTSP_PARAM_STRING_MAX];     // stream name pre suffix 
	    char           m_URLSuffix[RTSP_PARAM_STRING_MAX];        // stream name suffix
	    char           m_CSeq[RTSP_PARAM_STRING_MAX];             // RTSP command sequence number
	    char           m_URLHostPort[RTSP_BUFFER_SIZE];           // host:port part of the URL
	    unsigned       m_ContentLength;                           // SDP string size
};
}	// Namespace payloader
#endif // RTSPREADER_FROMDISK_H