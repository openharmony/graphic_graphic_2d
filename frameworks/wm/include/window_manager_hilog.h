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

#ifndef FRAMEWORKS_WM_INCLUDE_WINDOW_MANAGER_HILOG_H
#define FRAMEWORKS_WM_INCLUDE_WINDOW_MANAGER_HILOG_H

#include "hilog/log.h"
namespace OHOS {
static constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = {
    LOG_CORE,
    0,
    "WindowManager"
};

#define WMLOG_F(...)  (void)OHOS::HiviewDFX::HiLog::Fatal(LOG_LABEL, __VA_ARGS__)
#define WMLOG_E(...)  (void)OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, __VA_ARGS__)
#define WMLOG_W(...)  (void)OHOS::HiviewDFX::HiLog::Warn(LOG_LABEL, __VA_ARGS__)
#define WMLOG_I(...)  (void)OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, __VA_ARGS__)
#define WMLOG_D(...)  (void)OHOS::HiviewDFX::HiLog::Debug(LOG_LABEL, __VA_ARGS__)

#define _WM_DFUNC HiviewDFX::HiLog::Info
#define _WM_IFUNC HiviewDFX::HiLog::Info
#define _WM_WFUNC HiviewDFX::HiLog::Warn
#define _WM_EFUNC HiviewDFX::HiLog::Error

#define _WM_CPRINTF(func, fmt, ...) \
    func(LABEL, "<%{public}d>" fmt, __LINE__, ##__VA_ARGS__)

#define WMLOGD(fmt, ...) _WM_CPRINTF(_WM_DFUNC, " DEBUG " fmt, ##__VA_ARGS__)
#define WMLOGI(fmt, ...) _WM_CPRINTF(_WM_IFUNC, fmt, ##__VA_ARGS__)
#define WMLOGW(fmt, ...) _WM_CPRINTF(_WM_WFUNC, fmt, ##__VA_ARGS__)
#define WMLOGE(fmt, ...) _WM_CPRINTF(_WM_EFUNC, fmt, ##__VA_ARGS__)

#define _WM_FUNC __func__

#define WMLOGFD(fmt, ...) WMLOGD("%{public}s: " fmt, _WM_FUNC, ##__VA_ARGS__)
#define WMLOGFI(fmt, ...) WMLOGI("%{public}s: " fmt, _WM_FUNC, ##__VA_ARGS__)
#define WMLOGFW(fmt, ...) WMLOGW("%{public}s: " fmt, _WM_FUNC, ##__VA_ARGS__)
#define WMLOGFE(fmt, ...) WMLOGE("%{public}s: " fmt, _WM_FUNC, ##__VA_ARGS__)
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_WINDOW_MANAGER_HILOG_H
