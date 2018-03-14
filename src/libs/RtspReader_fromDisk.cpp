
//#include "RtspServerSession.h"
#include "RtspReader_fromDisk.h"

extern "C" {
    #include "rtspParser.c"
    #include <stdio.h>
};



/*Clase encargada de la lectura via rtsp*/
namespace payloader {

DEFINE_LOGGER(RtspReader_fromDisk, "RtspReader_fromDisk");
RtspReader_fromDisk::RtspReader_fromDisk(const std::string& url, const std::string& url1, const char *device) : input_url_(url),  output_url_(url1), input_device_(device) {
    ELOG_DEBUG("Creating source reader to %s", url.c_str());
    ifmt_ctx = NULL;
    reading_ = false;
    sink_ = NULL;

    init();

    m_RtspSessionID  = rand() << 16;         // create a session ID
    m_RtspSessionID |= rand();
    m_RtspSessionID |= 0x80000000;         
    m_StreamID       = -1;
    m_ClientRTPPort  =  0;
    m_ClientRTCPPort =  0;
    m_TcpTransport   =  false;
}


RtspReader_fromDisk::~RtspReader_fromDisk() {
    avformat_close_input(&ifmt_ctx);
}

int RtspReader_fromDisk::init(){
    av_register_all();
    //avcodec_register_all();
    avdevice_register_all();
    avformat_network_init();



    m_RtspCmdType   = RTSP_UNKNOWN;
    memset(m_URLPreSuffix, 0x00, sizeof(m_URLPreSuffix));
    memset(m_URLSuffix,    0x00, sizeof(m_URLSuffix));
    memset(m_CSeq,         0x00, sizeof(m_CSeq));
    memset(m_URLHostPort,  0x00, sizeof(m_URLHostPort));
    m_ContentLength  =  0;

    char errbuff[500];
    //ifmt_ctx = avformat_alloc_context();

    //Camara web...
   /* AVInputFormat *a = NULL;
    if (input_device_ != NULL) {
        a = av_find_input_format(input_device_);
    }*/

    ELOG_DEBUG("Opening source %s", input_url_.c_str());
    int res = avformat_open_input(&ifmt_ctx, input_url_.c_str(), 0, NULL);
    ELOG_DEBUG("Opening source result %d", res);
    if(res != 0){
        av_strerror(res, (char*)(&errbuff), 500);
        ELOG_ERROR("Error opening source %s", errbuff);
        return res;
    }

    ELOG_DEBUG("Finding stream info");
    res = avformat_find_stream_info(ifmt_ctx,NULL);
    ELOG_DEBUG("Finding stream info result %d", res);
    if(res < 0){
      av_strerror(res, (char*)(&errbuff), 500);
      ELOG_ERROR("Error finding stream info %s", errbuff);
      return res;
    }

    audio_stream_index_ = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audio_stream_index_ < 0)
        ELOG_WARN("No Audio stream found");
    
    video_stream_index_ = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (video_stream_index_ < 0){
        ELOG_WARN("No Video stream found");
    }else{
        ELOG_DEBUG("Video stream index %d", video_stream_index_);
    }

    av_dump_format(ifmt_ctx, 0, input_url_.c_str(), 0);

    //Para comprobar que codec tenemos 
    /*AVCodec * codec = NULL;
    while(codec = av_codec_next(codec))
    {
    ELOG_DEBUG("%s",codec->name);
    }*/

    //Output

    //Si squeremos pasar un flujo rtsp a archivo prueba.avi
   /* AVOutputFormat *omt2;
    omt2 = av_guess_format( NULL, "prueba.avi", NULL ); //Return the output format
    ELOG_DEBUG("Guess format %s", omt2);*/


    ret = avformat_alloc_output_context2(&ofmt_ctx, NULL, "rtsp" , output_url_.c_str());
    if (!ofmt_ctx) {
        av_log(NULL, AV_LOG_FATAL, "Could not allocate output format context for %s \n", output_url_.c_str());
        return -1;
    }else{
        ELOG_DEBUG("Allocated output format context" );
    }
       
