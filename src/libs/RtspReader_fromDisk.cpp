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
    avformat_network_init();
    av_register_all();
    avdevice_register_all();
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
    if (video_stream_index_ < 0)
        ELOG_WARN("No Video stream found");

    av_dump_format(ifmt_ctx, 0, input_url_.c_str(), 0);

    //Output


    ret = avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", output_url_.c_str());
    if (!ofmt_ctx) {
        av_log(NULL, AV_LOG_FATAL, "Could not allocate output format context.\n");
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

    //Write file header
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        printf( "Error occurred when opening output URL\n");
    }


    //ELOG_DEBUG("Video stream index %d, Audio Stream index %d", video_stream_index_, audio_stream_index_);
     ELOG_DEBUG("Video stream index %d", video_stream_index_);

    this->startReading();

    return true;

}

void RtspReader_fromDisk::setSink(RtpReceiver* receiver) {
    sink_ = receiver;
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
            sink_->sendPacket(avpacket_, video_stream_index_, ifmt_ctx, ofmt_ctx, start_time);
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