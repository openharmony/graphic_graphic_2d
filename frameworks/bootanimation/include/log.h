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

static OHOS::HiviewDFX::HiLogLabel label_boot = { LOG_CORE, 0xD001400, "BootAnimation" };
#define LOGD(fmt, ...) LOGPRINT(::OHOS::HiviewDFX::HiLog::Debug, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) LOGPRINT(::OHOS::HiviewDFX::HiLog::Info, fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) LOGPRINT(::OHOS::HiviewDFX::HiLog::Warn, fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) LOGPRINT(::OHOS::HiviewDFX::HiLog::Error, fmt, ##__VA_ARGS__)
#define LOGPRINT(func, fmt, ...) func(label_boot, \
    "%{public}s: " fmt, __func__, ##__VA_ARGS__)
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_LOG_H
