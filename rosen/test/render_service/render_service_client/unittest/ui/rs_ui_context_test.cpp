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

#include <chrono>
#include <thread>

#include "gtest/gtest.h"

#include "command/rs_animation_command.h"
#include "modifier_ng/custom/rs_content_style_modifier.h"
#include "transaction/rs_render_pipeline_client.h"
#include "transaction/rs_transaction.h"
#include "transaction/rs_transaction_data.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_context_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIContextTest::SetUpTestCase() {}
void RSUIContextTest::TearDownTestCase() {}
void RSUIContextTest::SetUp() {}
void RSUIContextTest::TearDown() {}

#ifdef RS_ENABLE_UNI_RENDER
/**
 * @tc.name: PostDelayTaskTest001
 * @tc.desc: Test the case where the task runner is empty
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, PostDelayTaskTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext1 = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    auto uiContext2 = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_TRUE(RSUIContextManager::Instance().rsUIContextMap_.size() > 1);
    bool flag = false;
    const std::function<void()>& task = [&flag]() { flag = true; };
    uint32_t delay = 0;
    uiContext1->PostDelayTask(task, delay);
    ASSERT_FALSE(flag);
}

/**
 * @tc.name: PostDelayTaskTest002
 * @tc.desc: Test the case where the random task runner isn't empty
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, PostDelayTaskTest002, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext1 = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    auto uiContext2 = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_TRUE(RSUIContextManager::Instance().rsUIContextMap_.size() > 1);
    uiContext2->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    ASSERT_FALSE(uiContext1->HasTaskRunner());
    ASSERT_TRUE(uiContext2->HasTaskRunner());
    bool flag = false;
    const std::function<void()>& task = [&flag]() { flag = true; };
    uint32_t delay = 0;
    uiContext1->PostDelayTask(task, delay);
    ASSERT_TRUE(flag);
    uiContext2->rsTransactionHandler_ = nullptr;
    uiContext2->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
}
#endif

#ifdef RS_ENABLE_UNI_RENDER
/**
 * @tc.name: SetRequestVsyncCallback
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, SetRequestVsyncCallback, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_TRUE(uiContext != nullptr);
    ASSERT_TRUE(RSUIContextManager::Instance().rsUIContextMap_.size() > 1);
    uiContext->SetRequestVsyncCallback(nullptr);
}

/**
 * @tc.name: RequestVsyncCallback
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, RequestVsyncCallback, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_TRUE(uiContext != nullptr);
    ASSERT_TRUE(RSUIContextManager::Instance().rsUIContextMap_.size() > 1);
    uiContext->SetRequestVsyncCallback(nullptr);
    EXPECT_EQ(uiContext->requestVsyncCallback_, nullptr);
    uiContext->RequestVsyncCallback();

    // test requestVsyncCallback_ not null
    uiContext->SetRequestVsyncCallback([]() {});
    EXPECT_NE(uiContext->requestVsyncCallback_, nullptr);
    uiContext->RequestVsyncCallback();
}
#endif

/**
 * @tc.name: DumpNodeTreeProcessorTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, DumpNodeTreeProcessorTest001, TestSize.Level1)
{
#ifdef RS_ENABLE_UNI_RENDER
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    auto transaction = uiContext->GetRSTransaction();
    ASSERT_NE(transaction, nullptr);
    auto canvasNode = RSCanvasNode::Create(false, false, uiContext);
    std::string out = "";
    uiContext->DumpNodeTreeProcessor(canvasNode->GetId(), 0, 0, out);
    ASSERT_TRUE(out.find("transactionFlags") != std::string::npos);
    ASSERT_TRUE(out.find("UIContext") != std::string::npos);
#endif
}

/**
 * @tc.name: DumpNodeTreeProcessorTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, DumpNodeTreeProcessorTest002, TestSize.Level1)
{
#ifdef RS_ENABLE_UNI_RENDER
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    auto transaction = uiContext->GetRSTransaction();
    ASSERT_NE(transaction, nullptr);
    std::string out = "";
    uiContext->DumpNodeTreeProcessor(0, 0, 0, out);
    ASSERT_TRUE(out.find("transactionFlags") == std::string::npos);
    ASSERT_TRUE(out.find("UIContext") == std::string::npos);
#endif
}

/**
 * @tc.name: DumpNodeTreeProcessorTest003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, DumpNodeTreeProcessorTest003, TestSize.Level1)
{
    bool enable = RSSystemProperties::GetRSClientMultiInstanceEnabled();
    if (enable) {
        OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
        auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
        uiContext->rsTransactionHandler_ = nullptr;
        auto transaction = uiContext->GetRSTransaction();
        ASSERT_EQ(transaction, nullptr);
        auto canvasNode = RSCanvasNode::Create(false, false, uiContext);
        std::string out = "";
        uiContext->DumpNodeTreeProcessor(canvasNode->GetId(), 0, 0, out);
        ASSERT_TRUE(out.find("transactionFlags") == std::string::npos);
        ASSERT_TRUE(out.find("UIContext") == std::string::npos);
    }
}

/**
 * @tc.name: StartCloseSyncTransactionFallbackTaskTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, StartCloseSyncTransactionFallbackTaskTest001, TestSize.Level1)
{
    const uint64_t syncId = 123;
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("runner");
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    RSUIContextManager::MutableInstance().handler_ = handler;

    ASSERT_TRUE(RSUIContextManager::MutableInstance().taskNames_.empty());
    RSUIContextManager::MutableInstance().StartCloseSyncTransactionFallbackTask(true, syncId);
    ASSERT_FALSE(RSUIContextManager::MutableInstance().taskNames_.empty());
    RSUIContextManager::MutableInstance().StartCloseSyncTransactionFallbackTask(false, syncId);
    ASSERT_TRUE(RSUIContextManager::MutableInstance().taskNames_.empty());

    ASSERT_TRUE(RSUIContextManager::MutableInstance().taskNames_.empty());
    RSUIContextManager::MutableInstance().StartCloseSyncTransactionFallbackTask(true, syncId);
    ASSERT_FALSE(RSUIContextManager::MutableInstance().taskNames_.empty());
    std::queue<std::string>().swap(RSUIContextManager::MutableInstance().taskNames_);
    RSUIContextManager::MutableInstance().StartCloseSyncTransactionFallbackTask(false, syncId);

    RSUIContextManager::MutableInstance().StartCloseSyncTransactionFallbackTask(true, syncId);
    ASSERT_FALSE(RSUIContextManager::MutableInstance().taskNames_.empty());
    sleep(8);

    RSUIContextManager::MutableInstance().StartCloseSyncTransactionFallbackTask(true, syncId);
    std::queue<std::string>().swap(RSUIContextManager::MutableInstance().taskNames_);
    ASSERT_TRUE(RSUIContextManager::MutableInstance().taskNames_.empty());
    sleep(8);
}

/**
 * @tc.name: CloseAllSyncTransactionTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, CloseAllSyncTransactionTest001, TestSize.Level1)
{
    const uint64_t syncId = 123;

    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = false;
    RSUIContextManager::MutableInstance().CloseAllSyncTransaction(syncId);

    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = true;
    RSUIContextManager::MutableInstance().rsUIContextMap_.clear();
    ASSERT_TRUE(RSUIContextManager::MutableInstance().rsUIContextMap_.empty());
    RSUIContextManager::MutableInstance().CloseAllSyncTransaction(syncId);

    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = true;
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_FALSE(RSUIContextManager::MutableInstance().rsUIContextMap_.empty());
    RSUIContextManager::MutableInstance().CloseAllSyncTransaction(syncId);

    ASSERT_FALSE(RSUIContextManager::MutableInstance().rsUIContextMap_.empty());
    auto it = RSUIContextManager::MutableInstance().rsUIContextMap_.begin();
    it->second->rsSyncTransactionHandler_ = nullptr;
    RSUIContextManager::MutableInstance().CloseAllSyncTransaction(syncId);

    RSUIContextManager::MutableInstance().rsUIContextMap_.clear();
    RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_FALSE(RSUIContextManager::MutableInstance().rsUIContextMap_.empty());
    auto syncHandler =
        RSUIContextManager::MutableInstance().rsUIContextMap_.begin()->second->GetSyncTransactionHandler();
    ASSERT_FALSE(syncHandler == nullptr);
    syncHandler->rsTransaction_ = nullptr;
    RSUIContextManager::MutableInstance().CloseAllSyncTransaction(syncId);

    RSUIContextManager::MutableInstance().rsUIContextMap_.clear();
    RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_FALSE(RSUIContextManager::MutableInstance().rsUIContextMap_.empty());
    auto syncHandler1 =
        RSUIContextManager::MutableInstance().rsUIContextMap_.begin()->second->GetSyncTransactionHandler();
    ASSERT_FALSE(syncHandler1 == nullptr);
    auto transaction = syncHandler1->GetCommonRSTransaction();
    ASSERT_FALSE(transaction == nullptr);
    transaction->rsTransactionHandler_ = nullptr;
    RSUIContextManager::MutableInstance().CloseAllSyncTransaction(syncId);
}

#ifdef RS_ENABLE_UNI_RENDER
/**
 * @tc.name: MoveModifierTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, MoveModifierTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto curContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    curContext->rsModifierManager_ = std::make_shared<RSModifierManager>();
    auto canvasNode = RSCanvasNode::Create(false, false, curContext);
    auto rsCustomModifier = std::make_shared<ModifierNG::RSContentStyleModifier>();
    rsCustomModifier->OnAttach(*canvasNode);
    ASSERT_NE(rsCustomModifier->node_.lock(), nullptr);
    curContext->rsModifierManager_->AddModifier(rsCustomModifier);
    auto notMoveModifier = std::make_shared<ModifierNG::RSContentStyleModifier>();
    curContext->rsModifierManager_->AddModifier(notMoveModifier);
    ASSERT_EQ(curContext->rsModifierManager_->modifiers_.size(), 2);

    auto newContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    newContext->rsModifierManager_ = std::make_shared<RSModifierManager>();

    curContext->MoveModifier(newContext, canvasNode->GetId());
    ASSERT_EQ(curContext->rsModifierManager_->modifiers_.size(), 1);
    ASSERT_EQ(newContext->rsModifierManager_->modifiers_.size(), 1);
}

/**
 * @tc.name: MoveModifierTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, MoveModifierTest002, TestSize.Level1)
{
    NodeId id = 0;
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto curContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    curContext->rsModifierManager_ = nullptr;
    curContext->MoveModifier(nullptr, id);

    curContext->rsModifierManager_ = std::make_shared<RSModifierManager>();
    auto rsCustomModifier = std::make_shared<ModifierNG::RSContentStyleModifier>();
    curContext->rsModifierManager_->AddModifier(rsCustomModifier);
    ASSERT_FALSE(curContext->rsModifierManager_->modifiers_.empty());

    curContext->MoveModifier(nullptr, id);
    ASSERT_FALSE(curContext->rsModifierManager_->modifiers_.empty());
}

/**
 * @tc.name: MoveModifierTest003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, MoveModifierTest003, TestSize.Level1)
{
    NodeId id = 0;
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto curContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    curContext->rsModifierManager_ = nullptr;
    ASSERT_EQ(curContext->rsModifierManager_, nullptr);

    auto newContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    newContext->rsModifierManager_ = std::make_shared<RSModifierManager>();
    ASSERT_NE(newContext->rsModifierManager_, nullptr);

    curContext->MoveModifier(nullptr, id);
    ASSERT_TRUE(newContext->rsModifierManager_->modifiers_.empty());
}

/**
 * @tc.name: UiPiplineNum001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, UiPiplineNum001, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto curContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_EQ(curContext->GetUiPiplineNum(), UI_PiPLINE_NUM_UNDEFINED);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), UI_PiPLINE_NUM_UNDEFINED);
}

/**
 * @tc.name: UiPiplineNum002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, UiPiplineNum002, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto curContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_EQ(curContext->GetUiPiplineNum(), UI_PiPLINE_NUM_UNDEFINED);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), UI_PiPLINE_NUM_UNDEFINED);
 
    curContext->AttachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 1);
 
    curContext->AttachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 2);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 1);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 0);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 0);
}

/**
 * @tc.name: UiPiplineNum003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, UiPiplineNum003, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto curContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);

    curContext->AttachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 1);
 
    curContext->AttachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 2);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 1);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 0);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 0);
}

/**
 * @tc.name: UiPiplineNum004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, UiPiplineNum004, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto curContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_EQ(curContext->GetUiPiplineNum(), UI_PiPLINE_NUM_UNDEFINED);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), UI_PiPLINE_NUM_UNDEFINED);
 
    curContext->AttachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 1);
 
    curContext->AttachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 2);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 1);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 0);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 0);

    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 0);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 0);
}

/**
 * @tc.name: UiPiplineNum005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, UiPiplineNum005, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto curContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_EQ(curContext->GetUiPiplineNum(), UI_PiPLINE_NUM_UNDEFINED);

    auto newContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_EQ(newContext->GetUiPiplineNum(), UI_PiPLINE_NUM_UNDEFINED);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), UI_PiPLINE_NUM_UNDEFINED);

    newContext->DetachFromUI();
    ASSERT_EQ(newContext->GetUiPiplineNum(), UI_PiPLINE_NUM_UNDEFINED);

    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), UI_PiPLINE_NUM_UNDEFINED);

    newContext->DetachFromUI();
    ASSERT_EQ(newContext->GetUiPiplineNum(), UI_PiPLINE_NUM_UNDEFINED);
 
    curContext->AttachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 1);

    newContext->AttachFromUI();
    ASSERT_EQ(newContext->GetUiPiplineNum(), 1);
 
    curContext->AttachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 2);

    newContext->AttachFromUI();
    ASSERT_EQ(newContext->GetUiPiplineNum(), 2);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 1);

    newContext->DetachFromUI();
    ASSERT_EQ(newContext->GetUiPiplineNum(), 1);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 0);

    newContext->DetachFromUI();
    ASSERT_EQ(newContext->GetUiPiplineNum(), 0);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 0);

    newContext->DetachFromUI();
    ASSERT_EQ(newContext->GetUiPiplineNum(), 0);

    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 0);

    newContext->DetachFromUI();
    ASSERT_EQ(newContext->GetUiPiplineNum(), 0);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 0);

    newContext->DetachFromUI();
    ASSERT_EQ(newContext->GetUiPiplineNum(), 0);
}

/**
 * @tc.name: UiPiplineNum006
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, UiPiplineNum006, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto curContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_EQ(curContext->GetUiPiplineNum(), UI_PiPLINE_NUM_UNDEFINED);

    curContext->AttachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 1);
 
    curContext->AttachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 2);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 1);
 
    curContext->DetachFromUI();
    ASSERT_EQ(curContext->GetUiPiplineNum(), 0);

    auto newContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_EQ(newContext->GetUiPiplineNum(), UI_PiPLINE_NUM_UNDEFINED);

    newContext->AttachFromUI();
    ASSERT_EQ(newContext->GetUiPiplineNum(), 1);
 
    newContext->AttachFromUI();
    ASSERT_EQ(newContext->GetUiPiplineNum(), 2);
 
    newContext->DetachFromUI();
    ASSERT_EQ(newContext->GetUiPiplineNum(), 1);
 
    newContext->DetachFromUI();
    ASSERT_EQ(newContext->GetUiPiplineNum(), 0);

    newContext->DetachFromUI();
    ASSERT_EQ(newContext->GetUiPiplineNum(), 0);
}
#endif

/**
 * @tc.name: RebuildStateTest
 * @tc.desc: Test SetRebuildState and GetRebuildState
 * @tc.type: FUNC
 */
