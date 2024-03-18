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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_LOG_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_LOG_H

#include <hilog/log.h>

namespace OHOS {
#if (defined(__aarch64__) || defined(__x86_64__))
#define BPUBI64  "%{public}ld"
#define BPUB_SIZE "%{public}lu"
#define BPUBU64  "%{public}lu"
#else
#define BPUBI64  "%{public}lld"
#define BPUB_SIZE "%{public}u"
#define BPUBU64  "%{public}llu"
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001400

#undef LOG_TAG
#define LOG_TAG "BootAnimation"

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
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_LOG_H
