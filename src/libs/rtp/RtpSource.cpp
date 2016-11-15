
/*
 * RtpSource.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: pedro
 */

#include "RtpSource.h"
using boost::asio::ip::udp;

namespace erizo {
  DEFINE_LOGGER(RtpSource, "RtpSource");

  RtpSource::RtpSource(const int mediaPort, MediaSink* mediaSink){
    mediaSink_ = mediaSink;
    socket_.reset(new udp::socket(io_service_, 
          udp::endpoint(udp::v4(), 
            mediaPort)));
    resolver_.reset(new udp::resolver(io_service_));
    socket_->async_receive_from(boost::asio::buffer(buffer_, LENGTH), sender_endpoint, 
        boost::bind(&RtpSource::handleReceive, this, boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    
    io_service_.run();
    //rtpSource_thread_= boost::thread(&RtpSource::eventLoop,this);
  }

  RtpSource::~RtpSource() {
    ELOG_DEBUG("STOOOOOOOP ");
    io_service_.stop();
    rtpSource_thread_.join();

  }

  void RtpSource::handleReceive(const::boost::system::error_code& error, 
      size_t bytes_recvd) {

    if (error) {
      ELOG_DEBUG("ERROR ");
    }
    if (bytes_recvd>0&&this->mediaSink_){
      ELOG_DEBUG("Received data %d", (int)bytes_recvd);
      this->mediaSink_->deliverRtpData((char*)buffer_, (int)bytes_recvd);
      socket_->async_receive_from(boost::asio::buffer(buffer_, LENGTH), sender_endpoint, 
        boost::bind(&RtpSource::handleReceive, this, boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    }
  }
  void RtpSource::eventLoop() {
    io_service_.run();
    ELOG_DEBUG("STOOOOOOOP RUN");
  }

} /* namespace erizo */