HWTEST_F(RSUIContextTest, RebuildStateTest, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_NE(uiContext, nullptr);
    uiContext->SetRebuildState(RebuildState::Rebuilding);
    EXPECT_EQ(uiContext->GetRebuildState(), RebuildState::Rebuilding);
    uiContext->SetRebuildState(RebuildState::Normal);
    EXPECT_EQ(uiContext->GetRebuildState(), RebuildState::Normal);
}

/**
 * @tc.name: WaitForRebuildNormal_AlreadyNormal
 * @tc.desc: Test WaitForRebuildNormal returns true immediately when state is Normal.
 * @tc.type: FUNC
 * @tc.require: issues30915
 */
HWTEST_F(RSUIContextTest, WaitForRebuildNormal_AlreadyNormal, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_NE(uiContext, nullptr);
    uiContext->SetRebuildState(RebuildState::Normal);
    EXPECT_TRUE(uiContext->WaitForRebuildNormal(0));
}

/**
 * @tc.name: WaitForRebuildNormal_Timeout
 * @tc.desc: Test WaitForRebuildNormal returns false when Rebuilding and timeout expires.
 * @tc.type: FUNC
 * @tc.require: issues30915
 */
HWTEST_F(RSUIContextTest, WaitForRebuildNormal_Timeout, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_NE(uiContext, nullptr);
    uiContext->SetRebuildState(RebuildState::Rebuilding);
    EXPECT_FALSE(uiContext->WaitForRebuildNormal(0));
}

