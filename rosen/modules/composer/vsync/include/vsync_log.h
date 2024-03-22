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

namespace OHOS {
namespace {
// The "0xD001400" is the domain ID for graphic module that alloted by the OS.
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001400
#undef LOG_TAG
#define LOG_TAG "Vsync"
}

#if (defined(__aarch64__) || defined(__x86_64__))
#define VPUBI64  "%{public}ld"
#define VPUB_SIZE "%{public}lu"
#define VPUBU64  "%{public}lu"
#else
#define VPUBI64  "%{public}lld"
#define VPUB_SIZE "%{public}u"
#define VPUBU64  "%{public}llu"
#endif

#define VLOG_BASE(func, fmt, ...) \
    func(LOG_CORE, "%{public}s: " fmt, __func__, ##__VA_ARGS__)

#ifdef DEBUG
#define VLOGD(fmt, ...) VLOG_BASE(HILOG_DEBUG, fmt, ##__VA_ARGS__)
#else
#define VLOGD(fmt, ...)
#endif
#define VLOGI(fmt, ...) VLOG_BASE(HILOG_INFO, fmt, ##__VA_ARGS__)
#define VLOGW(fmt, ...) VLOG_BASE(HILOG_WARN, fmt, ##__VA_ARGS__)
#define VLOGE(fmt, ...) VLOG_BASE(HILOG_ERROR, fmt, ##__VA_ARGS__)

#define VLOG_SUCCESS(fmt, ...) VLOGI("Success, Way: " fmt, ##__VA_ARGS__)
#define VLOG_FAILURE(fmt, ...) VLOGE("Failure, Reason: " fmt, ##__VA_ARGS__)
#define VLOG_FAILURE_NO(vsync_error) VLOG_FAILURE("%{public}s", GSErrorStr(vsync_error).c_str())
#define VLOG_FAILURE_RET(vsync_error) \
    do {                              \
        VLOG_FAILURE_NO(vsync_error); \
        return vsync_error;           \
    } while (0)
#define VLOG_FAILURE_API(api, ret) VLOG_FAILURE(#api " failed with %{public}s", GSErrorStr(ret).c_str())

#define VLOG_ERROR(errno, fmt, ...) \
    VLOGE(fmt ", means %{public}s", ##__VA_ARGS__, strerror(errno))

#define VLOG_ERROR_API(ret, api) \
    VLOG_ERROR(ret, #api " failed with %{public}d", ret)
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_INCLUDE_VSYNC_LOG_H