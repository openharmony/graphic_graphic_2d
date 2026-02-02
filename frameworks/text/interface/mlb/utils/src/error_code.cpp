/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "error_code.h"

namespace OHOS::MLB {
const std::unordered_map<TextErrorCode, std::string> ERROR_MESSAGES {
    { BUSINESS_ERROR_INVALID_PARAM, "Invalid parameters" },
    { ERROR_INVALID_PARAM, "Invalid parameters" },
    { ERROR_FILE_NOT_EXIST, "File does not exist" },
    { ERROR_FILE_OPEN_FAILED, "Failed to open file" },
    { ERROR_FILE_SEEK_FAILED, "Failed to seek file" },
    { ERROR_FILE_SIZE_FAILED, "Failed to get file size" },
    { ERROR_FILE_READ_FAILED, "Failed to read file" },
    { ERROR_FILE_EMPTY, "File is empty" },
    { ERROR_FILE_CORRUPTED, "File is corrupted or invalid font format" },
};

std::string TextResultBase::ToString() const
{
    return (ERROR_MESSAGES.count(errorCode) ? ERROR_MESSAGES.at(errorCode) : "Unknown error") +
           (!detailedInfo.empty() ? ": " : "") + detailedInfo;
}
} // namespace OHOS::MLB
