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
#include <gtest/gtest.h>
#include "vsync_sampler.h"
#include <iostream>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
int64_t g_incrementalHardwareVsyncTimestamp = 0;
constexpr int32_t RANGE_PERCENT = 3; // 3%
constexpr int32_t FULL_PERCENT = 100; // 100%
}
class VSyncSampleTest : public testing::Test {
public:
    int64_t GetHardwareVsyncTimestamp(int32_t refreshRate);
    sptr<VSyncSampler> vsyncSampler = CreateVSyncSampler();
};

int64_t VSyncSampleTest::GetHardwareVsyncTimestamp(int32_t refreshRate)
{
    if (refreshRate == 0) {
        return 0;
    }
    srand(time(0));
    int64_t period = 1000000000 / refreshRate; // 1000000000ns
    int randNum = rand() % FULL_PERCENT;
    int64_t diff = period * RANGE_PERCENT / FULL_PERCENT;
    int64_t timestampInterval = period - diff + (diff * 2) * randNum / FULL_PERCENT;
    g_incrementalHardwareVsyncTimestamp += timestampInterval;
    return g_incrementalHardwareVsyncTimestamp;
}

/*
* Function: VSyncSampleTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: VSyncSample_30Hz_Test
 */
HWTEST_F(VSyncSampleTest, VSyncSample_30Hz_Test, Function | MediumTest | Level2)
{
    vsyncSampler->StartSample(true);
    for (int i = 0; i < 20; i++) { // test 20 times
        bool keepSampling = vsyncSampler->AddSample(GetHardwareVsyncTimestamp(30));
        if (!keepSampling) {
            break;
        }
    }
    int64_t period = vsyncSampler->GetPeriod();
    EXPECT_GT(period, 32000000); // 32000000ns
    EXPECT_LT(period, 35000000); // 35000000ns
}

/*
* Function: VSyncSampleTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: VSyncSample_60Hz_Test
 */
HWTEST_F(VSyncSampleTest, VSyncSample_60Hz_Test, Function | MediumTest | Level2)
{
    vsyncSampler->StartSample(true);
    for (int i = 0; i < 20; i++) { // test 20 times
        bool keepSampling = vsyncSampler->AddSample(GetHardwareVsyncTimestamp(60));
        if (!keepSampling) {
            break;
        }
    }
    int64_t period = vsyncSampler->GetPeriod();
    EXPECT_GT(period, 16000000); // 16000000ns
    EXPECT_LT(period, 18000000); // 18000000ns
}

/*
* Function: VSyncSampleTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: VSyncSample_90Hz_Test
 */
HWTEST_F(VSyncSampleTest, VSyncSample_90Hz_Test, Function | MediumTest | Level2)
{
    vsyncSampler->StartSample(true);
    for (int i = 0; i < 20; i++) { // test 20 times
        bool keepSampling = vsyncSampler->AddSample(GetHardwareVsyncTimestamp(90));
        if (!keepSampling) {
            break;
        }
    }
    int64_t period = vsyncSampler->GetPeriod();
    EXPECT_GT(period, 10000000); // 10000000ns
    EXPECT_LT(period, 12000000); // 12000000ns
}

/*
* Function: VSyncSampleTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: VSyncSample_120Hz_Test
 */
HWTEST_F(VSyncSampleTest, VSyncSample_120Hz_Test, Function | MediumTest | Level2)
{
    vsyncSampler->StartSample(true);
    for (int i = 0; i < 20; i++) { // test 20 times
        bool keepSampling = vsyncSampler->AddSample(GetHardwareVsyncTimestamp(120));
        if (!keepSampling) {
            break;
        }
    }
    int64_t period = vsyncSampler->GetPeriod();
    EXPECT_GT(period, 7000000); // 7000000ns
    EXPECT_LT(period, 9000000); // 9000000ns
}
}
