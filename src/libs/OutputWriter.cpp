#include "OutputWriter.h"
#include <sys/time.h>
#include "opencv2/opencv.hpp"
#include <iostream>

namespace payloader {

DEFINE_LOGGER(OutputWriter, "OutputWriter");

OutputWriter::OutputWriter(const std::string& url) : output_url_(url) {
	av_context_ = NULL;

}

OutputWriter::~OutputWriter() {

  if (av_context_ != NULL) {
      av_write_trailer(av_context_);
  }

  if (av_context_ != NULL) {
      avio_close(av_context_->pb);
      avformat_free_context(av_context_);
      av_context_ = NULL;
  }
}
int OutputWriter::init(AVCodecContext *pCodecCtx) {

  av_register_all();
  avcodec_register_all();

  av_context_ = avformat_alloc_context();
  if (av_context_ == NULL) {
    ELOG_ERROR("Error allocating memory for IO context");
    return -1;
  }

  output_url_.copy(av_context_->filename, sizeof(av_context_->filename), 0);

  av_context_->oformat = av_guess_format(NULL,  av_context_->filename, NULL);
  if (!av_context_->oformat) {
    ELOG_ERROR("Error guessing format %s", av_context_->filename);
    return -1;
  }

  ELOG_DEBUG("Output format %s", av_context_->oformat->long_name);
  
  int res = avio_open2(&av_context_->pb, av_context_->filename, AVIO_FLAG_WRITE, NULL, NULL);
  if (res < 0) {
     ELOG_ERROR("Error opening output file");
     return -1;
  }


    av_context_->oformat->video_codec = AV_CODEC_ID_MPEG4;

    // Video track
    AVCodec* videoCodec = avcodec_find_encoder(av_context_->oformat->video_codec);
    if (videoCodec == NULL) {
      ELOG_ERROR("Could not find video codec");
      return -1;
    }

    video_stream_ = avformat_new_stream(av_context_, videoCodec);
    video_stream_->id = 0;
    video_stream_->time_base = (AVRational) { 1, 25 };
    video_stream_->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    video_stream_->codecpar->codec_id = av_context_->oformat->video_codec;
    video_stream_->codecpar->width = pCodecCtx->width;
    video_stream_->codecpar->height = pCodecCtx->height;
    video_stream_->codecpar->format = AV_PIX_FMT_YUV420P;

    av_context_->streams[0] = video_stream_;
  
  res = avformat_write_header(av_context_, NULL);
  if (res < 0) {
    ELOG_ERROR("Error writing header");
    return -1;
  }

  one=0;// solo habrimos el archivo una vez 


  return 0;
}

int OutputWriter::init(AudioCodecInfo audioInfo, VideoCodecInfo videoInfo){
 //  i=0;
	// av_register_all();
	// avcodec_register_all();

	// av_context_ = avformat_alloc_context();
	// if (av_context_ == NULL) {
 //  	ELOG_ERROR("Error allocating memory for IO context");
 //  	return -1;
 //  }

	// output_url_.copy(av_context_->filename, sizeof(av_context_->filename), 0);
	// av_context_->oformat = av_guess_format(NULL,  av_context_->filename, NULL);
 //  if (!av_context_->oformat) {
 //   	ELOG_ERROR("Error guessing format %s", av_context_->filename);
 //   	return -1;
 //  }

 //  ELOG_DEBUG("Output format %s", av_context_->oformat->long_name);
	
 //  int res = avio_open2(&av_context_->pb, av_context_->filename, AVIO_FLAG_WRITE, NULL, NULL);
 //  if (res < 0) {
	//    ELOG_ERROR("Error opening output file");
	//    return -1;
	// }
    
 //  if (videoInfo.enabled) {

 //    av_context_->oformat->video_codec = videoInfo.codec;

 //  	// Video track
 //  	AVCodec* videoCodec = avcodec_find_encoder(av_context_->oformat->video_codec);
 //    if (videoCodec == NULL) {
 //      ELOG_ERROR("Could not find video codec");
 //      return -1;
 //    }

 //    video_stream_ = avformat_new_stream(av_context_, videoCodec);
 //    video_stream_->id = 0;
 //   	video_stream_->time_base = (AVRational) { 1, 25 };
 //    video_stream_->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
 //    video_stream_->codecpar->codec_id = av_context_->oformat->video_codec;
 //    video_stream_->codecpar->width = videoInfo.width;
 //    video_stream_->codecpar->height = videoInfo.height;
 //    video_stream_->codecpar->format = AV_PIX_FMT_YUV420P;

 //    av_context_->streams[0] = video_stream_;
 //  }

 //  // if (audioInfo.enabled) {

 //  //   av_context_->oformat->audio_codec = audioInfo.codec;

 //  //   AVCodec* audioCodec = avcodec_find_encoder(av_context_->oformat->audio_codec);
 //  //   if (audioCodec == NULL) {
 //  //       ELOG_ERROR("Could not find audio codec");
 //  //       return -1;
 //  //   }

 //  //   audio_stream_ = avformat_new_stream(av_context_, audioCodec);
 //  //   audio_stream_->id = 1;
 //  //   audio_stream_->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
 //  //   audio_stream_->codecpar->codec_id = av_context_->oformat->audio_codec;
 //  //   audio_stream_->codecpar->sample_rate = av_context_->oformat->audio_codec == AV_CODEC_ID_PCM_MULAW ? 8000 : 48000;
 //  //   audio_stream_->time_base = (AVRational) { 1, audio_stream_->codecpar->sample_rate};
 //  //   audio_stream_->codecpar->channels = av_context_->oformat->audio_codec == AV_CODEC_ID_PCM_MULAW ? 1 : 2;

 //  //   av_context_->streams[1] = audio_stream_;
 //  // }

 //  res = avformat_write_header(av_context_, NULL);

 //  if (res < 0) {
 //    ELOG_ERROR("Error writing header");
 //    return -1;
 //  }
  
  return 0;

}

void OutputWriter::receivePacket(AVPacket& packet, AVMediaType type) {

	ELOG_DEBUG("Stream video %d", av_context_->streams[0]->id);
	ELOG_DEBUG("Stream audio %d", av_context_->streams[1]->id);

	ELOG_DEBUG("Received packet pts: %ld, dts: %ld,  index %d, type %d", packet.pts, packet.dts, packet.stream_index, type);
	if (type == AVMEDIA_TYPE_VIDEO)
		packet.stream_index = 0;
	if (type == AVMEDIA_TYPE_AUDIO)
		packet.stream_index = 1;
	
	av_interleaved_write_frame(av_context_, &packet);
	
}

void OutputWriter::receiveFrame(AVFrame* frame, AVMediaType type) {

  if (type == AVMEDIA_TYPE_VIDEO) {

    ELOG_DEBUG("Received frame");

    int w = frame->width;
    int h = frame->height;
    int pix_fmt = AV_PIX_FMT_YUV420P;

    AVFrame *frameRGB;
    frameRGB = av_frame_alloc();
    uint8_t *buffer;
    int numBytes;

    AVPixelFormat pFormat = AV_PIX_FMT_BGR24;
    numBytes = avpicture_get_size(pFormat, w, h) ;
    buffer = (uint8_t *) av_malloc(numBytes*sizeof(uint8_t));
    avpicture_fill((AVPicture *) frameRGB, buffer, pFormat, w, h);

    struct SwsContext * img_convert_ctx;
    img_convert_ctx = sws_getCachedContext(NULL, w, h, (AVPixelFormat)pix_fmt, w, h, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL,NULL);
    sws_scale(img_convert_ctx, ((AVPicture*)frame)->data, ((AVPicture*)frame)->linesize, 0, h, ((AVPicture *)frameRGB)->data, ((AVPicture *)frameRGB)->linesize);

    //OpenCV
   
    // cv::imshow("display.png",img);
    // cv::imwrite("display.png",img);
    // cv::waitKey(1);
    

    if(one==0){ //En principio hecho con un do-while

       output.open ( "outputVideo.avi", CV_FOURCC('D','I','V','X'), 10, cv::Size ( w,h), true );
       one++;
       ELOG_DEBUG("hecho %d",one);
     } 
 
    cv::Mat img(frame->height, frame->width, CV_8UC3, frameRGB->data[0] ); 
    output.write ( img );
  
  } else if (type == AVMEDIA_TYPE_AUDIO) {

   }
}

  void OutputWriter::saveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
  // FILE *pFile;
  // char szFilename[32]="prueba";
  int  y;
  
  // // Open file
  // sprintf(szFilename, "frame%d.ppm", iFrame);
  // pFile=fopen(szFilename, "wb");
  // if(pFile==NULL)
  //   return;
  
  // // Write header
  // fprintf(av_context_->filename, "P6\n%d %d\n255\n", width, height);
  
  // Write pixel data
  //for(y=0; y<height; y++)
  //fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, av_context_->pb);
  
  // Close file
  //fclose(av_context_->pb);
}
        


void OutputWriter::setSink(FrameReceiver* receiver) {
  sink_ = receiver;
  return;
}

}	// Namespace payloader