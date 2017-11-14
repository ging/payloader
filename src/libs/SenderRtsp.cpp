// Roughly based on: https://ffmpeg.org/doxygen/trunk/muxing_8c-source.html

#include <sender_rtsp.h>

namespace payloader {

DEFINE_LOGGER(sender_rtsp, "sender_rtsp");

// /* Add an output stream. */
// static AVStream *add_stream(AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id) // DEBEMOS HACER ESTO AQUI ¿ O ENCODER?
// {
//     AVCodecContext *c;
//     AVStream *st;

//     /* find the encoder */
//     *codec = avcodec_find_encoder(codec_id);
//     if (!(*codec)) {
//         av_log(NULL, AV_LOG_ERROR, "Could not find encoder for '%s'.\n", avcodec_get_name(codec_id));
//     }
//     else {
//         st = avformat_new_stream(oc, *codec);
//         if (!st) {
//             av_log(NULL, AV_LOG_ERROR, "Could not allocate stream.\n");
//         }
//         else {
//             st->id = oc->nb_streams - 1;//Queda un frame menos
//             st->time_base.den = st->pts.den = 90000;
//             st->time_base.num = st->pts.num = 1;

//             c = st->codec;
//             c->codec_id = codec_id;
//             c->bit_rate = 400000;
//             c->width = 352;
//             c->height = 288;
//             c->time_base.den = STREAM_FRAME_RATE;
//             c->time_base.num = 1;
//             c->gop_size = 12; /* emit one intra frame every twelve frames at most */
//             c->pix_fmt = STREAM_PIX_FMT;
//         }
//     }

//     return st;
// }


// /* Prepare   a dummy image. */
// static void fill_yuv_image(AVPicture *pict, int frame_index, int width, int height)
// {
//     int x, y, i;

//     i = frame_index;

//     /* Y */
//     for (y = 0; y < height; y++)
//         for (x = 0; x < width; x++)
//             pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;

//     /* Cb and Cr */
//     for (y = 0; y < height / 2; y++) {
//         for (x = 0; x < width / 2; x++) {
//             pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
//             pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
//         }
//     }
// }

static int write_video_frame(AVFormatContext *oc, AVStream *st, int frameCount, AVPacket *pkt)
{
    int ret = 0;
    AVCodecContext *c = st->codec;

   // fill_yuv_image(&dst_picture, frameCount, c->width, c->height);// coge una imagen ficticia y la mete en un picture

    //Aqui tengo que conseguir pasar los paquetes del input reader


    AVPacket pkt = { 0 };
    int got_packet;
    av_init_packet(&pkt);

    /* encode the image */ //AQUI HACE LO QUE TIENE QUE HACER EL ENCODER pero lo hace el input reader
    frame->pts = frameCount;
  /*  ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);// Escribe el frame en un paquete pero el input reader no nos da paquetes ya¿

    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error encoding video frame.\n");
    }
    else {*/
        if (got_packet) {
            pkt.stream_index = st->index;
            pkt.pts = av_rescale_q_rnd(pkt.pts, c->time_base, st->time_base, AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            ret = av_write_frame(oc, &pkt);// Manda el frame
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error while writing video frame.\n");
            }
        }
    //}

    return ret;// paquete con el frame
}

void init( AVFormatContext *outContext, const char url ){

    printf("starting...\n");

    const char *url = "rtsp://rtsp://138.4.13.92:8554/ej";
    //const char *url = "rtsp://192.168.33.19:1935/ffmpeg/0";


    av_log_set_level(AV_LOG_DEBUG);
    //av_log_set_level(AV_LOG_TRACE);

    av_register_all();
    avformat_network_init();

   avformat_alloc_output_context2(&outContext, NULL, "rtsp", url); //Asignamos contexto de SALIDA

    if (!outContext) {
        av_log(NULL, AV_LOG_FATAL, "Could not allocate an output context for '%s'.\n", url);
        goto end;
    }

    if (!outContext->oformat) {
        av_log(NULL, AV_LOG_FATAL, "Could not create the output format for '%s'.\n", url);
        goto end;
    }

    video_st = add_stream(outContext, &video_codec, VIDEO_CODEC_ID);// Trama vacía

    /* Now that all the parameters are set, we can open the video codec and allocate the necessary encode buffers. */
    if (video_st) {
        av_log(NULL, AV_LOG_DEBUG, "Video stream codec %s.\n ", avcodec_get_name(video_st->codec->codec_id));

        ret = open_video(outContext, video_codec, video_st); // Input reader
        if (ret < 0) {
            av_log(NULL, AV_LOG_FATAL, "Open video stream failed.\n");
            goto end;
        }
    }
    else {
        av_log(NULL, AV_LOG_FATAL, "Add video stream for the codec '%s' failed.\n", avcodec_get_name(VIDEO_CODEC_ID));
        goto end;
    }

    av_dump_format(outContext, 0, url, 1);

    ret = avformat_write_header(outContext, NULL);
    if (ret != 0) {
        av_log(NULL, AV_LOG_ERROR, "Failed to connect to RTSP server for '%s'.\n", url);
        goto end;
    }

  //  sendFrame(outContext,video_st);

    return;
}

void sendFrame(AVFormatContext outContext, AVStream video_st){

    printf("Press any key to start streaming...\n");
    getchar();

    auto startSend = std::chrono::system_clock::now();

    while (video_st) {
        frameCount++;
        auto startFrame = std::chrono::system_clock::now();

        ret = write_video_frame(outContext, video_st, frameCount);

        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Write video frame failed.\n", url);
            goto end;
        }

        auto streamDuration = std::chrono::duration_cast<chrono::milliseconds>(std::chrono::system_clock::now() - startSend).count();

        printf("Elapsed time %ldms, video stream pts %ld.\n", streamDuration, video_st->pts.val);

        if (streamDuration / 1000.0 > STREAM_DURATION) {
            break;
        }
        else {
            auto frameDuration = std::chrono::duration_cast<chrono::milliseconds>(std::chrono::system_clock::now() - startFrame).count();
            std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000.0 / STREAM_FRAME_RATE - frameDuration)));
        }
    }

    if (video_st) {
        avcodec_close(video_st->codec);
        av_free(src_picture.data[0]);
        av_free(dst_picture.data[0]);
        av_frame_free(&frame);
    }

    avformat_free_context(outContext);

end:
    printf("finished.\n");

    getchar();
}



}   // Namespace payloader
