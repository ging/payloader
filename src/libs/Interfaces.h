#ifndef INTERFACES_H_
#define INTERFACES_H_

extern "C" {
	#include <libavformat/avformat.h>
}

namespace payloader {

class PacketReceiver {
public:
	virtual void receivePacket(AVPacket& packet, AVMediaType type) = 0;
	virtual ~PacketReceiver() {}
};

class FrameReceiver {
public:
	virtual void receiveFrame(AVFrame* frame, AVMediaType type) = 0;
	virtual ~FrameReceiver() {}
};

}	// Namespace payloader

#endif // INTERFACES_H_ 