/**
 * @tc.name: WaitForRebuildNormal_NotifyWakes
 * @tc.desc: Test WaitForRebuildNormal wakes up when another thread sets state to Normal.
 * @tc.type: FUNC
 * @tc.require: issues30915
 */
HWTEST_F(RSUIContextTest, WaitForRebuildNormal_NotifyWakes, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_NE(uiContext, nullptr);
    uiContext->SetRebuildState(RebuildState::Rebuilding);

    std::thread notifier([uiContext]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        uiContext->SetRebuildState(RebuildState::Normal);
    });
    EXPECT_TRUE(uiContext->WaitForRebuildNormal(1000));
    notifier.join();
}

#ifdef RS_MODIFIERS_DRAW_ENABLE
/**
 * @tc.name: DestroyModifiersDrawTest
 * @tc.desc: Test DestroyModifiersDraw normal flow and null-check early return
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, DestroyModifiersDrawTest, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_NE(rsUIContext->modifiersDrawThread_, nullptr);
        ASSERT_NE(rsUIContext->canvasModifiersDrawAgent_, nullptr);
        rsUIContext->DestroyModifiersDraw();
        ASSERT_EQ(rsUIContext->modifiersDrawThread_, nullptr);
        ASSERT_EQ(rsUIContext->canvasModifiersDrawAgent_, nullptr);
    }
    // Already destroyed, modifiersDrawThread_ is nullptr, early return branch
    rsUIContext->DestroyModifiersDraw();
    ASSERT_EQ(rsUIContext->modifiersDrawThread_, nullptr);
}

/**
 * @tc.name: CreateCommitTransactionCallback_NullModifiersDrawThread
 * @tc.desc: Test CreateCommitTransactionCallback returns nullptr when modifiersDrawThread_ is null
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, CreateCommitTransactionCallback_NullModifiersDrawThread, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    rsUIContext->modifiersDrawThread_ = nullptr;
    auto callback = rsUIContext->CreateCommitTransactionCallback();
    ASSERT_EQ(callback, nullptr);
}

/**
 * @tc.name: CreateCommitTransactionCallback_LambdaNullModifiersDrawThread
 * @tc.desc: Test CreateCommitTransactionCallback lambda when modifiersDrawThread_ is null after Destroy
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, CreateCommitTransactionCallback_LambdaNullModifiersDrawThread, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_EQ(rsUIContext->modifiersDrawThread_, nullptr);
        return;
    }
    auto callback = rsUIContext->CreateCommitTransactionCallback();
    ASSERT_NE(callback, nullptr);
    // Destroy modifiersDrawThread_ so lambda inner null-check triggers
    rsUIContext->DestroyModifiersDraw();
    ASSERT_EQ(rsUIContext->modifiersDrawThread_, nullptr);
    auto renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    auto transactionData = std::make_unique<RSTransactionData>();
    uint32_t transactionDataIndex = 0;
    // Should not crash, lambda returns early on null modifiersDrawThread_
    callback(renderPipelineClient, std::move(transactionData), transactionDataIndex);
}

/**
 * @tc.name: CreateCommitTransactionCallback_LambdaNullCanvasModifiersDrawAgent
 * @tc.desc: Test CreateCommitTransactionCallback lambda inner ScheduleTask when canvasModifiersDrawAgent_ is null
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, CreateCommitTransactionCallback_LambdaNullCanvasModifiersDrawAgent, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_EQ(rsUIContext->canvasModifiersDrawAgent_, nullptr);
        return;
    }
    auto callback = rsUIContext->CreateCommitTransactionCallback();
    ASSERT_NE(callback, nullptr);
    // Set canvasModifiersDrawAgent_ to null but keep modifiersDrawThread_ alive
    rsUIContext->canvasModifiersDrawAgent_ = nullptr;
    auto renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    auto transactionData = std::make_unique<RSTransactionData>();
    uint32_t transactionDataIndex = 0;
    // Should not crash, ScheduleTask lambda skips CommitTransaction when agent is null
    callback(renderPipelineClient, std::move(transactionData), transactionDataIndex);
    // Cleanup: canvasModifiersDrawAgent_ is already null, cannot call DestroyModifiersDraw()
    // which would dereference it. Clean up modifiersDrawThread_ directly instead.
    rsUIContext->modifiersDrawThread_->Destroy();
    rsUIContext->modifiersDrawThread_ = nullptr;
}

/**
 * @tc.name: FlushCanvasDrawingNodeBuffersTest_NullCanvasModifiersDrawAgent
 * @tc.desc: Test FlushCanvasDrawingNodeBuffers with null canvasModifiersDrawAgent_
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, FlushCanvasDrawingNodeBuffersTest_NullCanvasModifiersDrawAgent, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    rsUIContext->canvasDrawingNodeUpdated_ = true;
    rsUIContext->canvasModifiersDrawAgent_ = nullptr;
    rsUIContext->FlushCanvasDrawingNodeBuffers();
    // canvasModifiersDrawAgent_ is null, SubmitAndCollectCanvasBuffers is skipped,
    // but function still completes and sets canvasDrawingNodeUpdated_ = false
    EXPECT_FALSE(rsUIContext->canvasDrawingNodeUpdated_);
}

/**
 * @tc.name: FlushCanvasDrawingNodeBuffersTest
 * @tc.desc: Test FlushCanvasDrawingNodeBuffers early return when hybrid canvas disabled
 * @tc.type: FUNC
 */
