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

#ifndef RENDER_SERVICE_BASE_CORE_COMMON_RS_LOG_H
#define RENDER_SERVICE_BASE_CORE_COMMON_RS_LOG_H

// NOT redundant, we need PRIu64/PRId64 for logging
#include <cinttypes>
#include <string>
#include <hilog/log.h>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
// The "0xD001400" is the domain ID for graphic module that alloted by the OS.
constexpr OHOS::HiviewDFX::HiLogLabel LABEL_RS = { LOG_CORE, 0xD001400, "OHOS::RS" };
constexpr OHOS::HiviewDFX::HiLogLabel LABEL_ROSEN = { LOG_CORE, 0xD001400, "OHOS::ROSEN" };
constexpr const char* DEBUG_GRAPHIC_LOG_FLAG = "debug.graphic.logflag";

class RSB_EXPORT RSLog {
public:
    enum Tag { RS = 0, RS_CLIENT };
    enum Level { LEVEL_INFO = 0, LEVEL_DEBUG, LEVEL_WARN, LEVEL_ERROR, LEVEL_FATAL };
    virtual ~RSLog() = default;
};

void RSB_EXPORT RSLogOutput(RSLog::Tag tag, RSLog::Level level, const char* format, ...);

void RSB_EXPORT RSLogEOutput(const char* format, ...);
void RSB_EXPORT RSLogWOutput(const char* format, ...);
void RSB_EXPORT RSLogDOutput(const char* format, ...);

enum RSLogFlag {
    // screen
    FLAG_DEBUG_SCREEN = 0x00000001,

    // node
    FLAG_DEBUG_NODE = 0x00000002,

    // effect
    FLAG_DEBUG_EFFECT = 0x00000004,

    // pipeline
    FLAG_DEBUG_PIPELINE = 0x00000008,

    // modifier
    FLAG_DEBUG_MODIFIER = 0x00000010,

    // buffer
    FLAG_DEBUG_BUFFER = 0x00000020,

    // layer
    FLAG_DEBUG_LAYER = 0x00000040,

    // composer
    FLAG_DEBUG_COMPOSER = 0x00000080,

    // vsync
    FLAG_DEBUG_VSYNC = 0x00000100,

    // drawing
    FLAG_DEBUG_DRAWING = 0x00000200,

    // prevalidate
    FLAG_DEBUG_PREVALIDATE = 0x00000400,

    // ipc
    FLAG_DEBUG_IPC = 0x00000800,
};

class RSLogManager {
public:
    RSLogManager();
    ~RSLogManager() = default;

    static RSLogManager& GetInstance();
    bool SetRSLogFlag(std::string& flag);
    inline bool IsRSLogFlagEnabled(RSLogFlag flag)
    {
        return logFlag_ & flag;
    }

private:
    uint32_t logFlag_ = 0;

    bool IsFlagValid(std::string& flag);

    static constexpr uint32_t INPUT_FLAG_MIN_LENGTH = 2;

    static constexpr uint32_t INPUT_FLAG_MAX_LENGTH = 10;

    static constexpr uint32_t NUMERICAL_BASE = 16;
};

} // namespace Rosen
} // namespace OHOS

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001406

#undef LOG_TAG
#define LOG_TAG "OHOS::RS"

