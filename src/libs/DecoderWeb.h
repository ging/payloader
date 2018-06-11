#ifndef DECODERWEB_H_
#define DECODERWEB_H_

#include "Interfaces.h"
#include "Codecs.h"

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#include "logger.h"
#include <queue>

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/avconfig.h>
}


namespace payloader {

class DecoderWeb : public PacketReceiver {
    DECLARE_LOGGER();
	public:
	    DecoderWeb();
	    virtual ~DecoderWeb();
	    int init(AVCodecContext *pCodecCtx);
	    int init(AudioCodecInfo audioInfo, VideoCodecInfo videoInfo);
	    void setSink(FrameReceiver* receiver);
	    void setSink(RtpReceiver* receiver);
	    void receivePacket(AVPacket& packet, AVMediaType type);
		void sendPacket(AVPacket *pkt);


	private:
	   	FrameReceiver* sink_;
	   //FrameReceiver* sink_1;
	   	AVCodec* vDecoder_;
	   	AVCodecContext* vDecoderContext_;
	   	AVCodecContext* vDecoderContext_orig;
	   	AVFrame* vFrame_;
	   	AVCodec* aDecoder_;
	   	AVCodecContext* aDecoderContext_;
	   	AVFrame* aFrame_;
};
}	// Namespace payloader
#endif // DECODER_WEB_H_