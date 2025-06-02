/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "platform/common/rs_log.h"

#include <cstdarg>
#include <cstdio>
#include <climits>
#include <securec.h>
#include <hilog/log.h>
#ifdef NOT_BUILD_FOR_OHOS_SDK
#include <parameters.h>
#endif

namespace OHOS {
namespace Rosen {
namespace {
inline const OHOS::HiviewDFX::HiLogLabel& GenerateLabel(RSLog::Tag tag)
{
    return (tag == RSLog::Tag::RS) ? LABEL_RS : LABEL_ROSEN;
}
}

void RSLogOutput(RSLog::Tag tag, RSLog::Level level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    switch (level) {
        case RSLog::LEVEL_INFO:
            OHOS::HiviewDFX::HiLog::Info(GenerateLabel(tag), format, args);
            break;
#ifndef ROSEN_DISABLE_DEBUGLOG
        case RSLog::LEVEL_DEBUG:
            OHOS::HiviewDFX::HiLog::Debug(GenerateLabel(tag), format, args);
            break;
#endif
        case RSLog::LEVEL_WARN:
            OHOS::HiviewDFX::HiLog::Warn(GenerateLabel(tag), format, args);
            break;
        case RSLog::LEVEL_ERROR:
            OHOS::HiviewDFX::HiLog::Error(GenerateLabel(tag), format, args);
            break;
        case RSLog::LEVEL_FATAL:
            OHOS::HiviewDFX::HiLog::Fatal(GenerateLabel(tag), format, args);
            break;
        default:
            break;
    }
    va_end(args);
}

bool ConvertToLongUint(const std::string& str, unsigned long& value, int8_t base = 10)
{
    char* end;
    errno = 0;
    value = std::strtoul(str.c_str(), &end, base);
    if (end == str.c_str()) {
        return false;
    }
    if (errno == ERANGE && value == ULONG_MAX) {
        return false;
    }
    if (*end != '\0') {
        return false;
    }
    return true;
}

RSLogManager::RSLogManager()
{
#ifdef NOT_BUILD_FOR_OHOS_SDK
    std::string flag = OHOS::system::GetParameter(DEBUG_GRAPHIC_LOG_FLAG, "0X0");
    if (IsFlagValid(flag)) {
        unsigned long value = 0;
        std::string subFlag = flag.substr(INPUT_FLAG_MIN_LENGTH);
        if (!ConvertToLongUint(subFlag, value, NUMERICAL_BASE)) {
            RS_LOGD("RSLogManager %{public}s ConvertToLongUint failed", subFlag.c_str());
        }
        logFlag_ = static_cast<uint32_t>(value);
        RS_LOGI("RSLogManager init log flag: 0x%{public}X(%{public}u)", logFlag_, logFlag_);
    }
#endif
}

RSLogManager& RSLogManager::GetInstance()
{
    static RSLogManager rsLogManager;
    return rsLogManager;
}

bool RSLogManager::SetRSLogFlag(std::string& flag)
{
    if (IsFlagValid(flag)) {
#ifdef NOT_BUILD_FOR_OHOS_SDK
        OHOS::system::SetParameter(DEBUG_GRAPHIC_LOG_FLAG, flag);
#endif
        unsigned long value = 0;
        std::string subFlag = flag.substr(INPUT_FLAG_MIN_LENGTH);
        if (!ConvertToLongUint(subFlag, value, NUMERICAL_BASE)) {
            RS_LOGD("RSLogManager %{public}s ConvertToLongUint failed", subFlag.c_str());
        }
        logFlag_ = static_cast<uint32_t>(value);
        RS_LOGI("RSLogManager set log flag: 0x%{public}X(%{public}u)", logFlag_, logFlag_);
        return true;
    }
    return false;
}

bool RSLogManager::IsFlagValid(std::string& flag)
{
    if ((flag.length() > INPUT_FLAG_MIN_LENGTH) && (flag.length() <= INPUT_FLAG_MAX_LENGTH)
        && (flag.substr(0, INPUT_FLAG_MIN_LENGTH) == "0x" || flag.substr(0, INPUT_FLAG_MIN_LENGTH) == "0X")) {
        return true;
    }
    return false;
}

} // namespace Rosen
} // namespace OHOS
