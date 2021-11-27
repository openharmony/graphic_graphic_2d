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

#ifndef FRAMEWORKS_DUMPER_INCLUDE_GRAPHIC_DUMPER_HILOG_H
#define FRAMEWORKS_DUMPER_INCLUDE_GRAPHIC_DUMPER_HILOG_H

#include "hilog/log.h"
namespace OHOS {
#define GD_CPRINTF(func, fmt, ...) func(LABEL, "<%{public}d>" fmt, __LINE__, ##__VA_ARGS__)

#define GDLOGD(fmt, ...) GD_CPRINTF(HiviewDFX::HiLog::Debug, fmt, ##__VA_ARGS__)
#define GDLOGI(fmt, ...) GD_CPRINTF(HiviewDFX::HiLog::Info, fmt, ##__VA_ARGS__)
#define GDLOGW(fmt, ...) GD_CPRINTF(HiviewDFX::HiLog::Warn, fmt, ##__VA_ARGS__)
#define GDLOGE(fmt, ...) GD_CPRINTF(HiviewDFX::HiLog::Error, fmt, ##__VA_ARGS__)

#define GDLOGFD(fmt, ...) GDLOGD("%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define GDLOGFI(fmt, ...) GDLOGI("%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define GDLOGFW(fmt, ...) GDLOGW("%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define GDLOGFE(fmt, ...) GDLOGE("%{public}s: " fmt, __func__, ##__VA_ARGS__)

#define GDLOG_SUCCESS(fmt, ...) GDLOGI("Success, Way: " fmt, ##__VA_ARGS__)
#define GDLOG_FAILURE(fmt, ...) GDLOGE("Failure, Reason: " fmt, ##__VA_ARGS__)
#define GDLOG_FAILURE_NO(gs_error) GDLOG_FAILURE("%{public}s", GSErrorStr(gs_error).c_str())
#define GDLOG_FAILURE_RET(gs_error) \
    do {                              \
        GDLOG_FAILURE_NO(gs_error); \
        return gs_error;           \
    } while (0)
#define GDLOG_FAILURE_API(api, ret) GDLOG_FAILURE(#api " failed with %{public}s", GSErrorStr(ret).c_str())

#define GDLOG_ERROR(errno, fmt, ...) \
    GDLOGE(fmt ", means %{public}s", ##__VA_ARGS__, strerror(errno))

#define GDLOG_ERROR_API(ret, api) \
    GDLOG_ERROR(ret, #api " failed with %{public}d", ret)
} // namespace OHOS

#endif // FRAMEWORKS_DUMPER_INCLUDE_GRAPHIC_DUMPER_HILOG_H
