/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MLB_ERROR_CODE_H
#define MLB_ERROR_CODE_H

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
        return ERROR_MESSAGES.at(errorCode) + ": " + detailedInfo;
    }

    constexpr static TextResult Success(T r = nullptr)
    {
        return TextResult(true, ERROR_NONE, "", r);
    }

    constexpr static TextResult Error(TextErrorCode code, const std::string& detail = "", T value = nullptr)
    {
        return TextResult(false, code, detail, value);
    }

    constexpr static TextResult Invalid(const std::string& detail = "", T value = nullptr)
    {
        return TextResult(false, ERROR_INVALID_PARAM, detail, value);
    }
};
} // namespace OHOS::MLB
#endif