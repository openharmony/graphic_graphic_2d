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
#include <securec.h>

#include <hilog/log.h>

namespace OHOS {
namespace Rosen {
namespace {
constexpr int MAX_LOG_LENGTH = 2048;

// The "0xD001400" is the domain ID for graphic module that alloted by the OS.
constexpr OHOS::HiviewDFX::HiLogLabel LABEL_RS = { LOG_CORE, 0xD001400, "OHOS::RS" };
constexpr OHOS::HiviewDFX::HiLogLabel LABEL_ROSEN = { LOG_CORE, 0xD001400, "OHOS::ROSEN" };

inline const OHOS::HiviewDFX::HiLogLabel& GenerateLabel(RSLog::Tag tag)
{
    return (tag == RSLog::Tag::RS) ? LABEL_RS : LABEL_ROSEN;
}
}


void RSLogOutput(RSLog::Tag tag, RSLog::Level level, const char* format, ...)
{
    char logStr[MAX_LOG_LENGTH] = {0};
    va_list args;
    va_start(args, format);
    int ret = vsprintf_s(logStr, MAX_LOG_LENGTH, format, args);
    if (ret == -1) { // vsprintf_s failed
        OHOS::HiviewDFX::HiLog::Error(GenerateLabel(tag), "print log error in vsprintf_s");
        va_end(args);
        return;
    }
    va_end(args);
    switch (level) {
        case RSLog::LEVEL_INFO:
            OHOS::HiviewDFX::HiLog::Info(GenerateLabel(tag), "%{public}s", logStr);
            break;
#ifndef ROSEN_DISABLE_DEBUGLOG
        case RSLog::LEVEL_DEBUG:
            OHOS::HiviewDFX::HiLog::Debug(GenerateLabel(tag), "%{public}s", logStr);
            break;
#endif
        case RSLog::LEVEL_WARN:
            OHOS::HiviewDFX::HiLog::Warn(GenerateLabel(tag), "%{public}s", logStr);
            break;
        case RSLog::LEVEL_ERROR:
            OHOS::HiviewDFX::HiLog::Error(GenerateLabel(tag), "%{public}s", logStr);
            break;
        case RSLog::LEVEL_FATAL:
            OHOS::HiviewDFX::HiLog::Fatal(GenerateLabel(tag), "%{public}s", logStr);
            break;
        default:
            break;
    }
}
} // namespace Rosen
} // namespace OHOS
