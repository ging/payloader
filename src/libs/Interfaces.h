#ifndef INTERFACES_H_
#define INTERFACES_H_

extern "C" {
	#include <libavformat/avformat.h>
}

namespace payloader {

class FrameReceiver {
public:
	virtual void receiveFrame(AVFrame* frame, AVMediaType type) = 0;
	virtual ~FrameReceiver() {}
};


class RtpReceiver {
public:
	virtual void receiveRtpPacket(unsigned char* inBuff, int buffSize) = 0;
	virtual void sendPacket(AVPacket* pkt) = 0;
	virtual ~RtpReceiver() {}
};


class PacketReceiver {
public:
	virtual void receivePacket(AVPacket& packet, AVMediaType type) = 0;
	virtual int init(AVCodecContext *pCodecCtx) = 0;
	virtual void setSink(FrameReceiver* receiver) = 0;
	virtual void setSink(RtpReceiver* receiver) = 0;
	virtual void sendPacket(AVPacket *pkt) = 0;
	//virtual void setSink(PacketReceiver* receiver) = 0;
	virtual ~PacketReceiver() {}
};

}	// Namespace payloader

#endif // INTERFACES_H_ 