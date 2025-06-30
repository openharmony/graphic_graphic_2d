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

#ifndef RS_CORE_FEATURE_HPAE_OFFLINE_UTIL_H
#define RS_CORE_FEATURE_HPAE_OFFLINE_UTIL_H
#include "platform/common/rs_log.h"

#if defined(ROSEN_OHOS)
#define RS_OFFLINE_LOGD(fmt, ...) RS_LOGD("hpae_offline: " fmt, ##__VA_ARGS__)
#define RS_OFFLINE_LOGI(fmt, ...) RS_LOGI("hpae_offline: " fmt, ##__VA_ARGS__)
#define RS_OFFLINE_LOGW(fmt, ...) RS_LOGW("hpae_offline: " fmt, ##__VA_ARGS__)
#define RS_OFFLINE_LOGE(fmt, ...) RS_LOGE("hpae_offline: " fmt, ##__VA_ARGS__)
#else
#define RS_OFFLINE_LOGD(fmt, ...)
#define RS_OFFLINE_LOGI(fmt, ...)
#define RS_OFFLINE_LOGW(fmt, ...)
#define RS_OFFLINE_LOGE(fmt, ...)
#endif

#endif // RS_CORE_FEATURE_HPAE_OFFLINE_UTIL_H