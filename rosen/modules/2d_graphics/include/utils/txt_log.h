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

#ifndef TXT_LOG_H
#define TXT_LOG_H

#ifdef HM_TXT_ENABLE
#include <hilog/log.h>
#endif

namespace OHOS {
namespace Rosen {

#ifdef HM_TXT_ENABLE

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001408

#undef LOG_TAG
#define LOG_TAG "Txt"

#define TXT_LOGD(fmt, ...)               \
    HILOG_DEBUG(LOG_CORE, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define TXT_LOGI(fmt, ...)              \
    HILOG_INFO(LOG_CORE, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define TXT_LOGW(fmt, ...)              \
    HILOG_WARN(LOG_CORE, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define TXT_LOGE(fmt, ...)               \
    HILOG_ERROR(LOG_CORE, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#else
#define TXT_LOGD(fmt, ...)
#define TXT_LOGI(fmt, ...)
#define TXT_LOGW(fmt, ...)
#define TXT_LOGE(fmt, ...)
#endif
} // namespace Rosen
} // namespace OHOS
#endif
