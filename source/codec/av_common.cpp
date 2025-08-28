#include "codec/av_common.hpp"

AVRationalInfo::AVRationalInfo(AVRational av_rational)
{
    den = av_rational.den;
    num = av_rational.num;
}
AVRational AVRationalInfo::get_av_rational()const
{
    return AVRational{ num, den };
}
double AVRationalInfo::get_double()
{
    return num == 0 || den == 0 ? 0. : (double)num / (double)den;
}

