#pragma once

struct AVStream;

class AVStreamPtr
{
public:
    AVStreamPtr(AVStream* stream);
private:
    AVStream* m_stream = nullptr;
};