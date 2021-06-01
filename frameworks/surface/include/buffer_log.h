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
#define _B_FMT(color) "\033[" color "m"

#ifdef __aarch64__
#define BPUBI64  "%{public}ld"
#define BPUBSize "%{public}lu"
#define BPUBU64  "%{public}lu"
#else
#define BPUBI64  "%{public}lld"
#define BPUBSize "%{public}u"
#define BPUBU64  "%{public}llu"
#endif

#define _B_RESET     ";0"
#define _B_BOLD      ";1"
#define _B_UNDERLINE ";4"
#define _B_REVERSE   ";7"
#define _B_FG_BLACK  ";30"
#define _B_FG_RED    ";31"
#define _B_FG_GREEN  ";32"
#define _B_FG_YELLOW ";33"
#define _B_FG_BLUE   ";34"
#define _B_FG_PURPLE ";35"
#define _B_FG_CYAN   ";36"
#define _B_FG_WHITE  ";37"

#define _B_DCOLOR _B_FG_WHITE
#define _B_ICOLOR _B_FG_CYAN
#define _B_WCOLOR _B_FG_YELLOW
#define _B_ECOLOR _B_FG_RED _B_BOLD

#define _B_DFUNC HiviewDFX::HiLog::Debug
#define _B_IFUNC HiviewDFX::HiLog::Info
#define _B_WFUNC HiviewDFX::HiLog::Warn
#define _B_EFUNC HiviewDFX::HiLog::Error

#define _B_CPRINTF(func, color, fmt, ...) \
    func(LABEL, _B_FMT(color) "<%{public}d>" fmt _B_FMT(_B_RESET), __LINE__, ##__VA_ARGS__)

#define BLOGD(fmt, ...) _B_CPRINTF(_B_DFUNC, _B_DCOLOR, " DEBUG " fmt, ##__VA_ARGS__)
#define BLOGI(fmt, ...) _B_CPRINTF(_B_IFUNC, _B_ICOLOR, fmt, ##__VA_ARGS__)
#define BLOGW(fmt, ...) _B_CPRINTF(_B_WFUNC, _B_WCOLOR, fmt, ##__VA_ARGS__)
#define BLOGE(fmt, ...) _B_CPRINTF(_B_EFUNC, _B_ECOLOR, fmt, ##__VA_ARGS__)

#define _B_FUNC __func__

#define BLOGFD(fmt, ...) BLOGD("%{public}s: " fmt, _B_FUNC, ##__VA_ARGS__)
#define BLOGFI(fmt, ...) BLOGI("%{public}s: " fmt, _B_FUNC, ##__VA_ARGS__)
#define BLOGFW(fmt, ...) BLOGW("%{public}s: " fmt, _B_FUNC, ##__VA_ARGS__)
#define BLOGFE(fmt, ...) BLOGE("%{public}s: " fmt, _B_FUNC, ##__VA_ARGS__)

#define BLOG_SUCCESS(fmt, ...) BLOGFI("Success, Way: " fmt, ##__VA_ARGS__)
#define BLOG_SUCCESS_ID(id, fmt, ...) BLOGFI("Success [%{public}d], Way: " fmt, id, ##__VA_ARGS__)

#define BLOG_INVALID(fmt, ...) BLOGFW("Invalid, " fmt, ##__VA_ARGS__)

#define BLOG_FAILURE(fmt, ...) BLOGFE("Failure, Reason: " fmt, ##__VA_ARGS__)
#define BLOG_FAILURE_RET(ret)                                     \
    do {                                                          \
        BLOG_FAILURE("%{public}s", SurfaceErrorStr(ret).c_str()); \
        return ret;                                               \
    } while (0)
#define BLOG_FAILURE_API(api, ret) \
    BLOG_FAILURE(#api " failed, then %{public}s", SurfaceErrorStr(ret).c_str())

#define BLOG_FAILURE_ID(id, fmt, ...) BLOGFE("Failure [%{public}d], Reason: " fmt, id, ##__VA_ARGS__)
#define BLOG_FAILURE_ID_RET(id, ret)                                     \
    do {                                                                 \
        BLOG_FAILURE_ID(id, "%{public}s", SurfaceErrorStr(ret).c_str()); \
        return ret;                                                      \
    } while (0)
#define BLOG_FAILURE_ID_API(id, api, ret) \
    BLOG_FAILURE_ID(id, #api " failed, then %{public}s", SurfaceErrorStr(ret).c_str())
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_BUFFER_LOG_H
