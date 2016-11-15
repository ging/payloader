
/*
 * RtpSource.h
 *
 *  Created on: Aug 2, 2012
 *      Author: pedro
 */

#ifndef RTPSOURCE_H_
#define RTPSOURCE_H_

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include "../MediaDefinitions.h"
#include "../logger.h"

namespace erizo {

class RtpSource: public MediaSource {
	DECLARE_LOGGER();

public:
  RtpSource(const int mediaPort, MediaSink* mediaSink);
	virtual ~RtpSource();

private:

  static const int LENGTH = 1500;
  boost::scoped_ptr<boost::asio::ip::udp::socket> socket_;
  boost::scoped_ptr<boost::asio::ip::udp::resolver> resolver_;
  boost::scoped_ptr<boost::asio::ip::udp::resolver::query> query_;
	boost::asio::ip::udp::resolver::iterator iterator_;
  boost::asio::ip::udp::endpoint sender_endpoint;
	boost::asio::io_service io_service_;
  boost::thread rtpSource_thread_;
  char* buffer_[LENGTH];
  bool running_;
  void handleReceive(const::boost::system::error_code& error, 
    size_t bytes_recvd);
  void errorRec();
  void eventLoop();
};


} /* namespace erizo */
#endif /* RTPSINK_H_ */
