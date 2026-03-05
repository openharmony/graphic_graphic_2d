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

#include <gtest/gtest.h>
#include "vsync_sampler.h"
#include "vsync_generator.h"

#include "event_handler.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t SAMPLER_NUMBER = 6;
constexpr int64_t PERIOD_FOR_120 = 8333333;

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
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->frameCount_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->diffSum_ = 0;
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
* Function: AddSampleForAS
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddSample
 */
HWTEST_F(VSyncSamplerTest, AddSampleForAS, Function | MediumTest| Level3)
{
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->isAdaptive_ = true;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->lastAdaptiveTime_ = SystemTime();
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->hardwareVSyncStatus_ = true;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->numSamples_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->frameCount_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->diffSum_ = 0;
    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->period_ = PERIOD_FOR_120;

    auto generator = CreateVSyncGenerator();
    static_cast<impl::VSyncGenerator*>(generator.GetRefPtr())->pulse_ = 2700000;
    static_cast<impl::VSyncGenerator*>(generator.GetRefPtr())->period_ = PERIOD_FOR_120;
    for (int i = 0; i < 8; i++) {
        VSyncSamplerTest::vsyncSampler->AddSample(18000000 + PERIOD_FOR_120 * i);
    }
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->frameCount_, 8);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->diffSum_, 0);

    for (int i = 8; i < 15; i++) {
        VSyncSamplerTest::vsyncSampler->AddSample(18000000 + PERIOD_FOR_120 * i);
    }
    ASSERT_EQ(static_cast<impl::VSyncGenerator*>(generator.GetRefPtr())->referenceTime_,
        18000000 + PERIOD_FOR_120 * 14);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->frameCount_, 0);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->diffSum_, 0);

    VSyncSamplerTest::vsyncSampler->AddSample(18000000 + PERIOD_FOR_120 * 15 + 2700000);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->frameCount_, 1);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->diffSum_, 2700000);

    for (int i = 1; i < 360; i++) {
        VSyncSamplerTest::vsyncSampler->AddSample(18000000 + PERIOD_FOR_120 * (15 + i) + 2700000);
    }
    ASSERT_EQ(static_cast<impl::VSyncGenerator*>(generator.GetRefPtr())->referenceTime_,
        20700000 + PERIOD_FOR_120 * 374);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->frameCount_, 0);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->diffSum_, 0);

    static_cast<impl::VSyncGenerator*>(generator.GetRefPtr())->pulse_ = 0;
    VSyncSamplerTest::vsyncSampler->AddSample(18000000 + PERIOD_FOR_120 * 375 + 5400000);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->frameCount_, 1);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->diffSum_, 0);

    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->isAdaptive_ = false;
    VSyncSamplerTest::vsyncSampler->AddSample(18000000 + PERIOD_FOR_120 * 376 + 8100000);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->frameCount_, 1);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->diffSum_, 0);

    static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr())->hardwareVSyncStatus_ = false;
    ASSERT_EQ(VSyncSamplerTest::vsyncSampler->AddSample(18000000 + PERIOD_FOR_120 * 377 + 8100000), true);
    Reset();
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
    VSyncSamplerTest::vsyncSampler->RegSetScreenVsyncEnabledCallback(nullptr);
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

/*
* Function: RegSetScreenVsyncEnabledCallbackForRenderService
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test RegSetScreenVsyncEnabledCallbackForRenderService
 */
HWTEST_F(VSyncSamplerTest, RegSetScreenVsyncEnabledCallbackForRenderService, Function | MediumTest| Level3)
{
    auto sampler = static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr());
    sampler->updateVsyncEnabledScreenIdCallback_ = [](bool vsyncEnabledScreenId)->bool{
    return false;};
    sampler->RegSetScreenVsyncEnabledCallbackForRenderService(true, nullptr);
    ASSERT_NE(sampler->updateVsyncEnabledScreenIdCallback_, nullptr);
    sampler->updateVsyncEnabledScreenIdCallback_ = nullptr;
}

/*
* Function: RegSetScreenVsyncEnabledCallbackForRenderService
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test RegSetScreenVsyncEnabledCallbackForRenderService when
                      updateVsyncEnabledScreenIdCallback_ is nullptr
 *                 2. test RegSetScreenVsyncEnabledCallbackForRenderService when
                      updateVsyncEnabledScreenIdCallback_ return true
 *                 3. test RegSetScreenVsyncEnabledCallbackForRenderService when
                      handler is nullptr
 */
