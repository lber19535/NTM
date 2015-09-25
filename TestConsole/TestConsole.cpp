// TestConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
};

#include <iostream>
#include <string>
#include <fstream>

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame);

int main()
{
    av_register_all();

    AVFormatContext *context = nullptr;

    char *file_path = "C:\\Users\\bill_lv\\Documents\\Visual Studio 2015\\Projects\\NTM\\TestConsole\\TestFile\\Wildlife.wmv";

    if (avformat_open_input(&context, file_path, nullptr,0) != 0)
    {
        std::cout << "file not found" << std::endl;
    }

    if (avformat_find_stream_info(context, nullptr) < 0)
    {
        std::cout << "find info failed" << std::endl;
    }

    std::cout << "audio codec name " << context->streams << std::endl;

    av_dump_format(context, 0, file_path, 0);

    std::cout << "streams array size" << context->nb_streams << std::endl;
    
    AVCodecContext *p_video_codec_ctx = nullptr;
    AVCodecContext *p_audio_codec_ctx = nullptr;

    int video_stream = -1;
    int audio_stream = -1;

    for (unsigned i = 0; i < context->nb_streams; i++)
    {
        if (context->streams[i]->codec->codec_type == AVMediaType::AVMEDIA_TYPE_VIDEO) 
        {
            video_stream = i;
        }
        if (context->streams[i]->codec->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO)
        {
            audio_stream = i;
        }
    }

    if (video_stream == -1)
    {
        std::cout << "do not find the video stream" << std::endl;
    }
    else
    {
        std::cout << "video track is " << video_stream << std::endl;
        p_video_codec_ctx = context->streams[video_stream]->codec;
    }

    if (audio_stream == -1)
    {
        std::cout << "do not find the audio stream" << std::endl;
    }
    else
    {
        std::cout << "audio track is " << audio_stream << std::endl;
        p_audio_codec_ctx = context->streams[audio_stream]->codec;
    }

    auto p_audio_codec = avcodec_find_decoder(p_audio_codec_ctx->codec_id);
    auto p_video_codec = avcodec_find_decoder(p_video_codec_ctx->codec_id);

    if (p_audio_codec == nullptr)
    {
        std::cout << "audio codec can not find" << std::endl;
    }
    else
    {
        std::cout << "codec type is " << p_audio_codec->name << std::endl;
    }
    
    if (p_video_codec == nullptr)
    {
        std::cout << "video codec can not find" << std::endl;
    }
    else
    {
        std::cout << "codec type is " << p_video_codec->name << std::endl;
    }

    auto p_output_audio_ctx = avcodec_alloc_context3(p_audio_codec);
    auto p_output_video_ctx = avcodec_alloc_context3(p_video_codec);

    if (avcodec_copy_context(p_output_audio_ctx, p_audio_codec_ctx) != 0)
    {
        std::cout << "context cpoy failed" << std::endl;
    }

    if (avcodec_copy_context(p_output_video_ctx, p_video_codec_ctx) != 0)
    {
        std::cout << "context cpoy failed" << std::endl;
    }

    if (avcodec_open2(p_output_audio_ctx, p_audio_codec, nullptr) < 0)
    {
        std::cout << "codec open failed" << std::endl;
    }

    if (avcodec_open2(p_output_video_ctx, p_video_codec, nullptr) < 0)
    {
        std::cout << "codec open failed" << std::endl;
    }

    auto p_frame = av_frame_alloc();
    auto p_frame_rgb = av_frame_alloc();

    auto pic_size = avpicture_get_size(AVPixelFormat::PIX_FMT_RGB24, p_output_video_ctx->width, p_output_video_ctx->height);

    auto buffer = (uint8_t *)av_malloc(pic_size * sizeof(uint8_t));

    avpicture_fill((AVPicture *)p_frame_rgb, buffer, AVPixelFormat::PIX_FMT_RGB24, p_output_video_ctx->width, p_output_video_ctx->height);

    auto sws_ctx = sws_getContext(p_output_video_ctx->width, p_output_video_ctx->height, p_output_video_ctx->pix_fmt, p_output_video_ctx->width, p_output_video_ctx->height, AVPixelFormat::PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);

    AVPacket packet;

    int frame_ptr = 0;
    int i = 0;
    while (av_read_frame(context, &packet) >= 0)
    {
        if (packet.stream_index == video_stream) 
        {
            avcodec_decode_video2(p_output_video_ctx, p_frame, &frame_ptr, &packet);

            if (frame_ptr)
            {
                sws_scale(sws_ctx, (uint8_t const * const *)p_frame->data, p_frame->linesize, 0, p_output_video_ctx->height, p_frame_rgb->data, p_frame_rgb->linesize);

                std::cout << "i " << i << std::endl;
                if (++i <= 5)
                {
                    SaveFrame(p_frame_rgb, p_output_video_ctx->width, p_output_video_ctx->height,0);
                }
            }
        }
    }

    av_free_packet(&packet);

    getchar();
    return 0;
}

void SaveFrame(AVFrame *pFrame, int width, int height, int index) {
    std::fstream stream;
    
    std::string prefix = "C:\\Users\\bill_lv\\Documents\\Visual Studio 2015\\Projects\\NTM\\TestConsole\\output\\output_frame_";
    std::string end = ".ppm";
    std::string num = std::to_string(index);
    std::string file_name = prefix + num + end;

    stream.open(file_name, std::ios::out | std::ios::app | std::ios::binary);
    for (int i = 0; i < height; i++)
    {
        stream << pFrame->data;
    }
    stream.close();
    
}

