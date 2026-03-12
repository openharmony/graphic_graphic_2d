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

#include <string>

#include "gtest/gtest.h"
#include "rs_profiler.h"
#include "rs_profiler_telemetry.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

bool ContainAll(const std::string& text, std::vector<std::string> items)
{
    bool result = true;
    for (auto& item : items) {
        result = result && (text.find(item) != std::string::npos);
    }
    return result;
}

/*
 * @tc.name: ContainAll
 * @tc.desc: Test ContainAll
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RenderProfilerTelemetryTest, ContainAll, Level1)
{
    std::string sample = "\nTemperature: 0.000000 \xC2\xB0"
                         "C\nCurrent: 0.000000 mA\nVoltage: 0.000000 V"
                         "\nRAM Total: 0.000000 GB\nRAM Free: 0.000000 GB"
                         "\nGPU: 0.000000 GHz(min: 0.000000 GHz max: 0.000000 GHz load: 0.000000 %)";
    EXPECT_TRUE(ContainAll(sample, { "Temperature:", "Current:", "Voltage:", "RAM Total" }));
    EXPECT_FALSE(ContainAll(sample, { "lorem", "ipsum", "non", "exist" }));
    EXPECT_FALSE(
        ContainAll(sample, { "Temperature:", "Current:", "Voltage:", "RAM Total", "lorem", "ipsum", "non", "exist" }));
}

/*
 * @tc.name: GetDeviceInfoString
 * @tc.desc: Test GetDeviceInfoString
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RenderProfilerTelemetryTest, GetDeviceInfoString, Level1)
{
    auto info = RSTelemetry::GetDeviceInfoString();

    EXPECT_FALSE(info.empty());
    EXPECT_TRUE(ContainAll(info, { "Temperature:", "Current:", "Voltage:", "RAM Total:", "RAM Free:", "GPU:" }));
}

/*
 * @tc.name: GetCpuAffinityString
 * @tc.desc: Test GetCpuAffinityString
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RenderProfilerTelemetryTest, GetCpuAffinityString, Level1)
{
    auto info = RSTelemetry::GetCpuAffinityString();

    EXPECT_FALSE(info.empty());
    EXPECT_TRUE(ContainAll(info, { "Cpu affinity mask:" }));
}

/*
 * @tc.name: GetDeviceFrequencyString
 * @tc.desc: Test GetDeviceFrequencyString
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST(RenderProfilerTelemetryTest, GetDeviceFrequencyString, Level1)
{
    auto info = RSTelemetry::GetDeviceFrequencyString();

    EXPECT_FALSE(info.empty());
    EXPECT_TRUE(ContainAll(info, { "CPU0:", "GPU:" }));
}
} // namespace OHOS::Rosen
