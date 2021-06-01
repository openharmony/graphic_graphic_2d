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

#ifndef FRAMEWORKS_VSYNC_INCLUDE_VSYNC_LOG_H
#define FRAMEWORKS_VSYNC_INCLUDE_VSYNC_LOG_H

#include <hilog/log.h>

#ifdef __aarch64__
#define VPUBI64  "%{public}ld"
#define VPUBSize "%{public}lu"
#define VPUBU64  "%{public}lu"
#else
#define VPUBI64  "%{public}lld"
#define VPUBSize "%{public}u"
#define VPUBU64  "%{public}llu"
#endif

#define __VLOG(func, fmt, ...) \
    func(LABEL, "%{public}s: " fmt "\033[0m", __func__, ##__VA_ARGS__)

#define VLOGD(fmt, ...) __VLOG(::OHOS::HiviewDFX::HiLog::Debug, "\033[0m" fmt, ##__VA_ARGS__)
#define VLOGI(fmt, ...) __VLOG(::OHOS::HiviewDFX::HiLog::Info,  "\033[36m" fmt, ##__VA_ARGS__)
#define VLOGW(fmt, ...) __VLOG(::OHOS::HiviewDFX::HiLog::Warn,  "\033[1;33m" fmt, ##__VA_ARGS__)
#define VLOGE(fmt, ...) __VLOG(::OHOS::HiviewDFX::HiLog::Error, "\033[1;31m" fmt, ##__VA_ARGS__)

#define VLOG_SUCCESS(fmt, ...) VLOGI("Success, Way: " fmt, ##__VA_ARGS__);
#define VLOG_FAILURE(fmt, ...) VLOGE("Failure, Reason: " fmt, ##__VA_ARGS__);
#define VLOG_FAILURE_NO(vsync_error) VLOG_FAILURE("%{public}s", VsyncErrorStr(vsync_error).c_str())
#define VLOG_FAILURE_RET(vsync_error) \
    do {                              \
        VLOG_FAILURE_NO(vsync_error); \
        return vsync_error;           \
    } while (0)
#define VLOG_FAILURE_API(api, ret) VLOG_FAILURE(#api " failed with %{public}s", VsyncErrorStr(ret).c_str())

#define VLOG_ERROR(errno, fmt, ...) \
    VLOGE(fmt ", means %{public}s", ##__VA_ARGS__, strerror(errno))

#define VLOG_ERROR_API(ret, api) \
    VLOG_ERROR(ret, #api " failed with %{public}d", ret)

#endif // FRAMEWORKS_VSYNC_INCLUDE_VSYNC_LOG_H
