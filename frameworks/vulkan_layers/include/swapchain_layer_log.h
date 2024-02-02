/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef SWAPCHAIN_LAYER_LOG_H
#define SWAPCHAIN_LAYER_LOG_H

#include <hilog/log.h>
namespace OHOS {
namespace {
// The "0xD001405" is the domain ID for graphic module that alloted by the OS.
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001405, "VulkanSwapchainLayer" };
}

#define SWLOGD(fmt, ...) OHOS::HiviewDFX::HiLog::Debug(LABEL, \
    "<%{public}d>%{public}s: " fmt, __LINE__, __func__, ##__VA_ARGS__)
#define SWLOGI(fmt, ...) OHOS::HiviewDFX::HiLog::Info(LABEL, \
    "<%{public}d>%{public}s: " fmt, __LINE__, __func__, ##__VA_ARGS__)
#define SWLOGW(fmt, ...) OHOS::HiviewDFX::HiLog::Warn(LABEL, \
    "<%{public}d>%{public}s: " fmt, __LINE__, __func__, ##__VA_ARGS__)
#define SWLOGE(fmt, ...) OHOS::HiviewDFX::HiLog::Error(LABEL, \
    "<%{public}d>%{public}s: " fmt, __LINE__, __func__, ##__VA_ARGS__)
} // namespace OHOS
#endif // SWAPCHAIN_LAYER_LOG_H
