/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "callback/rs_screen_callback_manager.h"
#include "gtest/gtest.h"
#include "public/rs_screen_manager_agent.h"
#include "rs_screen_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSScreenCallbackManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    class MockCoreListener : public RSIScreenManagerListener {
        sptr<IRemoteObject> OnScreenConnected(ScreenId id, const std::shared_ptr<HdiOutput>& output,
            const sptr<RSScreenProperty>& property) override { return nullptr; };
        void OnScreenDisconnected(ScreenId id) override {};
        void OnHwcRestored(ScreenId id, const std::shared_ptr<HdiOutput>& output,
            const sptr<RSScreenProperty>& property) override {};
        void OnHwcDead(ScreenId id) override {};
        void OnScreenPropertyChanged(ScreenId id, const sptr<RSScreenProperty>& property) override {};
        void OnScreenRefresh(ScreenId id) override {};
        void OnVBlankIdle(ScreenId id, uint64_t ns) override {};
        void OnVirtualScreenConnected(ScreenId id, ScreenId associatedScreenId,
            const sptr<RSScreenProperty>& property) override {};
        void OnVirtualScreenDisconnected(ScreenId id) override {};
        void OnHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData) override {};
        void OnActiveScreenIdChanged(ScreenId activeScreenId) override {};
        void OnScreenBacklightChanged(ScreenId id, uint32_t level) override {};
    };

    std::shared_ptr<RSScreenCallbackManager> callbackMgr_ = std::make_shared<RSScreenCallbackManager>();

    sptr<MockCoreListener> coreListener_ = sptr<MockCoreListener>::MakeSptr();
};

void RSScreenCallbackManagerTest::SetUpTestCase() {}

void RSScreenCallbackManagerTest::TearDownTestCase() {}

void RSScreenCallbackManagerTest::SetUp() {}

void RSScreenCallbackManagerTest::TearDown() {}

