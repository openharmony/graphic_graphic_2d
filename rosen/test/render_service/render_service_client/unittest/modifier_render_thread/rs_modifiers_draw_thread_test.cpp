/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <parameters.h>

#include "command/rs_canvas_node_command.h"
#include "command/rs_node_command.h"
#include "command/rs_root_node_command.h"
#include "modifier_render_thread/rs_modifiers_draw_thread.h"
#include "modifier_render_thread/rs_modifiers_draw.h"
#include "recording/draw_cmd.h"
#include "render_context/shader_cache.h"
#include "command/rs_animation_command.h"
#include "command/rs_command.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_handler.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_ui_director.h"

#ifdef RS_ENABLE_VK
#include "src/platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr const int64_t DELAY_TIME = 1000;
constexpr const char* TASK_NAME = "TaskName";
constexpr uint32_t HYBRID_MAX_ENABLE_OP_CNT = 11;  // max value for enable hybrid op
constexpr int32_t DEFAULT_WIDTH = 100;
constexpr int32_t DEFAULT_HEIGHT = 100;

class RSRenderThreadClientHybridTest : public RSIRenderClient {
public:
    RSRenderThreadClientHybridTest() = default;
    ~RSRenderThreadClientHybridTest() = default;

    void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override {};
    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override {};
};
class RSModifiersDrawThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::unique_ptr<RSIRenderClient> CreateRenderThreadClientHybrid()
    {
        return std::make_unique<RSRenderThreadClientHybridTest>();
    }

    std::shared_ptr<RSIRenderClient> CreateRenderThreadClientHybridSharedPtr()
    {
        return std::make_shared<RSRenderThreadClientHybridTest>();
    }
};

void RSModifiersDrawThreadTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void RSModifiersDrawThreadTest::TearDownTestCase() {}
void RSModifiersDrawThreadTest::SetUp() {}
void RSModifiersDrawThreadTest::TearDown()
{
    RSModifiersDraw::ClearCanvasDrawingNodeMemory();
}

/**
 * @tc.name: FlushImplicitTransaction001
 * @tc.desc: test func FlushImplicitTransaction when hybridCallback is null
 * @tc.type: FUNC
 * @tc.require: issueICII2M
 */
HWTEST_F(RSModifiersDrawThreadTest, FlushImplicitTransaction001, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClientHybrid();
    ASSERT_NE(renderThreadClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, FINISHED);
    transaction->AddRemoteCommand(command, nodeId, FollowType::NONE);
    auto hybridrenderEnable = system::GetParameter("const.graphics.hybridrenderenable", "0");
    system::SetParameter("const.graphics.hybridrenderenable", "1");
    transaction->FlushImplicitTransaction(timestamp);
    system::SetParameter("const.graphics.hybridrenderenable", hybridrenderEnable);
}

/**
 * @tc.name: FlushImplicitTransaction002
 * @tc.desc: test func FlushImplicitTransaction when hybridCallback is not null
 * @tc.type: FUNC
 * @tc.require: issueICII2M
 */
HWTEST_F(RSModifiersDrawThreadTest, FlushImplicitTransaction002, TestSize.Level1)
{
    auto transaction = std::make_shared<RSTransactionHandler>();
    uint64_t timestamp = 1;
    auto renderThreadClient = CreateRenderThreadClientHybrid();
    ASSERT_NE(renderThreadClient, nullptr);
    transaction->SetRenderThreadClient(renderThreadClient);
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(nodeId, 1, 1, FINISHED);
    transaction->AddRemoteCommand(command, nodeId, FollowType::NONE);
    CommitTransactionCallback callback =
        [] (std::shared_ptr<RSIRenderClient> &renderServiceClient,
        std::unique_ptr<RSTransactionData>&& rsTransactionData, uint32_t& transactionDataIndex) {};
    RSTransactionHandler::SetCommitTransactionCallback(callback);
    transaction->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: GetInstanceTest001
 * @tc.desc: test results of Instance
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawThreadTest, GetInstanceTest001, TestSize.Level1)
{
    ASSERT_EQ(&RSModifiersDrawThread::Instance(), &RSModifiersDrawThread::Instance());
}