HWTEST_F(VSyncSamplerTest, RegSetScreenVsyncEnabledCallbackForRenderService002, Function | MediumTest| Level3)
{
    ScreenId vsyncEnabledScreenId = 0;
    auto sampler = static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr());
    sampler->updateVsyncEnabledScreenIdCallback_ = nullptr;
    sampler->RegSetScreenVsyncEnabledCallbackForRenderService(vsyncEnabledScreenId, nullptr);
    sampler->updateVsyncEnabledScreenIdCallback_ = [](bool vsyncEnabledScreenId)->bool{
        return true;};
    sampler->RegSetScreenVsyncEnabledCallbackForRenderService(vsyncEnabledScreenId, nullptr);
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(false);
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    sampler->RegSetScreenVsyncEnabledCallbackForRenderService(vsyncEnabledScreenId + 1, nullptr);
    ASSERT_NE(sampler->vsyncEnabledScreenId_, vsyncEnabledScreenId);
    sampler->updateVsyncEnabledScreenIdCallback_ = nullptr;
}

/*
* Function: ProcessVSyncScreenIdWhilePowerStatusChanged
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test ProcessVSyncScreenIdWhilePowerStatusChanged
 */
HWTEST_F(VSyncSamplerTest, ProcessVSyncScreenIdWhilePowerStatusChanged, Function | MediumTest| Level3)
{
    VSyncSamplerTest::vsyncSampler->ProcessVSyncScreenIdWhilePowerStatusChanged(100,
    ScreenPowerStatus::POWER_STATUS_ON, nullptr, false);
    auto sampler = static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr());
    ASSERT_EQ(sampler->judgeVSyncEnabledScreenWhilePowerStatusChangedCallback_, nullptr);
}

/*
* Function: ProcessVSyncScreenIdWhilePowerStatusChanged
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test ProcessVSyncScreenIdWhilePowerStatusChanged002
 */
HWTEST_F(VSyncSamplerTest, ProcessVSyncScreenIdWhilePowerStatusChanged002, Function | MediumTest| Level3)
{
    auto sampler = static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr());
    sampler->ProcessVSyncScreenIdWhilePowerStatusChanged(100, ScreenPowerStatus::POWER_STATUS_OFF, nullptr, false);
    sampler->ProcessVSyncScreenIdWhilePowerStatusChanged(100, ScreenPowerStatus::POWER_STATUS_SUSPEND, nullptr, false);

    ASSERT_EQ(sampler->judgeVSyncEnabledScreenWhilePowerStatusChangedCallback_, nullptr);
    sampler->vsyncEnabledScreenId_ = 0;
    sampler->RegJudgeVSyncEnabledScreenWhilePowerStatusChangedCallback([](ScreenId screenId, ScreenPowerStatus status,
        ScreenId enabledScreenId)->ScreenId{return 0;});
    sampler->ProcessVSyncScreenIdWhilePowerStatusChanged(100, ScreenPowerStatus::POWER_STATUS_ON, nullptr, true);
    sampler->vsyncEnabledScreenId_ = 1;
    sampler->ProcessVSyncScreenIdWhilePowerStatusChanged(100, ScreenPowerStatus::POWER_STATUS_ON, nullptr, true);
    ASSERT_NE(sampler->judgeVSyncEnabledScreenWhilePowerStatusChangedCallback_, nullptr);
}

/*
* Function: JudgeVSyncEnabledScreenWhileHotPlug
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test JudgeVSyncEnabledScreenWhileHotPlug
 */
HWTEST_F(VSyncSamplerTest, JudgeVSyncEnabledScreenWhileHotPlug, Function | MediumTest| Level3)
{   uint64_t id = 100;
    VSyncSamplerTest::vsyncSampler->SetVsyncEnabledScreenId(id);
    VSyncSamplerTest::vsyncSampler->RegUpdateFoldScreenConnectStatusLockedCallback([](uint64_t screenId,
    bool connected){return;});
    uint64_t enableId = VSyncSamplerTest::vsyncSampler->JudgeVSyncEnabledScreenWhileHotPlug(id + 1, true);
    ASSERT_EQ(enableId, id);
}

/*
* Function: JudgeVSyncEnabledScreenWhileHotPlug
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test JudgeVSyncEnabledScreenWhileHotPlug002
 */
