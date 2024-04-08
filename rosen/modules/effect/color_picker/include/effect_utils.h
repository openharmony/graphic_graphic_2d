/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef EFFECT_UTILS_H
#define EFFECT_UTILS_H

#include <hilog/log.h>

namespace OHOS {
namespace Rosen {
// The "0xD001400" is the domain ID for graphic module that alloted by the OS.
#undef LOG_DOMAIN
#define LOG_DOMAIN 1405

#undef LOG_TAG
#define LOG_TAG "EffectNapi"

#define EFFECT_LOG_D(fmt, ...) HILOG_DEBUG(LOG_CORE, fmt, ##__VA_ARGS__)

#define EFFECT_LOG_I(fmt, ...) HILOG_INFO(LOG_CORE, fmt, ##__VA_ARGS__)

#define EFFECT_LOG_E(fmt, ...) HILOG_ERROR(LOG_CORE, fmt, ##__VA_ARGS__)
} // namespace Rosen
} // namespace OHOS
#endif // EFFECT_UTILS_H
