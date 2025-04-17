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

#ifndef SAFUZZ_LOG_H
#define SAFUZZ_LOG_H

#include <chrono>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <string>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
inline std::string GetCurrentSysTime()
{
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&nowTime), "%m-%d %H:%M:%S");
    auto milliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
    auto ms = milliSeconds - seconds;
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

#define SAFUZZ_LOGI(format, ...)                                                                   \
    do {                                                                                           \
        ROSEN_LOGI("[SAFUZZ] " format, ##__VA_ARGS__);                                             \
        fprintf(stdout, "%s I [SAFUZZ] " format "\n", GetCurrentSysTime().c_str(), ##__VA_ARGS__); \
    } while (0)

#define SAFUZZ_LOGD(format, ...)                                                                   \
    do {                                                                                           \
        ROSEN_LOGD("[SAFUZZ] " format, ##__VA_ARGS__);                                             \
        fprintf(stdout, "%s D [SAFUZZ] " format "\n", GetCurrentSysTime().c_str(), ##__VA_ARGS__); \
    } while (0)

#define SAFUZZ_LOGE(format, ...)                                                                   \
    do {                                                                                           \
        ROSEN_LOGE("[SAFUZZ] " format, ##__VA_ARGS__);                                             \
        fprintf(stdout, "%s E [SAFUZZ] " format "\n", GetCurrentSysTime().c_str(), ##__VA_ARGS__); \
    } while (0)

#define SAFUZZ_LOGW(format, ...)                                                                   \
    do {                                                                                           \
        ROSEN_LOGW("[SAFUZZ] " format, ##__VA_ARGS__);                                             \
        fprintf(stdout, "%s W [SAFUZZ] " format "\n", GetCurrentSysTime().c_str(), ##__VA_ARGS__); \
    } while (0)

#define SAFUZZ_LOGF(format, ...)                                                                   \
    do {                                                                                           \
        ROSEN_LOGF("[SAFUZZ] " format, ##__VA_ARGS__);                                             \
        fprintf(stdout, "%s F [SAFUZZ] " format "\n", GetCurrentSysTime().c_str(), ##__VA_ARGS__); \
    } while (0)

} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_LOG_H
