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

#include <gtest/gtest.h>
#include <test_header.h>

#include "hgm_config_callback_manager.h"
#include "hgm_test_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr pid_t TEST_PID_1 = 1001;
constexpr pid_t TEST_PID_2 = 1002;
constexpr pid_t TEST_PID_3 = 1003;
constexpr int32_t TEST_REFRESH_RATE = 60;
constexpr int32_t TEST_REFRESH_RATE_MODE = 1;
constexpr int32_t TEST_MAX_XCOMPONENT_ID_NUMS = 50;
}

class MockHgmConfigChangeCallback : public RSIHgmConfigChangeCallback {
public:
    MockHgmConfigChangeCallback() = default;
    ~MockHgmConfigChangeCallback() override = default;

    void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) override
    {
        configChangedCalled_ = true;
        if (configData != nullptr) {
            isSyncConfig_ = configData->GetIsSyncConfig();
        }
    }

    void OnHgmRefreshRateModeChanged(int32_t refreshRateModeName) override
    {
        refreshRateModeChangedCalled_ = true;
        refreshRateMode_ = refreshRateModeName;
    }

    void OnHgmRefreshRateUpdate(int32_t refreshRateUpdate) override
    {
        refreshRateUpdateCalled_ = true;
        refreshRate_ = refreshRateUpdate;
    }

    void Reset()
    {
        configChangedCalled_ = false;
        refreshRateModeChangedCalled_ = false;
        refreshRateUpdateCalled_ = false;
        isSyncConfig_ = false;
        refreshRateMode_ = 0;
        refreshRate_ = 0;
    }

    bool configChangedCalled_ = false;
    bool refreshRateModeChangedCalled_ = false;
    bool refreshRateUpdateCalled_ = false;
    bool isSyncConfig_ = false;
    int32_t refreshRateMode_ = 0;
    int32_t refreshRate_ = 0;
};

class MockFrameRateLinkerExpectedFpsUpdateCallback :
    public RSIFrameRateLinkerExpectedFpsUpdateCallback {
public:
    MockFrameRateLinkerExpectedFpsUpdateCallback() = default;
    ~MockFrameRateLinkerExpectedFpsUpdateCallback() override = default;

    void OnFrameRateLinkerExpectedFpsUpdate(
        pid_t dstPid, const std::string& xcomponentId, int32_t expectedFps) override
    {
        callbackCalled_ = true;
        dstPid_ = dstPid;
        xcomponentId_ = xcomponentId;
        expectedFps_ = expectedFps;
    }

    void Reset()
    {
        callbackCalled_ = false;
        dstPid_ = 0;
        xcomponentId_.clear();
        expectedFps_ = 0;
    }

    bool callbackCalled_ = false;
    pid_t dstPid_ = 0;
    std::string xcomponentId_;
    int32_t expectedFps_ = 0;
};

class HgmConfigCallbackManagerTest : public HgmTestBase {
public:
    static void SetUpTestCase()
    {
        HgmTestBase::SetUpTestCase();
    }
    static void TearDownTestCase() {}
};