HWTEST_F(RSUIContextTest, FlushCanvasDrawingNodeBuffersTest, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_NE(uiContext, nullptr);
    uiContext->FlushCanvasDrawingNodeBuffers();

    // transaction handler null path
    auto savedTransaction = uiContext->rsTransactionHandler_;
    uiContext->rsTransactionHandler_ = nullptr;
    uiContext->FlushCanvasDrawingNodeBuffers();
    uiContext->rsTransactionHandler_ = savedTransaction;
    SUCCEED();
}

/**
 * @tc.name: CreateCommitTransactionCallbackNullTest
 * @tc.desc: Test callback returned by CreateCommitTransactionCallback handles null inputs
 * @tc.type: FUNC
 */
HWTEST_F(RSUIContextTest, CreateCommitTransactionCallbackNullTest, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_NE(uiContext, nullptr);
    auto callback = uiContext->CreateCommitTransactionCallback();
    uint32_t index = 0;
    std::shared_ptr<RSRenderPipelineClient> nullClient = nullptr;
    std::unique_ptr<RSTransactionData> nullData = nullptr;
    callback(nullClient, std::move(nullData), index);
    SUCCEED();
}

/**
 * @tc.name: CreateCommitTransactionCallbackExpiredContextTest
 * @tc.desc: Test callback handles expired RSUIContext
 * @tc.type: FUNC
 */
