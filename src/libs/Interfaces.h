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

class RawReceiver {
public:
	virtual void receiveRawPacket(AVPacket& packet) = 0;
	virtual ~RawReceiver() {}
};

}	// Namespace payloader

#endif // INTERFACES_H_ 