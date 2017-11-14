/*Clase innecesaria por tener ya el InputReader*/


void init( AVFormatContext *outContext, const char url ){

    //  const char *url = "rtsp://rtsp://138.4.13.92:8554/ej";
    // //const char *url = "rtsp://192.168.33.19:1935/ffmpeg/0";

}



static int open_video(AVFormatContext *oc, AVCodec *codec, AVStream *st)
{
    int ret;
    AVCodecContext *c = st->codec;

    /* open the codec */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Could not open video codec.\n", avcodec_get_name(c->codec_id));
    }
    else {

        /* allocate and init a re-usable frame */
        frame = av_frame_alloc();//Inicia la trama
        if (!frame) {
            av_log(NULL, AV_LOG_ERROR, "Could not allocate video frame.\n");
            ret = -1;
        }
        else {
            frame->format = c->pix_fmt;
            frame->width = c->width;
            frame->height = c->height;

            /* Allocate the encoded raw picture. */
            ret = avpicture_alloc(&dst_picture, c->pix_fmt, c->width, c->height);//Allocate memory for the pixels of a picture and setup the AVPicture fields for it.
            if (ret < 0) {                                                       //Para asignar un buffer de imagen y completar los datos de imagen en una llamada, use avpicture_alloc ().
                av_log(NULL, AV_LOG_ERROR, "Could not allocate picture.\n");
            }
            else {
                /* copy data and linesize picture pointers to frame */
                *((AVPicture *)frame) = dst_picture; // coipa la estructura del picture en un frame
            }
        }
    }

    return ret;
}