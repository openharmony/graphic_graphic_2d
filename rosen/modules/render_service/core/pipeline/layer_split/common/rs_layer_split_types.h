/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef RS_LAYER_SPLIT_TYPES_H
#define RS_LAYER_SPLIT_TYPES_H
#include <stdint.h>
#include <deque>
#include <mutex>
#include <memory>

namespace OHOS::Rosen {

enum class PlanStatus : int8_t {
    OFF,
    PREPARE,
    ON,
    LEAVE
};

enum class SurfaceStatus : int8_t {
    INIT,
    REGISTER,
    UNREGISTER,
    UNKNOWN
};

} // namespace OHOS::Rosen

#define LAYER_SPLIT_LOG_ENABLE
#ifdef LAYER_SPLIT_LOG_ENABLE
#define LAYER_SPLIT_LOGI(format, ...)                                 \
    do {                                                          \
        HILOG_INFO(LOG_CORE, "[LAYER_SPLIT] " format, ##__VA_ARGS__); \
    } while (0)

#define LAYER_SPLIT_LOGD(format, ...)                                  \
    do {                                                           \
        HILOG_DEBUG(LOG_CORE, "[LAYER_SPLIT] " format, ##__VA_ARGS__); \
    } while (0)
#else
#define LAYER_SPLIT_LOGI(format, ...)
#define LAYER_SPLIT_LOGD(format, ...)
#endif

#define LAYER_SPLIT_LOGW(format, ...)                                 \
    do {                                                          \
        HILOG_WARN(LOG_CORE, "[LAYER_SPLIT] " format, ##__VA_ARGS__); \
    } while (0)

#define LAYER_SPLIT_LOGE(format, ...)                                  \
    do {                                                           \
        HILOG_ERROR(LOG_CORE, "[LAYER_SPLIT] " format, ##__VA_ARGS__); \
    } while (0)

#endif // RS_LAYER_SPLIT_TYPES_H