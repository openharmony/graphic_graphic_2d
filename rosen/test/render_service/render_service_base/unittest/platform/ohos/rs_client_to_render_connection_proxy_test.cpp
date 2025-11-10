/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <if_system_ability_manager.h>
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <mutex>
#include <system_ability_definition.h>
#include <unistd.h>

#include "feature/capture/rs_ui_capture.h"
#include "file_ex.h"
#include "platform/ohos/rs_client_to_render_connection_proxy.h"
#include "platform/ohos/rs_render_service_connect_hub.h"
#include "command/rs_animation_command.h"
#include "command/rs_node_showing_command.h"
#include "iconsumer_surface.h"
#include "pixel_map.h"
#include "feature/capture/rs_capture_pixelmap_manager.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
class MockRSBrightnessInfoChangeCallback : public IRemoteProxy<RSIBrightnessInfoChangeCallback> {
public:
    explicit MockRSBrightnessInfoChangeCallback() : IRemoteProxy<RSIBrightnessInfoChangeCallback>(nullptr) {};
    virtual ~MockRSBrightnessInfoChangeCallback() noexcept = default;

    void OnBrightnessInfoChange(ScreenId screenId, const BrightnessInfo& brightnessInfo) override {}
};
} // namespace
class RSClientToRenderConnectionProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<RSClientToRenderConnectionProxy> proxy;
};

void RSClientToRenderConnectionProxyTest::SetUpTestCase()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    proxy = std::make_shared<RSClientToRenderConnectionProxy>(remoteObject);
}
void RSClientToRenderConnectionProxyTest::TearDownTestCase() {}
void RSClientToRenderConnectionProxyTest::SetUp() {}
void RSClientToRenderConnectionProxyTest::TearDown() {}
// Add test function forward declaration
void WaitNeedRegisterTypefaceReply(uint8_t rspRpc, int& retryCount);

class IRemoteObjectMock : public IRemoteObject {
public:
    IRemoteObjectMock() : IRemoteObject {u"IRemoteObjectMock"}
    {
    }

    ~IRemoteObjectMock()
    {
    }

