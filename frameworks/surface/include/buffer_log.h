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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_BUFFER_LOG_H
#define FRAMEWORKS_SURFACE_INCLUDE_BUFFER_LOG_H

#include <hilog/log.h>

namespace OHOS {
namespace {
// The "0xD001401" is the domain ID for surface module that alloted by the OS.
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001401
#undef LOG_TAG
#define LOG_TAG "Bufferqueue"
}

#if (defined(__aarch64__) || defined(__x86_64__))
#define BPUBI64  "%{public}ld"
#define BPUB_SIZE "%{public}lu"
#define BPUBU64  "%{public}lu"
#else
#define BPUBI64  "%{public}lld"
#define BPUB_SIZE "%{public}u"
#define BPUBU64  "%{public}llu"
#endif

#define B_DFUNC HILOG_DEBUG
#define B_IFUNC HILOG_INFO
#define B_WFUNC HILOG_WARN
#define B_EFUNC HILOG_ERROR

#define B_CNPRINTF(func, fmt, ...) \
    func(LOG_CORE, "(%{public}s) %{public}s: " fmt, \
        name_.c_str(), __func__, ##__VA_ARGS__)

#define B_CPRINTF(func, fmt, ...) \
    func(LOG_CORE, "<%{public}d>%{public}s: " fmt, \
        __LINE__, __func__, ##__VA_ARGS__)

#define BLOGFD(fmt, ...) B_CPRINTF(B_DFUNC, "plz use self logfunc," fmt, ##__VA_ARGS__)
#define BLOGFI(fmt, ...) B_CPRINTF(B_IFUNC, "plz use self logfunc," fmt, ##__VA_ARGS__)
#define BLOGFW(fmt, ...) B_CPRINTF(B_WFUNC, "plz use self logfunc," fmt, ##__VA_ARGS__)
#define BLOGFE(fmt, ...) B_CPRINTF(B_EFUNC, "plz use self logfunc," fmt, ##__VA_ARGS__)

#define BLOGD(fmt, ...) B_CPRINTF(B_DFUNC, fmt, ##__VA_ARGS__)
#define BLOGI(fmt, ...) B_CPRINTF(B_IFUNC, fmt, ##__VA_ARGS__)
#define BLOGW(fmt, ...) B_CPRINTF(B_WFUNC, fmt, ##__VA_ARGS__)
#define BLOGE(fmt, ...) B_CPRINTF(B_EFUNC, fmt, ##__VA_ARGS__)

#define BLOGND(fmt, ...) B_CNPRINTF(B_DFUNC, fmt, ##__VA_ARGS__)
#define BLOGNI(fmt, ...) B_CNPRINTF(B_IFUNC, fmt, ##__VA_ARGS__)
#define BLOGNW(fmt, ...) B_CNPRINTF(B_WFUNC, fmt, ##__VA_ARGS__)
#define BLOGNE(fmt, ...) B_CNPRINTF(B_EFUNC, fmt, ##__VA_ARGS__)

#define BLOGN_SUCCESS(fmt, ...) \
    BLOGNI("Success, Way: " fmt, ##__VA_ARGS__)

#define BLOGN_SUCCESS_ID(id, fmt, ...) \
    BLOGNI("Success [%{public}d], Way: " fmt, id, ##__VA_ARGS__)

#define BLOGN_INVALID(fmt, ...) \
    BLOGNW("Invalid, " fmt, ##__VA_ARGS__)

#define BLOGN_FAILURE(fmt, ...) \
    BLOGNE("Failure, Reason: " fmt, ##__VA_ARGS__)

#define BLOGN_FAILURE_RET(ret)                                     \
    do {                                                          \
        BLOGN_FAILURE("%{public}s", GSErrorStr(ret).c_str()); \
        return ret;                                               \
    } while (0)

#define BLOGN_FAILURE_API(api, ret) \
    BLOGN_FAILURE(#api " failed, then %{public}s", GSErrorStr(ret).c_str())

#define BLOGN_FAILURE_ID(id, fmt, ...) \
    BLOGNE("Failure [%{public}d], Reason: " fmt, id, ##__VA_ARGS__)

#define BLOGN_FAILURE_ID_RET(id, ret)                                     \
    do {                                                                 \
        BLOGN_FAILURE_ID(id, "%{public}s", GSErrorStr(ret).c_str()); \
        return ret;                                                      \
    } while (0)

#define BLOGN_FAILURE_ID_API(id, api, ret) \
    BLOGN_FAILURE_ID(id, #api " failed, then %{public}s", GSErrorStr(ret).c_str())

#define BLOGE_CHECK_AND_RETURN_RET(cond, ret, fmt, ...)  \
    do {                                                 \
        if (!(cond)) {                                   \
            BLOGE(fmt, ##__VA_ARGS__);                   \
            return ret;                                  \
        }                                                \
    } while (0)

#define BLOGE_CHECK_AND_RETURN(cond, fmt, ...)           \
    do {                                                 \
        if (!(cond)) {                                   \
            BLOGE(fmt, ##__VA_ARGS__);                   \
            return;                                      \
        }                                                \
    } while (0)

#define BLOGE_CHECK_AND_BREAK(cond, fmt, ...)            \
    if (1) {                                             \
        if (!(cond)) {                                   \
            BLOGE(fmt, ##__VA_ARGS__);                   \
            break;                                       \
        }                                                \
    } else void (0)

#define BLOGE_CHECK_AND_CONTINUE(cond)                   \
    if (1) {                                             \
        if (!(cond)) {                                   \
            BLOGE("%{public}s, check failed!", #cond);   \
            continue;                                    \
        }                                                \
    } else void (0)
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_BUFFER_LOG_H
