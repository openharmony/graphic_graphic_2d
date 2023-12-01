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

#include "vsync_sampler.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t SAMPLER_NUMBER = 6;
}
class VSyncSamplerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<VSyncSampler> vsyncSampler = nullptr;
};

void VSyncSamplerTest::SetUpTestCase()
{
    vsyncSampler = CreateVSyncSampler();
}

void VSyncSamplerTest::TearDownTestCase() {}

namespace {
/*
* Function: AddSample001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddSample
 */
HWTEST_F(VSyncSamplerTest, AddSample001, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->AddSample(0), true);
    VSyncSamplerTest::vsyncSampler->Reset();
}

/*
* Function: AddSample002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddSample
 */
HWTEST_F(VSyncSamplerTest, AddSample002, Function | MediumTest| Level3)
{
    bool ret = true;
    for (int i = 0; i < SAMPLER_NUMBER; i++) {
        ret = VSyncSamplerTest::vsyncSampler->AddSample(i);
    }
    ASSERT_EQ(ret, false);
    VSyncSamplerTest::vsyncSampler->Reset();
}

/*
* Function: BeginSample001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call BeginSample
 */
HWTEST_F(VSyncSamplerTest, BeginSample001, Function | MediumTest| Level3)
{
    bool ret = true;
    for (int i = 0; i < SAMPLER_NUMBER; i++) {
        ret = VSyncSamplerTest::vsyncSampler->AddSample(i);
    }
    ASSERT_EQ(ret, false);
    
    VSyncSamplerTest::vsyncSampler->BeginSample();
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->AddSample(6), true);
    VSyncSamplerTest::vsyncSampler->Reset();
}

/*
* Function: GetPeriod001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetPeriod
 */
HWTEST_F(VSyncSamplerTest, GetPeriod001, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->GetPeriod(), 0);
}

/*
* Function: GetPeriod002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetPeriod
 */
HWTEST_F(VSyncSamplerTest, GetPeriod002, Function | MediumTest| Level3)
{
    bool ret = true;
    for (int i = 0; i < SAMPLER_NUMBER; i++) {
        ret = VSyncSamplerTest::vsyncSampler->AddSample(i);
    }
    ASSERT_EQ(ret, false);
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->GetPeriod(), 1);
    VSyncSamplerTest::vsyncSampler->Reset();
}

/*
* Function: GetPhase001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetPhase
 */
HWTEST_F(VSyncSamplerTest, GetPhase001, Function | MediumTest| Level3)
{
    VSyncSamplerTest::vsyncSampler->AddSample(0);
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->GetPhase(), 0);
    VSyncSamplerTest::vsyncSampler->Reset();
}

/*
* Function: GetPhase002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetPhase
 */
HWTEST_F(VSyncSamplerTest, GetPhase002, Function | MediumTest| Level3)
{
    bool ret = true;
    for (int i = 0; i < SAMPLER_NUMBER; i++) {
        ret = VSyncSamplerTest::vsyncSampler->AddSample(i);
    }
    ASSERT_EQ(ret, false);
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->GetPhase(), 0);
    VSyncSamplerTest::vsyncSampler->Reset();
}

/*
* Function: GetRefrenceTime001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetRefrenceTime
 */
HWTEST_F(VSyncSamplerTest, GetRefrenceTime001, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->GetRefrenceTime(), 0);
}

/*
* Function: GetRefrenceTime002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetRefrenceTime
 */
HWTEST_F(VSyncSamplerTest, GetRefrenceTime002, Function | MediumTest| Level3)
{
    bool ret = true;
    for (int i = 1; i < SAMPLER_NUMBER + 1; i++) {
        ret = VSyncSamplerTest::vsyncSampler->AddSample(i);
    }
    ASSERT_EQ(ret, false);
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->GetRefrenceTime(), 1);
}

/*
* Function: GetHardwarePeriod001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetHardwarePeriod
 */
HWTEST_F(VSyncSamplerTest, GetHardwarePeriod001, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->GetHardwarePeriod(), 1);
}

/*
* Function: GetHardwarePeriod002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetHardwarePeriod
 */
HWTEST_F(VSyncSamplerTest, GetHardwarePeriod002, Function | MediumTest| Level3)
{
    VSyncSamplerTest::vsyncSampler->SetPendingPeriod(1);
    bool ret = true;
    for (int i = 1; i < SAMPLER_NUMBER + 1; i++) {
        ret = VSyncSamplerTest::vsyncSampler->AddSample(i);
    }
    ASSERT_EQ(ret, false);
    VSyncSamplerTest::vsyncSampler->SetPendingPeriod(1);
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->GetHardwarePeriod(), 1);
    VSyncSamplerTest::vsyncSampler->Reset();
}

/*
* Function: AddPresentFenceTime001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddPresentFenceTime
 */
HWTEST_F(VSyncSamplerTest, AddPresentFenceTime001, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->AddPresentFenceTime(6), true);
    VSyncSamplerTest::vsyncSampler->Reset();
}

/*
* Function: AddPresentFenceTime002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddPresentFenceTime
 */
HWTEST_F(VSyncSamplerTest, AddPresentFenceTime002, Function | MediumTest| Level3)
{
    bool ret = true;
    for (int i = 1; i < SAMPLER_NUMBER + 1; i++) {
        ret = VSyncSamplerTest::vsyncSampler->AddSample(i);
    }
    ASSERT_EQ(ret, false);
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->AddPresentFenceTime(SAMPLER_NUMBER + 1), false);
    VSyncSamplerTest::vsyncSampler->Reset();
}
} // namespace
} // namespace Rosen
} // namespace OHOS