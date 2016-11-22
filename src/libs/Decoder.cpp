#include "Decoder.h"
#include <sys/time.h>

namespace payloader {

DEFINE_LOGGER(Decoder, "Decoder");

Decoder::Decoder() {

}

Decoder::~Decoder() {

}

int Decoder::init(){



    return true;

}

void Decoder::setSink(RawReceiver* receiver) {
	sink_ = receiver;
}

void Decoder::receivePacket(AVPacket& packet, AVMediaType type) {

	ELOG_DEBUG("Received packet %d", packet.size);

}



}	// Namespace payloader