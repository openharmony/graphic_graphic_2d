/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef TEXT_LOG_H
#define TEXT_LOG_H

#ifdef ENABLE_OHOS_ENHANCE
#include <hilog/log.h>
#endif

template<typename... Args>
void ignore_unused(Args&&...)
{}
namespace OHOS {
namespace Rosen {
#ifdef ENABLE_OHOS_ENHANCE

#undef TEXT_LOG_DOMAIN
#define TEXT_LOG_DOMAIN 0xD001408

#undef TEXT_LOG_TAG
#define TEXT_LOG_TAG "Text"

#define TEXT_LOG_LIMIT_HOURS 3600
#define TEXT_LOG_LIMIT_MINUTE 60
#define TEXT_LOG_LIMIT_PRINT_FREQUENCY 3

#define TEXT_LOGD(fmt, ...)              \
    HILOG_DEBUG(LOG_CORE, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define TEXT_LOGI(fmt, ...)              \
    HILOG_INFO(LOG_CORE, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define TEXT_LOGW(fmt, ...)              \
    HILOG_WARN(LOG_CORE, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define TEXT_LOGE(fmt, ...)              \
    HILOG_ERROR(LOG_CORE, "%{public}s: " fmt, __func__, ##__VA_ARGS__)

#define TEXT_PRINT_LIMIT(type, level, intervals, canPrint, frequency)                                    \
    do {                                                                                                 \
        static auto last = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>();   \
        static uint32_t supressed = 0;                                                                   \
        static int printCount = 0;                                                                       \
        auto now = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()); \
        auto duration = now - last;                                                                      \
        if (duration.count() >= (intervals)) {                                                           \
            last = now;                                                                                  \
            uint32_t supressedCnt = supressed;                                                           \
            supressed = 0;                                                                               \
            printCount = 1;                                                                              \
            if (supressedCnt != 0) {                                                                     \
                ((void)HILOG_IMPL((type), (level), TEXT_LOG_DOMAIN, TEXT_LOG_TAG,                                  \
                    "%{public}s log suppressed cnt %{public}u", __func__, supressedCnt));                \
            }                                                                                            \
            (canPrint) = true;                                                                           \
        } else {                                                                                         \
            if ((printCount++) < (frequency)) {                                                          \
                (canPrint) = true;                                                                       \
            } else {                                                                                     \
                supressed++;                                                                             \
                (canPrint) = false;                                                                      \
            }                                                                                            \
        }                                                                                                \
    } while (0)

#define TEXT_LOGI_LIMIT3_HOUR(fmt, ...)                                                                  \
    do {                                                                                                 \
        bool can = true;                                                                                 \
        TEXT_LOGD(fmt, ##__VA_ARGS__);                                                                   \
        TEXT_PRINT_LIMIT(LOG_CORE, LOG_INFO, TEXT_LOG_LIMIT_HOURS, can, TEXT_LOG_LIMIT_PRINT_FREQUENCY); \
        if (can) {                                                                                       \
            TEXT_LOGI(fmt, ##__VA_ARGS__);                                                               \
        }                                                                                                \
    } while (0)

#define TEXT_LOGW_LIMIT3_HOUR(fmt, ...)                                                                  \
    do {                                                                                                 \
        bool can = true;                                                                                 \
        TEXT_LOGD(fmt, ##__VA_ARGS__);                                                                   \
        TEXT_PRINT_LIMIT(LOG_CORE, LOG_WARN, TEXT_LOG_LIMIT_HOURS, can, TEXT_LOG_LIMIT_PRINT_FREQUENCY); \
        if (can) {                                                                                       \
            TEXT_LOGW(fmt, ##__VA_ARGS__);                                                               \
        }                                                                                                \
    } while (0)

#define TEXT_LOGE_LIMIT3_HOUR(fmt, ...)                                                                   \
    do {                                                                                                  \
        bool can = true;                                                                                  \
        TEXT_LOGD(fmt, ##__VA_ARGS__);                                                                    \
        TEXT_PRINT_LIMIT(LOG_CORE, LOG_ERROR, TEXT_LOG_LIMIT_HOURS, can, TEXT_LOG_LIMIT_PRINT_FREQUENCY); \
        if (can) {                                                                                        \
            TEXT_LOGE(fmt, ##__VA_ARGS__);                                                                \
        }                                                                                                 \
    } while (0)

#define TEXT_LOGI_LIMIT3_MIN(fmt, ...)                                                                    \
    do {                                                                                                  \
        bool can = true;                                                                                  \
        TEXT_LOGD(fmt, ##__VA_ARGS__);                                                                    \
        TEXT_PRINT_LIMIT(LOG_CORE, LOG_INFO, TEXT_LOG_LIMIT_MINUTE, can, TEXT_LOG_LIMIT_PRINT_FREQUENCY); \
        if (can) {                                                                                        \
            TEXT_LOGI(fmt, ##__VA_ARGS__);                                                                \
        }                                                                                                 \
    } while (0)

#define TEXT_LOGW_LIMIT3_MIN(fmt, ...)                                                                    \
    do {                                                                                                  \
        bool can = true;                                                                                  \
        TEXT_LOGD(fmt, ##__VA_ARGS__);                                                                    \
        TEXT_PRINT_LIMIT(LOG_CORE, LOG_WARN, TEXT_LOG_LIMIT_MINUTE, can, TEXT_LOG_LIMIT_PRINT_FREQUENCY); \
        if (can) {                                                                                        \
            TEXT_LOGW(fmt, ##__VA_ARGS__);                                                                \
        }                                                                                                 \
    } while (0)

#define TEXT_LOGE_LIMIT3_MIN(fmt, ...)                                                                     \
    do {                                                                                                   \
        bool can = true;                                                                                   \
        TEXT_LOGD(fmt, ##__VA_ARGS__);                                                                     \
        TEXT_PRINT_LIMIT(LOG_CORE, LOG_ERROR, TEXT_LOG_LIMIT_MINUTE, can, TEXT_LOG_LIMIT_PRINT_FREQUENCY); \
        if (can) {                                                                                         \
            TEXT_LOGE(fmt, ##__VA_ARGS__);                                                                 \
        }                                                                                                  \
    } while (0)

#else
#define TEXT_LOGD(...) ignore_unused(__VA_ARGS__)
#define TEXT_LOGI(...) ignore_unused(__VA_ARGS__)
#define TEXT_LOGW(...) ignore_unused(__VA_ARGS__)
#define TEXT_LOGE(...) ignore_unused(__VA_ARGS__)

#define TEXT_LOGI_LIMIT3_HOUR(...) ignore_unused(__VA_ARGS__)
#define TEXT_LOGW_LIMIT3_HOUR(...) ignore_unused(__VA_ARGS__)
#define TEXT_LOGE_LIMIT3_HOUR(...) ignore_unused(__VA_ARGS__)

#define TEXT_LOGI_LIMIT3_MIN(...) ignore_unused(__VA_ARGS__)
#define TEXT_LOGW_LIMIT3_MIN(...) ignore_unused(__VA_ARGS__)
#define TEXT_LOGE_LIMIT3_MIN(...) ignore_unused(__VA_ARGS__)

#endif

#define TEXT_ERROR_CHECK(ret, statement, format, ...)  \
    do {                                               \
        if (!(ret)) {                                  \
            TEXT_LOGE(format, ##__VA_ARGS__);          \
            statement;                                 \
        }                                              \
    } while (0)

#define TEXT_INFO_CHECK(ret, statement, format, ...)   \
    do {                                               \
        if (!(ret)) {                                  \
            TEXT_LOGI(format, ##__VA_ARGS__);          \
            statement;                                 \
        }                                              \
    } while (0)

#define TEXT_CHECK(ret, statement)                     \
    do {                                               \
        if (!(ret)) {                                  \
            statement;                                 \
        }                                              \
    } while (0)

#define TEXT_CHECK_RETURN_VALUE(ret, result)           \
    do {                                               \
        if (!(ret)) {                                  \
            return result;                             \
        }                                              \
    } while (0)


} // namespace Rosen
} // namespace OHOS
#endif
