#ifndef OUTPUTWRITER_H_
#define OUTPUTWRITER_H_

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


class OutputWriter : public PacketReceiver {
    DECLARE_LOGGER();
	public:
	    OutputWriter(const std::string& url);
	    virtual ~OutputWriter();
	    int init();
	    void receivePacket(AVPacket& packet, AVMediaType type);

	private:
		AVFormatContext* av_context_;
		std::string output_url_;

};
}	// Namespace payloader
#endif // OUTPUTWRITER_H_