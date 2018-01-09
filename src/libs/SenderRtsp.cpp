#include "SenderRtsp.h"
#include <sys/time.h>

namespace payloader {

DEFINE_LOGGER(SenderRtsp, "SenderRtsp");

SenderRtsp::SenderRtsp(const std::string& url) : output_url_(url)  {
    ELOG_DEBUG("Creating source sender to %s", output_url_.c_str());
    outContext = NULL;
}

SenderRtsp::~SenderRtsp() {
    if (outContext != NULL) {
      av_write_trailer(outContext);
  }

  if (outContext != NULL) {
      avio_close(outContext->pb);
      avformat_free_context(outContext);
      outContext = NULL;
  }
}


int SenderRtsp::init(AVCodecContext *pCodecCtx){

    av_log_set_level(AV_LOG_DEBUG);
    av_log_set_level(AV_LOG_TRACE);

    ELOG_DEBUG("starting sender...\n");

    av_register_all();
    avcodec_register_all();
    avformat_network_init();//RTP

   // static const char* output_url_formats[] = { NULL, "mp3", "ogg", "avi", "rtsp" };

    
    // outContext = avformat_alloc_context();
    // if (outContext == NULL) {
    //     ELOG_ERROR("Error allocating memory for IO context");
    // return -1;   
    // }else{
    //      ELOG_DEBUG("Allocated an output_url_ context for %s .\n", output_url_.c_str());
    //    //  goto end;
    // }
    //output_url_.copy(outContext->filename, sizeof(outContext->filename), 0);

    AVOutputFormat* outputfmt = NULL;
    outputfmt = av_guess_format("rtsp",NULL, NULL);
     if (!outputfmt) {
        ELOG_ERROR("Error guessing format %s", output_url_.c_str());
    }

    outContext = avformat_alloc_context();

    ret = avformat_alloc_output_context2(&outContext, outputfmt, "rtsp", output_url_.c_str());
    if (!outContext) {
        av_log(NULL, AV_LOG_FATAL, "Could not allocate output format context.\n");
        return -1;
    }

   
    if (!outputfmt) {
        outContext->oformat->video_codec = AV_CODEC_ID_MPEG4;
        av_log(NULL, AV_LOG_FATAL, "Could not find file format of detached audio.\n");
        ELOG_DEBUG("Asignado formato AV_CODEC_ID_MPEG4 por defecto. ")
    
    }else{
        outContext->oformat = outputfmt;
        ELOG_DEBUG("Asignado formato encontrado por guess.")
    }

    /* open the output file, if needed */ 
    if (!(outputfmt->flags & AVFMT_NOFILE))
        ret = avio_open(&outContext->pb, output_url_.c_str(), AVIO_FLAG_WRITE);
         if (ret < 0) {
             av_log(NULL, AV_LOG_FATAL, "Could not open output_url_ file to write to it.\n");
    
            }else{
                 ELOG_DEBUG("Opened output file.\n");
            }

  

    // Video track
    AVCodec* videoCodec = avcodec_find_encoder(outContext->oformat->video_codec);
    if (videoCodec == NULL) {
      ELOG_ERROR("Could not find video codec");
      return -1;
    }

    video_stream_ = avformat_new_stream(outContext, videoCodec);
    video_stream_->id = 0;
    video_stream_->time_base = (AVRational) { 1, 25 };
    video_stream_->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    video_stream_->codecpar->codec_id = outContext->oformat->video_codec;
    video_stream_->codecpar->width = pCodecCtx->width;
    video_stream_->codecpar->height = pCodecCtx->height;
    video_stream_->codecpar->format = AV_PIX_FMT_YUV420P;

    outContext->streams[0] = video_stream_;
  
    ret = avformat_write_header(outContext, NULL);
    if (ret != 0) {
        av_log(NULL, AV_LOG_ERROR, "Failed to connect to RTSP server for %s.\n",output_url_.c_str());
        //goto end;
    }else{
         ELOG_DEBUG("Conected to RTSP server for '%s'.\n", output_url_.c_str());
    }

return 1;
}
void SenderRtsp::receiveRtpPacket(unsigned char* inBuff, int buffSize){
return;
}


void SenderRtsp::sendPacket(AVPacket& pkt){

    //ELOG_DEBUG("Press any key to start streaming...\n");
    //getchar();

    ELOG_DEBUG("Stream video %d", outContext->streams[0]->id);
    ELOG_DEBUG("Trying to send packet of %d bytes", pkt.size);


    ret = av_interleaved_write_frame(outContext, &pkt);// Manda el paquete
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while writing video packet.\n");
    }
}


}   // Namespace payloader

