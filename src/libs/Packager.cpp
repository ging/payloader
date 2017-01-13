#include "Packager.h"
#include "rtp/RtpFragmenter.h"
#include <sys/time.h>
#include "utils/Clock.h"
#include "utils/ClockUtils.h"
#include "rtp/RtpHeaders.h"

namespace payloader {

DEFINE_LOGGER(Packager, "Packager");

Packager::Packager() {
    sink_ = NULL;
}

Packager::~Packager() {
    free(rtpBuffer_); 
    rtpBuffer_ = NULL;
    free(outBuff_); 
    outBuff_ = NULL;
}

int Packager::init() {

    videoSeqNum_ = 0;

    rtpBuffer_ = (unsigned char*) malloc(PACKAGED_BUFFER_SIZE);
    outBuff_ = (unsigned char*) malloc(PACKAGED_BUFFER_SIZE);

    return true;

}

void Packager::setSink(RtpReceiver* receiver) {
	sink_ = receiver;
}

void Packager::receivePacket(AVPacket& packet, AVMediaType type) {

    if (type == AVMEDIA_TYPE_VIDEO) {

        ELOG_DEBUG("Received packet %d", packet.size);


        unsigned char* inBuff = packet.data;
        unsigned int buffSize = packet.size;
        int pts = packet.pts;


        RtpFragmenter frag(inBuff, buffSize);
        bool lastFrame = false;
        unsigned int outlen = 0;
        uint64_t millis = ClockUtils::timePointToMs(clock::now());
        // timestamp_ += 90000 / mediaInfo.videoCodec.frameRate;
        // int64_t pts = av_rescale(lastPts_, 1000000, (long int)video_time_base_);

        do {
            outlen = 0;
            frag.getPacket(outBuff_, &outlen, &lastFrame);
            RtpHeader rtpHeader;
            rtpHeader.setMarker(lastFrame?1:0);
            rtpHeader.setSeqNumber(videoSeqNum_++);
            if (pts == 0) {
                rtpHeader.setTimestamp(av_rescale(millis, 90000, 1000));
            } else {
                rtpHeader.setTimestamp(av_rescale(pts, 90000, 1000));
            }
            rtpHeader.setSSRC(55543);
            rtpHeader.setPayloadType(100);
            memcpy(rtpBuffer_, &rtpHeader, rtpHeader.getHeaderLength());
            memcpy(&rtpBuffer_[rtpHeader.getHeaderLength()], outBuff_, outlen);

            int l = outlen + rtpHeader.getHeaderLength();

            ELOG_DEBUG("Sending RTP fragment %d", l);


            if (sink_ != NULL) {
                sink_->receiveRtpPacket(rtpBuffer_, l);
            }

        } while (!lastFrame);
    
    } else if (type == AVMEDIA_TYPE_AUDIO) {
    


    }

}



}	// Namespace payloader