/**
 * @tc.name: GetInstance
 * @tc.desc: Verify GetInstance returns singleton instance
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmConfigCallbackManagerTest, GetInstance, Function | SmallTest | Level1)
{
    auto instance1 = HgmConfigCallbackManager::GetInstance();
    auto instance2 = HgmConfigCallbackManager::GetInstance();
    ASSERT_EQ(instance1, instance2);
}

/**
 * @tc.name: RegisterHgmConfigChangeCallback_NullCallback
 * @tc.desc: Verify RegisterHgmConfigChangeCallback with null callback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmConfigCallbackManagerTest, RegisterHgmConfigChangeCallback_NullCallback,
    Function | SmallTest | Level1)
{
    auto manager = HgmConfigCallbackManager::GetInstance();
    manager->RegisterHgmConfigChangeCallback(TEST_PID_1, nullptr);
}

/**
 * @tc.name: RegisterHgmConfigChangeCallback_ValidCallback
 * @tc.desc: Verify RegisterHgmConfigChangeCallback with valid callback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmConfigCallbackManagerTest, RegisterHgmConfigChangeCallback_ValidCallback,
    Function | SmallTest | Level1)
{
    auto manager = HgmConfigCallbackManager::GetInstance();
    auto callback = sptr<MockHgmConfigChangeCallback>(new MockHgmConfigChangeCallback());
    manager->RegisterHgmConfigChangeCallback(TEST_PID_1, callback);
    EXPECT_TRUE(callback->configChangedCalled_);
    EXPECT_TRUE(callback->isSyncConfig_);
    manager->UnRegisterHgmConfigChangeCallback(TEST_PID_1);
}

/**
 * @tc.name: RegisterHgmRefreshRateModeChangeCallback_NullCallback
 * @tc.desc: Verify RegisterHgmRefreshRateModeChangeCallback with null callback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmConfigCallbackManagerTest,
    RegisterHgmRefreshRateModeChangeCallback_NullCallback, Function | SmallTest | Level1)
{
    auto manager = HgmConfigCallbackManager::GetInstance();
    manager->RegisterHgmRefreshRateModeChangeCallback(TEST_PID_1, nullptr);
}

/**
 * @tc.name: RegisterHgmRefreshRateModeChangeCallback_ValidCallback
 * @tc.desc: Verify RegisterHgmRefreshRateModeChangeCallback with valid callback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmConfigCallbackManagerTest,
    RegisterHgmRefreshRateModeChangeCallback_ValidCallback, Function | SmallTest | Level1)
{
    auto manager = HgmConfigCallbackManager::GetInstance();
    auto callback = sptr<MockHgmConfigChangeCallback>(new MockHgmConfigChangeCallback());
    manager->RegisterHgmRefreshRateModeChangeCallback(TEST_PID_1, callback);
    EXPECT_TRUE(callback->refreshRateModeChangedCalled_);
    manager->UnRegisterHgmConfigChangeCallback(TEST_PID_1);
}

/**
 * @tc.name: RegisterHgmRefreshRateUpdateCallback_NullCallback
 * @tc.desc: Verify RegisterHgmRefreshRateUpdateCallback with null callback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmConfigCallbackManagerTest,
    RegisterHgmRefreshRateUpdateCallback_NullCallback, Function | SmallTest | Level1)
{
    auto manager = HgmConfigCallbackManager::GetInstance();
    auto callback = sptr<MockHgmConfigChangeCallback>(new MockHgmConfigChangeCallback());
    manager->RegisterHgmRefreshRateUpdateCallback(TEST_PID_1, callback);
    EXPECT_TRUE(callback->refreshRateUpdateCalled_);
    callback->Reset();
    manager->RegisterHgmRefreshRateUpdateCallback(TEST_PID_1, nullptr);
}

/**
 * @tc.name: RegisterHgmRefreshRateUpdateCallback_ValidCallback
 * @tc.desc: Verify RegisterHgmRefreshRateUpdateCallback with valid callback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmConfigCallbackManagerTest,
    RegisterHgmRefreshRateUpdateCallback_ValidCallback, Function | SmallTest | Level1)
{
    auto manager = HgmConfigCallbackManager::GetInstance();
    auto callback = sptr<MockHgmConfigChangeCallback>(new MockHgmConfigChangeCallback());
    manager->RegisterHgmRefreshRateUpdateCallback(TEST_PID_1, callback);
    EXPECT_TRUE(callback->refreshRateUpdateCalled_);
    manager->UnRegisterHgmConfigChangeCallback(TEST_PID_1);
}

/**
 * @tc.name: RegisterXComponentExpectedFrameRateCallback_NullCallback
 * @tc.desc: Verify RegisterXComponentExpectedFrameRateCallback with null callback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmConfigCallbackManagerTest,
    RegisterXComponentExpectedFrameRateCallback_NullCallback, Function | SmallTest | Level1)
{
    auto manager = HgmConfigCallbackManager::GetInstance();
    auto callback = sptr<MockFrameRateLinkerExpectedFpsUpdateCallback>(
        new MockFrameRateLinkerExpectedFpsUpdateCallback());
    manager->RegisterXComponentExpectedFrameRateCallback(TEST_PID_1, TEST_PID_2, callback);
    callback->Reset();
    manager->RegisterXComponentExpectedFrameRateCallback(TEST_PID_1, TEST_PID_2, nullptr);
}

/**
 * @tc.name: RegisterXComponentExpectedFrameRateCallback_ValidCallback
 * @tc.desc: Verify RegisterXComponentExpectedFrameRateCallback with valid callback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmConfigCallbackManagerTest,
    RegisterXComponentExpectedFrameRateCallback_ValidCallback, Function | SmallTest | Level1)
{
    auto manager = HgmConfigCallbackManager::GetInstance();
    auto callback = sptr<MockFrameRateLinkerExpectedFpsUpdateCallback>(
        new MockFrameRateLinkerExpectedFpsUpdateCallback());
    manager->RegisterXComponentExpectedFrameRateCallback(TEST_PID_1, TEST_PID_2, callback);
    manager->UnRegisterHgmConfigChangeCallback(TEST_PID_1);
}

/**
 * @tc.name: RegisterXComponentExpectedFrameRateCallback_WithExistingFrameRate
 * @tc.desc: Verify RegisterXComponentExpectedFrameRateCallback with existing frame rate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmConfigCallbackManagerTest,
    RegisterXComponentExpectedFrameRateCallback_WithExistingFrameRate,
    Function | SmallTest | Level1)
{
    auto manager = HgmConfigCallbackManager::GetInstance();
    const std::string xcomponentId = "test_xcomponent";
    manager->SyncXComponentExpectedFrameRateCallback(TEST_PID_2, xcomponentId, TEST_REFRESH_RATE);

    auto callback = sptr<MockFrameRateLinkerExpectedFpsUpdateCallback>(
        new MockFrameRateLinkerExpectedFpsUpdateCallback());
    manager->RegisterXComponentExpectedFrameRateCallback(TEST_PID_1, TEST_PID_2, callback);
    EXPECT_TRUE(callback->callbackCalled_);
    EXPECT_EQ(callback->dstPid_, TEST_PID_2);
    EXPECT_EQ(callback->xcomponentId_, xcomponentId);
    EXPECT_EQ(callback->expectedFps_, TEST_REFRESH_RATE);
    manager->UnRegisterHgmConfigChangeCallback(TEST_PID_1);
    manager->UnRegisterHgmConfigChangeCallback(TEST_PID_2);
}

/**
 * @tc.name: SyncHgmConfigChangeCallback_NoExtraPid
 * @tc.desc: Verify SyncHgmConfigChangeCallback without extra pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmConfigCallbackManagerTest, SyncHgmConfigChangeCallback_NoExtraPid,
    Function | SmallTest | Level1)
{
    auto manager = HgmConfigCallbackManager::GetInstance();
    manager->SyncHgmConfigChangeCallback(0);
}

/**
 * @tc.name: SyncHgmConfigChangeCallback_WithExtraPid
 * @tc.desc: Verify SyncHgmConfigChangeCallback with extra pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmConfigCallbackManagerTest, SyncHgmConfigChangeCallback_WithExtraPid,
    Function | SmallTest | Level1)
{
    auto manager = HgmConfigCallbackManager::GetInstance();
    manager->SyncHgmConfigChangeCallback(TEST_PID_1);
}