/**
 * @tc.name: SetCacheDir001
 * @tc.desc: test results of SetCacheDir
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawThreadTest, SetCacheDir001, TestSize.Level1)
{
    const std::string shaderCacheDir_ = "/path/shadercachedir";
    RSModifiersDrawThread::Instance().SetCacheDir(shaderCacheDir_);
    ASSERT_NE(ShaderCache::Instance().filePath_.find(shaderCacheDir_), std::string::npos);
}

/**
 * @tc.name: Start001
 * @tc.desc: test results of Start
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawThreadTest, Start001, TestSize.Level1)
{
    RSModifiersDrawThread::Instance().Start();
    ASSERT_TRUE(RSModifiersDrawThread::Instance().isStarted_);
    RSModifiersDrawThread::Instance().Start();
    ASSERT_TRUE(RSModifiersDrawThread::Instance().isStarted_);
    ASSERT_NE(nullptr, RSModifiersDrawThread::Instance().runner_);
}

/**
 * @tc.name: PostTask001
 * @tc.desc: test results of PostTask
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawThreadTest, PostTask001, TestSize.Level1)
{
    RSModifiersDrawThread::Instance().PostTask([]() {});
    ASSERT_EQ(RSModifiersDrawThread::Instance().isStarted_, true);
}

/**
 * @tc.name: PostTask002
 * @tc.desc: test results of PostTask
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawThreadTest, PostTask002, TestSize.Level1)
{
    RSModifiersDrawThread::Instance().Start();
    RSModifiersDrawThread::Instance().PostTask([]() {});
    ASSERT_NE(RSModifiersDrawThread::Instance().handler_, nullptr);
}

/**
 * @tc.name: ScheduleTask001
 * @tc.desc: test results of ScheduleTask
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawThreadTest, ScheduleTask001, TestSize.Level1)
{
    auto task = []() {};  /* some task */
    auto future = RSModifiersDrawThread::Instance().ScheduleTask(task);
    ASSERT_EQ(future.valid(), true);
}

/**
 * @tc.name: ConvertTransactionTest001
 * @tc.desc: test results of ConvertTransaction of canvas drawing node
 * @tc.type: FUNC
 * @tc.require: issueIC1FSX
 */
HWTEST_F(RSModifiersDrawThreadTest, ConvertTransactionTest001, TestSize.Level1)
{
    auto& modifiersDrawThread = RSModifiersDrawThread::Instance();
    NodeId nodeId = 1;
    auto mType = RSModifierType::CONTENT_STYLE;
    auto cmdList = std::make_shared<Drawing::DrawCmdList>();
    cmdList->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::CANVAS);
    auto cmd = std::make_unique<RSCanvasNodeUpdateRecording>(nodeId, cmdList, static_cast<uint16_t>(mType));
    auto transactionData = std::make_unique<RSTransactionData>();
    transactionData->AddCommand(std::move(cmd), nodeId, FollowType::NONE);
    auto renderThreadClient = CreateRenderThreadClientHybridSharedPtr();
    bool isNeedCommit = true;
    modifiersDrawThread.PostSyncTask([&, renderThreadClient]() {
        RSModifiersDrawThread::ConvertTransaction(transactionData, renderThreadClient, isNeedCommit);
    });
    ASSERT_NE(transactionData, nullptr);
}

/**
 * @tc.name: ConvertTransactionTest002
 * @tc.desc: test results of ConvertTransaction of text
 * @tc.type: FUNC
 * @tc.require: issueIC1FSX
 */
HWTEST_F(RSModifiersDrawThreadTest, ConvertTransactionTest002, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto mType = RSModifierType::CONTENT_STYLE;
    auto cmdList = std::make_shared<Drawing::DrawCmdList>();
    cmdList->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::TEXT);
    auto cmd = std::make_unique<RSCanvasNodeUpdateRecording>(nodeId, cmdList, static_cast<uint16_t>(mType));
    auto transactionData = std::make_unique<RSTransactionData>();
    transactionData->AddCommand(std::move(cmd), nodeId, FollowType::NONE);
    auto renderThreadClient = CreateRenderThreadClientHybridSharedPtr();
    bool isNeedCommit = true;
    RSModifiersDrawThread::Instance().PostSyncTask(
        [&]() { RSModifiersDrawThread::ConvertTransaction(transactionData, renderThreadClient, isNeedCommit); });
    ASSERT_NE(transactionData, nullptr);
}

