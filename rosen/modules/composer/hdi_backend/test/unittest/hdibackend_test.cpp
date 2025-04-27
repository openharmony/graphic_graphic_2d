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

#include "hdi_backend.h"
#include "mock_hdi_device.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiBackendTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline HdiBackend* hdiBackend_ = nullptr;
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
};

void HdiBackendTest::SetUpTestCase()
{
    hdiBackend_ = HdiBackend::GetInstance();
    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
}

void HdiBackendTest::TearDownTestCase() {}

namespace {
/*
* Function: SetHdiBackendDevice001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetHdiBackendDevice
*                  2. check ret
*/
HWTEST_F(HdiBackendTest, SetHdiBackendDevice001, Function | MediumTest| Level3)
{
    // hdiDeviceMock_ is nullptr
    ASSERT_EQ(HdiBackendTest::hdiBackend_->SetHdiBackendDevice(nullptr), ROSEN_ERROR_INVALID_ARGUMENTS);
    // init hdiDeviceMock_
    ASSERT_EQ(HdiBackendTest::hdiBackend_->SetHdiBackendDevice(HdiBackendTest::hdiDeviceMock_), ROSEN_ERROR_OK);
    // the device_ in hdiBackend_ is not nullptr alredy
    ASSERT_EQ(HdiBackendTest::hdiBackend_->SetHdiBackendDevice(HdiBackendTest::hdiDeviceMock_), ROSEN_ERROR_OK);
}

/*
* Function: RegScreenHotplug001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegScreenHotplug(nullptr, nullptr)
*                  2. check ret
*/
HWTEST_F(HdiBackendTest, RegScreenHotplug001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegScreenHotplug(nullptr, nullptr), ROSEN_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: RegScreenHotplug002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegScreenHotplug(func, nullptr)
*                  2. check ret
*/
HWTEST_F(HdiBackendTest, RegScreenHotplug002, Function | MediumTest| Level3)
{
    auto func = [](OutputPtr &, bool, void*) -> void {};
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegScreenHotplug(func, nullptr), ROSEN_ERROR_OK);
}

/*
* Function: RegPrepareComplete001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegPrepareComplete(nullptr, nullptr)
*                  2. check ret
*/
HWTEST_F(HdiBackendTest, RegPrepareComplete001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegPrepareComplete(nullptr, nullptr), ROSEN_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: RegPrepareComplete002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegPrepareComplete(func, nullptr)
*                  2. check ret
*/
HWTEST_F(HdiBackendTest, RegPrepareComplete002, Function | MediumTest| Level3)
{
    auto func = [](sptr<Surface> &, const struct PrepareCompleteParam &param, void* data) -> void {};
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegPrepareComplete(func, nullptr), ROSEN_ERROR_OK);
}

/*
* Function: RegHwcDeadListener001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegHwcDeadListener
*                  2. check ret
*/
HWTEST_F(HdiBackendTest, RegHwcDeadListener001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegHwcDeadListener(nullptr, nullptr), ROSEN_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: RegHwcDeadListener002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegHwcDeadListener
*                  2. check ret
*/
HWTEST_F(HdiBackendTest, RegHwcDeadListener002, Function | MediumTest| Level3)
{
    auto func = [](void* data) -> void {};
    EXPECT_CALL(*hdiDeviceMock_, RegHwcDeadCallback(_, _)).WillRepeatedly(testing::Return(false));
    RosenError ret = HdiBackendTest::hdiBackend_->RegHwcDeadListener(func, nullptr);
    ASSERT_EQ(ret, ROSEN_ERROR_API_FAILED);

    EXPECT_CALL(*hdiDeviceMock_, RegHwcDeadCallback(_, _)).WillRepeatedly(testing::Return(true));
    ret = HdiBackendTest::hdiBackend_->RegHwcDeadListener(func, nullptr);
    ASSERT_EQ(ret, ROSEN_ERROR_OK);
}

/*
* Function: SetVsyncSamplerEnabled
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetVsyncSamplerEnabled()
*                  2. call GetVsyncSamplerEnabled
*                  3. check ret
*/
HWTEST_F(HdiBackendTest, SetVsyncSamplerEnabled, Function | MediumTest| Level3)
{
    OutputPtr output = HdiOutput::CreateHdiOutput(0);
    hdiBackend_->SetVsyncSamplerEnabled(nullptr, false);
    ASSERT_EQ(hdiBackend_->GetVsyncSamplerEnabled(nullptr), false);
    hdiBackend_->SetVsyncSamplerEnabled(output, false);
    ASSERT_EQ(hdiBackend_->GetVsyncSamplerEnabled(output), false);
}

/*
* Function: ResetDevice
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call ResetDevice()
*                  2. check ret
*/
HWTEST_F(HdiBackendTest, ResetDevice, Function | MediumTest| Level3)
{
    hdiBackend_->ResetDevice();
    hdiBackend_->OnScreenHotplug(-1, true);
    auto iter = hdiBackend_->outputs_.find(-1);
    EXPECT_EQ(iter, hdiBackend_->outputs_.end());
}

/*
* Function: OnPrepareComplete001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OnPrepareComplete() with invalid param
*                  2. no crash
*/
HWTEST_F(HdiBackendTest, OnPrepareComplete001, Function | MediumTest| Level3)
{
    OutputPtr output = HdiOutput::CreateHdiOutput(0);
    ASSERT_NE(output, nullptr);

    std::vector<LayerInfoPtr> newLayerInfos;
    for (size_t i = 0; i < 3; i++) {
        newLayerInfos.emplace_back(nullptr);
    }
    hdiBackend_->OnPrepareComplete(true, output, newLayerInfos);
}

/*
 * Function: RegScreenVBlankIdleCallback001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call RegScreenVBlankIdleCallback()
 *                  2. check ret
 */
HWTEST_F(HdiBackendTest, RegScreenVBlankIdleCallback001, Function | MediumTest | Level3)
{
    RosenError res = hdiBackend_->RegScreenVBlankIdleCallback(nullptr, nullptr);
    EXPECT_EQ(res, ROSEN_ERROR_INVALID_ARGUMENTS);
}

/*
 * Function: SetPendingMode001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call SetPendingMode()
 *                  2. check ret
 */
HWTEST_F(HdiBackendTest, SetPendingMode001, Function | MediumTest | Level3)
{
    int64_t period = 1;
    int64_t timestamp = 1;
    hdiBackend_->SetPendingMode(nullptr, period, timestamp);

    OutputPtr output = HdiOutput::CreateHdiOutput(0);
    hdiBackend_->SetPendingMode(output, period, timestamp);
    output->sampler_->Reset();
    auto pendingPeriod = output->sampler_->GetHardwarePeriod();
    EXPECT_EQ(pendingPeriod, period);
}

/*
 * Function: StartSample001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call StartSample()
 *                  2. check ret
 */
HWTEST_F(HdiBackendTest, StartSample001, Function | MediumTest | Level3)
{
    hdiBackend_->StartSample(nullptr);

    OutputPtr output = HdiOutput::CreateHdiOutput(0);
    hdiBackend_->SetPendingMode(output, 0, 0);
    output->sampler_->SetHardwareVSyncStatus(false);
    output->sampler_->SetVsyncSamplerEnabled(true);
    hdiBackend_->StartSample(output);
    EXPECT_TRUE(output->sampler_->GetHardwareVSyncStatus());
}

/*
 * Function: InitLoadOptParams001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call InitLoadOptParams()
 *                  2. check ret
 */
HWTEST_F(HdiBackendTest, InitLoadOptParams001, Function | MediumTest | Level3)
{
    LoadOptParamsForHdiBackend params = {};

    hdiBackend_->InitLoadOptParams(params);
    EXPECT_EQ(hdiBackend_->loadOptParamsForHdiBackend_.loadOptParamsForHdiOutput.switchParams.size(), 0);
}
} // namespace
} // namespace Rosen
} // namespace OHOS