#define ROSEN_LOGI(format, ...) \
    HILOG_INFO(LOG_CORE, format, ##__VA_ARGS__)
#define ROSEN_LOGD(format, ...)                   \
    HILOG_DEBUG(LOG_CORE, format, ##__VA_ARGS__); \
    OHOS::Rosen::RSLogDOutput(format, ##__VA_ARGS__)
#define ROSEN_LOGE(format, ...)                   \
    HILOG_ERROR(LOG_CORE, format, ##__VA_ARGS__); \
    OHOS::Rosen::RSLogEOutput(format, ##__VA_ARGS__)
#define ROSEN_LOGW(format, ...)                  \
    HILOG_WARN(LOG_CORE, format, ##__VA_ARGS__); \
    OHOS::Rosen::RSLogWOutput(format, ##__VA_ARGS__)
#define ROSEN_LOGF(format, ...) HILOG_FATAL(LOG_CORE, format, ##__VA_ARGS__)

#define RS_LOGI(format, ...) HILOG_INFO(LOG_CORE, format, ##__VA_ARGS__)
#define RS_LOGD(format, ...)                      \
    HILOG_DEBUG(LOG_CORE, format, ##__VA_ARGS__); \
    OHOS::Rosen::RSLogDOutput(format, ##__VA_ARGS__)
#define RS_LOGE(format, ...)                      \
    HILOG_ERROR(LOG_CORE, format, ##__VA_ARGS__); \
    OHOS::Rosen::RSLogEOutput(format, ##__VA_ARGS__)
#define RS_LOGW(format, ...)                     \
    HILOG_WARN(LOG_CORE, format, ##__VA_ARGS__); \
    OHOS::Rosen::RSLogWOutput(format, ##__VA_ARGS__)
#define RS_LOGF(format, ...) HILOG_FATAL(LOG_CORE, format, ##__VA_ARGS__)

#define CONDITION(cond)     (__builtin_expect((cond) != 0, 0))

#ifndef RS_LOGD_IF
#define RS_LOGD_IF(cond, format, ...) \
    ( (CONDITION(cond)) \
    ? ((void)HILOG_DEBUG(LOG_CORE, format, ##__VA_ARGS__)) \
    : (void)0)
#endif

#ifndef RS_LOGI_IF
#define RS_LOGI_IF(cond, format, ...) \
    ( (CONDITION(cond)) \
    ? ((void)HILOG_INFO(LOG_CORE, format, ##__VA_ARGS__)) \
    : (void)0)
#endif

#ifndef RS_LOGW_IF
#define RS_LOGW_IF(cond, format, ...) \
    ( (CONDITION(cond)) \
    ? ((void)HILOG_WARN(LOG_CORE, format, ##__VA_ARGS__)) \
    : (void)0)
#endif

#ifndef RS_LOGE_IF
#define RS_LOGE_IF(cond, format, ...) \
    ( (CONDITION(cond)) \
    ? ((void)HILOG_ERROR(LOG_CORE, format, ##__VA_ARGS__)) \
    : (void)0)
#endif

#ifndef RS_LOGF_IF
#define RS_LOGF_IF(cond, format, ...) \
    ( (CONDITION(cond)) \
    ? ((void)HILOG_FATAL(LOG_CORE, format, ##__VA_ARGS__)) \
    : (void)0)
#endif

#ifndef ROSEN_LOGD_IF
#define ROSEN_LOGD_IF RS_LOGD_IF
#endif

#ifndef ROSEN_LOGI_IF
#define ROSEN_LOGI_IF RS_LOGI_IF
#endif

#ifndef ROSEN_LOGW_IF
#define ROSEN_LOGW_IF RS_LOGW_IF
#endif

#ifndef ROSEN_LOGE_IF
#define ROSEN_LOGE_IF RS_LOGE_IF
#endif

#ifndef ROSEN_LOGF_IF
#define ROSEN_LOGF_IF RS_LOGF_IF
#endif

#define RS_LOG_ENABLE(flag) (RSLogManager::GetInstance().IsRSLogFlagEnabled(flag))

#define DEBUG_SCREEN RS_LOG_ENABLE(FLAG_DEBUG_SCREEN)

#define DEBUG_NODE RS_LOG_ENABLE(FLAG_DEBUG_NODE)

#define DEBUG_MODIFIER RS_LOG_ENABLE(FLAG_DEBUG_MODIFIER)

#define DEBUG_BUFFER RS_LOG_ENABLE(FLAG_DEBUG_BUFFER)

#define DEBUG_LAYER RS_LOG_ENABLE(FLAG_DEBUG_LAYER)

#define DEBUG_COMPOSER RS_LOG_ENABLE(FLAG_DEBUG_COMPOSER)

#define DEBUG_PIPELINE RS_LOG_ENABLE(FLAG_DEBUG_PIPELINE)

#define DEBUG_VSYNC RS_LOG_ENABLE(FLAG_DEBUG_VSYNC)

#define DEBUG_DRAWING RS_LOG_ENABLE(FLAG_DEBUG_DRAWING)

#define DEBUG_PREVALIDATE RS_LOG_ENABLE(FLAG_DEBUG_PREVALIDATE)

#define DEBUG_IPC RS_LOG_ENABLE(FLAG_DEBUG_IPC)

#define RS_LOGE_LIMIT(func, line, format, ...)                                                                    \
{                                                                                                                 \
    static constexpr uint64_t LOG_PRINT_INTERVAL_IN_SECOND = 20;                                                  \
    static std::atomic<bool> isFirstTime##func##line = true;                                                      \
    static std::atomic<uint64_t> prePrintTime##func##line = static_cast<uint64_t>(                                \
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())     \
            .count());                                                                                            \
    uint64_t currTime = static_cast<uint64_t>(                                                                    \
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())     \
            .count());                                                                                            \
    if ((currTime - prePrintTime##func##line >= LOG_PRINT_INTERVAL_IN_SECOND) || isFirstTime##func##line) {       \
        prePrintTime##func##line = static_cast<uint64_t>(                                                         \
            std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()) \
                .count());                                                                                        \
        isFirstTime##func##line = false;                                                                          \
        RS_LOGE(format, ##__VA_ARGS__);                                                                           \
    }                                                                                                             \
}

#define RS_LOGI_LIMIT(format, ...)                                                                                 \
{                                                                                                                  \
    static constexpr uint64_t LOG_PRINT_INTERVAL_IN_SECOND = 5;                                                    \
    static std::atomic<bool> isFirst##__func__##__line__ = true;                                                   \
    static std::atomic<uint64_t> preTime##__func__##__line__ = static_cast<uint64_t>(                              \
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())      \
            .count());                                                                                             \
    uint64_t currTime = static_cast<uint64_t>(                                                                     \
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())      \
            .count());                                                                                             \
    if ((currTime - preTime##__func__##__line__ >= LOG_PRINT_INTERVAL_IN_SECOND) || isFirst##__func__##__line__) { \
        preTime##__func__##__line__ = static_cast<uint64_t>(                                                       \
            std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())  \
                .count());                                                                                         \
        isFirst##__func__##__line__ = false;                                                                       \
        RS_LOGI(format, ##__VA_ARGS__);                                                                            \
    }                                                                                                              \
}

#endif // RENDER_SERVICE_BASE_CORE_COMMON_RS_LOG_H