/**
 * @tc.name: ConvertTransactionTest003
 * @tc.desc: test results of ConvertTransaction of 15 text op
 * @tc.type: FUNC
 * @tc.require: issueICEFNX
 */
HWTEST_F(RSModifiersDrawThreadTest, ConvertTransactionTest003, TestSize.Level1)
{
    NodeId nodeId = 1;
    uint16_t propertyId = 1;
    const int opCnt = 15;
    auto cmdList = std::make_shared<Drawing::DrawCmdList>();
    cmdList->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::TEXT);
    auto transactionData = std::make_unique<RSTransactionData>();
    Drawing::Brush brush;
    Drawing::BrushHandle brushHandle;
    Drawing::DrawOpItem::BrushToBrushHandle(brush, *cmdList, brushHandle);
    ASSERT_TRUE(cmdList->AddDrawOp<Drawing::DrawBackgroundOpItem::ConstructorHandle>(brushHandle));
    ASSERT_FALSE(cmdList->IsEmpty());
    for (int i = 0; i < opCnt; i++) {
        auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
        transactionData->AddCommand(std::move(cmd), nodeId, FollowType::NONE);
        nodeId++;
        propertyId++;
    }
    auto renderThreadClient = CreateRenderThreadClientHybridSharedPtr();
    bool isNeedCommit = true;
    RSModifiersDrawThread::Instance().PostSyncTask(
        [&]() { RSModifiersDrawThread::ConvertTransaction(transactionData, renderThreadClient, isNeedCommit); });
    ASSERT_NE(transactionData, nullptr);
}

/**
 * @tc.name: ConvertTransactionTest004
 * @tc.desc: test results of ConvertTransaction of drawCmdList = nullptr
 * @tc.type: FUNC
 * @tc.require: issueICEFNX
 */
HWTEST_F(RSModifiersDrawThreadTest, ConvertTransactionTest004, TestSize.Level1)
{
    NodeId nodeId = 1;
    uint16_t propertyId = 1;
    auto cmdList = std::make_shared<Drawing::DrawCmdList>();
    cmdList->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::TEXT);
    auto transactionData = std::make_unique<RSTransactionData>();
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, nullptr, propertyId);
    transactionData->AddCommand(std::move(cmd), nodeId, FollowType::NONE);
    auto renderThreadClient = CreateRenderThreadClientHybridSharedPtr();
    bool isNeedCommit = true;
    RSModifiersDrawThread::Instance().PostSyncTask(
        [&]() { RSModifiersDrawThread::ConvertTransaction(transactionData, renderThreadClient, isNeedCommit); });
    ASSERT_NE(transactionData, nullptr);
}

/**
 * @tc.name: ConvertTransactionTest005
 * @tc.desc: test results of ConvertTransaction of Canvas & MAX Width/Height
 * @tc.type: FUNC
 * @tc.require: issueICEFNX
 */
HWTEST_F(RSModifiersDrawThreadTest, ConvertTransactionTest005, TestSize.Level1)
{
    NodeId nodeId = 1;
    uint16_t propertyId = 1;
    auto cmdList = std::make_shared<Drawing::DrawCmdList>();
    cmdList->SetWidth(INT_MAX);
    cmdList->SetHeight(INT_MAX);
    cmdList->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::CANVAS);
    auto transactionData = std::make_unique<RSTransactionData>();
    Drawing::Brush brush;
    Drawing::BrushHandle brushHandle;
    Drawing::DrawOpItem::BrushToBrushHandle(brush, *cmdList, brushHandle);
    ASSERT_TRUE(cmdList->AddDrawOp<Drawing::DrawBackgroundOpItem::ConstructorHandle>(brushHandle));
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
    transactionData->AddCommand(std::move(cmd), nodeId, FollowType::NONE);
    auto renderThreadClient = CreateRenderThreadClientHybridSharedPtr();
    bool isNeedCommit = true;
    RSModifiersDrawThread::Instance().PostSyncTask(
        [&]() { RSModifiersDrawThread::ConvertTransaction(transactionData, renderThreadClient, isNeedCommit); });
    ASSERT_NE(transactionData, nullptr);
}

