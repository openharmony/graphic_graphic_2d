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
    // the device_ in hdiBackend_ is not nullptr already
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
    EXPECT_CALL(*hdiDeviceMock_, RegHotPlugCallback(_, _)).WillRepeatedly(testing::Return(0));
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegScreenHotplug(func, nullptr), ROSEN_ERROR_OK);
    EXPECT_CALL(*hdiDeviceMock_, RegHotPlugCallback(_, _)).WillRepeatedly(testing::Return(-1));
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegScreenHotplug(func, nullptr), ROSEN_ERROR_API_FAILED);
}

/*
* Function: RegScreenRefresh001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegScreenRefresh(nullptr, nullptr)
*                  2. check ret
*/
HWTEST_F(HdiBackendTest, RegScreenRefresh001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegScreenRefresh(nullptr, nullptr), ROSEN_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: RegScreenRefresh002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegScreenRefresh(func, nullptr)
*                  2. check ret
*/
HWTEST_F(HdiBackendTest, RegScreenRefresh002, Function | MediumTest| Level3)
{
    EXPECT_CALL(*hdiDeviceMock_, RegRefreshCallback(_, _)).WillRepeatedly(testing::Return(0));
    auto func = [](uint32_t, void*) -> void {};
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegScreenRefresh(func, nullptr), ROSEN_ERROR_OK);
    EXPECT_CALL(*hdiDeviceMock_, RegRefreshCallback(_, _)).WillRepeatedly(testing::Return(-1));
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegScreenRefresh(func, nullptr), ROSEN_ERROR_API_FAILED);
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
    EXPECT_CALL(*hdiDeviceMock_, Destroy());
    hdiBackend_->ResetDevice();
    hdiBackend_->OnScreenHotplug(-1, true);
    auto iter = hdiBackend_->outputs_.find(-1);
    EXPECT_EQ(iter, hdiBackend_->outputs_.end());
    hdiBackend_->ResetDevice();
    EXPECT_EQ(hdiBackend_->outputs_.size(), 0);
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
    hdiBackend_->device_ = hdiDeviceMock_;
    RosenError res = hdiBackend_->RegScreenVBlankIdleCallback(nullptr, nullptr);
    EXPECT_EQ(res, ROSEN_ERROR_INVALID_ARGUMENTS);
    auto func = [](uint32_t devId, uint64_t ns, void* data) -> void {};
    EXPECT_CALL(*hdiDeviceMock_, RegScreenVBlankIdleCallback(_, _)).WillRepeatedly(testing::Return(0));
    res = hdiBackend_->RegScreenVBlankIdleCallback(func, nullptr);
    EXPECT_EQ(res, ROSEN_ERROR_OK);
    EXPECT_CALL(*hdiDeviceMock_, RegScreenVBlankIdleCallback(_, _)).WillRepeatedly(testing::Return(-1));
    res = hdiBackend_->RegScreenVBlankIdleCallback(func, nullptr);
    EXPECT_EQ(res, ROSEN_ERROR_API_FAILED);
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
    static_cast<impl::VSyncSampler*>(output->sampler_.GetRefPtr())->period_ = 0;
    static_cast<impl::VSyncSampler*>(output->sampler_.GetRefPtr())->phase_ = 0;
    static_cast<impl::VSyncSampler*>(output->sampler_.GetRefPtr())->referenceTime_ = 0;
    static_cast<impl::VSyncSampler*>(output->sampler_.GetRefPtr())->error_ = 0;
    static_cast<impl::VSyncSampler*>(output->sampler_.GetRefPtr())->firstSampleIndex_ = 0;
    static_cast<impl::VSyncSampler*>(output->sampler_.GetRefPtr())->numSamples_ = 0;
    static_cast<impl::VSyncSampler*>(output->sampler_.GetRefPtr())->modeUpdated_ = false;
    static_cast<impl::VSyncSampler*>(output->sampler_.GetRefPtr())->hardwareVSyncStatus_ = true;
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
    VSyncSampler::SetScreenVsyncEnabledCallback cb = [](uint64_t screenId, bool enabled) {};
    output->sampler_->RegSetScreenVsyncEnabledCallback(cb);
    hdiBackend_->StartSample(output);
    EXPECT_TRUE(static_cast<impl::VSyncSampler*>(output->sampler_.GetRefPtr())->hardwareVSyncStatus_);
}

