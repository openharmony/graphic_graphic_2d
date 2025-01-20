/*
 * Copyright (c)  2025 Huawei Device Co., Ltd.
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
#include "utils/perfmonitor_reporter.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void PerfmonitorReporter::ResetStatsData()
{
    StaticFactory::ResetStatsData();
}
void PerfmonitorReporter::ResetPerfEventData()
{
    StaticFactory::ResetPerfEventData();
}

std::map<std::string, std::vector<uint16_t>> PerfmonitorReporter::GetBlurStatsData()
{
    return StaticFactory::GetBlurStatsData();
}
 std::map<std::string, RsBlurEvent> PerfmonitorReporter::GetBlurPerfEventData()
{
    return StaticFactory::GetBlurPerfEventData();
}

std::map<std::string, std::vector<uint16_t>> PerfmonitorReporter::GetTextureStatsData()
{
    return StaticFactory::GetTextureStatsData();
}

std::map<std::string, RsTextureEvent> PerfmonitorReporter::GetTexturePerfEventData()
{
    return StaticFactory::GetTexturePerfEventData();
}

int16_t PerfmonitorReporter::GetSplitRange(int64_t duration)
{
    return StaticFactory::GetSplitRange(duration);
}

bool PerfmonitorReporter::IsOpenPerf()
{
    return StaticFactory::IsOpenPerf();
}

int64_t PerfmonitorReporter::GetCurrentTime()
{
    return StaticFactory::GetCurrentTime();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
