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
    (void)hdiBackend_->SetHdiBackendDevice(hdiDeviceMock_);
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
    HdiBackendTest::hdiBackend_->device_ = HdiBackendTest::hdiDeviceMock_;
    auto func = [](OutputPtr& output, bool connected, void* data) -> void {
        (void)output;
        (void)connected;
        (void)data;
    };
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
    HdiBackendTest::hdiBackend_->device_ = HdiBackendTest::hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, RegRefreshCallback(_, _)).WillRepeatedly(testing::Return(0));
    auto func = [](uint32_t devId, void* data) -> void {
        (void)devId;
        (void)data;
    };
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
    auto func = [](void* data) -> void { (void)data; };
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
    auto func = [](uint32_t devId, uint64_t ns, void* data) -> void {
        (void)devId;
        (void)ns;
        (void)data;
    };
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
    VSyncSampler::SetScreenVsyncEnabledCallback cb = [](uint64_t screenId, bool enabled) {
        (void)screenId;
        (void)enabled;
    };
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

    auto func = [](uint32_t devId, uint32_t envId, const std::vector<int32_t>& eventData, void* data) -> void {
        (void)devId;
        (void)envId;
        (void)eventData;
        (void)data;
    };
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
 * Function: OnHdiBackendHotPlugEvent_ConnectCreatesOutput
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: connected=true creates output in backend map
 */
