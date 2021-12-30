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

#ifndef RENDER_SERVICE_BASE_CORE_COMMON_RS_LOG_H
#define RENDER_SERVICE_BASE_CORE_COMMON_RS_LOG_H

#include <string>

namespace OHOS {
namespace Rosen {
class RSLog {
public:
    enum Level { LEVEL_INFO = 0, LEVEL_DEBUG, LEVEL_WARN, LEVEL_ERROR, LEVEL_FATAL };
    RSLog() : tag_("OHOS::ROSEN") {}
    virtual ~RSLog() {}
    virtual int Output(RSLog::Level level, const char* format, ...);
    void SetTag(const std::string& tag)
    {
        tag_ = tag;
    }
    const std::string& GetTag() const
    {
        return tag_;
    }

private:
    std::string tag_;
};
} // namespace Rosen
} // namespace OHOS

// Macro functions for printing ROSEN's log to the log system on the OS platform.
// Normally, you can use the maros ROSEN_LOGX, it will set a default log tag as "ROSEN".
// And if you want specific a log tag by yourself, you may should use ROSEN_LOGX_WITH_TAG.
#ifdef ROSEN_DEBUG
#define ROSEN_LOGI_WITH_TAG(tag, format, ...)                           \
    do {                                                                \
        RSLog logger;                                                   \
        logger.SetTag(tag);                                             \
        logger.Output(RSLog::Level::LEVEL_INFO, format, ##__VA_ARGS__); \
    } while (0)

#define ROSEN_LOGD_WITH_TAG(tag, format, ...)                            \
    do {                                                                 \
        RSLog logger;                                                    \
        logger.SetTag(tag);                                              \
        logger.Output(RSLog::Level::LEVEL_DEBUG, format, ##__VA_ARGS__); \
    } while (0)

#define ROSEN_LOGE_WITH_TAG(tag, format, ...)                            \
    do {                                                                 \
        RSLog logger;                                                    \
        logger.SetTag(tag);                                              \
        logger.Output(RSLog::Level::LEVEL_ERROR, format, ##__VA_ARGS__); \
    } while (0)

#define ROSEN_LOGW_WITH_TAG(tag, format, ...)                           \
    do {                                                                \
        RSLog logger;                                                   \
        logger.SetTag(tag);                                             \
        logger.Output(RSLog::Level::LEVEL_WARN, format, ##__VA_ARGS__); \
    } while (0)

#define ROSEN_LOGF_WITH_TAG(tag, format, ...)                            \
    do {                                                                 \
        RSLog logger;                                                    \
        logger.SetTag(tag);                                              \
        logger.Output(RSLog::Level::LEVEL_FATAL, format, ##__VA_ARGS__); \
    } while (0)

#else
#define ROSEN_LOGI_WITH_TAG(tag, format, ...)
#define ROSEN_LOGD_WITH_TAG(tag, format, ...)
#define ROSEN_LOGE_WITH_TAG(tag, format, ...)
#define ROSEN_LOGW_WITH_TAG(tag, format, ...)
#define ROSEN_LOGF_WITH_TAG(tag, format, ...)
#endif

#define ROSEN_LOGI(format, ...) ROSEN_LOGI_WITH_TAG(RSLog().GetTag().c_str(), format, ##__VA_ARGS__)
#define ROSEN_LOGD(format, ...) ROSEN_LOGD_WITH_TAG(RSLog().GetTag().c_str(), format, ##__VA_ARGS__)
#define ROSEN_LOGE(format, ...) ROSEN_LOGE_WITH_TAG(RSLog().GetTag().c_str(), format, ##__VA_ARGS__)
#define ROSEN_LOGW(format, ...) ROSEN_LOGW_WITH_TAG(RSLog().GetTag().c_str(), format, ##__VA_ARGS__)
#define ROSEN_LOGF(format, ...) ROSEN_LOGF_WITH_TAG(RSLog().GetTag().c_str(), format, ##__VA_ARGS__)

#endif // RENDER_SERVICE_BASE_CORE_COMMON_RS_LOG_H
