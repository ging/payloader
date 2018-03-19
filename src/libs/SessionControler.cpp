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
boost::array<char, 256> buf;

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
 RTSP_CMD_TYPES m_RtspCmdType;  
       // global session state parameters
      int            m_RtspSessionID;
      int         m_RtspClient;                              // RTSP socket of that session
      int            m_StreamID;                                // number of simulated stream of that session
      u_short        m_ClientRTPPort;                           // client port for UDP based RTP transport
      u_short        m_ClientRTCPPort;                          // client port for UDP based RTCP transport  
      bool           m_TcpTransport;                            // if Tcp based streaming was activated
     // CStreamer    * m_Streamer;                                // the UDP or TCP streamer of that session

      // parameters of the last received RTSP request

                                // command type (if any) of the current request
      char           m_URLPreSuffix[RTSP_PARAM_STRING_MAX];     // stream name pre suffix 
      char           m_URLSuffix[RTSP_PARAM_STRING_MAX];        // stream name suffix
      char           m_CSeq[RTSP_PARAM_STRING_MAX];             // RTSP command sequence number
      char           m_URLHostPort[RTSP_BUFFER_SIZE];           // host:port part of the URL
      unsigned       m_ContentLength;      
      char   response[1024];                     // SDP string size
      // We use a boost::array to hold the received data. 
  tcp_connection(boost::asio::io_service& io_service): socket_(io_service){


  }
  // Call boost::asio::async_write() to serve the data to the client. 
  // We are using boost::asio::async_write(), 
  // rather than ip::tcp::socket::async_write_some(), 
  // to ensure that the entire block of data is sent.
  void start(){
 
  
  printf("LEO\n");

  socket_.async_read_some(boost::asio::buffer(buf),
      boost::bind(&tcp_connection::handle_read, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

  /*boost::asio::async_read(socket_,
        boost::asio::buffer(buf, 64),
         boost::bind(&tcp_connection::handle_read, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));*/
    // writing the message for current time
      //boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
      boost::system::error_code error;
   
      
      //ESCUCHAR
   
      // The boost::asio::buffer() function automatically determines 
      // the size of the array to help prevent buffer overruns.
     // size_t len =socket.read_some(boost::asio::buffer(buf), error);

   // printf("RECIBIMOS: \n");

    
   
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

}
private:

  // handle_write() is responsible for any further actions 
  // for this client connection.
  void handle_write(const boost::system::error_code& /*error*/,
      size_t /*bytes_transferred*/){
  }
   void handle_read(const boost::system::error_code& error,
      size_t /*bytes_transferred*/){
     data = buf.data();
     size_t len = strlen(data);
    // printf("RECIBO: \n\n");
  //Para escribir por pantalla el buffer
  //int res = Handle_RtspRequest(data, len);
 
    //printf("%c\n", data[0]);
   
      //std::cout.write(data, len);//buscar len
      RTSP_CMD_TYPES C = Handle_RtspRequest(data,len);
                    
    
     
 socket_.async_read_some(boost::asio::buffer(buf),
      boost::bind(&tcp_connection::handle_read, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

  }
  RTSP_CMD_TYPES Handle_RtspRequest(char const * aRequest, unsigned aRequestSize){
  printf("\nDentro \n");
          if (ParseRtspRequest(aRequest,aRequestSize))
          {
            printf("%d\n", m_RtspCmdType);
              switch (m_RtspCmdType)
              {
                  case RTSP_OPTIONS:  { Handle_RtspOPTION();   break; };
                  case RTSP_DESCRIBE: { Handle_RtspDESCRIBE();printf("DESCRIBE!!!!!!!!!!!!!!!!!!!!!!!!");break; };
                  /*case RTSP_SETUP:    { Handle_RtspSETUP();    break; };
                  case RTSP_PLAY:     { Handle_RtspPLAY();     break; };*/
                  default: {};
              };
          };
          printf("fuera\n\n");
          return m_RtspCmdType;
};
char const * DateHeader() 
{
    char buf[200];    
    time_t tt = time(NULL);
    strftime(buf, sizeof buf, "Date: %a, %b %d %Y %H:%M:%S GMT", gmtime(&tt));
    return buf;
}

void Handle_RtspOPTION()
{
        printf("haciendo options\n");   
        snprintf(response,sizeof(response),
        "RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
        "Public: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n\r\n",m_CSeq);

        boost::asio::async_write(socket_, boost::asio::buffer(response),
        boost::bind(&tcp_connection::handle_write, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    //writeResponse();
}

void Handle_RtspDESCRIBE()
{
    printf("haciendo descibe\n");   
    char   SDPBuf[1024];
    char   URLBuf[1024];

    // check whether we know a stream with the URL which is requested
    m_StreamID = -1;        // invalid URL
    if ((strcmp(m_URLPreSuffix,"mjpeg") == 0) && (strcmp(m_URLSuffix,"1") == 0)) m_StreamID = 0; else
    if ((strcmp(m_URLPreSuffix,"mjpeg") == 0) && (strcmp(m_URLSuffix,"2") == 0)) m_StreamID = 1;
    if (m_StreamID == -1)
    {   // Stream not available
        snprintf(response,sizeof(response),
            "RTSP/1.0 404 Stream Not Found\r\nCSeq: %s\r\n%s\r\n",
            m_CSeq, 
            DateHeader());
/*boost::asio::async_write(socket_, boost::asio::buffer(response),
        boost::bind(&tcp_connection::handle_write, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
        //send(m_RtspClient,response,strlen(response),0);   
        return;*/
    };

    // simulate DESCRIBE server response
    char OBuf[256];
    char * ColonPtr;
    strcpy(OBuf,m_URLHostPort);
    ColonPtr = strstr(OBuf,":");
    if (ColonPtr != nullptr) ColonPtr[0] = 0x00;

    snprintf(SDPBuf,sizeof(SDPBuf),
        "v=0\r\n"
        "o=- %d 1 IN IP4 %s\r\n"           
        "s=\r\n"
        "t=0 0\r\n"                                            // start / stop - 0 -> unbounded and permanent session
        "m=video 0 RTP/AVP 26\r\n"                             // currently we just handle UDP sessions
        "c=IN IP4 0.0.0.0\r\n",
        rand(),
        OBuf);
    char StreamName[64];
    switch (m_StreamID)
    {
        case 0: strcpy(StreamName,"mjpeg/1"); break;
        case 1: strcpy(StreamName,"mjpeg/2"); break;
    };
    snprintf(URLBuf,sizeof(URLBuf),
        "rtsp://%s/%s",
        m_URLHostPort,
        StreamName);
    snprintf(response,sizeof(response),
        "RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
        "%s\r\n"
        "Content-Base: %s/\r\n"
        "Content-Type: application/sdp\r\n"
        "Content-Length: %d\r\n\r\n"
        "%s",
        m_CSeq,
        DateHeader(),
        URLBuf,
        strlen(SDPBuf),
        SDPBuf);

    boost::asio::async_write(socket_, boost::asio::buffer(response),
        boost::bind(&tcp_connection::handle_write, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
 
};
bool ParseRtspRequest(char const * aRequest, unsigned aRequestSize){
    char     CmdName[RTSP_PARAM_STRING_MAX];
    char     CurRequest[RTSP_BUFFER_SIZE];
    unsigned CurRequestSize; 

    //init();
    CurRequestSize = aRequestSize;
    memcpy(CurRequest,aRequest,aRequestSize);

    // check whether the request contains information about the RTP/RTCP UDP client ports (SETUP command)
    char * ClientPortPtr;
    char * TmpPtr;
    char   CP[1024];
    char * pCP;
  
    ClientPortPtr = strstr(CurRequest,"client_port");
    if (ClientPortPtr != nullptr)
    {
        TmpPtr = strstr(ClientPortPtr,"\r\n");
        if (TmpPtr != nullptr) 
        {
            TmpPtr[0] = 0x00;
            strcpy(CP,ClientPortPtr);
            pCP = strstr(CP,"=");
            if (pCP != nullptr)
            {
                pCP++;
                strcpy(CP,pCP);
                pCP = strstr(CP,"-");
                if (pCP != nullptr)
                {
                    pCP[0] = 0x00;
                    m_ClientRTPPort  = atoi(CP);
                    m_ClientRTCPPort = m_ClientRTPPort + 1;
                };
            };
        };
    };

    // Read everything up to the first space as the command name
    bool parseSucceeded = false;
    unsigned i;
    for (i = 0; i < sizeof(CmdName)-1 && i < CurRequestSize; ++i) 
    {
        char c = CurRequest[i];
        if (c == ' ' || c == '\t') 
        {
            parseSucceeded = true;
            break;
        }
        CmdName[i] = c;
    }
    CmdName[i] = '\0';
    if (!parseSucceeded) return false;

    // find out the command type
    if (strstr(CmdName,"OPTIONS")   != nullptr){ m_RtspCmdType = RTSP_OPTIONS;}  else
    if (strstr(CmdName,"DESCRIBE")  != nullptr){ m_RtspCmdType = RTSP_DESCRIBE;} else
    if (strstr(CmdName,"SETUP")     != nullptr) m_RtspCmdType = RTSP_SETUP;    else
    if (strstr(CmdName,"PLAY")      != nullptr) m_RtspCmdType = RTSP_PLAY;     else
    if (strstr(CmdName,"TEARDOWN")  != nullptr) m_RtspCmdType = RTSP_TEARDOWN;
    
    // check whether the request contains transport information (UDP or TCP)
    if (m_RtspCmdType == RTSP_SETUP)
    {
        TmpPtr = strstr(CurRequest,"RTP/AVP/TCP");
        if (TmpPtr != nullptr) m_TcpTransport = true; else m_TcpTransport = false;
    };

    // Skip over the prefix of any "rtsp://" or "rtsp:/" URL that follows:
    unsigned j = i+1;
    while (j < CurRequestSize && (CurRequest[j] == ' ' || CurRequest[j] == '\t')) ++j; // skip over any additional white space
    for (; (int)j < (int)(CurRequestSize-8); ++j) 
    {
        if ((CurRequest[j]   == 'r' || CurRequest[j]   == 'R')   && 
            (CurRequest[j+1] == 't' || CurRequest[j+1] == 'T') && 
            (CurRequest[j+2] == 's' || CurRequest[j+2] == 'S') && 
            (CurRequest[j+3] == 'p' || CurRequest[j+3] == 'P') && 
             CurRequest[j+4] == ':' && CurRequest[j+5] == '/') 
        {
            j += 6;
            if (CurRequest[j] == '/') 
            {   // This is a "rtsp://" URL; skip over the host:port part that follows:
                ++j;
                unsigned uidx = 0;
                while (j < CurRequestSize && CurRequest[j] != '/' && CurRequest[j] != ' ') 
                {   // extract the host:port part of the URL here
                    m_URLHostPort[uidx] = CurRequest[j];
                    uidx++;
                    ++j;
                };
            } 
            else --j;
            i = j;
            break;
        }
    }

    // Look for the URL suffix (before the following "RTSP/"):
    parseSucceeded = false;
    for (unsigned k = i+1; (int)k < (int)(CurRequestSize-5); ++k) 
    {
        if (CurRequest[k]   == 'R'   && CurRequest[k+1] == 'T'   && 
            CurRequest[k+2] == 'S'   && CurRequest[k+3] == 'P'   && 
            CurRequest[k+4] == '/') 
        {
            while (--k >= i && CurRequest[k] == ' ') {}
            unsigned k1 = k;
            while (k1 > i && CurRequest[k1] != '/') --k1;
            if (k - k1 + 1 > sizeof(m_URLSuffix)) return false;
            unsigned n = 0, k2 = k1+1;

            while (k2 <= k) m_URLSuffix[n++] = CurRequest[k2++];
            m_URLSuffix[n] = '\0';

            if (k1 - i > sizeof(m_URLPreSuffix)) return false;
            n = 0; k2 = i + 1;
            while (k2 <= k1 - 1) m_URLPreSuffix[n++] = CurRequest[k2++];
            m_URLPreSuffix[n] = '\0';
            i = k + 7; 
            parseSucceeded = true;
            break;
        }
    }
    if (!parseSucceeded) return false;

    // Look for "CSeq:", skip whitespace, then read everything up to the next \r or \n as 'CSeq':
    parseSucceeded = false;
    for (j = i; (int)j < (int)(CurRequestSize-5); ++j) 
    {
        if (CurRequest[j]   == 'C' && CurRequest[j+1] == 'S' && 
            CurRequest[j+2] == 'e' && CurRequest[j+3] == 'q' && 
            CurRequest[j+4] == ':') 
        {
            j += 5;
            while (j < CurRequestSize && (CurRequest[j] ==  ' ' || CurRequest[j] == '\t')) ++j;
            unsigned n;
            for (n = 0; n < sizeof(m_CSeq)-1 && j < CurRequestSize; ++n,++j) 
            {
                char c = CurRequest[j];
                if (c == '\r' || c == '\n') 
                {
                    parseSucceeded = true;
                    break;
                }
                m_CSeq[n] = c;
            }
            m_CSeq[n] = '\0';
            break;
        }
    }
    if (!parseSucceeded) return false;

    // Also: Look for "Content-Length:" (optional)
    for (j = i; (int)j < (int)(CurRequestSize-15); ++j) 
    {
        if (CurRequest[j]    == 'C'  && CurRequest[j+1]  == 'o'  && 
            CurRequest[j+2]  == 'n'  && CurRequest[j+3]  == 't'  && 
            CurRequest[j+4]  == 'e'  && CurRequest[j+5]  == 'n'  && 
            CurRequest[j+6]  == 't'  && CurRequest[j+7]  == '-'  && 
            (CurRequest[j+8] == 'L' || CurRequest[j+8]   == 'l') &&
            CurRequest[j+9]  == 'e'  && CurRequest[j+10] == 'n' && 
            CurRequest[j+11] == 'g' && CurRequest[j+12]  == 't' && 
            CurRequest[j+13] == 'h' && CurRequest[j+14] == ':') 
        {
            j += 15;
            while (j < CurRequestSize && (CurRequest[j] ==  ' ' || CurRequest[j] == '\t')) ++j;
            unsigned num;
            if (sscanf(&CurRequest[j], "%u", &num) == 1) m_ContentLength = num;
        }
    }
    return true;
};

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
    }

    // Call start_accept() to initiate the next accept operation.
    start_accept();
  }

  tcp::acceptor acceptor_;
};