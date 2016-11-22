#ifndef DECODER_H_
#define DECODER_H_

#include "Interfaces.h"

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


class Decoder : public PacketReceiver {
    DECLARE_LOGGER();
	public:
	    Decoder();
	    virtual ~Decoder();
	    int init();
	    void setSink(RawReceiver* receiver);
	    void receivePacket(AVPacket& packet, AVMediaType type);

	private:
	   	RawReceiver* sink_;

};
}	// Namespace payloader
#endif // DECODER_H_