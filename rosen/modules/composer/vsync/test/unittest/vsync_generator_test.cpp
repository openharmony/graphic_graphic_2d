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

#include "vsync_generator.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class VSyncGeneratorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<VSyncGenerator> vsyncGenerator_;
};

void VSyncGeneratorTest::SetUpTestCase()
{
    vsyncGenerator_ = CreateVSyncGenerator();
}

void VSyncGeneratorTest::TearDownTestCase()
{
    vsyncGenerator_ = nullptr;
    DestroyVSyncGenerator();
}

class VSyncGeneratorTestCallback : public VSyncGenerator::Callback {
public:
    void OnVSyncEvent(int64_t now, int64_t period, uint32_t refreshRate, VSyncMode vsyncMode) override;
    void OnPhaseOffsetChanged(int64_t phaseOffset) override;
    void OnConnsRefreshRateChanged(const std::vector<std::pair<uint64_t, uint32_t>> &refreshRates) override;
};

void VSyncGeneratorTestCallback::OnVSyncEvent(int64_t now, int64_t period, uint32_t refreshRate, VSyncMode vsyncMode)
{
}

void VSyncGeneratorTestCallback::OnPhaseOffsetChanged(int64_t phaseOffset)
{
}

void VSyncGeneratorTestCallback::OnConnsRefreshRateChanged(
    const std::vector<std::pair<uint64_t, uint32_t>> &refreshRates)
{
}

namespace {
/*
* Function: UpdateMode001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call UpdateMode
 */
HWTEST_F(VSyncGeneratorTest, UpdateMode001, Function | MediumTest| Level0)
{
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->UpdateMode(2, 0, 0), VSYNC_ERROR_OK);
}

/*
* Function: UpdateMode002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call UpdateMode
 */
HWTEST_F(VSyncGeneratorTest, UpdateMode002, Function | MediumTest| Level0)
{
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->UpdateMode(2, 0, -1), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: UpdateMode003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call UpdateMode
 */
HWTEST_F(VSyncGeneratorTest, UpdateMode003, Function | MediumTest| Level0)
{
    VSyncGeneratorTest::vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->UpdateMode(0, 0, 0), VSYNC_ERROR_OK);
    // 25000000 is period, refreshRate is 40hz，for JudgeRefreshRateLocked test
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->UpdateMode(25000000, 0, 0), VSYNC_ERROR_OK);
}

/*
* Function: AddListener001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddListener
 */
HWTEST_F(VSyncGeneratorTest, AddListener001, Function | MediumTest| Level0)
{
    sptr<VSyncGeneratorTestCallback> callback1 = new VSyncGeneratorTestCallback;
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->AddListener(2, callback1), VSYNC_ERROR_OK);
}

/*
* Function: AddListener002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddListener
 */
HWTEST_F(VSyncGeneratorTest, AddListener002, Function | MediumTest| Level0)
{
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->AddListener(2, nullptr), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: AddListener003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddListener
 */
HWTEST_F(VSyncGeneratorTest, AddListener003, Function | MediumTest| Level0)
{
    VSyncGeneratorTest::vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    VSyncGeneratorTest::vsyncGenerator_->UpdateMode(2, 0, 0);
    sptr<VSyncGeneratorTestCallback> callback = new VSyncGeneratorTestCallback;
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->AddListener(2, callback), VSYNC_ERROR_OK);
}

/*
* Function: RemoveListener001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RemoveListener
 */
HWTEST_F(VSyncGeneratorTest, RemoveListener001, Function | MediumTest| Level0)
{
    sptr<VSyncGeneratorTestCallback> callback2 = new VSyncGeneratorTestCallback;
    VSyncGeneratorTest::vsyncGenerator_->AddListener(2, callback2);
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->RemoveListener(callback2), VSYNC_ERROR_OK);
}

/*
* Function: RemoveListener002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RemoveListener
 */
HWTEST_F(VSyncGeneratorTest, RemoveListener002, Function | MediumTest| Level0)
{
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->RemoveListener(nullptr), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: RemoveListener003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RemoveListener
 */
HWTEST_F(VSyncGeneratorTest, RemoveListener003, Function | MediumTest| Level0)
{
    sptr<VSyncGeneratorTestCallback> callback3 = new VSyncGeneratorTestCallback;
    VSyncGeneratorTest::vsyncGenerator_->AddListener(2, callback3);
    sptr<VSyncGeneratorTestCallback> callback4 = new VSyncGeneratorTestCallback;
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->RemoveListener(callback4), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: ChangePhaseOffset001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call ChangePhaseOffset
 */
HWTEST_F(VSyncGeneratorTest, ChangePhaseOffset001, Function | MediumTest| Level0)
{
    sptr<VSyncGeneratorTestCallback> callback5 = new VSyncGeneratorTestCallback;
    VSyncGeneratorTest::vsyncGenerator_->AddListener(2, callback5);
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->ChangePhaseOffset(callback5, 1), VSYNC_ERROR_OK);
}

/*
* Function: ChangePhaseOffset002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call ChangePhaseOffset
 */
HWTEST_F(VSyncGeneratorTest, ChangePhaseOffset002, Function | MediumTest| Level0)
{
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->ChangePhaseOffset(nullptr, 1), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: ChangePhaseOffset003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call ChangePhaseOffset
 */
HWTEST_F(VSyncGeneratorTest, ChangePhaseOffset003, Function | MediumTest| Level0)
{
    sptr<VSyncGeneratorTestCallback> callback6 = new VSyncGeneratorTestCallback;
    VSyncGeneratorTest::vsyncGenerator_->AddListener(2, callback6);
    sptr<VSyncGeneratorTestCallback> callback7 = new VSyncGeneratorTestCallback;
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->ChangePhaseOffset(callback7, 1), VSYNC_ERROR_INVALID_OPERATING);
}
} // namespace
} // namespace Rosen
} // namespace OHOS