#pragma once

struct AVCodec;

class AVCodecPtr
{
public:
    AVCodecPtr(AVCodec* codec);
private:
    AVCodec* m_codec = nullptr;
};