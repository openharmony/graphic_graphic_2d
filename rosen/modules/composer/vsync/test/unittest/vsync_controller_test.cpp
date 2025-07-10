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
#include <vsync_controller.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
static int64_t SystemTime()
{
    timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return int64_t(t.tv_sec) * 1000000000LL + t.tv_nsec; // 1000000000ns == 1s
}

class VSyncControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline sptr<VSyncController> vsyncController_ = nullptr;
    static inline sptr<VSyncGenerator> vsyncGenerator_ = nullptr;
    static constexpr const int32_t WAIT_SYSTEM_ABILITY_REPORT_DATA_SECONDS = 5;
};

class VSyncControllerCallback : public VSyncController::Callback {
public:
    void OnVSyncEvent(int64_t now, int64_t period,
        uint32_t refreshRate, VSyncMode vsyncMode, uint32_t vsyncMaxRefreshRate) override;
    void OnDVSyncEvent(int64_t now, int64_t period,
        uint32_t refreshRate, VSyncMode vsyncMode, uint32_t vsyncMaxRefreshRate) override;
    void OnConnsRefreshRateChanged(const std::vector<std::pair<uint64_t, uint32_t>> &refreshRates) override;
};

void VSyncControllerTest::SetUpTestCase()
{
    vsyncGenerator_ = CreateVSyncGenerator();
    vsyncController_ = new VSyncController(vsyncGenerator_, 0);
}

void VSyncControllerTest::TearDownTestCase()
{
    sleep(WAIT_SYSTEM_ABILITY_REPORT_DATA_SECONDS);
    vsyncGenerator_ = nullptr;
    DestroyVSyncGenerator();
    vsyncController_ = nullptr;
}

void VSyncControllerCallback::OnVSyncEvent(int64_t now, int64_t period,
    uint32_t refreshRate, VSyncMode vsyncMode, uint32_t vsyncMaxRefreshRate) {}
void VSyncControllerCallback::OnDVSyncEvent(int64_t now, int64_t period,
    uint32_t refreshRate, VSyncMode vsyncMode, uint32_t vsyncMaxRefreshRate) {}

void VSyncControllerCallback::OnConnsRefreshRateChanged(const std::vector<std::pair<uint64_t, uint32_t>> &refreshRates)
{
}

/*
* Function: SetEnable
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetEnable
 */
HWTEST_F(VSyncControllerTest, SetEnable001, Function | MediumTest | Level2)
{
    bool vsyncEnabledFlag = false;
    ASSERT_EQ(VSyncControllerTest::vsyncController_->SetEnable(true, vsyncEnabledFlag), VSYNC_ERROR_OK);
    ASSERT_EQ(vsyncEnabledFlag, true);
}

/*
* Function: SetCallback001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. generate a VSyncControllerCallback obj
*                  2. call SetCallback
 */
HWTEST_F(VSyncControllerTest, SetCallback001, Function | MediumTest | Level2)
{
    VSyncControllerCallback* callback = new VSyncControllerCallback;
    ASSERT_EQ(VSyncControllerTest::vsyncController_->SetCallback(callback), VSYNC_ERROR_OK);
    delete callback;
}

/*
* Function: SetCallback002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetCallback with nullptr
 */
HWTEST_F(VSyncControllerTest, SetCallback002, Function | MediumTest | Level2)
{
    ASSERT_EQ(VSyncControllerTest::vsyncController_->SetCallback(nullptr), VSYNC_ERROR_NULLPTR);
}

/*
* Function: SetPhaseOffset001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetPhaseOffset
 */
HWTEST_F(VSyncControllerTest, SetPhaseOffset001, Function | MediumTest | Level2)
{
    ASSERT_EQ(VSyncControllerTest::vsyncController_->SetPhaseOffset(2), VSYNC_ERROR_OK);
}

/*
* Function: SetEnable002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetEnable
 */
HWTEST_F(VSyncControllerTest, SetEnable002, Function | MediumTest | Level2)
{
    bool vsyncEnabledFlag = true;
    ASSERT_EQ(VSyncControllerTest::vsyncController_->SetEnable(false, vsyncEnabledFlag), VSYNC_ERROR_OK);
    ASSERT_EQ(vsyncEnabledFlag, false);
}

