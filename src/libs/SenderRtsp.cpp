#include "SenderRtsp.h"
#include <sys/time.h>

namespace payloader {

DEFINE_LOGGER(SenderRtsp, "SenderRtsp");

SenderRtsp::SenderRtsp()  {
    ELOG_DEBUG("Creating source sender...");
    //ofmt_ctx = NULL;
}

SenderRtsp::~SenderRtsp() {
   /* if (ofmt_ctx != NULL) {
      av_write_trailer(ofmt_ctx);*/
 // }

  /*if (ofmt_ctx != NULL) {
      avio_close(ofmt_ctx->pb);
      avformat_free_context(ofmt_ctx);
      ofmt_ctx = NULL;
  }*/
}


int SenderRtsp::init(){

    av_log_set_level(AV_LOG_DEBUG);
    av_log_set_level(AV_LOG_TRACE);

    ELOG_DEBUG("starting sender...\n");

    av_register_all();
    avcodec_register_all();
    avformat_network_init();//RTP

    //output_url_.copy(ofmt_ctx->filename, sizeof(ofmt_ctx->filename), 0);
return 1;
}

void SenderRtsp::receiveRtpPacket(unsigned char* inBuff, int buffSize){
return;
}

//Pensar de si el origen del parametro es puntero o no 
void SenderRtsp::sendPacket(AVPacket pkt, int video_stream_index_, AVFormatContext *ifmt_ctx, AVFormatContext *ofmt_ctx, int64_t start_time){

    //ELOG_DEBUG("Press any key to start streaming...\n");
    //getchar();

    ELOG_DEBUG("Stream video %d", ofmt_ctx->streams[pkt.stream_index]->id);
    ELOG_DEBUG("Trying to send packet of %d bytes", pkt.size);

    //FIX：No PTS (Example: Raw H.264)
    //Simple Write PTS
    if(pkt.pts==AV_NOPTS_VALUE){
        //Write PTS
        AVRational time_base1=ifmt_ctx->streams[video_stream_index_]->time_base;
        //Duration between 2 frames (us)
            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(ifmt_ctx->streams[video_stream_index_]->r_frame_rate);
            //Parameters
            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
            pkt.dts=pkt.pts;
            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
        }
        //Important:Delay
        if(pkt.stream_index==video_stream_index_){
            AVRational time_base=ifmt_ctx->streams[video_stream_index_]->time_base;
            AVRational time_base_q={1,AV_TIME_BASE};
            int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
            int64_t now_time = av_gettime() - start_time;
            if (pts_time > now_time)
                av_usleep(pts_time - now_time);

    }

    in_stream  = ifmt_ctx->streams[pkt.stream_index];
    out_stream = ofmt_ctx->streams[pkt.stream_index];
    /* copy packet */
    //Convert PTS/DTS
    pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
    pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
    pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
    pkt.pos = -1;
    //Print to Screen
    if(pkt.stream_index==video_stream_index_){
        printf("Send %8d video frames to output URL\n",frame_index);
        frame_index++;
    }


    ELOG_DEBUG("Readed packet pts: %ld, dts: %ld,  index %d", pkt.pts, pkt.dts, pkt.stream_index);
    //ret = av_write_frame(ofmt_ctx, &pkt);
    ret = av_interleaved_write_frame(ofmt_ctx, &pkt);// Manda el paquete
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while muxing video packet.\n");
    }else{
         av_log(NULL, AV_LOG_DEBUG, "Todo ok.\n");
    }
    av_free_packet(&pkt);
}


}   // Namespace payloader

