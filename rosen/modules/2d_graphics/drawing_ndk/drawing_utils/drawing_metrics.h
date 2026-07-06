/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef DRAWING_METRICS_H
#define DRAWING_METRICS_H

#if defined(HISTOGRAM_MANAGEMENT_ENABLE) && (HISTOGRAM_MANAGEMENT_ENABLE == 1)
#include "histogram_plugin_macros.h"
#define API_STATS_HISTOGRAM(...) \
    do { \
        static std::atomic<uint32_t> times{0}; \
        if (times.fetch_add(1, std::memory_order_relaxed) % 1024 == 0) { \
            HISTOGRAM_BOOLEAN(__VA_ARGS__); \
        } \
    } while (0)
#else
#define API_STATS_HISTOGRAM(...)
#endif

#endif // DRAWING_METRICS_H
