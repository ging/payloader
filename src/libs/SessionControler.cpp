#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp> 
#include "RtspReader_fromDisk.h"


using boost::asio::ip::tcp;

std::string make_daytime_string(){
  std::time_t now = std::time(0);
  return std::ctime(&now);
}
class tcp_connection
  // Using shared_ptr and enable_shared_from_this 
  // because we want to keep the tcp_connection object alive 
  // as long as there is an operation that refers to it.
  : public boost::enable_shared_from_this<tcp_connection>
{
public:
  char *data;
  typedef boost::shared_ptr<tcp_connection> pointer;

  static pointer create(boost::asio::io_service& io_service)
  {
    return pointer(new tcp_connection(io_service));
  }
  tcp::socket& socket()
  {
    return socket_;
  }

  // Call boost::asio::async_write() to serve the data to the client. 
  // We are using boost::asio::async_write(), 
  // rather than ip::tcp::socket::async_write_some(), 
  // to ensure that the entire block of data is sent.

  void start()
  {
      // We use a boost::array to hold the received data. 
  boost::array<char, 256> buf;

  boost::asio::async_read(socket_,
        boost::asio::buffer(buf),
         boost::bind(&tcp_connection::handle_read, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    // writing the message for current time
      //boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
      boost::system::error_code error;
   
      
      //ESCUCHAR
   
      // The boost::asio::buffer() function automatically determines 
      // the size of the array to help prevent buffer overruns.
     // size_t len =socket.read_some(boost::asio::buffer(buf), error);

   // printf("RECIBIMOS: \n");
     data = buf.data();
    //Para escribir por pantalla el buffer
    //std::cout.write(buf.data(), 64);//buscar len
  
    //printf("%d\n",Handle_RtspRequest(data,64));
    //m_message = response;

    // When initiating the asynchronous operation, 
    // and if using boost::bind(), 
    // we must specify only the arguments 
    // that match the handler's parameter list. 
    // In this code, both of the argument placeholders 
    // (boost::asio::placeholders::error 
    // and boost::asio::placeholders::bytes_transferred) 
    // could potentially have been removed, 
    // since they are not being used in handle_write().

   /* boost::asio::async_write(socket_, boost::asio::buffer(response),
        boost::bind(&tcp_connection::handle_write, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));*/
  }
private:
  tcp_connection(boost::asio::io_service& io_service)
    : socket_(io_service)
  {
  }
  // handle_write() is responsible for any further actions 
  // for this client connection.
  void handle_write(const boost::system::error_code& /*error*/,
      size_t /*bytes_transferred*/)
  {
  }
   void handle_read(const boost::system::error_code& /*error*/,
      size_t /*bytes_transferred*/)
  {
  }
  tcp::socket socket_;
  std::string m_message;
};






class tcp_server
{
public:
    char* data;
  // Constructor: initialises an acceptor to listen on TCP port 13.
  tcp_server(boost::asio::io_service& io_service)
    : acceptor_(io_service, tcp::endpoint(tcp::v4(), 8554))
  {
    // start_accept() creates a socket and 
    // initiates an asynchronous accept operation 
    // to wait for a new connection.
    start_accept();
  }
  char* getterData(){
    return data;
  }

private:
  void start_accept()
  {
    // creates a socket
    tcp_connection::pointer new_connection =
      tcp_connection::create(acceptor_.get_io_service());

    // initiates an asynchronous accept operation 
    // to wait for a new connection. 
    acceptor_.async_accept(new_connection->socket(),
        boost::bind(&tcp_server::handle_accept, this, new_connection,
          boost::asio::placeholders::error));
  }

  // handle_accept() is called when the asynchronous accept operation 
  // initiated by start_accept() finishes. It services the client request
  void handle_accept(tcp_connection::pointer new_connection,
      const boost::system::error_code& error)
  {
    if (!error)
    {   
      new_connection->start();
      data = new_connection->data;
    }

    // Call start_accept() to initiate the next accept operation.
    start_accept();
  }

  tcp::acceptor acceptor_;
};