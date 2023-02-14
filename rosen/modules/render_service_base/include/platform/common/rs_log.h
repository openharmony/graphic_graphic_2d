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

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSLog {
public:
    enum Tag { RS = 0, RS_CLIENT };
    enum Level { LEVEL_INFO = 0, LEVEL_DEBUG, LEVEL_WARN, LEVEL_ERROR, LEVEL_FATAL };
    virtual ~RSLog() = default;
};

void RSB_EXPORT RSLogOutput(RSLog::Tag tag, RSLog::Level level, const char* format, ...);
} // namespace Rosen
} // namespace OHOS

#define ROSEN_LOGI(format, ...) \
    RSLogOutput(RSLog::Tag::RS_CLIENT, RSLog::Level::LEVEL_INFO, format, ##__VA_ARGS__)
#define ROSEN_LOGD(format, ...) \
    RSLogOutput(RSLog::Tag::RS_CLIENT, RSLog::Level::LEVEL_DEBUG, format, ##__VA_ARGS__)
#define ROSEN_LOGE(format, ...) \
    RSLogOutput(RSLog::Tag::RS_CLIENT, RSLog::Level::LEVEL_ERROR, format, ##__VA_ARGS__)
#define ROSEN_LOGW(format, ...) \
    RSLogOutput(RSLog::Tag::RS_CLIENT, RSLog::Level::LEVEL_WARN, format, ##__VA_ARGS__)
#define ROSEN_LOGF(format, ...) \
    RSLogOutput(RSLog::Tag::RS_CLIENT, RSLog::Level::LEVEL_FATAL, format, ##__VA_ARGS__)

#define RS_LOGI(format, ...) \
    RSLogOutput(RSLog::Tag::RS, RSLog::Level::LEVEL_INFO, format, ##__VA_ARGS__)
#define RS_LOGD(format, ...) \
    RSLogOutput(RSLog::Tag::RS, RSLog::Level::LEVEL_DEBUG, format, ##__VA_ARGS__)
#define RS_LOGE(format, ...) \
    RSLogOutput(RSLog::Tag::RS, RSLog::Level::LEVEL_ERROR, format, ##__VA_ARGS__)
#define RS_LOGW(format, ...) \
    RSLogOutput(RSLog::Tag::RS, RSLog::Level::LEVEL_WARN, format, ##__VA_ARGS__)
#define RS_LOGF(format, ...) \
    RSLogOutput(RSLog::Tag::RS, RSLog::Level::LEVEL_FATAL, format, ##__VA_ARGS__)

#endif // RENDER_SERVICE_BASE_CORE_COMMON_RS_LOG_H
