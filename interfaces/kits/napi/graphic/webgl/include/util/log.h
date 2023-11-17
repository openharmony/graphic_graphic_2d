/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef WEBGL_LOG_H
#define WEBGL_LOG_H

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <inttypes.h>
#include <string>

#include "hilog/log.h"

template<typename ...Args>
void PrintLogE(const std::string &func, int line, const std::string &format, Args... args)
{
    std::string prefix = "[%{public}s:%{public}d]";
    std::string formatFull = prefix + format;
    OHOS::HiviewDFX::HiLogLabel label {LOG_CORE, 0xD001400, "WebGL"};
    OHOS::HiviewDFX::HiLog::Error(label, formatFull.c_str(), func.c_str(), line, args...);
}

template<typename ...Args>
void PrintLogI(const std::string &func, int line, const std::string &format, Args... args)
{
    std::string prefix = "[%{public}s:%{public}d]";
    std::string formatFull = prefix + format;
    OHOS::HiviewDFX::HiLogLabel label {LOG_CORE, 0xD001400, "WebGL"};
    OHOS::HiviewDFX::HiLog::Info(label, formatFull.c_str(), func.c_str(), line, args...);
}

template<typename ...Args>
void PrintLogD(const std::string &func, int line, const std::string &format, Args... args)
{
    std::string prefix = "[%{public}s:%{public}d]";
    std::string formatFull = prefix + format;
    OHOS::HiviewDFX::HiLogLabel label {LOG_CORE, 0xD001400, "WebGL"};
    OHOS::HiviewDFX::HiLog::Debug(label, formatFull.c_str(), func.c_str(), line, args...);
}

#define LOGI(...) PrintLogI(__func__, __LINE__, ##__VA_ARGS__)
#define LOGE(...) PrintLogE(__func__, __LINE__, ##__VA_ARGS__)
#define LOGD(...) PrintLogD(__func__, __LINE__, ##__VA_ARGS__)

#endif // WEBGL_LOG_H
