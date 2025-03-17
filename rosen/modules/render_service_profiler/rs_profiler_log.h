/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RS_PROFILER_LOG_H
#define RS_PROFILER_LOG_H

#include "platform/common/rs_log.h"

// NOLINTBEGIN
#define HRPI(format, ...) RS_LOGI("[HRP] " format, ##__VA_ARGS__)
#define HRPD(format, ...) RS_LOGD("[HRP] " format, ##__VA_ARGS__)
#define HRPE(format, ...) RS_LOGE("[HRP] " format, ##__VA_ARGS__)
#define HRPW(format, ...) RS_LOGW("[HRP] " format, ##__VA_ARGS__)
#define HRPIDN(format, ...) ((CONDITION(DEBUG_NODE)) ? RS_LOGI("[HRP] " format, ##__VA_ARGS__) : (void)0)
#define HRPIDM(format, ...) ((CONDITION(DEBUG_MODIFIER)) ? RS_LOGI("[HRP] " format, ##__VA_ARGS__) : (void)0)
// NOLINTEND

#endif // RS_PROFILER_LOG_H