/**
 * @tc.name: ConvertTransactionTest006
 * @tc.desc: test results of ConvertTransaction of Text & MAX Width/Height
 * @tc.type: FUNC
 * @tc.require: issueICEFNX
 */
HWTEST_F(RSModifiersDrawThreadTest, ConvertTransactionTest006, TestSize.Level1)
{
    NodeId nodeId = 1;
    uint16_t propertyId = 1;
    auto cmdList = std::make_shared<Drawing::DrawCmdList>();
    cmdList->SetWidth(INT_MAX);
    cmdList->SetHeight(INT_MAX);
    cmdList->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::TEXT);
    auto transactionData = std::make_unique<RSTransactionData>();
    Drawing::Brush brush;
    Drawing::BrushHandle brushHandle;
    Drawing::DrawOpItem::BrushToBrushHandle(brush, *cmdList, brushHandle);
    ASSERT_TRUE(cmdList->AddDrawOp<Drawing::DrawBackgroundOpItem::ConstructorHandle>(brushHandle));
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
    transactionData->AddCommand(std::move(cmd), nodeId, FollowType::NONE);
    auto renderThreadClient = CreateRenderThreadClientHybridSharedPtr();
    bool isNeedCommit = true;
    RSModifiersDrawThread::Instance().PostSyncTask(
        [&]() { RSModifiersDrawThread::ConvertTransaction(transactionData, renderThreadClient, isNeedCommit); });
    ASSERT_NE(transactionData, nullptr);
}

/**
 * @tc.name: ConvertTransactionTest007
 * @tc.desc: test results of ConvertTransaction of HMSYMBOL
 * @tc.type: FUNC
 * @tc.require: issueICEFNX
 */
HWTEST_F(RSModifiersDrawThreadTest, ConvertTransactionTest007, TestSize.Level1)
{
    NodeId nodeId = 1;
    uint16_t propertyId = 1;
    auto cmdList = std::make_shared<Drawing::DrawCmdList>();
    cmdList->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::HMSYMBOL);
    auto transactionData = std::make_unique<RSTransactionData>();
    Drawing::Brush brush;
    Drawing::BrushHandle brushHandle;
    Drawing::DrawOpItem::BrushToBrushHandle(brush, *cmdList, brushHandle);
    ASSERT_TRUE(cmdList->AddDrawOp<Drawing::DrawBackgroundOpItem::ConstructorHandle>(brushHandle));
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
    transactionData->AddCommand(std::move(cmd), nodeId, FollowType::NONE);
    auto renderThreadClient = CreateRenderThreadClientHybridSharedPtr();
    bool isNeedCommit = true;
    RSModifiersDrawThread::Instance().PostSyncTask(
        [&]() { RSModifiersDrawThread::ConvertTransaction(transactionData, renderThreadClient, isNeedCommit); });
}

/**
 * @tc.name: ConvertTransactionTest008
 * @tc.desc: test results of ConvertTransaction of HMSYMBOL
 * @tc.type: FUNC
 * @tc.require: issueICEFNX
 */
HWTEST_F(RSModifiersDrawThreadTest, ConvertTransactionTest008, TestSize.Level1)
{
    NodeId nodeId = 1;
    uint16_t propertyId = 1;
    auto transactionData = std::make_unique<RSTransactionData>();
    int opCnt = HYBRID_MAX_ENABLE_OP_CNT + 1;
    for (int i = 0; i < opCnt; i++) {
        auto cmdList = std::make_shared<Drawing::DrawCmdList>();
        cmdList->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::HMSYMBOL);
        Drawing::Brush brush;
        Drawing::BrushHandle brushHandle;
        Drawing::DrawOpItem::BrushToBrushHandle(brush, *cmdList, brushHandle);
        ASSERT_TRUE(cmdList->AddDrawOp<Drawing::DrawBackgroundOpItem::ConstructorHandle>(brushHandle));
        auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
        transactionData->AddCommand(std::move(cmd), nodeId, FollowType::NONE);
    }
    auto renderThreadClient = CreateRenderThreadClientHybridSharedPtr();
    bool isNeedCommit = true;
    RSModifiersDrawThread::Instance().PostSyncTask(
        [&]() { RSModifiersDrawThread::ConvertTransaction(transactionData, renderThreadClient, isNeedCommit); });
}