HWTEST_F(VSyncSamplerTest, JudgeVSyncEnabledScreenWhileHotPlug002, Function | MediumTest| Level3)
{
    ScreenId id = 100;
    auto sampler = static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr());
    sampler->SetVsyncEnabledScreenId(id);
    sampler->updateFoldScreenConnectStatusLockedCallback_ = nullptr;
    ScreenId enableId = sampler->JudgeVSyncEnabledScreenWhileHotPlug(id + 1, true);
    ASSERT_EQ(enableId, INVALID_SCREEN_ID);
    sampler->vsyncEnabledScreenId_ = -1;
    sampler->RegUpdateFoldScreenConnectStatusLockedCallback([](ScreenId screenId, bool connected) {return;});
    enableId = sampler->JudgeVSyncEnabledScreenWhileHotPlug(id, true);
    ASSERT_EQ(enableId, id);
    sampler->vsyncEnabledScreenId_ = id + 1;
    enableId = sampler->JudgeVSyncEnabledScreenWhileHotPlug(id, false);
    ASSERT_EQ(enableId, sampler->vsyncEnabledScreenId_);
    sampler->vsyncEnabledScreenId_ = id;
    sampler->RegGetScreenVsyncEnableByIdCallback([](uint64_t vsyncEnabledScreenId)->ScreenId{return 0;});
    enableId = sampler->JudgeVSyncEnabledScreenWhileHotPlug(id, false);
    ASSERT_EQ(enableId, 0);
}

/*
* Function: RegUpdateVsyncEnabledScreenIdCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test RegUpdateVsyncEnabledScreenIdCallback
 */
HWTEST_F(VSyncSamplerTest, RegUpdateVsyncEnabledScreenIdCallback, Function | MediumTest| Level3)
{
    auto sampler = static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr());
    sampler->RegUpdateVsyncEnabledScreenIdCallback([](uint64_t screenId)->bool{return true;});
    ASSERT_NE(sampler->updateVsyncEnabledScreenIdCallback_, nullptr);
    sampler->updateVsyncEnabledScreenIdCallback_ = nullptr;
}

/*
* Function: RegJudgeVSyncEnabledScreenWhilePowerStatusChangedCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test RegJudgeVSyncEnabledScreenWhilePowerStatusChangedCallback
 */
HWTEST_F(VSyncSamplerTest, RegJudgeVSyncEnabledScreenWhilePowerStatusChangedCallback, Function | MediumTest| Level3)
{
    auto sampler = static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr());
    sampler->RegJudgeVSyncEnabledScreenWhilePowerStatusChangedCallback([](uint64_t screenId,
    ScreenPowerStatus status, uint64_t enabledScreenId)->uint64_t{return 0;});
    ASSERT_NE(sampler->judgeVSyncEnabledScreenWhilePowerStatusChangedCallback_, nullptr);
    sampler->judgeVSyncEnabledScreenWhilePowerStatusChangedCallback_ = nullptr;
}

/*
* Function: RegUpdateFoldScreenConnectStatusLockedCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test RegUpdateFoldScreenConnectStatusLockedCallback
 */
HWTEST_F(VSyncSamplerTest, RegUpdateFoldScreenConnectStatusLockedCallback, Function | MediumTest| Level3)
{
    auto sampler = static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr());
    sampler->RegUpdateFoldScreenConnectStatusLockedCallback([](uint64_t screenId,
    bool connected){return;});
    ASSERT_NE(sampler->updateFoldScreenConnectStatusLockedCallback_, nullptr);
    sampler->updateFoldScreenConnectStatusLockedCallback_ = nullptr;
}

/*
* Function: RegSetScreenVsyncEnableByIdCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test RegSetScreenVsyncEnableByIdCallback
 */
HWTEST_F(VSyncSamplerTest, RegSetScreenVsyncEnableByIdCallback, Function | MediumTest| Level3)
{
    auto sampler = static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr());
    sampler->RegSetScreenVsyncEnableByIdCallback([](uint64_t vsyncEnabledScreenId,
    uint64_t screenId, bool enabled){return;});
    ASSERT_NE(sampler->setScreenVsyncEnableByIdCallback_, nullptr);
    sampler->setScreenVsyncEnableByIdCallback_ = nullptr;
}

/*
* Function: RegGetScreenVsyncEnableByIdCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test RegGetScreenVsyncEnableByIdCallback
 */
HWTEST_F(VSyncSamplerTest, RegGetScreenVsyncEnableByIdCallback, Function | MediumTest| Level3)
{
    auto sampler = static_cast<impl::VSyncSampler*>(vsyncSampler.GetRefPtr());
    sampler->RegGetScreenVsyncEnableByIdCallback([](uint64_t vsyncEnabledScreenId)->uint64_t{return 0;});
    ASSERT_NE(sampler->getScreenVsyncEnableByIdCallback_, nullptr);
    sampler->getScreenVsyncEnableByIdCallback_ = nullptr;
}
} // namespace
} // namespace Rosen
} // namespace OHOS