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

namespace {
    const int MAX_LOG_LENGTH = 2048;
}
namespace OHOS {
namespace Rosen {
inline OHOS::HiviewDFX::HiLogLabel GenerateLabel(const std::string& tag)
{
    // The "0xD001400" is the domain ID for graphic module that alloted by the OS.
    OHOS::HiviewDFX::HiLogLabel label = { LOG_CORE, 0xD001400, tag.c_str() };
    return label;
}

int RSLog::Output(RSLog::Level level, const char* format, ...)
{
    char logStr[MAX_LOG_LENGTH] = {0};
    va_list args;
    va_start(args, format);
    int ret = vsprintf_s(logStr, MAX_LOG_LENGTH, format, args);
    if (ret == -1) { // vsprintf_s failed
        OHOS::HiviewDFX::HiLog::Error(GenerateLabel(tag_), "print log error in vsprintf_s");
        va_end(args);
        return -1;
    }
    va_end(args);
    switch (level) {
        case LEVEL_INFO:
            OHOS::HiviewDFX::HiLog::Info(GenerateLabel(tag_), "%{public}s", logStr);
            break;
        case LEVEL_DEBUG:
            OHOS::HiviewDFX::HiLog::Debug(GenerateLabel(tag_), "%{public}s", logStr);
            break;
        case LEVEL_WARN:
            OHOS::HiviewDFX::HiLog::Warn(GenerateLabel(tag_), "%{public}s", logStr);
            break;
        case LEVEL_ERROR:
            OHOS::HiviewDFX::HiLog::Error(GenerateLabel(tag_), "%{public}s", logStr);
            break;
        case LEVEL_FATAL:
            OHOS::HiviewDFX::HiLog::Fatal(GenerateLabel(tag_), "%{public}s", logStr);
            break;
    }
    return 0;
}
} // namespace Rosen
} // namespace OHOS