/**
 * @tc.name: ConvertTransactionTest009
 * @tc.desc: test results of ConvertTransaction of HMSYMBOL
 * @tc.type: FUNC
 * @tc.require: issueICEFNX
 */
HWTEST_F(RSModifiersDrawThreadTest, ConvertTransactionTest009, TestSize.Level1)
{
    NodeId nodeId = 1;
    uint16_t propertyId = 1;
    auto transactionData = std::make_unique<RSTransactionData>();
    int opCnt = HYBRID_MAX_ENABLE_OP_CNT + 1;
    for (int i = 0; i < opCnt; i++) {
        auto cmdList = std::make_shared<Drawing::DrawCmdList>();
        cmdList->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::HMSYMBOL);
        Drawing::Brush brush;
        Drawing::BrushHandle brushHandle;
        Drawing::DrawOpItem::BrushToBrushHandle(brush, *cmdList, brushHandle);
        ASSERT_TRUE(cmdList->AddDrawOp<Drawing::DrawBackgroundOpItem::ConstructorHandle>(brushHandle));
        auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
        transactionData->AddCommand(std::move(cmd), nodeId, FollowType::NONE);
    }
    auto renderThreadClient = CreateRenderThreadClientHybridSharedPtr();
    bool isNeedCommit = true;
    RSModifiersDrawThread::SetIsFirstFrame(true);
    RSModifiersDrawThread::Instance().PostSyncTask(
        [&]() { RSModifiersDrawThread::ConvertTransaction(transactionData, renderThreadClient, isNeedCommit); });
}

/**
 * @tc.name: ConvertTransactionTest010
 * @tc.desc: test results of ConvertTransaction of HMSYMBOL
 * @tc.type: FUNC
 * @tc.require: issueICEFNX
 */
HWTEST_F(RSModifiersDrawThreadTest, ConvertTransactionTest010, TestSize.Level1)
{
    NodeId nodeId = 1;
    uint16_t propertyId = 1;
    auto transactionData = std::make_unique<RSTransactionData>();
    int opCnt = HYBRID_MAX_ENABLE_OP_CNT + 1;
    for (int i = 0; i < opCnt; i++) {
        auto cmdList = std::make_shared<Drawing::DrawCmdList>();
        cmdList->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::HMSYMBOL);
        Drawing::Brush brush;
        Drawing::BrushHandle brushHandle;
        Drawing::DrawOpItem::BrushToBrushHandle(brush, *cmdList, brushHandle);
        ASSERT_TRUE(cmdList->AddDrawOp<Drawing::DrawBackgroundOpItem::ConstructorHandle>(brushHandle));
        auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
        transactionData->AddCommand(std::move(cmd), nodeId, FollowType::NONE);
    }
    auto renderThreadClient = CreateRenderThreadClientHybridSharedPtr();
    bool isNeedCommit = true;
    RSModifiersDrawThread::SetIsFirstFrame(false);
    RSModifiersDrawThread::Instance().PostSyncTask(
        [&]() { RSModifiersDrawThread::ConvertTransaction(transactionData, renderThreadClient, isNeedCommit); });
}

/**
 * @tc.name: ConvertTransactionTest011
 * @tc.desc: test results of ConvertTransaction of HMSYMBOL
 * @tc.type: FUNC
 * @tc.require: issueICEFNX
 */
