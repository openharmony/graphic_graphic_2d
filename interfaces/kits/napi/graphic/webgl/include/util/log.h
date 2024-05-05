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
#include <cstdarg>
#include <cstdio>

#include "hilog/log.h"
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001400

#undef LOG_TAG
#define LOG_TAG "WebGL"

#define LOGI(format, ...) \
    HILOG_INFO(LOG_CORE, format, ##__VA_ARGS__)
#define LOGD(format, ...) \
    HILOG_DEBUG(LOG_CORE, format, ##__VA_ARGS__)
#define LOGE(format, ...) \
    HILOG_ERROR(LOG_CORE, format, ##__VA_ARGS__)
#define LOGW(format, ...) \
    HILOG_WARN(LOG_CORE, format, ##__VA_ARGS__)
#define LOGF(format, ...) \
    HILOG_FATAL(LOG_CORE, format, ##__VA_ARGS__)

#endif // WEBGL_LOG_H