/*
 * @tc.name: SetAndGetCoreListenerTest
 * @tc.desc: Test SetCoreListener and GetCoreListener
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCallbackManagerTest, SetAndGetCoreListenerTest, TestSize.Level0)
{
    if (callbackMgr_->coreListener_) {
        callbackMgr_->coreListener_ = nullptr;
    }
    ASSERT_EQ(callbackMgr_->GetCoreListener(), nullptr);
    callbackMgr_->SetCoreListener(coreListener_);
    ASSERT_NE(callbackMgr_->GetCoreListener(), nullptr);
    callbackMgr_->coreListener_ = nullptr;
}

/*
 * @tc.name: AddOrRemoveAgentListenerTest
 * @tc.desc: Test AddAgentListener and RemoveAgentListener
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCallbackManagerTest, AddOrRemoveAgentListenerTest, TestSize.Level0)
{
    auto screenManagerAgentListener = sptr<RSScreenManagerAgentListener>::MakeSptr();
    ASSERT_EQ(callbackMgr_->agentListeners_.size(), 0);
    callbackMgr_->AddAgentListener(screenManagerAgentListener);
    ASSERT_EQ(callbackMgr_->agentListeners_.size(), 1);
    callbackMgr_->RemoveAgentListener(screenManagerAgentListener);
    ASSERT_EQ(callbackMgr_->agentListeners_.size(), 0);
}

/*
 * @tc.name: NotifyScreenConnectedAndDisconnectTest
 * @tc.desc: Test NotifyScreenConnected and NotifyScreenDisconnected
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCallbackManagerTest, NotifyScreenConnectedAndDisconnectTest, TestSize.Level0)
{
    ScreenId screenId = 100;
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    property->isVirtual_ = true;
    ScreenPresenceEvent event { .id = screenId, .output = nullptr, .property = property };
    if (callbackMgr_->coreListener_) {
        callbackMgr_->coreListener_ = nullptr;
    }
    ASSERT_EQ(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyScreenConnected(event);
    callbackMgr_->NotifyScreenDisconnected(screenId);
    callbackMgr_->SetCoreListener(coreListener_);
    ASSERT_NE(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyScreenConnected(event);
    auto iter = callbackMgr_->clientToRenderConns_.find(screenId);
    ASSERT_NE(iter, callbackMgr_->clientToRenderConns_.end());
    callbackMgr_->NotifyScreenDisconnected(screenId);
    ASSERT_EQ(callbackMgr_->clientToRenderConns_.size(), 0);
    callbackMgr_->coreListener_ = nullptr;
}

/*
 * @tc.name: NotifyScreenPropertyUpdatedTest
 * @tc.desc: Test NotifyScreenPropertyUpdated
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCallbackManagerTest, NotifyScreenPropertyUpdatedTest, TestSize.Level0)
{
    ScreenId screenId = 100;
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    property->isVirtual_ = true;
    ASSERT_EQ(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyScreenPropertyUpdated(screenId, property);
    callbackMgr_->SetCoreListener(coreListener_);
    ASSERT_NE(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyScreenPropertyUpdated(screenId, property);
    callbackMgr_->coreListener_ = nullptr;
}

/*
 * @tc.name: NotifyScreenRefreshTest
 * @tc.desc: Test NotifyScreenRefresh
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCallbackManagerTest, NotifyScreenRefreshTest, TestSize.Level0)
{
    ScreenId screenId = 100;
    ASSERT_EQ(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyScreenRefresh(screenId);
    callbackMgr_->SetCoreListener(coreListener_);
    ASSERT_NE(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyScreenRefresh(screenId);
    callbackMgr_->coreListener_ = nullptr;
}

/*
 * @tc.name: NotifyHwcRestoredTest
 * @tc.desc: Test NotifyHwcRestored
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCallbackManagerTest, NotifyHwcRestoredTest, TestSize.Level0)
{
    ScreenId screenId = 100;
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    ScreenPresenceEvent event { .id = screenId, .output = nullptr, .property = property };
    ASSERT_EQ(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyHwcRestored(event);
    callbackMgr_->SetCoreListener(coreListener_);
    ASSERT_NE(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyHwcRestored(event);
    callbackMgr_->coreListener_ = nullptr;
}

/*
 * @tc.name: NotifyHwcDeadTest
 * @tc.desc: Test NotifyHwcDead
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCallbackManagerTest, NotifyHwcDeadTest, TestSize.Level0)
{
    ScreenId screenId = 100;
    ASSERT_EQ(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyHwcDead(screenId);
    callbackMgr_->SetCoreListener(coreListener_);
    ASSERT_NE(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyHwcDead(screenId);
    callbackMgr_->coreListener_ = nullptr;
}

/*
 * @tc.name: NotifyHwcEventTest
 * @tc.desc: Test NotifyHwcEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCallbackManagerTest, NotifyHwcEventTest, TestSize.Level0)
{
    uint32_t deviceId = 100;
    uint32_t eventId = 100;
    std::vector<int32_t> eventData = {};
    ASSERT_EQ(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyHwcEvent(deviceId, eventId, eventData);
    callbackMgr_->SetCoreListener(coreListener_);
    ASSERT_NE(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyHwcEvent(deviceId, eventId, eventData);
    callbackMgr_->coreListener_ = nullptr;
}

/*
 * @tc.name: NotifyVBlankIdleTest
 * @tc.desc: Test NotifyVBlankIdle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCallbackManagerTest, NotifyVBlankIdleTest, TestSize.Level0)
{
    ScreenId screenId = 100;
    uint64_t time = 100;
    ASSERT_EQ(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyVBlankIdle(screenId, time);
    callbackMgr_->SetCoreListener(coreListener_);
    ASSERT_NE(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyVBlankIdle(screenId, time);
    callbackMgr_->coreListener_ = nullptr;
}

/*
 * @tc.name: NotifyVirtualScreenPresenceChangedTest
 * @tc.desc: Test NotifyVirtualScreenPresenceChanged
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCallbackManagerTest, NotifyVirtualScreenPresenceChangedTest, TestSize.Level0)
{
    ScreenId screenId = 100;
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    ASSERT_EQ(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyVirtualScreenPresenceChanged(screenId, true, INVALID_SCREEN_ID, property);
    callbackMgr_->SetCoreListener(coreListener_);
    ASSERT_NE(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyVirtualScreenPresenceChanged(screenId, true, INVALID_SCREEN_ID, property);
    callbackMgr_->NotifyVirtualScreenPresenceChanged(screenId, false, INVALID_SCREEN_ID, property);
    callbackMgr_->coreListener_ = nullptr;
}

/*
 * @tc.name: NotifyActiveScreenIdChangedTest
 * @tc.desc: Test NotifyActiveScreenIdChanged
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCallbackManagerTest, NotifyActiveScreenIdChangedTest, TestSize.Level0)
{
    ScreenId screenId = 100;
    ASSERT_EQ(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyActiveScreenIdChanged(screenId);
    callbackMgr_->SetCoreListener(coreListener_);
    ASSERT_NE(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyActiveScreenIdChanged(screenId);
    callbackMgr_->coreListener_ = nullptr;
}

/*
 * @tc.name: NotifyScreenBacklightChangedTest
 * @tc.desc: Test NotifyScreenBacklightChanged
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCallbackManagerTest, NotifyScreenBacklightChangedTest, TestSize.Level0)
{
    ScreenId screenId = 100;
    uint32_t level = 100;
    ASSERT_EQ(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyScreenBacklightChanged(screenId, level);
    callbackMgr_->SetCoreListener(coreListener_);
    ASSERT_NE(callbackMgr_->coreListener_, nullptr);
    callbackMgr_->NotifyScreenBacklightChanged(screenId, level);
    callbackMgr_->coreListener_ = nullptr;
}

/*
 * @tc.name: GetClientToRenderConnectionTest
 * @tc.desc: Test GetClientToRenderConnection
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCallbackManagerTest, GetClientToRenderConnectionTest, TestSize.Level0)
{
    ScreenId screenId = 100;
    if (callbackMgr_->clientToRenderConns_.size() != 0) {
        callbackMgr_->clientToRenderConns_.clear();
    }
    ASSERT_EQ(callbackMgr_->clientToRenderConns_.size(), 0);
    callbackMgr_->GetClientToRenderConnection(screenId);
    callbackMgr_->clientToRenderConns_[screenId] = nullptr;
    ASSERT_EQ(callbackMgr_->clientToRenderConns_.size(), 1);
    callbackMgr_->GetClientToRenderConnection(screenId);
    callbackMgr_->clientToRenderConns_.clear();
    ASSERT_EQ(callbackMgr_->clientToRenderConns_.size(), 0);
}

/*
 * @tc.name: NotifyScreenConnectedToAgentListenersTest
 * @tc.desc: Test NotifyScreenConnectedToAgentListeners
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCallbackManagerTest, NotifyScreenConnectedToAgentListenersTest, TestSize.Level0)
{
    ScreenId screenId = 100;
    ASSERT_EQ(callbackMgr_->GetCoreListener(), nullptr);
    callbackMgr_->NotifyScreenConnectedToAgentListeners(screenId, ScreenChangeReason::DEFAULT, nullptr);
}

/*
 * @tc.name: NotifyScreenDisconnectedToAgentListenersTest
 * @tc.desc: Test NotifyScreenDisconnectedToAgentListeners
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenCallbackManagerTest, NotifyScreenDisconnectedToAgentListenersTest, TestSize.Level0)
{
    ScreenId screenId = 100;
    ASSERT_EQ(callbackMgr_->GetCoreListener(), nullptr);
    callbackMgr_->NotifyScreenDisconnectedToAgentListeners(screenId, ScreenChangeReason::DEFAULT);
}

} // namespace OHOS::Rosen