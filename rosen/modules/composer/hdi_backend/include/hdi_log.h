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

#ifndef HDI_BACKEND_HDI_LOG_H
#define HDI_BACKEND_HDI_LOG_H

#include <hilog/log.h>

#include "graphic_error.h"

namespace OHOS {
namespace Rosen {

namespace {
// The "0xD001400" is the domain ID for graphic module that alloted by the OS.
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001400
#undef LOG_TAG
#define LOG_TAG "Composer"
}

#define __HLOG(func, fmt, ...) \
    func(LOG_CORE, "%{public}s: " fmt, __func__, ##__VA_ARGS__)

#define HLOGD(fmt, ...) __HLOG(HILOG_DEBUG, fmt, ##__VA_ARGS__)
#define HLOGI(fmt, ...) __HLOG(HILOG_INFO, fmt, ##__VA_ARGS__)
#define HLOGW(fmt, ...) __HLOG(HILOG_WARN, fmt, ##__VA_ARGS__)
#define HLOGE(fmt, ...) __HLOG(HILOG_ERROR, fmt, ##__VA_ARGS__)

#define HLOG_SUCCESS(fmt, ...) HLOGI("Success, Way: " fmt, ##__VA_ARGS__)
#define HLOG_FAILURE(fmt, ...) HLOGE("Failure, Reason: " fmt, ##__VA_ARGS__)

#define HLOG_ERROR(errno, fmt, ...) \
    HLOGE(fmt ", means %{public}s", ##__VA_ARGS__, strerror(errno))

#define HLOG_ERROR_API(ret, api) \
    HLOG_ERROR(ret, #api " failed with %{public}d", ret)

} // namespace Rosen
} // namespace OHOS

#endif // HDI_BACKEND_HDI_LOG_H