/*
 * Function: RegHwcEventCallback001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call egHwcEventCallback
 *                  2. check ret
 */
HWTEST_F(HdiBackendTest, RegHwcEventCallback001, Function | MediumTest| Level3)
{
    RosenError res = hdiBackend_->RegHwcEventCallback(nullptr, nullptr);
    EXPECT_EQ(res, ROSEN_ERROR_INVALID_ARGUMENTS);

    auto func = [](uint32_t devId, uint32_t envId, const std::vector<int32_t>& eventData, void* data) -> void {};
    EXPECT_CALL(*hdiDeviceMock_, RegHwcEventCallback(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    hdiBackend_->device_ = hdiDeviceMock_;
    RosenError ret = HdiBackendTest::hdiBackend_->RegHwcEventCallback(func, nullptr);
    ASSERT_EQ(ret, ROSEN_ERROR_API_FAILED);
 
    EXPECT_CALL(*hdiDeviceMock_, RegHwcEventCallback(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    ret = HdiBackendTest::hdiBackend_->RegHwcEventCallback(func, nullptr);
    ASSERT_EQ(ret, ROSEN_ERROR_OK);
}

/*
 * Function: OnHdiBackendHotPlugEvent
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. get HdiBackend::GetInstance()
 *                  2. call OnHdiBackendHotPlugEvent
 *                  3. check result
 */
HWTEST_F(HdiBackendTest, OnHdiBackendHotPlugEvent, Function | MediumTest| Level3)
{
    HdiBackend* data = HdiBackend::GetInstance();
    EXPECT_NE(data, nullptr);
    data->OnHdiBackendHotPlugEvent(0, false, nullptr);
    EXPECT_NE(data, nullptr);
    data->OnHdiBackendHotPlugEvent(1, true, data);
    EXPECT_NE(data, nullptr);
    data->onScreenHotplugCb_ = nullptr;
    data->OnHdiBackendHotPlugEvent(1, false, nullptr);
    EXPECT_EQ(data->outputs_.find(1), data->outputs_.end());
}

/*
 * Function: OnHdiBackendRefreshEvent
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. get HdiBackend::GetInstance()
 *                  2. call OnHdiBackendRefreshEvent
 *                  3. check result
 */
HWTEST_F(HdiBackendTest, OnHdiBackendRefreshEvent, Function | MediumTest| Level3)
{
    HdiBackend* data = HdiBackend::GetInstance();
    EXPECT_NE(data, nullptr);
    data->OnHdiBackendRefreshEvent(0, nullptr);
    EXPECT_NE(data, nullptr);
    data->OnHdiBackendRefreshEvent(1, data);
    EXPECT_NE(data, nullptr);
}

/*
 * Function: OnScreenRefresh
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. get HdiBackend::GetInstance()
 *                  2. call OnScreenRefresh
 *                  3. check result
 */
HWTEST_F(HdiBackendTest, OnScreenRefresh, Function | MediumTest| Level3)
{
    HdiBackend* data = HdiBackend::GetInstance();
    EXPECT_NE(data, nullptr);
    data->OnScreenRefresh(0);
    std::atomic<bool> ran{false};
    auto onScreenRefreshFunc = [&ran](uint32_t, void*) -> void { ran.store(true); };
    EXPECT_CALL(*hdiDeviceMock_, RegRefreshCallback(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_EQ(data->RegScreenRefresh(onScreenRefreshFunc, data), ROSEN_ERROR_OK);
    data->OnScreenRefresh(0);
    EXPECT_TRUE(ran.load());
    data->onScreenRefreshCb_ = nullptr;
    data->OnScreenRefresh(0);
}

} // namespace
} // namespace Rosen
} // namespace OHOS