    ofmt = ofmt_ctx -> oformat;
     if (!ofmt) {
        ELOG_ERROR("Error format %s", output_url_.c_str());
    }else{
        ELOG_DEBUG("Output format: %s",ofmt -> name );
    }
    int i;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
        if (!out_stream) {
            printf( "Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
        }
        //Copy the settings of AVCodecContext
        ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
        if (ret < 0) {
            printf( "Failed to copy context from input to output stream codec context\n");
        }
        out_stream->codec->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    av_dump_format(ofmt_ctx, 0, output_url_.c_str(), 1);


    /* open the output file, if needed */ 
    if (!(ofmt->flags & AVFMT_NOFILE))
        ret = avio_open(&ofmt_ctx->pb, output_url_.c_str(), AVIO_FLAG_WRITE);
    if (ret < 0) {
        av_log(NULL, AV_LOG_FATAL, "Could not open output_url_ file to write to it.\n");
    }else{
        ELOG_DEBUG("Opened output file.\n");
    }

    // Find the codec.
     AVCodec *codec;
     codec = avcodec_find_encoder(ofmt_ctx->oformat->video_codec);
     if (codec == NULL) {
         fprintf(stderr, "Codec not found\n");
         return -1;
     }else{
        ELOG_DEBUG("Codec encontrado %s.\n", codec->name);
     }

    //ff_rtsp_connect(ofmt_ctx);
//Intento de quitar el tcp fallido
    AVDictionary *d = NULL;
    av_dict_set(&d, "rtsp_transport", "udp", 0);

    //Write file header
   
   /* ret = avformat_write_header(ofmt_ctx,  &d);
    if (ret < 0) {
        printf( "Error occurred when opening output URL, ret = %d \n", ret);
    }
    */
    av_dict_free(&d);

    //ELOG_DEBUG("Video stream index %d, Audio Stream index %d", video_stream_index_, audio_stream_index_);
     ELOG_DEBUG("Video stream index %d", video_stream_index_);

    //this->startReading();
     this->socketReciver();

    return true;

}

void RtspReader_fromDisk::setSink(RtpReceiver* receiver) {
    sink_ = receiver;
}

using boost::asio::ip::tcp;
std::string make_daytime_string(){
  std::time_t now = std::time(0);
  return std::ctime(&now);
}
void RtspReader_fromDisk::socketReciver() {
  //Asignamos la memoria necesaria dado que sino sera aleatoria su posición y hacemos nun cast dado que en c++ no hay cast desde void*
  PRTSP_MESSAGE rtspStruct;
  rtspStruct = (PRTSP_MESSAGE)malloc(sizeof(RTSP_MESSAGE));
   try
  {
     ELOG_DEBUG("Escuchando... en el 8554");
   

    // Any program that uses asio need to have at least one io_service object
    boost::asio::io_service io_service;

    // acceptor object needs to be created to listen for new connections, port 8854 and ipv4
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8554));

    for (;;)
    {
      printf("socket abierto\n\n");
      // creates a socket
      tcp::socket socket(io_service);

      // wait and listen
      acceptor.accept(socket);

      // prepare message to send back to client
      //std::string message = make_daytime_string();
     
    
      
      // writing the message for current time
      //boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
      boost::system::error_code error;
   
      
      //ESCUCHAR
      // We use a boost::array to hold the received data. 
      boost::array<char, 256> buf;
   
      // The boost::asio::buffer() function automatically determines 
      // the size of the array to help prevent buffer overruns.
      size_t len = socket.read_some(boost::asio::buffer(buf), error);

      //Para escribir por pantalla el buffer
      printf("RECIBIMOS: \n");
      std::cout.write(buf.data(), len);

      char *data = buf.data();
    
      printf("%d\n",Handle_RtspRequest(data,len));

      boost::system::error_code ignored_error;
      std:string response1 = response;
      boost::asio::write(socket, boost::asio::buffer(response1), ignored_error); 
/*
      printf("Struct request o response creado y ahora a poner los parametros que queremos.\n");
      rtspStruct->type = TYPE_RESPONSE;
      rtspStruct->message.request.command =  "Public: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n\r\n";
      rtspStruct->message.response.statusCode = atoi("200");
      printf("CHANGED CODE: %d\n",  rtspStruct->message.response.statusCode);
      rtspStruct->message.response.statusString = "RTSP/1.0 200 OK";
      printf("CHANGED STRIG: %s\n",  rtspStruct->message.response.statusString); 
*/

      // When the server closes the connection, 
      // the ip::tcp::socket::read_some() function will exit with the boost::asio::error::eof error, 
      // which is how we know to exit the loop.
      /*if (error == boost::asio::error::eof)
        break; // Connection closed cleanly by peer.
      else if (error)
        throw boost::system::system_error(error); // Some other error.*/
    }
    //printf("Saliendo de fuera :%c\n",rtspStruct->type );
  }
  catch (std::exception& e)
  {
    printf("Saliengo de fuera, ALGO HA PETADO\n");
    std::cerr << e.what() << std::endl;
  }
}