HWTEST_F(RSUIContextTest, CreateCommitTransactionCallbackExpiredContextTest, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_NE(uiContext, nullptr);
    auto callback = uiContext->CreateCommitTransactionCallback();
    uint64_t token = uiContext->GetToken();
    RSUIContextManager::MutableInstance().DestroyContext(token);
    uiContext.reset();

    uint32_t index = 0;
    auto renderClient = std::make_shared<RSRenderPipelineClient>();
    auto transactionData = std::make_unique<RSTransactionData>();
    callback(renderClient, std::move(transactionData), index);
    SUCCEED();
}

/**
 * @tc.name: UnblockUIThreadTest
 * @tc.desc: Test UnblockUIThread with canBlockUIThread_ true and false
 * @tc.type: FUNC
 */
HWTEST_F(RSUIContextTest, UnblockUIThreadTest, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_NE(uiContext, nullptr);
    uiContext->canBlockUIThread_ = false;
    uiContext->UnblockUIThread();

    uiContext->canBlockUIThread_ = true;
    uiContext->UnblockUIThread();
    EXPECT_FALSE(uiContext->canBlockUIThread_);
}

/**
 * @tc.name: ModifiersDrawThreadAccessorsTest
 * @tc.desc: Test getters are consistent with hybrid canvas enabled state
 * @tc.type: FUNC
 */
