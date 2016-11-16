#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>
#include <libavutil/time.h>

#include "libs/ExternalInput.h"
#include "libs/rtp/RtpSink.h"
#include "libs/rtp/RtpSource.h"
#include "libs/MediaProcessor.h"
#include "libs/ExternalOutput.h"

int main(int argc, const char* argv[]) {
  const char *url = "file:/home/alvaro/Documentos/arqueopterix/payloader_test/extras/video_out.mp4";
  // const char *url = "file:/home/alvaro/Documentos/arqueopterix/payloader_test/extras/video_out.flv";

  erizo::InputProcessor* ip = new erizo::InputProcessor();
  erizo::ExternalOutput* eo = new	erizo::ExternalOutput(url);
  eo->init();
  erizo::MediaInfo om;
  om.hasVideo = true;
  // om.hasAudio = true;
  om.videoCodec.height = 704;
  om.videoCodec.width = 396;
  ip->init(om, eo);

  erizo::RtpSource* rtp = new erizo::RtpSource(3011, ip);
  // rtp->setMediaSink(ip);
}