HWTEST_F(RSModifiersDrawThreadTest, ConvertTransactionTest011, TestSize.Level1)
{
    NodeId nodeId = 1;
    uint16_t propertyId = 1;
    auto transactionData = std::make_unique<RSTransactionData>();
    int opCnt = HYBRID_MAX_ENABLE_OP_CNT;
    for (int i = 0; i < opCnt; i++) {
        auto cmdList = std::make_shared<Drawing::DrawCmdList>();
        cmdList->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::HMSYMBOL);
        Drawing::Brush brush;
        Drawing::BrushHandle brushHandle;
        Drawing::DrawOpItem::BrushToBrushHandle(brush, *cmdList, brushHandle);
        ASSERT_TRUE(cmdList->AddDrawOp<Drawing::DrawBackgroundOpItem::ConstructorHandle>(brushHandle));
        auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
        transactionData->AddCommand(std::move(cmd), nodeId, FollowType::NONE);
    }
    auto renderThreadClient = CreateRenderThreadClientHybridSharedPtr();
    bool isNeedCommit = true;
    RSModifiersDrawThread::SetIsFirstFrame(false);
    RSModifiersDrawThread::Instance().PostSyncTask(
        [&]() { RSModifiersDrawThread::ConvertTransaction(transactionData, renderThreadClient, isNeedCommit); });
}

/**
 * @tc.name: ConvertTransactionTest012
 * @tc.desc: test results of ConvertTransaction for multiple op
 * @tc.type: FUNC
 * @tc.require: issueICOVMP
 */
HWTEST_F(RSModifiersDrawThreadTest, ConvertTransactionTest012, TestSize.Level2)
{
    NodeId nodeId = 1;
    uint16_t propertyId = 1;
    // generate canvas drawCmdList
    auto cmdListForCanvas = std::make_shared<Drawing::DrawCmdList>(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    cmdListForCanvas->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::CANVAS);
    auto cmdForCanvas = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdListForCanvas, propertyId);

    Drawing::Rect rect = {0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT};
    Drawing::ClipOp op = Drawing::ClipOp::INTERSECT;
    bool doAntiAlias = false;
    Drawing::ClipRectOpItem::ConstructorHandle constructorHandle =
        Drawing::ClipRectOpItem::ConstructorHandle(rect, op, doAntiAlias);
    // generate hmsymbol drawCmdList1
    auto cmdList1 = std::make_shared<Drawing::DrawCmdList>(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    cmdList1->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::HMSYMBOL);
    ASSERT_TRUE(cmdList1->AddDrawOp<Drawing::ClipRectOpItem::ConstructorHandle>(constructorHandle));
    auto cmd1 = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList1, propertyId);
    // generate hmsymbol drawCmdList2
    auto cmdList2 = std::make_shared<Drawing::DrawCmdList>(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    cmdList2->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::HMSYMBOL);
    ASSERT_TRUE(cmdList2->AddDrawOp<Drawing::ClipRectOpItem::ConstructorHandle>(constructorHandle));
    auto cmd2 = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList2, propertyId);

    // add command
    auto transactionData = std::make_unique<RSTransactionData>();
    transactionData->AddCommand(std::move(cmdForCanvas), nodeId, FollowType::NONE);
    transactionData->AddCommand(std::move(cmd1), nodeId, FollowType::NONE);
    transactionData->AddCommand(std::move(cmd2), nodeId, FollowType::NONE);

    auto renderThreadClient = CreateRenderThreadClientHybridSharedPtr();
    bool isNeedCommit = true;
    RSModifiersDrawThread::SetIsFirstFrame(false);
    RSModifiersDrawThread::Instance().PostSyncTask(
        [&]() { RSModifiersDrawThread::ConvertTransaction(transactionData, renderThreadClient, isNeedCommit); });
    ASSERT_NE(transactionData, nullptr);
}

/**
 * @tc.name: RemoveTask001
 * @tc.desc: test results of remove task while handle is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIC3PRG
 */
