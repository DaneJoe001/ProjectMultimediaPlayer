#include "codec/av_error.hpp"

AVError::AVError(int error_code)
    :m_error_code(error_code) {
}

std::string AVError::error_msg()
{
    if (m_error_code >= 0)
    {
        return std::string("No error!");
    }
    return std::string(av_err2str(m_error_code));
}

int AVError::error_code()const
{
    return m_error_code;
}

AVError::operator bool()const
{
    if (m_error_code < 0)
    {
        return false;
    }
    return true;
}

bool AVError::operator==(int error_code)
{
    return m_error_code == error_code;
}