/*
* Function: SetPhaseOffset002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetPhaseOffset
 */
HWTEST_F(VSyncControllerTest, SetPhaseOffset002, Function | MediumTest | Level2)
{
    ASSERT_EQ(VSyncControllerTest::vsyncController_->SetPhaseOffset(2), VSYNC_ERROR_INVALID_OPERATING);
}

/*
* Function: ChangeAdaptiveStatus
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call ChangeAdaptiveStatus
 */
HWTEST_F(VSyncControllerTest, ChangeAdaptiveStatus, Function | MediumTest | Level2)
{
    VSyncControllerTest::vsyncController_->phaseOffset_ = 10;
    VSyncControllerTest::vsyncController_->normalPhaseOffset_ = 20;
    VSyncControllerTest::vsyncController_->ChangeAdaptiveStatus(true);
    ASSERT_NE(VSyncControllerTest::vsyncController_->phaseOffset_,
        VSyncControllerTest::vsyncController_->normalPhaseOffset_);
    VSyncControllerTest::vsyncController_->ChangeAdaptiveStatus(false);
    ASSERT_EQ(VSyncControllerTest::vsyncController_->phaseOffset_,
        VSyncControllerTest::vsyncController_->normalPhaseOffset_);
}
 
/*
* Function: AdjustAdaptiveOffset
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AdjustAdaptiveOffset
 */
HWTEST_F(VSyncControllerTest, AdjustAdaptiveOffset, Function | MediumTest | Level2)
{
    VSyncControllerTest::vsyncController_->phaseOffset_ = 20;
    VSyncControllerTest::vsyncController_->AdjustAdaptiveOffset(0, 20);
    ASSERT_EQ(VSyncControllerTest::vsyncController_->phaseOffset_, 20);
    VSyncControllerTest::vsyncController_->AdjustAdaptiveOffset(100, 20);
    ASSERT_EQ(VSyncControllerTest::vsyncController_->phaseOffset_, 40);
}
 
/*
* Function: ResetOffset
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call ResetOffset
 */
HWTEST_F(VSyncControllerTest, ResetOffset, Function | MediumTest | Level2)
{
    VSyncControllerTest::vsyncController_->phaseOffset_ = 10;
    VSyncControllerTest::vsyncController_->normalPhaseOffset_ = 20;
    VSyncControllerTest::vsyncController_->ResetOffset();
    ASSERT_EQ(VSyncControllerTest::vsyncController_->phaseOffset_,
        VSyncControllerTest::vsyncController_->normalPhaseOffset_);
}
 
/*
* Function: NeedPreexecuteAndUpdateTs
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NeedPreexecuteAndUpdateTs
 */
HWTEST_F(VSyncControllerTest, NeedPreexecuteAndUpdateTs, Function | MediumTest | Level2)
{
    VSyncControllerTest::vsyncController_->generator_ = nullptr;
    int64_t timestamp = 0;
    int64_t period = 0;
    ASSERT_EQ(VSyncControllerTest::vsyncController_->NeedPreexecuteAndUpdateTs(timestamp, period), false);
    VSyncControllerTest::vsyncController_->generator_ = vsyncGenerator_;
    auto vsyncGeneratorImpl = static_cast<impl::VSyncGenerator*>(
        VSyncControllerTest::vsyncController_->generator_.GetRefPtr());
    vsyncGeneratorImpl->period_ = 10000000;
    VSyncControllerTest::vsyncController_->lastVsyncTime_ = SystemTime();
    ASSERT_EQ(VSyncControllerTest::vsyncController_->NeedPreexecuteAndUpdateTs(timestamp, period), true);
}

/**
 * @tc.name: GetPhaseOffset
 * @tc.desc: Test GetPhaseOffset function
 * @tc.type: FUNC
 * @tc.require: issueICHRJD
 */
HWTEST_F(VSyncControllerTest, GetPhaseOffset, Function | MediumTest | Level2)
{
    // 2700000 = 2.7ms , standard phase offset time
    VSyncControllerTest::vsyncController_->phaseOffset_ = 2700000;
    auto offset = VSyncControllerTest::vsyncController_->GetPhaseOffset();
    // 2700000 = 2.7ms , standard phase offset time
    ASSERT_EQ(offset, 2700000);
}
}