HWTEST_F(RSModifiersDrawThreadTest, RemoveTask001, TestSize.Level1)
{
    bool tag = false;
    auto task = [&tag] {
        tag = true;
    };
    RSModifiersDrawThread::Instance().PostTask(task, TASK_NAME, DELAY_TIME);
    RSModifiersDrawThread::Instance().handler_ = nullptr;
    RSModifiersDrawThread::Instance().RemoveTask(TASK_NAME);
    ASSERT_FALSE(tag);

    // restore handle_
    RSModifiersDrawThread::Instance().handler_ =
        std::make_shared<AppExecFwk::EventHandler>(RSModifiersDrawThread::Instance().runner_);
}

/**
 * @tc.name: RemoveTask002
 * @tc.desc: test results of remove task while handle isn't nullptr
 * @tc.type: FUNC
 * @tc.require: issueIC3PRG
 */
HWTEST_F(RSModifiersDrawThreadTest, RemoveTask002, TestSize.Level1)
{
    bool tag = false;
    auto task = [&tag] {
        tag = true;
    };
    RSModifiersDrawThread::Instance().PostTask(task, TASK_NAME, DELAY_TIME);
    RSModifiersDrawThread::Instance().RemoveTask(TASK_NAME);
    ASSERT_FALSE(tag);
}

/**
 * @tc.name: ClearEventResource001
 * @tc.desc: test results of ClearEventResource, while handler/runner are(not) nullptr.
 * @tc.type: FUNC
 * @tc.require: issueICCICO
 */
HWTEST_F(RSModifiersDrawThreadTest, ClearEventResource001, TestSize.Level1)
{
    RSModifiersDrawThread::Instance().Start();
    ASSERT_NE(RSModifiersDrawThread::Instance().handler_, nullptr);
    ASSERT_NE(RSModifiersDrawThread::Instance().runner_, nullptr);
    RSModifiersDrawThread::Instance().ClearEventResource();
    ASSERT_EQ(RSModifiersDrawThread::Instance().handler_, nullptr);
    ASSERT_EQ(RSModifiersDrawThread::Instance().runner_, nullptr);
    RSModifiersDrawThread::Instance().ClearEventResource();
    RSModifiersDrawThread::Instance().Destroy();
}

/**
 * @tc.name: GetIfFirstFrame001
 * @tc.desc: test results of GetIsFirstFrame, while SetIsFirstFrame false.
 * @tc.type: FUNC
 * @tc.require: issueICCICO
 */
HWTEST_F(RSModifiersDrawThreadTest, GetIsFirstFrame001, TestSize.Level1)
{
    RSModifiersDrawThread::SetIsFirstFrame(false);
    ASSERT_EQ(RSModifiersDrawThread::Instance().GetIsFirstFrame(), false);
}

/**
 * @tc.name: GetIfFirstFrame002
 * @tc.desc: test results of ClearEventResource, while SetIsFirstFrame true.
 * @tc.type: FUNC
 * @tc.require: issueICCICO
 */
HWTEST_F(RSModifiersDrawThreadTest, GetIsFirstFrame002, TestSize.Level1)
{
    RSModifiersDrawThread::SetIsFirstFrame(true);
    ASSERT_EQ(RSModifiersDrawThread::Instance().GetIsFirstFrame(), true);
}

/**
 * @tc.name: CreateDrawingContext001
 * @tc.desc: test results of GetRecyclableSingletonPtr while reset singleton
 * @tc.type:FUNC
 * @tc.require: issueICDVVY
 */
HWTEST_F(RSModifiersDrawThreadTest, CreateDrawingContext001, TestSize.Level2)
{
    auto& singletonPtr = RsVulkanContext::GetRecyclableSingletonPtr("");
    singletonPtr.reset();
    (void)RsVulkanContext::GetRecyclableSingleton();
    ASSERT_NE(RsVulkanContext::GetRecyclableSingletonPtr(), nullptr);
}

/**
 * @tc.name: CreateDrawingContext002
 * @tc.desc: test results of GetRecyclableSingletonPtr after GetRecyclableSingleton()
 * @tc.type:FUNC
 * @tc.require: issueICDVVY
 */
HWTEST_F(RSModifiersDrawThreadTest, CreateDrawingContext002, TestSize.Level2)
{
    (void)RsVulkanContext::GetRecyclableSingleton();
    ASSERT_NE(RsVulkanContext::GetRecyclableSingletonPtr(), nullptr);
}