void RtspReader_fromDisk::startReading() {
    
    start_time=av_gettime();
    avpacket_.data = NULL;
    sink_->init();
    reading_ = true;

    while (av_read_frame(ifmt_ctx, &avpacket_) >= 0) {        
        if (sink_ != NULL) {
            AVMediaType type = AVMEDIA_TYPE_UNKNOWN;
            if (avpacket_.stream_index == video_stream_index_)
                type = AVMEDIA_TYPE_VIDEO;
            else if (avpacket_.stream_index == audio_stream_index_)
                type = AVMEDIA_TYPE_AUDIO;
            //Estoy pasando algo que ya es puntero ifmt_ctx
            sink_->sendPacket(avpacket_, video_stream_index_, ifmt_ctx, ofmt_ctx, start_time, type);
        }
    }

    ELOG_DEBUG("Ended source reading");
    reading_ = false;
    av_packet_unref(&avpacket_);
}

void RtspReader_fromDisk::deliverLoop() {
    // while (reading_ == true) {
    //  queue_mutex_.lock();
    //  if (packet_queue_.size() > 0) {
    //      ELOG_DEBUG("Delivering packet %ld", packet_queue_.front().pts);

    //      AVMediaType type = AVMEDIA_TYPE_UNKNOWN;
    //      if (packet_queue_.front().stream_index == video_stream_index_)
    //          type = AVMEDIA_TYPE_VIDEO;
    //      else if (packet_queue_.front().stream_index == audio_stream_index_)
    //          type = AVMEDIA_TYPE_AUDIO;

    //      if (sink_ != NULL && packet_queue_.front().pts > 0) {
    //          sink_->receivePacket(packet_queue_.front(), type);
    //      }

    //      packet_queue_.pop();
    //      queue_mutex_.unlock();
    //  } else {
    //      queue_mutex_.unlock();
    //      usleep(DELIVERING_INTERVAL);
    //  }
    // }
}

      
  RTSP_CMD_TYPES RtspReader_fromDisk::Handle_RtspRequest(char const * aRequest, unsigned aRequestSize)
  {
          if (ParseRtspRequest(aRequest,aRequestSize))
          {
              switch (m_RtspCmdType)
              {
                  case RTSP_OPTIONS:  { Handle_RtspOPTION();printf("m_RtspCmdType options HANDLE: %d\n", m_RtspCmdType);   break; };
                  case RTSP_DESCRIBE: { Handle_RtspDESCRIBE();printf("m_RtspCmdType describe: %d\n", m_RtspCmdType); break; };
                  /*case RTSP_SETUP:    { Handle_RtspSETUP();    break; };
                  case RTSP_PLAY:     { Handle_RtspPLAY();     break; };*/
                  default: {};
              };
          };
          return m_RtspCmdType;
      };


bool RtspReader_fromDisk::ParseRtspRequest(char const * aRequest, unsigned aRequestSize)
{
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
    if (strstr(CmdName,"OPTIONS")   != nullptr){ m_RtspCmdType = RTSP_OPTIONS;printf("m_RtspCmdType options DENTRO: %d\n", m_RtspCmdType);}  else
    if (strstr(CmdName,"DESCRIBE")  != nullptr){ m_RtspCmdType = RTSP_DESCRIBE;printf("m_RtspCmdType describe: %d\n", m_RtspCmdType);} else
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
void RtspReader_fromDisk::writeResponse()
{
  printf("socket abierto enviando\n\n");
  printf("Contestamos al %d con: \n%s\n", m_RtspCmdType,response);
  /*boost::system::error_code ignored_error;
  std:string response1 = response;
  boost::asio::write(socket, boost::asio::buffer(response1), ignored_error); */
}

char const * RtspReader_fromDisk::DateHeader() 
{
    char buf[200];    
    time_t tt = time(NULL);
    strftime(buf, sizeof buf, "Date: %a, %b %d %Y %H:%M:%S GMT", gmtime(&tt));
    return buf;
}


void RtspReader_fromDisk::Handle_RtspOPTION()
{
    

    snprintf(response,sizeof(response),
        "RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
        "Public: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n\r\n",m_CSeq);
    //writeResponse();
};

void RtspReader_fromDisk::Handle_RtspDESCRIBE()
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

        send(m_RtspClient,response,strlen(response),0);   
        return;
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
 
}

}   // Namespace payloader