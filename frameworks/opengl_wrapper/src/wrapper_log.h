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

#ifndef FRAMEWORKS_OPENGL_WRAPPER_WRAPPER_LOG_H
#define FRAMEWORKS_OPENGL_WRAPPER_WRAPPER_LOG_H

#include <hilog/log.h>

namespace OHOS {
namespace {
// The "0xD001400" is the domain ID for graphic module that alloted by the OS.
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001400, "OpenGLWrapper" };
}

#ifdef __aarch64__
#define WPUBI64  "%{public}ld"
#define WPUBSize "%{public}lu"
#define WPUBU64  "%{public}lu"
#else
#define WPUBI64  "%{public}lld"
#define WPUBSize "%{public}u"
#define WPUBU64  "%{public}llu"
#endif

#define _W_DFUNC HiviewDFX::HiLog::Debug
#define _W_IFUNC HiviewDFX::HiLog::Info
#define _W_WFUNC HiviewDFX::HiLog::Warn
#define _W_EFUNC HiviewDFX::HiLog::Error

#define _W_CPRINTF(func, fmt, ...) \
    func(LABEL, "<%{public}d>%{public}s: " fmt, \
        __LINE__, __func__, ##__VA_ARGS__)

#ifdef EGL_WRAPPER_DEBUG_ENABLE
#define WLOGD(fmt, ...) _W_CPRINTF(_W_DFUNC, fmt, ##__VA_ARGS__)
#else
#define WLOGD(fmt, ...)
#endif
#define WLOGI(fmt, ...) _W_CPRINTF(_W_IFUNC, fmt, ##__VA_ARGS__)
#define WLOGW(fmt, ...) _W_CPRINTF(_W_WFUNC, fmt, ##__VA_ARGS__)
#define WLOGE(fmt, ...) _W_CPRINTF(_W_EFUNC, fmt, ##__VA_ARGS__)

} // namespace OHOS

#endif // FRAMEWORKS_OPENGL_WRAPPER_WRAPPER_LOG_H
