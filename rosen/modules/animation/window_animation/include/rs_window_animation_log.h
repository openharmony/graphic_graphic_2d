/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef WINDOW_ANIMATION_RS_WINDOW_ANIMATION_LOG_H
#define WINDOW_ANIMATION_RS_WINDOW_ANIMATION_LOG_H

#include <hilog/log.h>

namespace OHOS {
namespace Rosen {
namespace {
// The "0xD001400" is the domain ID for graphic module that allocated by the OS.
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001400, "RSWindowAnimation" };
}

#define ___WALOG(func, fmt, ...) func(LABEL, "%{public}s: " fmt, __func__, ##__VA_ARGS__)

#define WALOGD(fmt, ...) ___WALOG(::OHOS::HiviewDFX::HiLog::Debug, fmt, ##__VA_ARGS__)
#define WALOGI(fmt, ...) ___WALOG(::OHOS::HiviewDFX::HiLog::Info, fmt, ##__VA_ARGS__)
#define WALOGW(fmt, ...) ___WALOG(::OHOS::HiviewDFX::HiLog::Warn, fmt, ##__VA_ARGS__)
#define WALOGE(fmt, ...) ___WALOG(::OHOS::HiviewDFX::HiLog::Error, fmt, ##__VA_ARGS__)
} // namespace Rosen
} // namespace OHOS

#endif // WINDOW_ANIMATION_RS_WINDOW_ANIMATION_LOG_H