HWTEST_F(RSUIContextTest, ModifiersDrawThreadAccessorsTest, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    ASSERT_NE(uiContext, nullptr);
    bool hybridCanvasEnabled = RSSystemProperties::GetHybridRenderCanvasEnabled();
    EXPECT_EQ(uiContext->GetModifiersDrawThread() != nullptr, hybridCanvasEnabled);
    EXPECT_EQ(uiContext->GetCanvasModifiersDrawThread() != nullptr, hybridCanvasEnabled);
    EXPECT_EQ(uiContext->GetCanvasModifiersDraw() != nullptr, hybridCanvasEnabled);
}
#endif

#ifdef RS_MODIFIERS_DRAW_ENABLE
/**
 * @tc.name: FlushCanvasDrawingNodeBuffersTest001
 * @tc.desc: Test FlushCanvasDrawingNodeBuffers with canvasDrawingNodeUpdated false
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, FlushCanvasDrawingNodeBuffersTest001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    rsUIContext->canvasDrawingNodeUpdated_ = false;
    rsUIContext->FlushCanvasDrawingNodeBuffers();
    EXPECT_FALSE(rsUIContext->canvasDrawingNodeUpdated_);
}
 
/**
 * @tc.name: FlushCanvasDrawingNodeBuffersTest002
 * @tc.desc: Test FlushCanvasDrawingNodeBuffers with null transaction
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, FlushCanvasDrawingNodeBuffersTest002, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    rsUIContext->canvasDrawingNodeUpdated_ = true;
    rsUIContext->rsTransactionHandler_ = nullptr;
    rsUIContext->FlushCanvasDrawingNodeBuffers();
    EXPECT_TRUE(rsUIContext->canvasDrawingNodeUpdated_);
}
 
/**
 * @tc.name: FlushCanvasDrawingNodeBuffersTest003
 * @tc.desc: Test FlushCanvasDrawingNodeBuffers with canvasDrawingNodeBufferFlushed true
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, FlushCanvasDrawingNodeBuffersTest003, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    rsUIContext->canvasDrawingNodeUpdated_ = true;
    rsUIContext->canvasDrawingNodeBufferFlushed_ = true;
    rsUIContext->FlushCanvasDrawingNodeBuffers();
    EXPECT_FALSE(rsUIContext->canvasDrawingNodeUpdated_);
}

/**
 * @tc.name: RSModifiersDrawThreadDestroyTest001
 * @tc.desc: Test RSModifiersDrawThread::Destroy when started_ is true
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, RSModifiersDrawThreadDestroyTest001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_EQ(rsUIContext->modifiersDrawThread_, nullptr);
        return;
    }
    ASSERT_NE(rsUIContext->modifiersDrawThread_, nullptr);
    // Start() is called inside CreateCommitTransactionCallback
    auto callback = rsUIContext->CreateCommitTransactionCallback();
    ASSERT_NE(callback, nullptr);
    ASSERT_TRUE(rsUIContext->modifiersDrawThread_->started_);
    rsUIContext->modifiersDrawThread_->Destroy();
    ASSERT_FALSE(rsUIContext->modifiersDrawThread_->started_);
    ASSERT_EQ(rsUIContext->modifiersDrawThread_->handler_, nullptr);
    ASSERT_EQ(rsUIContext->modifiersDrawThread_->runner_, nullptr);
}

/**
 * @tc.name: RSModifiersDrawThreadDestroyTest002
 * @tc.desc: Test RSModifiersDrawThread::Destroy when started_ is false
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, RSModifiersDrawThreadDestroyTest002, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_EQ(rsUIContext->modifiersDrawThread_, nullptr);
        return;
    }
    // Save handler_/runner_ before setting started_ = false
    auto savedHandler = rsUIContext->modifiersDrawThread_->handler_;
    auto savedRunner = rsUIContext->modifiersDrawThread_->runner_;
    rsUIContext->modifiersDrawThread_->started_ = false;
    rsUIContext->modifiersDrawThread_->Destroy();
    // Early return: started_ is false, handler_/runner_ should remain unchanged
    ASSERT_FALSE(rsUIContext->modifiersDrawThread_->started_);
    ASSERT_EQ(rsUIContext->modifiersDrawThread_->handler_, savedHandler);
    ASSERT_EQ(rsUIContext->modifiersDrawThread_->runner_, savedRunner);
}

/**
 * @tc.name: RSModifiersDrawThreadPostSyncTaskTest001
 * @tc.desc: Test RSModifiersDrawThread::PostSyncTask when handler_ is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, RSModifiersDrawThreadPostSyncTaskTest001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_EQ(rsUIContext->modifiersDrawThread_, nullptr);
        return;
    }
    rsUIContext->modifiersDrawThread_->handler_ = nullptr;
    bool executed = false;
    rsUIContext->modifiersDrawThread_->PostSyncTask([&executed]() { executed = true; });
    EXPECT_FALSE(executed);
}

/**
 * @tc.name: RSCanvasModifiersDrawDestroyTest001
 * @tc.desc: Test RSCanvasModifiersDraw::Destroy when threadStarted_ is true
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, RSCanvasModifiersDrawDestroyTest001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_EQ(rsUIContext->canvasModifiersDrawAgent_, nullptr);
        return;
    }
    ASSERT_NE(rsUIContext->canvasModifiersDrawAgent_, nullptr);
    auto& draw = rsUIContext->canvasModifiersDrawAgent_->canvasModifiersDraw_;
    ASSERT_NE(draw, nullptr);
    draw->StartThread();
    ASSERT_TRUE(draw->threadStarted_);
    draw->Destroy();
    ASSERT_FALSE(draw->threadStarted_);
    ASSERT_EQ(draw->handler_, nullptr);
    ASSERT_EQ(draw->runner_, nullptr);
}

/**
 * @tc.name: RSCanvasModifiersDrawDestroyTest002
 * @tc.desc: Test RSCanvasModifiersDraw::Destroy when threadStarted_ is false
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, RSCanvasModifiersDrawDestroyTest002, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_EQ(rsUIContext->canvasModifiersDrawAgent_, nullptr);
        return;
    }
    auto& draw = rsUIContext->canvasModifiersDrawAgent_->canvasModifiersDraw_;
    ASSERT_NE(draw, nullptr);
    // Save handler_/runner_ before setting threadStarted_ = false
    auto savedHandler = draw->handler_;
    auto savedRunner = draw->runner_;
    draw->threadStarted_ = false;
    draw->Destroy();
    // Early return: threadStarted_ is false, handler_/runner_ should remain unchanged
    ASSERT_FALSE(draw->threadStarted_);
    ASSERT_EQ(draw->handler_, savedHandler);
    ASSERT_EQ(draw->runner_, savedRunner);
}

/**
 * @tc.name: RSCanvasModifiersDrawAgent_DestroyTest
 * @tc.desc: Test RSCanvasModifiersDrawAgent::Destroy directly
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, RSCanvasModifiersDrawAgent_DestroyTest, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_EQ(rsUIContext->canvasModifiersDrawAgent_, nullptr);
        return;
    }
    ASSERT_NE(rsUIContext->canvasModifiersDrawAgent_, nullptr);
    auto& draw = rsUIContext->canvasModifiersDrawAgent_->canvasModifiersDraw_;
    ASSERT_NE(draw, nullptr);
    // Start thread first so Destroy has work to do
    draw->StartThread();
    ASSERT_TRUE(draw->threadStarted_);
    rsUIContext->canvasModifiersDrawAgent_->Destroy();
    ASSERT_FALSE(draw->threadStarted_);
    ASSERT_EQ(draw->handler_, nullptr);
    ASSERT_EQ(draw->runner_, nullptr);
}

/**
 * @tc.name: RSCanvasModifiersDraw_QueryMaxGpuBufferSizeTest
 * @tc.desc: Test RSCanvasModifiersDraw::QueryMaxGpuBufferSize calls PostSyncTask which starts thread
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, RSCanvasModifiersDraw_QueryMaxGpuBufferSizeTest, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_EQ(rsUIContext->canvasModifiersDrawAgent_, nullptr);
        return;
    }
    ASSERT_NE(rsUIContext->canvasModifiersDrawAgent_, nullptr);
    auto& draw = rsUIContext->canvasModifiersDrawAgent_->canvasModifiersDraw_;
    ASSERT_NE(draw, nullptr);
    // PostSyncTask internally calls StartThread which creates handler_ and starts the thread
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    draw->QueryMaxGpuBufferSize(maxWidth, maxHeight);
    // After QueryMaxGpuBufferSize, thread should be started
    ASSERT_TRUE(draw->threadStarted_);
    ASSERT_NE(draw->handler_, nullptr);
    // Cleanup
    draw->Destroy();
}

/**
 * @tc.name: RSCanvasModifiersDrawAgent_QueryMaxGpuBufferSizeTest
 * @tc.desc: Test RSCanvasModifiersDrawAgent::QueryMaxGpuBufferSize transparently calls inner method
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, RSCanvasModifiersDrawAgent_QueryMaxGpuBufferSizeTest, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_EQ(rsUIContext->canvasModifiersDrawAgent_, nullptr);
        return;
    }
    ASSERT_NE(rsUIContext->canvasModifiersDrawAgent_, nullptr);
    auto& draw = rsUIContext->canvasModifiersDrawAgent_->canvasModifiersDraw_;
    ASSERT_NE(draw, nullptr);
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    rsUIContext->canvasModifiersDrawAgent_->QueryMaxGpuBufferSize(maxWidth, maxHeight);
    // Transparent call should start the inner thread
    ASSERT_TRUE(draw->threadStarted_);
    ASSERT_NE(draw->handler_, nullptr);
    // Cleanup
    draw->Destroy();
}
/**
 * @tc.name: RSCanvasModifiersDraw_WaitAllTasksFinishTest001
 * @tc.desc: Test WaitAllTasksFinish when threadStarted_ is false
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, RSCanvasModifiersDraw_WaitAllTasksFinishTest001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_EQ(rsUIContext->canvasModifiersDrawAgent_, nullptr);
        return;
    }
    auto& draw = rsUIContext->canvasModifiersDrawAgent_->canvasModifiersDraw_;
    ASSERT_NE(draw, nullptr);
    ASSERT_FALSE(draw->threadStarted_);
    draw->WaitAllTasksFinish();
    ASSERT_FALSE(draw->threadStarted_);
}

/**
 * @tc.name: RSCanvasModifiersDraw_WaitAllTasksFinishTest002
 * @tc.desc: Test WaitAllTasksFinish when threadStarted_ is true
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, RSCanvasModifiersDraw_WaitAllTasksFinishTest002, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_EQ(rsUIContext->canvasModifiersDrawAgent_, nullptr);
        return;
    }
    auto& draw = rsUIContext->canvasModifiersDrawAgent_->canvasModifiersDraw_;
    ASSERT_NE(draw, nullptr);
    draw->StartThread();
    ASSERT_TRUE(draw->threadStarted_);
    draw->WaitAllTasksFinish();
    ASSERT_TRUE(draw->threadStarted_);
    draw->Destroy();
}

/**
 * @tc.name: RSCanvasModifiersDrawAgent_WaitAllTasksFinishTest
 * @tc.desc: Test RSCanvasModifiersDrawAgent::WaitAllTasksFinish transparently calls inner method
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, RSCanvasModifiersDrawAgent_WaitAllTasksFinishTest, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_EQ(rsUIContext->canvasModifiersDrawAgent_, nullptr);
        return;
    }
    auto& draw = rsUIContext->canvasModifiersDrawAgent_->canvasModifiersDraw_;
    ASSERT_NE(draw, nullptr);
    draw->StartThread();
    ASSERT_TRUE(draw->threadStarted_);
    rsUIContext->canvasModifiersDrawAgent_->WaitAllTasksFinish();
    ASSERT_TRUE(draw->threadStarted_);
    draw->Destroy();
}

/**
 * @tc.name: RSModifiersDrawThread_WaitAllTasksFinishTest001
 * @tc.desc: Test WaitAllTasksFinish when started_ is false
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, RSModifiersDrawThread_WaitAllTasksFinishTest001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_EQ(rsUIContext->modifiersDrawThread_, nullptr);
        return;
    }
    ASSERT_NE(rsUIContext->modifiersDrawThread_, nullptr);
    ASSERT_FALSE(rsUIContext->modifiersDrawThread_->started_);
    rsUIContext->modifiersDrawThread_->WaitAllTasksFinish();
    ASSERT_FALSE(rsUIContext->modifiersDrawThread_->started_);
}

/**
 * @tc.name: RSModifiersDrawThread_WaitAllTasksFinishTest002
 * @tc.desc: Test WaitAllTasksFinish when started_ is true
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, RSModifiersDrawThread_WaitAllTasksFinishTest002, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    ASSERT_NE(rsUIContext, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        ASSERT_EQ(rsUIContext->modifiersDrawThread_, nullptr);
        return;
    }
    ASSERT_NE(rsUIContext->modifiersDrawThread_, nullptr);
    auto callback = rsUIContext->CreateCommitTransactionCallback();
    ASSERT_NE(callback, nullptr);
    ASSERT_TRUE(rsUIContext->modifiersDrawThread_->started_);
    rsUIContext->modifiersDrawThread_->WaitAllTasksFinish();
    ASSERT_TRUE(rsUIContext->modifiersDrawThread_->started_);
    rsUIContext->modifiersDrawThread_->Destroy();
}
#endif
} // namespace OHOS::Rosen
