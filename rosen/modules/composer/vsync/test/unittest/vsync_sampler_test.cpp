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

static int64_t SystemTime()
{
    timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return int64_t(t.tv_sec) * 1000000000LL + t.tv_nsec; // 1000000000ns == 1s
}
}
class VSyncSamplerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static void Reset();

    static inline sptr<VSyncSampler> vsyncSampler = nullptr;
    static constexpr const int32_t WAIT_SYSTEM_ABILITY_REPORT_DATA_SECONDS = 5;
};

void VSyncSamplerTest::SetUpTestCase()
{
    vsyncSampler = CreateVSyncSampler();
}

void VSyncSamplerTest::TearDownTestCase()
{
    sleep(WAIT_SYSTEM_ABILITY_REPORT_DATA_SECONDS);
}

void VSyncSamplerTest::Reset()
{
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->period_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->phase_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->referenceTime_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->error_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->firstSampleIndex_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->numSamples_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->modeUpdated_ = false;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->hardwareVSyncStatus_ = true;
}

namespace {
/*
* Function: GetHardwarePeriodTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetHardwarePeriod
 */
HWTEST_F(VSyncSamplerTest, GetHardwarePeriodTest, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->GetHardwarePeriod(), 0);
}

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
    Reset();
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
    Reset();
}

/*
* Function: AddSample003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddSample
 */
HWTEST_F(VSyncSamplerTest, AddSample003, Function | MediumTest| Level3)
{
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->isAdaptive_ = true;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->lastAdaptiveTime_ = SystemTime();
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->hardwareVSyncStatus_ = true;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->numSamples_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->hardwareVSyncStatus_ = false;
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->AddSample(2000000), true);
    Reset();
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->isAdaptive_ = false;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->lastAdaptiveTime_ = 0;
}

/*
* Function: StartSample001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call StartSample
 */
HWTEST_F(VSyncSamplerTest, StartSample001, Function | MediumTest| Level3)
{
    bool ret = true;
    for (int i = 0; i < SAMPLER_NUMBER; i++) {
        ret = VSyncSamplerTest::vsyncSampler->AddSample(i);
    }
    ASSERT_EQ(ret, false);
    
    VSyncSamplerTest::vsyncSampler->StartSample(true);
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->AddSample(6), true);
    Reset();
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
    Reset();
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
    Reset();
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
    Reset();
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
    Reset();
}

/*
* Function: GetHardwarePeriod003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetHardwarePeriod
 */
HWTEST_F(VSyncSamplerTest, GetHardwarePeriod003, Function | MediumTest| Level3)
{
    VSyncSamplerTest::vsyncSampler->SetPendingPeriod(16666667); // 16666667ns
    bool ret = true;
    for (int i = 1; i <= 50; i++) { // add 50 samples
        ret = VSyncSamplerTest::vsyncSampler->AddSample(i * 16666667); // 16666667ns
    }
    ASSERT_EQ(ret, false);
    VSyncSamplerTest::vsyncSampler->SetPendingPeriod(8333333); // 8333333ns
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->GetHardwarePeriod(), 16666667); // 16666667ns
    VSyncSamplerTest::vsyncSampler->StartSample(true);
    VSyncSamplerTest::vsyncSampler->SetPendingPeriod(8333333); // 8333333ns
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->GetHardwarePeriod(), 8333333); // 8333333ns
    VSyncSamplerTest::vsyncSampler->SetPendingPeriod(0);
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->GetHardwarePeriod(), 8333333); // 8333333ns
    VSyncSamplerTest::vsyncSampler->SetPendingPeriod(-1);
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->GetHardwarePeriod(), 8333333); // 8333333ns
    Reset();
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
    VSyncSamplerTest::vsyncSampler->SetVsyncEnabledScreenId(0);
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->AddPresentFenceTime(0, 6), true);
    Reset();
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
    VSyncSamplerTest::vsyncSampler->SetVsyncEnabledScreenId(0);
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->AddPresentFenceTime(0, SAMPLER_NUMBER + 1), false);
    Reset();
}

/*
 * Function: AddPresentFenceTime003
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. preSetup: set vsyncEnabledScreenId 0
 *                  2. operation: test AddPresentFenceTime with screenId 0
 *                  3. result: AddPresentFenceTime function return true
 */
HWTEST_F(VSyncSamplerTest, AddPresentFenceTime003, Function | MediumTest| Level3)
{
    VSyncSamplerTest::vsyncSampler->SetVsyncEnabledScreenId(0);
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->AddPresentFenceTime(0, 0), true); // screenId 0
    Reset();
}

