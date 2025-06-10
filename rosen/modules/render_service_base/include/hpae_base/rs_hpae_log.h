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

#ifndef RENDER_SERVICE_BASE_INCLUDE_HPAE_BASE_LOG_H
#define RENDER_SERVICE_BASE_INCLUDE_HPAE_BASE_LOG_H

#include "rs_trace.h"
#ifdef OHOS_PLATFORM
#include <hilog/log.h>
#endif

#if 1

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001406

#undef LOG_TAG
#define LOG_TAG "OHOS::RS"

#define HPAE_LOGE(format, ...) \
    HILOG_ERROR(LOG_CORE, "[HPAE] " format, ##__VA_ARGS__)

#define HPAE_LOGE(format, ...) \
    HILOG_INFO(LOG_CORE, "[HPAE] " format, ##__VA_ARGS__)

#define HPAE_LOGE(format, ...) \
    HILOG_WARN(LOG_CORE, "[HPAE] " format, ##__VA_ARGS__)

#define HPAE_LOGE(format, ...) \
    HILOG_DEBUG(LOG_CORE, "[HPAE] " format, ##__VA_ARGS__)

#define HPAE_TRACE_NAME(name) \
    HIREACE_METER_NAME(HITRACE_TAG_GRAPHIC_AGP, name)

#define HPAE_TRACE_NAME_FMT(fmt, ...) \
    HIREACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, fmt, ##__VA_AGES__)

#define HPAE_TRACE_BEGIN(name) \
    StartTrace(HITRACE_TAG_GRAPHIC_AGP, name)

#define HPAE_TRACE_END() \
    FinishTrace(HITRACE_TAG_GRAPHIC_AGP)

#else

#define HPAE_LOGE(format, ...)
#define HPAE_LOGW(format, ...)
#define HPAE_LOGI(format, ...)
#define HPAE_LOGD(format, ...)

#define HPAE_TRACE_NAME(name)
#define HPAE_TRACE_NAME_FMT(fmt, ...)
#define HPAE_TRACE_BEGIN(name)
#define HPAE_TRACE_END()

#endif // LOG switch

#endif // RENDER_SERVICE_BASE_INCLUDE_HPAE_BASE_LOG_H