#include "Sender.h"
#include <sys/time.h>
using boost::asio::ip::udp;

namespace payloader {

DEFINE_LOGGER(Sender, "Sender");

Sender::Sender( sockaddr_in RecvAddr, int m_RtpSocket) : RtpSocket(m_RtpSocket), RecvAdd(RecvAddr) {
    printf("Iniciando sender\n");
    /*resolver_.reset(new udp::resolver(io_service_));
    socket_.reset(new udp::socket(io_service_, udp::endpoint(udp::v4(), 6970)));
    query_.reset(new udp::resolver::query(udp::v4(), url.c_str(), port.c_str()));//abrimos socket udp, solo rtp sin rtsp por encima 
    iterator_ = resolver_->resolve(*query_);*/
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
        //printf("enviando desde sender loop al puerto: %s\n", iterator_);
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

/*char        RtpBuf[2048];
 int m_SequenceNumber = 0;
  int  m_Timestamp      = 0;
memset(RtpBuf,0x00,sizeof(RtpBuf));
    // Prepare the first 4 byte of the packet. This is the Rtp over Rtsp header in case of TCP based transport
    RtpBuf[0]  = '$';        // magic number
    RtpBuf[1]  = 0;          // number of multiplexed subchannel on RTPS connection - here the RTP channel
    RtpBuf[2]  = (48 & 0x0000FF00) >> 8;
    RtpBuf[3]  = (48 & 0x000000FF);
    // Prepare the 12 byte RTP header
    RtpBuf[4]  = 0x80;                               // RTP version
    RtpBuf[5]  = 0x9a;                               // JPEG payload (26) and marker bit
    RtpBuf[7]  = m_SequenceNumber & 0x0FF;           // each packet is counted with a sequence counter
    RtpBuf[6]  = m_SequenceNumber >> 8;
    RtpBuf[8]  = (m_Timestamp & 0xFF000000) >> 24;   // each image gets a timestamp
    RtpBuf[9]  = (m_Timestamp & 0x00FF0000) >> 16;
    RtpBuf[10] = (m_Timestamp & 0x0000FF00) >> 8;
    RtpBuf[11] = (m_Timestamp & 0x000000FF);
    RtpBuf[12] = 0x13;                               // 4 byte SSRC (sychronization source identifier)
    RtpBuf[13] = 0xf9;                               // we just an arbitrary number here to keep it simple
    RtpBuf[14] = 0x7e;
    RtpBuf[15] = 0x67;
    // Prepare the 8 byte payload JPEG header
    RtpBuf[16] = 0x00;                               // type specific
    RtpBuf[17] = 0x00;                               // 3 byte fragmentation offset for fragmented images
    RtpBuf[18] = 0x00;
    RtpBuf[19] = 0x00;
    RtpBuf[20] = 0x01;                               // type
    RtpBuf[21] = 0x5e;                               // quality scale factor
    RtpBuf[22] = 0x06;                           // width  / 8 -> 48 pixel
    RtpBuf[23] = 0x04;   

m_SequenceNumber++;                              // prepare the packet counter for the next packet
    m_Timestamp += 3600;    
*/

/*if (m_TCPTransport) // RTP over RTSP - we send the buffer + 4 byte additional header
    send(m_Client,buffer,len + 4,0);
else   */             // UDP - we send just the buffer by skipping the 4 byte RTP over RTSP header
    sendto(RtpSocket,&buffer[4],len,0,(sockaddr *) & RecvAdd,sizeof(RecvAdd));// mando el paquete por el socket rtp a la recvaddr

//socket_->send_to(boost::asio::buffer(buffer, len), *iterator_);
    return len;
}

}	// Namespace payloader