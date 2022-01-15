/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ROSENRENDER_ROSEN_WEBGL_LOG
#define ROSENRENDER_ROSEN_WEBGL_LOG

#include "hilog/log.h"

template<typename ...Args>
void PrintLogE(const std::string &func, const std::string &format, Args... args)
{
    std::string prefix = "[%s]";
    std::string formatFull = prefix + format;
    OHOS::HiviewDFX::HiLogLabel label {LOG_CORE, 0xD001400, "WebGL"};
    OHOS::HiviewDFX::HiLog::Error(label, formatFull.c_str(), func.c_str(), args...);
}

template<typename ...Args>
void PrintLogI(const std::string &func, const std::string &format, Args... args)
{
    std::string prefix = "[%s]";
    std::string formatFull = prefix + format;
    OHOS::HiviewDFX::HiLogLabel label {LOG_CORE, 0xD001400, "WebGL"};
    OHOS::HiviewDFX::HiLog::Info(label, formatFull.c_str(), func.c_str(), args...);
}
#define LOGI(...) PrintLogI(__func__, ##__VA_ARGS__)
#define LOGE(...) PrintLogE(__func__, ##__VA_ARGS__)

#endif // ROSENRENDER_ROSEN_WEBGL_LOG
