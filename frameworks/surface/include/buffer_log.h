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
#ifdef __aarch64__
#define BPUBI64  "%{public}ld"
#define BPUBSize "%{public}lu"
#define BPUBU64  "%{public}lu"
#else
#define BPUBI64  "%{public}lld"
#define BPUBSize "%{public}u"
#define BPUBU64  "%{public}llu"
#endif

#define _B_DFUNC HiviewDFX::HiLog::Debug
#define _B_IFUNC HiviewDFX::HiLog::Info
#define _B_WFUNC HiviewDFX::HiLog::Warn
#define _B_EFUNC HiviewDFX::HiLog::Error

#define _B_CNPRINTF(func, fmt, ...) \
    func(LABEL, "(%{public}s)<%{public}d>%{public}s: " fmt, \
        name_.c_str(), __LINE__, __func__, ##__VA_ARGS__)

#define _B_CPRINTF(func, fmt, ...) \
    func(LABEL, "<%{public}d>%{public}s: " fmt, \
        __LINE__, __func__, ##__VA_ARGS__)

#define BLOGFD(fmt, ...) _B_CPRINTF(_B_DFUNC, "plz use self logfunc," fmt, ##__VA_ARGS__)
#define BLOGFI(fmt, ...) _B_CPRINTF(_B_IFUNC, "plz use self logfunc," fmt, ##__VA_ARGS__)
#define BLOGFW(fmt, ...) _B_CPRINTF(_B_WFUNC, "plz use self logfunc," fmt, ##__VA_ARGS__)
#define BLOGFE(fmt, ...) _B_CPRINTF(_B_EFUNC, "plz use self logfunc," fmt, ##__VA_ARGS__)

#define BLOGD(fmt, ...) _B_CPRINTF(_B_DFUNC, fmt, ##__VA_ARGS__)
#define BLOGI(fmt, ...) _B_CPRINTF(_B_IFUNC, fmt, ##__VA_ARGS__)
#define BLOGW(fmt, ...) _B_CPRINTF(_B_WFUNC, fmt, ##__VA_ARGS__)
#define BLOGE(fmt, ...) _B_CPRINTF(_B_EFUNC, fmt, ##__VA_ARGS__)

#define BLOGND(fmt, ...) _B_CNPRINTF(_B_DFUNC, fmt, ##__VA_ARGS__)
#define BLOGNI(fmt, ...) _B_CNPRINTF(_B_IFUNC, fmt, ##__VA_ARGS__)
#define BLOGNW(fmt, ...) _B_CNPRINTF(_B_WFUNC, fmt, ##__VA_ARGS__)
#define BLOGNE(fmt, ...) _B_CNPRINTF(_B_EFUNC, fmt, ##__VA_ARGS__)

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
        BLOGN_FAILURE("%{public}s", SurfaceErrorStr(ret).c_str()); \
        return ret;                                               \
    } while (0)

#define BLOGN_FAILURE_API(api, ret) \
    BLOGN_FAILURE(#api " failed, then %{public}s", SurfaceErrorStr(ret).c_str())

#define BLOGN_FAILURE_ID(id, fmt, ...) \
    BLOGNE("Failure [%{public}d], Reason: " fmt, id, ##__VA_ARGS__)

#define BLOGN_FAILURE_ID_RET(id, ret)                                     \
    do {                                                                 \
        BLOGN_FAILURE_ID(id, "%{public}s", SurfaceErrorStr(ret).c_str()); \
        return ret;                                                      \
    } while (0)

#define BLOGN_FAILURE_ID_API(id, api, ret) \
    BLOGN_FAILURE_ID(id, #api " failed, then %{public}s", SurfaceErrorStr(ret).c_str())
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_BUFFER_LOG_H
