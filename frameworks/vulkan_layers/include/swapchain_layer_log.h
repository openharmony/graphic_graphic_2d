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

#undef LOG_DOMAIN
#undef LOG_TAG
// The "0xD001405" is the domain ID for graphic module that alloted by the OS.
#define LOG_DOMAIN 0xD001405
#define LOG_TAG "VulkanSwapchainLayer"

#define SWLOGD(fmt, ...) HILOG_DEBUG(LOG_CORE, "<%{public}d>%{public}s: " fmt, __LINE__, __func__, ##__VA_ARGS__)
#define SWLOGI(fmt, ...) HILOG_INFO(LOG_CORE, "<%{public}d>%{public}s: " fmt, __LINE__, __func__, ##__VA_ARGS__)
#define SWLOGW(fmt, ...) HILOG_WARN(LOG_CORE, "<%{public}d>%{public}s: " fmt, __LINE__, __func__, ##__VA_ARGS__)
#define SWLOGE(fmt, ...) HILOG_ERROR(LOG_CORE, "<%{public}d>%{public}s: " fmt, __LINE__, __func__, ##__VA_ARGS__)
#define SWLOGF(fmt, ...) HILOG_FATAL(LOG_CORE, "<%{public}d>%{public}s: " fmt, __LINE__, __func__, ##__VA_ARGS__)
} // namespace OHOS
#endif // SWAPCHAIN_LAYER_LOG_H