HWTEST_F(HdiBackendTest, OnHdiBackendHotPlugEvent_ConnectCreatesOutput, Function | MediumTest| Level3)
{
    auto backend = HdiBackend::GetInstance();
    backend->outputs_.clear();
    backend->OnHdiBackendHotPlugEvent(99, true, backend);
    auto it = backend->outputs_.find(99);
    EXPECT_NE(it, backend->outputs_.end());
    // disconnect removes entry
    backend->onScreenHotplugCb_ = nullptr;
    backend->OnHdiBackendHotPlugEvent(99, false, backend);
    EXPECT_EQ(backend->outputs_.find(99), backend->outputs_.end());
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

/**
 * Function: InitDevice_DeviceAlreadyExists_Line234_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. device_ is not nullptr
 *                  2. call InitDevice
 *                  3. verify returns OK early (line 234 true)
 */
HWTEST_F(HdiBackendTest, InitDevice_DeviceAlreadyExists_Line234_True, Function | MediumTest | Level3)
{
    hdiBackend_->device_ = hdiDeviceMock_;

    RosenError ret = hdiBackend_->InitDevice();
    EXPECT_EQ(ret, ROSEN_ERROR_OK);
}

/**
 * Function: SetHdiBackendDevice_DeviceNull_Line250_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. input device is nullptr
 *                  2. call SetHdiBackendDevice
 *                  3. verify returns ROSEN_ERROR_INVALID_ARGUMENTS
 */
HWTEST_F(HdiBackendTest, SetHdiBackendDevice_DeviceNull_Line250_True, Function | MediumTest | Level3)
{
    ASSERT_EQ(hdiBackend_->SetHdiBackendDevice(nullptr), ROSEN_ERROR_INVALID_ARGUMENTS);
}

/**
 * Function: SetHdiBackendDevice_DeviceAlreadySet_Line255_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. device_ is already set
 *                  2. call SetHdiBackendDevice with valid device
 *                  3. verify returns ROSEN_ERROR_OK (line 255 true)
 */
HWTEST_F(HdiBackendTest, SetHdiBackendDevice_DeviceAlreadySet_Line255_True, Function | MediumTest | Level3)
{
    hdiBackend_->device_ = hdiDeviceMock_;

    RosenError ret = hdiBackend_->SetHdiBackendDevice(hdiDeviceMock_);
    EXPECT_EQ(ret, ROSEN_ERROR_OK);
}

/**
 * Function: RegHwcEventCallback_RegHwcEventCallbackFail_Line276_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. RegHwcEventCallback returns GRAPHIC_DISPLAY_SUCCESS
 *                  2. call RegHwcEventCallback
 *                  3. verify returns ROSEN_ERROR_OK
 */
HWTEST_F(HdiBackendTest, RegHwcEventCallback_RegHwcEventCallbackFail_Line276_True, Function | MediumTest | Level3)
{
    auto func = [](uint32_t devId, uint32_t envId, const std::vector<int32_t>& eventData, void* data) -> void {
        (void)devId;
        (void)envId;
        (void)eventData;
        (void)data;
    };
    hdiBackend_->device_ = hdiDeviceMock_;

    EXPECT_CALL(*hdiDeviceMock_, RegHwcEventCallback(_, _)).WillOnce(testing::Return(GRAPHIC_DISPLAY_FAILURE));

    RosenError ret = hdiBackend_->RegHwcEventCallback(func, nullptr);
    EXPECT_EQ(ret, ROSEN_ERROR_API_FAILED);
}

/**
 * Function: OnHdiBackendHotPlugEvent_DataNotNull_Line171_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. data is not nullptr
 *                  2. call OnHdiBackendHotPlugEvent
 *                  3. verify uses data as hdiBackend (line 171 true)
 */
HWTEST_F(HdiBackendTest, OnHdiBackendHotPlugEvent_DataNotNull_Line171_True, Function | MediumTest | Level3)
{
    HdiBackend* data = HdiBackend::GetInstance();
    EXPECT_NE(data, nullptr);

    data->OnHdiBackendHotPlugEvent(1, true, data);
    auto iter = data->outputs_.find(1);
    EXPECT_NE(iter, data->outputs_.end());
}

/**
 * Function: OnHdiBackendHotPlugEvent_DataNull_Line171_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. data is nullptr
 *                  2. call OnHdiBackendHotPlugEvent
 *                  3. verify uses GetInstance() (line 171 false, 174 true)
 */
HWTEST_F(HdiBackendTest, OnHdiBackendHotPlugEvent_DataNull_Line171_False, Function | MediumTest | Level3)
{
    HdiBackend* backend = HdiBackend::GetInstance();
    EXPECT_NE(backend, nullptr);

    backend->OnHdiBackendHotPlugEvent(1, true, nullptr);
    auto iter = backend->outputs_.find(1);
    EXPECT_NE(iter, backend->outputs_.end());
}

/**
 * Function: OnHdiBackendConnected_ConnectedTrue_Line201_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. connected is true
 *                  2. call OnHdiBackendConnected
 *                  3. verify CreateHdiOutput is called (line 201 true)
 */
HWTEST_F(HdiBackendTest, OnHdiBackendConnected_ConnectedTrue_Line201_True, Function | MediumTest | Level3)
{
    auto backend = HdiBackend::GetInstance();
    backend->outputs_.clear();

    backend->OnHdiBackendConnected(100, true);
    auto iter = backend->outputs_.find(100);
    EXPECT_NE(iter, backend->outputs_.end());
}

/**
 * Function: OnHdiBackendConnected_ConnectedFalse_Line201_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. connected is false
 *                  2. call OnHdiBackendConnected
 *                  3. verify CreateHdiOutput NOT called
 */
HWTEST_F(HdiBackendTest, OnHdiBackendConnected_ConnectedFalse_Line201_False, Function | MediumTest | Level3)
{
    auto backend = HdiBackend::GetInstance();
    backend->outputs_.clear();

    backend->OnHdiBackendConnected(100, false);
    auto iter = backend->outputs_.find(100);
    EXPECT_EQ(iter, backend->outputs_.end());
}

/**
 * Function: OnScreenHotplug_ScreenIdNotFound_Line218_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. screenId not in outputs_
 *                  2. call OnScreenHotplug
 *                  3. verify early return (line 218 true)
 */
HWTEST_F(HdiBackendTest, OnScreenHotplug_ScreenIdNotFound_Line218_True, Function | MediumTest | Level3)
{
    auto backend = HdiBackend::GetInstance();
    OutputPtr output = HdiOutput::CreateHdiOutput(100);
    backend->outputs_[100] = output;

    // Call with different screenId
    backend->OnScreenHotplug(999, true);

    // Verify 999 was not added
    auto iter = backend->outputs_.find(999);
    EXPECT_EQ(iter, backend->outputs_.end());
}

/**
 * Function: OnScreenHotplug_CallbackNotNull_Line223_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. onScreenHotplugCb_ is not nullptr
 *                  2. call OnScreenHotplug
 *                  3. verify callback is called
 */
HWTEST_F(HdiBackendTest, OnScreenHotplug_CallbackNotNull_Line223_True, Function | MediumTest | Level3)
{
    auto backend = HdiBackend::GetInstance();
    OutputPtr output = HdiOutput::CreateHdiOutput(100);
    backend->outputs_[100] = output;

    std::atomic<bool> callbackCalled{false};
    auto func = [&callbackCalled](OutputPtr& output, bool connected, void* data) -> void {
        (void)output;
        (void)connected;
        callbackCalled.store(true);
    };
    backend->onScreenHotplugCb_ = func;

    backend->OnScreenHotplug(100, true);
    EXPECT_TRUE(callbackCalled.load());
}

/**
 * Function: OnScreenHotplug_CallbackNull_Line223_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. onScreenHotplugCb_ is nullptr
 *                  2. call OnScreenHotplug
 *                  3. verify callback is not called
 */
HWTEST_F(HdiBackendTest, OnScreenHotplug_CallbackNull_Line223_False, Function | MediumTest | Level3)
{
    auto backend = HdiBackend::GetInstance();
    OutputPtr output = HdiOutput::CreateHdiOutput(100);
    backend->outputs_[100] = output;
    backend->onScreenHotplugCb_ = nullptr;

    backend->OnScreenHotplug(100, true);
    // connected is true, so output should NOT be erased
    auto iter = backend->outputs_.find(100);
    EXPECT_NE(iter, backend->outputs_.end());
}

/**
 * Function: OnScreenHotplug_NotConnected_Line227_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. connected is false
 *                  2. call OnScreenHotplug
 *                  3. verify output is erased (line 227 true)
 */
HWTEST_F(HdiBackendTest, OnScreenHotplug_NotConnected_Line227_True, Function | MediumTest | Level3)
{
    auto backend = HdiBackend::GetInstance();
    OutputPtr output = HdiOutput::CreateHdiOutput(100);
    backend->outputs_[100] = output;

    backend->OnScreenHotplug(100, false);
    auto iter = backend->outputs_.find(100);
    EXPECT_EQ(iter, backend->outputs_.end());
}

/**
 * Function: OnScreenHotplug_Connected_Line227_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. connected is true
 *                  2. call OnScreenHotplug
 *                  3. verify output is NOT erased
 */
HWTEST_F(HdiBackendTest, OnScreenHotplug_Connected_Line227_False, Function | MediumTest | Level3)
{
    auto backend = HdiBackend::GetInstance();
    OutputPtr output = HdiOutput::CreateHdiOutput(100);
    backend->outputs_[100] = output;

    backend->OnScreenHotplug(100, true);
    auto iter = backend->outputs_.find(100);
    EXPECT_NE(iter, backend->outputs_.end());
}

/**
 * Function: ResetDevice_DeviceNull_Line159_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. device_ is nullptr
 *                  2. call ResetDevice
 *                  3. verify no crash, outputs cleared
 */
HWTEST_F(HdiBackendTest, ResetDevice_DeviceNull_Line159_False, Function | MediumTest | Level3)
{
    hdiBackend_->device_ = nullptr;
    OutputPtr output = HdiOutput::CreateHdiOutput(0);
    hdiBackend_->outputs_[0] = output;

    hdiBackend_->ResetDevice();
    EXPECT_EQ(hdiBackend_->outputs_.size(), 0);
}

/**
 * Function: OnHdiBackendRefreshEvent_DataNotNull_Line183_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. data is not nullptr
 *                  2. call OnHdiBackendRefreshEvent
 *                  3. verify uses data as hdiBackend (line 183 true)
 */
HWTEST_F(HdiBackendTest, OnHdiBackendRefreshEvent_DataNotNull_Line183_True, Function | MediumTest | Level3)
{
    HdiBackend* data = HdiBackend::GetInstance();
    EXPECT_NE(data, nullptr);

    data->OnHdiBackendRefreshEvent(0, data);
    // Should not crash
}

/**
 * Function: OnHdiBackendRefreshEvent_DataNull_Line183_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. data is nullptr
 *                  2. call OnHdiBackendRefreshEvent
 *                  3. verify uses GetInstance() (line 183 false, 186 true)
 */
HWTEST_F(HdiBackendTest, OnHdiBackendRefreshEvent_DataNull_Line183_False, Function | MediumTest | Level3)
{
    HdiBackend* backend = HdiBackend::GetInstance();
    EXPECT_NE(backend, nullptr);

    backend->OnHdiBackendRefreshEvent(0, nullptr);
    // Should not crash
}

/**
 * Function: OnScreenRefresh_CallbackNull_Line194_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. onScreenRefreshCb_ is nullptr
 *                  2. call OnScreenRefresh
 *                  3. verify callback is not called
 */
HWTEST_F(HdiBackendTest, OnScreenRefresh_CallbackNull_Line194_False, Function | MediumTest | Level3)
{
    HdiBackend* backend = HdiBackend::GetInstance();
    backend->onScreenRefreshCb_ = nullptr;

    backend->OnScreenRefresh(0);
    // Callback should not be called when onScreenRefreshCb_ is nullptr
    EXPECT_EQ(backend->onScreenRefreshCb_, nullptr);
}

/**
 * Function: OnScreenRefresh_CallbackNotNull_Line194_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. onScreenRefreshCb_ is not nullptr
 *                  2. call OnScreenRefresh
 *                  3. verify callback is called
 */
HWTEST_F(HdiBackendTest, OnScreenRefresh_CallbackNotNull_Line194_True, Function | MediumTest | Level3)
{
    HdiBackend* backend = HdiBackend::GetInstance();
    std::atomic<bool> callbackCalled{false};
    auto func = [&callbackCalled](uint32_t, void*) -> void { callbackCalled.store(true); };
    EXPECT_CALL(*hdiDeviceMock_, RegRefreshCallback(_, _)).WillRepeatedly(testing::Return(0));
    backend->RegScreenRefresh(func, backend);

    backend->OnScreenRefresh(0);
    EXPECT_TRUE(callbackCalled.load());

    backend->onScreenRefreshCb_ = nullptr;
}

/**
 * Function: SetPendingMode_OutputNull_Line122_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. output is nullptr
 *                  2. call SetPendingMode
 *                  3. verify early return (line 122 true)
 */
HWTEST_F(HdiBackendTest, SetPendingMode_OutputNull_Line122_True, Function | MediumTest | Level3)
{
    OutputPtr nullOutput = nullptr;
    ASSERT_NE(hdiBackend_, nullptr);
    hdiBackend_->SetPendingMode(nullOutput, 1000, 2000);
}

/**
 * Function: SetPendingMode_OutputNotNull_Line122_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. output is not nullptr
 *                  2. call SetPendingMode
 *                  3. verify SetPendingMode is called
 */
HWTEST_F(HdiBackendTest, SetPendingMode_OutputNotNull_Line122_False, Function | MediumTest | Level3)
{
    OutputPtr output = HdiOutput::CreateHdiOutput(0);
    ASSERT_NE(output, nullptr);
    hdiBackend_->SetPendingMode(output, 1000, 2000);
}

/**
 * Function: StartSample_OutputNull_Line131_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. output is nullptr
 *                  2. call StartSample
 *                  3. verify early return (line 131 true)
 */
HWTEST_F(HdiBackendTest, StartSample_OutputNull_Line131_True, Function | MediumTest | Level3)
{
    OutputPtr nullOutput = nullptr;
    ASSERT_NE(hdiBackend_, nullptr);
    hdiBackend_->StartSample(nullOutput);
}

/**
 * Function: StartSample_OutputNotNull_Line131_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. output is not nullptr
 *                  2. call StartSample
 *                  3. verify StartVSyncSampler is called
 */
HWTEST_F(HdiBackendTest, StartSample_OutputNotNull_Line131_False, Function | MediumTest | Level3)
{
    OutputPtr output = HdiOutput::CreateHdiOutput(0);
    ASSERT_NE(hdiBackend_, nullptr);
    hdiBackend_->StartSample(output);
}

/**
 * Function: SetVsyncSamplerEnabled_OutputNull_Line140_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. output is nullptr
 *                  2. call SetVsyncSamplerEnabled
 *                  3. verify early return (line 140 true)
 */
HWTEST_F(HdiBackendTest, SetVsyncSamplerEnabled_OutputNull_Line140_True, Function | MediumTest | Level3)
{
    OutputPtr nullOutput = nullptr;
    ASSERT_NE(hdiBackend_, nullptr);
    hdiBackend_->SetVsyncSamplerEnabled(nullOutput, true);
}

/**
 * Function: SetVsyncSamplerEnabled_OutputNotNull_Line140_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. output is not nullptr
 *                  2. call SetVsyncSamplerEnabled
 *                  3. verify SetVsyncSamplerEnabled is called
 */
HWTEST_F(HdiBackendTest, SetVsyncSamplerEnabled_OutputNotNull_Line140_False, Function | MediumTest | Level3)
{
    OutputPtr output = HdiOutput::CreateHdiOutput(0);
    ASSERT_NE(output, nullptr);
    hdiBackend_->SetVsyncSamplerEnabled(output, true);
}

/**
 * Function: GetVsyncSamplerEnabled_OutputNull_Line149_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. output is nullptr
 *                  2. call GetVsyncSamplerEnabled
 *                  3. verify returns false (line 149 true)
 */
HWTEST_F(HdiBackendTest, GetVsyncSamplerEnabled_OutputNull_Line149_True, Function | MediumTest | Level3)
{
    bool ret = hdiBackend_->GetVsyncSamplerEnabled(nullptr);
    EXPECT_EQ(ret, false);
}

/**
 * Function: GetVsyncSamplerEnabled_OutputNotNull_Line149_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. output is not nullptr
 *                  2. call GetVsyncSamplerEnabled
 *                  3. verify returns output's value
 */
HWTEST_F(HdiBackendTest, GetVsyncSamplerEnabled_OutputNotNull_Line149_False, Function | MediumTest | Level3)
{
    OutputPtr output = HdiOutput::CreateHdiOutput(0);
    hdiBackend_->SetVsyncSamplerEnabled(output, false);

    bool ret = hdiBackend_->GetVsyncSamplerEnabled(output);
    EXPECT_EQ(ret, false);
}

/**
 * Function: CreateHdiOutput_VerifyOutputAdded
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call CreateHdiOutput
 *                  2. verify output is added to outputs_
 */
HWTEST_F(HdiBackendTest, CreateHdiOutput_VerifyOutputAdded, Function | MediumTest | Level3)
{
    auto backend = HdiBackend::GetInstance();
    backend->outputs_.clear();

    backend->CreateHdiOutput(200);
    auto iter = backend->outputs_.find(200);
    EXPECT_NE(iter, backend->outputs_.end());
}

/**
 * Function: OnHdiBackendConnected_CreateAndRegisterOutput
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. connected is true
 *                  2. verify CreateHdiOutput and output registration
 */
HWTEST_F(HdiBackendTest, OnHdiBackendConnected_CreateAndRegisterOutput, Function | MediumTest | Level3)
{
    auto backend = HdiBackend::GetInstance();
    backend->outputs_.clear();

    std::atomic<uint32_t> screenId{0};
    auto hotplugFunc = [&screenId](OutputPtr& output, bool connected, void* data) -> void {
        if (connected) {
            screenId.store(output->GetScreenId());
        }
    };
    backend->onScreenHotplugCb_ = hotplugFunc;

    backend->OnHdiBackendConnected(300, true);
    EXPECT_EQ(screenId.load(), 300u);
}

/**
 * Function: ResetDevice_ClearsOutputs
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call ResetDevice
 *                  2. verify device_ is set to nullptr and outputs_ is cleared
 */
HWTEST_F(HdiBackendTest, ResetDevice_ClearsOutputs, Function | MediumTest | Level3)
{
    auto backend = HdiBackend::GetInstance();
    OutputPtr output = HdiOutput::CreateHdiOutput(0);
    backend->outputs_[0] = output;
    backend->device_ = hdiDeviceMock_;

    EXPECT_CALL(*hdiDeviceMock_, Destroy());
    backend->ResetDevice();

    EXPECT_EQ(backend->device_, nullptr);
    EXPECT_EQ(backend->outputs_.size(), 0u);
}

/**
 * Function: InitDevice_AlreadyInitialized_Line234_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. device_ already initialized
 *                  2. call InitDevice
 *                  3. verify returns OK without reinitializing
 */
HWTEST_F(HdiBackendTest, InitDevice_AlreadyInitialized_Line234_True, Function | MediumTest | Level3)
{
    hdiBackend_->device_ = hdiDeviceMock_;

    RosenError ret = hdiBackend_->InitDevice();
    EXPECT_EQ(ret, ROSEN_ERROR_OK);
    // Verify GetInstance was not called again
}

/**
 * Function: InitDevice_SuccessfulInitialization
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. device_ is nullptr
 *                  2. GetInstance returns valid device
 *                  3. verify successful initialization
 */
HWTEST_F(HdiBackendTest, InitDevice_SuccessfulInitialization, Function | MediumTest | Level3)
{
    hdiBackend_->device_ = nullptr;

    RosenError ret = hdiBackend_->InitDevice();
    EXPECT_EQ(ret, ROSEN_ERROR_OK);
    EXPECT_NE(hdiBackend_->device_, nullptr);
}

} // namespace
} // namespace Rosen
} // namespace OHOS