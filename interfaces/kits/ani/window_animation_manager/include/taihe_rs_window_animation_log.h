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

#ifndef INTERFACES_KITS_ANI_TAIHE_RS_WINDOW_ANIMATION_LOG_H
#define INTERFACES_KITS_ANI_TAIHE_RS_WINDOW_ANIMATION_LOG_H

#include <hilog/log.h>

namespace OHOS {
namespace Rosen {
// The "0xD001400" is the domain ID for graphic module that allocated by the OS.
#define TWA_LOG_DOMAIN 0xD001404
#define TWA_LOG_TAG "TaiheRSWindowAnimation"
#define TWA_LOG_IMPL(level, fmt, ...) HiLogPrint(LOG_CORE, level, TWA_LOG_DOMAIN, TWA_LOG_TAG, fmt, ##__VA_ARGS__)

#define TWA_LOGD(fmt, ...) TWA_LOG_IMPL(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define TWA_LOGI(fmt, ...) TWA_LOG_IMPL(LOG_INFO, fmt, ##__VA_ARGS__)
#define TWA_LOGW(fmt, ...) TWA_LOG_IMPL(LOG_WARN, fmt, ##__VA_ARGS__)
#define TWA_LOGE(fmt, ...) TWA_LOG_IMPL(LOG_ERROR, fmt, ##__VA_ARGS__)
} // namespace Rosen
} // namespace OHOS

#endif // TAIHE_RS_WINDOW_ANIMATION_LOG_H