/**
 * @tc.name: GetVulkanDeviceStatus001
 * @tc.desc: test results of GetVulkanDeviceStatus while SetVulkanDeviceStatus CREATE_SUCCESS
 * @tc.type:FUNC
 * @tc.require: issueICNL0B
 */
HWTEST_F(RSModifiersDrawThreadTest, GetVulkanDeviceStatus001, TestSize.Level2)
{
    auto& interface = RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    interface.SetVulkanDeviceStatus(VulkanDeviceStatus::CREATE_SUCCESS);
    ASSERT_EQ(RsVulkanContext::GetSingleton().GetVulkanDeviceStatus(), VulkanDeviceStatus::CREATE_SUCCESS);
}

/**
 * @tc.name: ReleaseRecyclableSingleton001
 * @tc.desc: test results of ReleaseRecyclableSingleton while CheckDrawingContextRecyclable is false
 * @tc.type:FUNC
 * @tc.require: issueICNL0B
 */
HWTEST_F(RSModifiersDrawThreadTest, ReleaseRecyclableSingleton001, TestSize.Level2)
{
    RsVulkanContext::ReleaseDrawingContextMap();
    RsVulkanContext::SetRecyclable(true);
    RsVulkanContext::GetRecyclableSingleton().GetRecyclableDrawingContext();
    ASSERT_NE(RsVulkanContext::GetRecyclableSingletonPtr(), nullptr);
    RsVulkanContext::ReleaseRecyclableSingleton();
}

/**
 * @tc.name: ReleaseRecyclableSingleton002
 * @tc.desc: test results of ReleaseRecyclableSingleton while CheckDrawingContextRecyclable is true
 * @tc.type:FUNC
 * @tc.require: issueICNL0B
 */
HWTEST_F(RSModifiersDrawThreadTest, ReleaseRecyclableSingleton002, TestSize.Level2)
{
    RsVulkanContext::ReleaseDrawingContextMap();
    RsVulkanContext::SetRecyclable(true);
    RsVulkanContext::GetRecyclableSingleton().GetDrawingContext();
    ASSERT_NE(RsVulkanContext::GetRecyclableSingletonPtr(), nullptr);
    ASSERT_EQ(RsVulkanContext::CheckDrawingContextRecyclable(), false);
    RsVulkanContext::GetSingleton().GetVulkanDeviceStatus();
    RsVulkanContext::ReleaseRecyclableSingleton();
}

/**
 * @tc.name: CheckDrawingContextRecyclable001
 * @tc.desc: test results of CheckDrawingContextRecyclable after GetRecyclableDrawingContext
 * @tc.type:FUNC
 * @tc.require: issueICNL0B
 */
HWTEST_F(RSModifiersDrawThreadTest, CheckDrawingContextRecyclable001, TestSize.Level2)
{
    auto& recyclableSingleton = RsVulkanContext::GetRecyclableSingletonPtr();
    recyclableSingleton.reset();
    RsVulkanContext::ReleaseDrawingContextMap();
    RsVulkanContext::SetRecyclable(false);
    RsVulkanContext::GetRecyclableSingleton().SetIsProtected(true);
    RsVulkanContext::ReleaseDrawingContextMap();
    RsVulkanContext::GetRecyclableSingleton().GetRecyclableDrawingContext();
    ASSERT_EQ(RsVulkanContext::CheckDrawingContextRecyclable(), true);
}

/**
 * @tc.name: CheckDrawingContextRecyclable002
 * @tc.desc: test results of CheckDrawingContextRecyclable after GetDrawingContext
 * @tc.type:FUNC
 * @tc.require: issueICNL0B
 */
HWTEST_F(RSModifiersDrawThreadTest, CheckDrawingContextRecyclable002, TestSize.Level2)
{
    RsVulkanContext::ReleaseDrawingContextMap();
    RsVulkanContext::GetSingleton().SetIsProtected(true);
    RsVulkanContext::GetRecyclableSingleton().GetDrawingContext();
    ASSERT_EQ(RsVulkanContext::CheckDrawingContextRecyclable(), false);
}
} // namespace OHOS::Rosen