/*
 * Function: AddPresentFenceTime004
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. preSetup: set vsyncEnabledScreenId 0
 *                  2. operation: test AddPresentFenceTime with screenId 1
 *                  3. result: AddPresentFenceTime function return false
 */
HWTEST_F(VSyncSamplerTest, AddPresentFenceTime004, Function | MediumTest| Level3)
{
    VSyncSamplerTest::vsyncSampler->SetVsyncEnabledScreenId(0);
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->AddPresentFenceTime(1, 0), false); // screenId 1
    Reset();
}

/*
* Function: DumpTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call Dump
 */
HWTEST_F(VSyncSamplerTest, DumpTest, Function | MediumTest| Level3)
{
    for (int i = 1; i <= 10; i++) { // add 10 samples
        VSyncSamplerTest::vsyncSampler->AddSample(i * 16666667); // 16666667ns
    }
    std::string result = "";
    VSyncSamplerTest::vsyncSampler->Dump(result);
    ASSERT_NE(result.find("VSyncSampler"), std::string::npos);
}

/*
* Function: RegSetScreenVsyncEnabledCallbackTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RegSetScreenVsyncEnabledCallback
 */
HWTEST_F(VSyncSamplerTest, RegSetScreenVsyncEnabledCallbackTest, Function | MediumTest| Level3)
{
    bool result = false;
    VSyncSampler::SetScreenVsyncEnabledCallback cb = [&result](uint64_t screenId, bool enabled) {
        result = enabled;
    };
    VSyncSamplerTest::vsyncSampler->RegSetScreenVsyncEnabledCallback(cb);
    VSyncSamplerTest::vsyncSampler->SetScreenVsyncEnabledInRSMainThread(0, true);
    ASSERT_EQ(result, true);
    VSyncSamplerTest::vsyncSampler->SetScreenVsyncEnabledInRSMainThread(0, false);
    ASSERT_EQ(result, false);
}

/*
* Function: AddNegativeSamplesTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test add negative samples
 */
HWTEST_F(VSyncSamplerTest, AddNegativeSamplesTest, Function | MediumTest| Level3)
{
    VSyncSamplerTest::vsyncSampler->StartSample(true);
    bool ret = true;
    for (int i = 1; i < SAMPLER_NUMBER + 1; i++) { // add 10 samples
        ret = VSyncSamplerTest::vsyncSampler->AddSample(i * -16666667); // 16666667ns
    }
    ASSERT_EQ(ret, true);
}

/*
* Function: AddSamplesVarianceOversizeTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test samples variance oversize
 */
HWTEST_F(VSyncSamplerTest, AddSamplesVarianceOversizeTest, Function | MediumTest| Level3)
{
    VSyncSamplerTest::vsyncSampler->StartSample(true);
    int64_t floatingScope = 1000000; // 1000000ms
    bool ret = true;
    for (int i = 1; i <= 50; i++) { // add 50 samples
        floatingScope *= -1;
        ret = VSyncSamplerTest::vsyncSampler->AddSample(i * 16666667 + floatingScope); // 16666667ns
    }
    ASSERT_EQ(ret, true);
}

/*
* Function: RecordDisplayVSyncStatusTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test RecordDisplayVSyncStatus
 */
HWTEST_F(VSyncSamplerTest, RecordDisplayVSyncStatusTest, Function | MediumTest| Level3)
{
    vsyncSampler->RecordDisplayVSyncStatus(true);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->displayVSyncStatus_, true);
    vsyncSampler->RecordDisplayVSyncStatus(false);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->displayVSyncStatus_, false);
}

/*
* Function: RollbackHardwareVSyncStatusTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test RollbackHardwareVSyncStatus
 */
HWTEST_F(VSyncSamplerTest, RollbackHardwareVSyncStatusTest, Function | MediumTest| Level3)
{
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->hardwareVSyncStatus_ = false;
    vsyncSampler->RecordDisplayVSyncStatus(true);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->displayVSyncStatus_, true);
    vsyncSampler->RollbackHardwareVSyncStatus();
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->hardwareVSyncStatus_, true);
    vsyncSampler->RecordDisplayVSyncStatus(false);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->displayVSyncStatus_, false);
    vsyncSampler->RollbackHardwareVSyncStatus();
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->hardwareVSyncStatus_, false);
}

/*
* Function: SetAdaptive
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test SetAdaptive
 */
HWTEST_F(VSyncSamplerTest, SetAdaptive, Function | MediumTest| Level3)
{
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->isAdaptive_ = false;
    vsyncSampler->SetAdaptive(false);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->isAdaptive_, false);
    vsyncSampler->SetAdaptive(true);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->isAdaptive_, true);
}
} // namespace
} // namespace Rosen
} // namespace OHOS