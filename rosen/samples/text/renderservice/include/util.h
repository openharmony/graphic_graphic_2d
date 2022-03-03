/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef FRAMEWORKS_RENDERSERVER_INCLUDE_UTIL_H
#define FRAMEWORKS_RENDERSERVER_INCLUDE_UTIL_H

#include <cstdint>
#include <functional>

#include <hilog/log.h>

namespace OHOS {
#define LOG(fmt, ...) ::OHOS::HiviewDFX::HiLog::Info(             \
    ::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "RenderService"}, \
    "%{public}s: " fmt, __func__, ##__VA_ARGS__)

int64_t GetNowTime();
void PostTask(std::function<void()> func, uint32_t delayTime = 0);
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_UTIL_H
