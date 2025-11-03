#ifndef MLB_ERROR_CODE_H
#define MLB_ERROR_CODE_H

#include <js_native_api_types.h>
#include <string>
#include <unordered_map>

namespace OHOS::MLB {
enum TextErrorCode {
    ERROR_NONE,
    ERROR_INVALID_PARAM = 25900001,
    ERROR_FILE_NOT_EXIST,
    ERROR_FILE_OPEN_FAILED,
    ERROR_FILE_SEEK_FAILED,
    ERROR_FILE_SIZE_FAILED,
    ERROR_FILE_READ_FAILED,
    ERROR_FILE_EMPTY,
    ERROR_FILE_CORRUPTED,
};

const std::unordered_map<TextErrorCode, std::string> ERROR_MESSAGES {
    { ERROR_INVALID_PARAM, "Invalid parameters" },
    { ERROR_FILE_NOT_EXIST, "File does not exist" },
    { ERROR_FILE_OPEN_FAILED, "Failed to open file" },
    { ERROR_FILE_SEEK_FAILED, "Failed to seek file" },
    { ERROR_FILE_SIZE_FAILED, "Failed to get file size" },
    { ERROR_FILE_READ_FAILED, "Failed to read file" },
    { ERROR_FILE_EMPTY, "File is empty" },
    { ERROR_FILE_CORRUPTED, "File is corrupted or invalid font format" },
};

template<typename T>
struct TextResult {
    bool success;
    TextErrorCode errorCode;
    std::string detailedInfo;
    T result;

    TextResult(bool s = true, TextErrorCode code = ERROR_NONE, const std::string& detail = "", T r = nullptr)
        : success(s), errorCode(code), detailedInfo(detail), result(r)
    {
        // the value 401 do not change. It is defined on all system
        if (errorCode == 401) {
            errorCode = ERROR_INVALID_PARAM;
        }
    }

    std::string ToString() const
    {
        return ERROR_MESSAGES.at(errorCode) + " " + detailedInfo;
    }

    static TextResult Success(T r = nullptr)
    {
        return TextResult(true, ERROR_NONE, "", r);
    }

    static TextResult Error(TextErrorCode code, T value = nullptr, const std::string& detail = "")
    {
        return TextResult(false, code, detail, value);
    }
};
} // namespace OHOS::MLB
#endif