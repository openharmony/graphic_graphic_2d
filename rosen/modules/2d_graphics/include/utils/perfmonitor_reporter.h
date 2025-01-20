/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef PERFMONITOR_REPORTER_H
#define PERFMONITOR_REPORTER_H

#include "static_factory.h"
#include "utils/drawing_macros.h"
#include "utils/perfevent.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API PerfmonitorReporter {
public:
    static void ResetStatsData();
    static void ResetPerfEventData();
    static std::map<std::string, std::vector<uint16_t>> GetBlurStatsData();
    static std::map<std::string, RsBlurEvent> GetBlurPerfEventData();
    static std::map<std::string, std::vector<uint16_t>> GetTextureStatsData();
    static std::map<std::string, RsTextureEvent> GetTexturePerfEventData();
    static int16_t GetSplitRange(int64_t duration);
    static bool IsOpenPerf();
    static int64_t GetCurrentTime();
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
