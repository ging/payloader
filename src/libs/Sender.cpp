#include "Sender.h"
#include <sys/time.h>
using boost::asio::ip::udp;

namespace payloader {

DEFINE_LOGGER(Sender, "Sender");

Sender::Sender(const std::string url, const std::string port) {
    printf("PETO\n");
    resolver_.reset(new udp::resolver(io_service_));
    socket_.reset(new udp::socket(io_service_, udp::endpoint(udp::v4(), 0)));
    query_.reset(new udp::resolver::query(udp::v4(), url.c_str(), port.c_str()));
    iterator_ = resolver_->resolve(*query_);
}

Sender::~Sender() {
    sending_ = false;
    send_Thread_.join();
    io_service_.stop();
}
int Sender::init(const std::string& url, const std::string& port){
    return 0;
}

int Sender::init() {
    sending_ = true;
    send_Thread_ = boost::thread(&Sender::sendLoop, this);
    return true;
}
void Sender::sendPacket(AVPacket pkt, int video_stream_index_,  AVFormatContext *ifmt_ctx, AVFormatContext *ofmt_ctx, int64_t start_time, AVMediaType type){

}


void Sender::receiveRtpPacket(unsigned char* inBuff, int buffSize) {
    boost::mutex::scoped_lock lock(queueMutex_);
    if (sending_ == false)
        return;
    if (sendQueue_.size() < 1000) {
        dataPacket p_;
        memcpy(p_.data, inBuff, buffSize);
        p_.length = buffSize;
        sendQueue_.push(p_);
    }
    cond_.notify_one();
}

void Sender::sendLoop(){
    while (sending_ ) {
        boost::unique_lock<boost::mutex> lock(queueMutex_);
        while (sendQueue_.size() == 0) {
            cond_.wait(lock);
            if (!sending_) {
                return;
            }
        }
        this->sendData(sendQueue_.front().data, sendQueue_.front().length);
        sendQueue_.pop();
    }
}

int Sender::sendData(char* buffer, int len) {
    ELOG_DEBUG("Sending socket %d", len);

    socket_->send_to(boost::asio::buffer(buffer, len), *iterator_);
    return len;
}

}	// Namespace payloader