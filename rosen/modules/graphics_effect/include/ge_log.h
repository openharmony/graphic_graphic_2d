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

#ifndef GRAPHICS_EFFECT_LOG_H
#define GRAPHICS_EFFECT_LOG_H

#include <chrono>
#include <cstdint>
#include <functional>
#include <hilog/log.h>

namespace OHOS {
namespace Rosen {

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001406

#undef LOG_TAG
#define LOG_TAG "OHOS::GE"

#define LOGI(format, ...) HILOG_INFO(LOG_CORE, format, ##__VA_ARGS__)
#define LOGD(format, ...) HILOG_DEBUG(LOG_CORE, format, ##__VA_ARGS__)
#define LOGE(format, ...) HILOG_ERROR(LOG_CORE, format, ##__VA_ARGS__)
#define LOGW(format, ...) HILOG_WARN(LOG_CORE, format, ##__VA_ARGS__)
#define LOGF(format, ...) HILOG_FATAL(LOG_CORE, format, ##__VA_ARGS__)

#define GE_LOGI(format, ...) HILOG_INFO(LOG_CORE, format, ##__VA_ARGS__)
#define GE_LOGD(format, ...) HILOG_DEBUG(LOG_CORE, format, ##__VA_ARGS__)
#define GE_LOGE(format, ...) HILOG_ERROR(LOG_CORE, format, ##__VA_ARGS__)
#define GE_LOGW(format, ...) HILOG_WARN(LOG_CORE, format, ##__VA_ARGS__)
#define GE_LOGF(format, ...) HILOG_FATAL(LOG_CORE, format, ##__VA_ARGS__)
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_LOG_H
