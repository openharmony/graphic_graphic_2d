/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_CJ_DRAWING_LOG_H
#define OHOS_CJ_DRAWING_LOG_H

#include "hilog/log.h"

#ifdef LOG_DOMAIN
#undef LOG_DOMAIN
#endif
#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_DOMAIN 0xD001400
#define LOG_TAG "CjDrawing"

#define ROSEN_LOGI(format, ...) HILOG_INFO(LOG_CORE, format, ##__VA_ARGS__)
#define ROSEN_LOGD(format, ...) HILOG_DEBUG(LOG_CORE, format, ##__VA_ARGS__)
#define ROSEN_LOGE(format, ...) HILOG_ERROR(LOG_CORE, format, ##__VA_ARGS__)

#endif