    int32_t GetObjectRefCount()
    {
        return 0;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

    int Dump(int fd, const std::vector<std::u16string> &args)
    {
        return 0;
    }

    MOCK_METHOD4(SendRequest, int32_t(uint32_t, MessageParcel&, MessageParcel&, MessageOption&));
};

/**
 * @tc.name: SetFocusAppInfo Test
 * @tc.desc: SetFocusAppInfo Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, SetFocusAppInfo, TestSize.Level1)
{
    int32_t pid = 1;
    int32_t uid = 1;
    std::string bundleName("bundle");
    std::string abilityName("ability");
    uint64_t focusNodeId = 1;
    FocusAppInfo info = {
        .pid = pid,
        .uid = uid,
        .bundleName = bundleName,
        .abilityName = abilityName,
        .focusNodeId = focusNodeId};
    int32_t repCode;
    proxy->SetFocusAppInfo(info, repCode);
    ASSERT_EQ(repCode, 0);
}

/**
 * @tc.name: RegisterTransactionDataCallback01
 * @tc.desc: RegisterTransactionDataCallback Test normal
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, RegisterTransactionDataCallback01, TestSize.Level1)
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSITransactionDataCallback> callback = iface_cast<RSITransactionDataCallback>(remoteObject);
    proxy->RegisterTransactionDataCallback(1, 456, callback);
    ASSERT_NE(proxy->transactionDataIndex_, 5);
}

/**
 * @tc.name: TakeSurfaceCapture Test
 * @tc.desc: TakeSurfaceCapture Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, TakeSurfaceCapture, TestSize.Level1)
{
    NodeId id = 1;
    sptr<RSISurfaceCaptureCallback> callback;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    captureConfig.useDma = false;
    captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
    captureConfig.isSync = true;
    captureConfig.blackList = std::vector<NodeId>{0};
    ASSERT_FALSE(captureConfig.blackList.empty());
    RSSurfaceCaptureBlurParam blurParam;
    blurParam.isNeedBlur = true;
    blurParam.blurRadius = 10;
    Drawing::Rect specifiedAreaRect(0.f, 0.f, 0.f, 0.f);
    proxy->TakeSurfaceCapture(id, callback, captureConfig, blurParam, specifiedAreaRect);

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
    proxy->TakeSurfaceCapture(id, callback, captureConfig, blurParam, specifiedAreaRect);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: TakeUICaptureInRange Test
 * @tc.desc: TakeUICaptureInRange Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, TakeUICaptureInRange, TestSize.Level1)
{
    NodeId id = 1;
    sptr<RSISurfaceCaptureCallback> callback;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    captureConfig.useDma = false;
    captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
    captureConfig.isSync = true;

    proxy->TakeUICaptureInRange(id, callback, captureConfig);

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
    proxy->TakeUICaptureInRange(id, callback, captureConfig);
    ASSERT_NE(proxy->transactionDataIndex_, 5);
}

/**
 * @tc.name: TakeSurfaceCaptureSoloNode Test
 * @tc.desc: TakeSurfaceCaptureSoloNode Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, TakeSurfaceCaptureSoloNode, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    captureConfig.useDma = false;
    captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
    captureConfig.isSync = false;
    proxy->TakeSurfaceCaptureSoloNode(id, captureConfig);
    EXPECT_NE(proxy->transactionDataIndex_, 5);
}

/**
 * @tc.name: SetHwcNodeBounds Test
 * @tc.desc: SetHwcNodeBounds Test
 * @tc.type:FUNC
 * @tc.require: issueIB2O0L
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, SetHwcNodeBounds, TestSize.Level1)
{
    NodeId id = 1;
    proxy->SetHwcNodeBounds(id, 1.0f, 1.0f, 1.0f, 1.0f);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: SetWindowContainer Test
 * @tc.desc: SetWindowContainer Test
 * @tc.type:FUNC
 * @tc.require: issueIBIK1X
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, SetWindowContainer, TestSize.Level1)
{
    NodeId nodeId = {};
    proxy->SetWindowContainer(nodeId, false);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: ClearUifirstCache Test
 * @tc.desc: ClearUifirstCache Test
 * @tc.type:FUNC
 * @tc.require: issueICK4SM
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, ClearUifirstCacheTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    proxy->ClearUifirstCache(nodeId);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: TakeSurfaceCaptureWithAllWindowsTest
 * @tc.desc: TakeSurfaceCaptureWithAllWindows test to capture screen
 * @tc.type:FUNC
 * @tc.require: issueICQ74B
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, TakeSurfaceCaptureWithAllWindowsTest, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    NodeId nodeId = 1;
    bool checkDrmAndSurfaceLock = false;
    sptr<RSISurfaceCaptureCallback> callback;
    RSSurfaceCaptureConfig captureConfig;
    auto ret = proxy->TakeSurfaceCaptureWithAllWindows(nodeId, callback, captureConfig, checkDrmAndSurfaceLock);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
    ret = proxy->TakeSurfaceCaptureWithAllWindows(nodeId, callback, captureConfig, checkDrmAndSurfaceLock);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: FreezeScreenTest
 * @tc.desc: FreezeScreen test to freeze or unfreeze screen.
 * @tc.type:FUNC
 * @tc.require: issueICS2J8
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, FreezeScreenTest, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    NodeId nodeId = 1;
    bool isFreeze = false;
    auto ret = proxy->FreezeScreen(nodeId, isFreeze);
    EXPECT_EQ(ret, ERR_OK);
}

/*
 * @tc.name: DropFrameByPidWithInvalidParameter Test
 * @tc.desc: DropFrameByPidWithInvalidParameter Test
 * @tc.type:FUNC
 * @tc.require: issueICK4SM
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, DropFrameByPidWithInvalidParameter, TestSize.Level1)
{
    // MAX_DROP_FRAME_PID_LIST_SIZE = 1024
    std::vector<int32_t> vec(1025);
    ASSERT_EQ(proxy->DropFrameByPid(vec), ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetScreenFrameGravity Test
 * @tc.desc: SetScreenFrameGravity Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, SetScreenFrameGravity, TestSize.Level1)
{
    ScreenId id = 100;
    int32_t gravity = 5;
    proxy->SetScreenFrameGravity(id, gravity);
    ASSERT_NE(proxy->transactionDataIndex_, 5);
}
} // namespace Rosen
} // namespace OHOS