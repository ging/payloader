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




static int write_video_frame(AVFormatContext *oc, AVStream *st, int frameCount, AVPacket *pkt)
{
    int ret = 0;
    AVCodecContext *c = st->codec;

    /* encode the image */ //AQUI HACE LO QUE TIENE QUE HACER EL ENCODER pero lo hace el input reader
    frame->pts = frameCount;

    if (got_packet) {
        pkt.stream_index = st->index;
         pkt.pts = av_rescale_q_rnd(pkt.pts, c->time_base, st->time_base, AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        ret = av_write_frame(oc, &pkt);// Manda el frame
         if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error while writing video frame.\n");
         }
    }
    

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

    av_dump_format(outContext, 0, url, 1);

    ret = avformat_write_header(outContext, NULL);
    if (ret != 0) {
        av_log(NULL, AV_LOG_ERROR, "Failed to connect to RTSP server for '%s'.\n", url);
        goto end;
    }

  //  sendFrame(outContext,video_st);

    return;
}

void sendFrame(AVFormatContext outContext, AVStream video_st,  AVPacket *pkt){

    printf("Press any key to start streaming...\n");
    getchar();

    auto startSend = std::chrono::system_clock::now();

    while (pkt) {
        frameCount++;
        auto startFrame = std::chrono::system_clock::now();

        ret = write_video_frame(outContext, video_st, frameCount, *pkt);

        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Write video frame failed -- Send pkt failed.\n", url);
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

    // if (video_st) {
    //     avcodec_close(video_st->codec);
    //     av_free(src_picture.data[0]);
    //     av_free(dst_picture.data[0]);
    //     av_frame_free(&frame);
    // }

    avformat_free_context(outContext);

end:
    printf("finished.\n");

    getchar();
}



}   // Namespace payloader
