/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef HGM_CONFIG_LOG_H
#define HGM_CONFIG_LOG_H

#include <hilog/log.h>

namespace OHOS {
#if (defined(__aarch64__) || defined(__x86_64__))
#define PUBI64 "%{public}ld"
#define PUBU64 "%{public}lu"
#else
#define PUBI64 "%{public}lld"
#define PUBU64 "%{public}llu"
#endif


#define HGM_CPRINTF(func, fmt, ...) \
    HILOG_IMPL(LOG_CORE, func, 0xD001407, "graphic_2d_hgm", "<%{public}d>%{public}s: " fmt, \
        __LINE__, __func__, ##__VA_ARGS__)

#define HGM_LOGD(fmt, ...) HGM_CPRINTF(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define HGM_LOGI(fmt, ...) HGM_CPRINTF(LOG_INFO, fmt, ##__VA_ARGS__)
#define HGM_LOGW(fmt, ...) HGM_CPRINTF(LOG_WARN, fmt, ##__VA_ARGS__)
#define HGM_LOGE(fmt, ...) HGM_CPRINTF(LOG_ERROR, fmt, ##__VA_ARGS__)
} // namespace OHOS
#endif // HGM_CONFIG_LOG_H
