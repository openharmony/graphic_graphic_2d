/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef CJ_UI_EFFECT_LOG_H
#define CJ_UI_EFFECT_LOG_H

#include <cstdint>
#include <hilog/log.h>

namespace OHOS {
namespace Rosen {
const int32_t CJ_OK = 0;
const int32_t CJ_ERR_ILLEGAL_INSTANCE = -1;

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001405

#ifdef CJ_FILTER_H
#undef LOG_TAG
#define LOG_TAG "UiEffect_Filter"
#define FILTER_LOG_D(fmt, ...) HILOG_DEBUG(LOG_CORE, fmt, ##__VA_ARGS__)
#define FILTER_LOG_I(fmt, ...) HILOG_INFO(LOG_CORE, fmt, ##__VA_ARGS__)
#define FILTER_LOG_E(fmt, ...) HILOG_ERROR(LOG_CORE, fmt, ##__VA_ARGS__)
#else
#undef LOG_TAG
#define LOG_TAG "UiEffect_Effect"
#define UIEFFECT_LOG_D(fmt, ...) HILOG_DEBUG(LOG_CORE, fmt, ##__VA_ARGS__)
#define UIEFFECT_LOG_I(fmt, ...) HILOG_INFO(LOG_CORE, fmt, ##__VA_ARGS__)
#define UIEFFECT_LOG_E(fmt, ...) HILOG_ERROR(LOG_CORE, fmt, ##__VA_ARGS__)
#endif
} // namespace Rosen
} // namespace OHOS
#endif
