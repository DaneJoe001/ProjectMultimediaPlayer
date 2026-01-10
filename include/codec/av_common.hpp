#pragma once
extern "C"
{
#include "libavutil/rational.h"
}

struct AVRationalInfo
{
    AVRationalInfo(AVRational av_rational);
    AVRational get_av_rational()const;
    double get_double();
    int num;
    int den;
};