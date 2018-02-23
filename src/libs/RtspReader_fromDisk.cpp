#include "RtspReader_fromDisk.h"
#include <sys/time.h>
#include "SenderRtsp.h"


/*Clase encargada de la lectura via rtsp*/
namespace payloader {

DEFINE_LOGGER(RtspReader_fromDisk, "RtspReader_fromDisk");

RtspReader_fromDisk::RtspReader_fromDisk(const std::string& url, const std::string& url1, const char *device) : input_url_(url),  output_url_(url1), input_device_(device) {
    ELOG_DEBUG("Creating source reader to %s", url.c_str());
    ifmt_ctx = NULL;
    reading_ = false;
    sink_ = NULL;
}

RtspReader_fromDisk::~RtspReader_fromDisk() {
    avformat_close_input(&ifmt_ctx);
}

int RtspReader_fromDisk::init(){
    av_register_all();
    //avcodec_register_all();
    avdevice_register_all();
    avformat_network_init();

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
   try
  {
     ELOG_DEBUG("Escuchando... en el 8854");

    // Any program that uses asio need to have at least one io_service object
    boost::asio::io_service io_service;

    // acceptor object needs to be created to listen for new connections, port 8854 and ipv4
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8854));

    for (;;)
    {
      // creates a socket
      tcp::socket socket(io_service);

      // wait and listen
      acceptor.accept(socket);

      // prepare message to send back to client
      std::string message = make_daytime_string();

      boost::system::error_code ignored_error;

      // writing the message for current time
      boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
    }
  }
  catch (std::exception& e)
  {
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

}   // Namespace payloader