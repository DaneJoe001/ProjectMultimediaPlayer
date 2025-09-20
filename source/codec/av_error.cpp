#include "codec/av_error.hpp"

AVError::AVError(int error_code) noexcept : m_error_code(error_code) {}
AVError::AVError() noexcept : m_error_code(0) {}

std::string AVError::message() const
{
    if (m_error_code >= 0)
    {
        return std::string("No error");
    }
    char buffer[AV_ERROR_MAX_STRING_SIZE] = { 0 };
    av_strerror(m_error_code, buffer, sizeof(buffer));
    return std::string(buffer);
}

int AVError::code() const noexcept { return m_error_code; }
bool AVError::ok() const noexcept { return m_error_code >= 0; }
bool AVError::failed() const noexcept { return m_error_code < 0; }
AVError::operator bool() const noexcept { return ok(); }

bool AVError::operator==(int error_code) const noexcept { return m_error_code == error_code; }
AVError& AVError::operator=(int error_code) noexcept { m_error_code = error_code; return *this; }