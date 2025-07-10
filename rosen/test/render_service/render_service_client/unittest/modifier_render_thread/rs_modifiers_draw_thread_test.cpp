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
#include "recording/draw_cmd.h"
#include "render_context/shader_cache.h"
#include "command/rs_animation_command.h"
#include "command/rs_command.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_handler.h"

#ifdef RS_ENABLE_VK
#include "src/platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr const int64_t DELAY_TIME = 1000;
constexpr const char* TASK_NAME = "TaskName";
constexpr uint32_t HYBRID_MAX_ENABLE_OP_CNT = 11;  // max value for enable hybrid op

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
void RSModifiersDrawThreadTest::TearDown() {}

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
 * @tc.name: TargetCommand001
 * @tc.desc: test results of TargetCommand as (type,subtype) is (rsnode,updatexxx)
 * @tc.type: FUNC
 * @tc.require: issueIC1FSX
 */
HWTEST_F(RSModifiersDrawThreadTest, TargetCommand001, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto cmdList = std::make_shared<Drawing::DrawCmdList>();
    cmdList->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::CANVAS);
    uint64_t propertyId = 1;
#if defined(MODIFIER_NG)
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
#else
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdList>(
        nodeId, cmdList, propertyId, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
#endif
    ASSERT_TRUE(RSModifiersDrawThread::TargetCommand(Drawing::DrawCmdList::HybridRenderType::SVG, cmd->GetType(),
        cmd->GetSubType(), false));
}

/**
 * @tc.name: TargetCommand002
 * @tc.desc: test results of TargetCommand invalid
 * @tc.type: FUNC
 * @tc.require: issueIC1FSX
 */
HWTEST_F(RSModifiersDrawThreadTest, TargetCommand002, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto cmdList = std::make_shared<Drawing::DrawCmdList>();
    cmdList->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::NONE);
    uint64_t propertyId = 1;
#if defined(MODIFIER_NG)
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
#else
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdList>(
        nodeId, cmdList, propertyId, PropertyUpdateType::UPDATE_TYPE_INCREMENTAL);
#endif
    ASSERT_FALSE(RSModifiersDrawThread::TargetCommand(Drawing::DrawCmdList::HybridRenderType::NONE, cmd->GetType(),
        cmd->GetSubType(), false));
    ASSERT_FALSE(RSModifiersDrawThread::TargetCommand(Drawing::DrawCmdList::HybridRenderType::NONE, cmd->GetType(),
        cmd->GetSubType(), true));
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
#if defined(MODIFIER_NG)
        auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
#else
        auto cmd = std::make_unique<RSUpdatePropertyDrawCmdList>(
            nodeId, cmdList, propertyId, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
#endif
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
#if defined(MODIFIER_NG)
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, nullptr, propertyId);
#else
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdList>(
        nodeId, nullptr, propertyId, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
#endif
    transactionData->AddCommand(std::move(cmd), nodeId, FollowType::NONE);
    RSModifiersDrawThread::Instance().PostSyncTask(
        [&]() { RSModifiersDrawThread::ConvertTransaction(transactionData); });
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
#if defined(MODIFIER_NG)
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
#else
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdList>(
        nodeId, cmdList, propertyId, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
#endif
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
#if defined(MODIFIER_NG)
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
#else
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdList>(
        nodeId, cmdList, propertyId, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
#endif
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
#if defined(MODIFIER_NG)
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
#else
    auto cmd = std::make_unique<RSUpdatePropertyDrawCmdList>(
        nodeId, cmdList, propertyId, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
#endif
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
#if defined(MODIFIER_NG)
        auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
#else
        auto cmd = std::make_unique<RSUpdatePropertyDrawCmdList>(
            nodeId, cmdList, propertyId, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
#endif
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
#if defined(MODIFIER_NG)
        auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
#else
        auto cmd = std::make_unique<RSUpdatePropertyDrawCmdList>(
            nodeId, cmdList, propertyId, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
#endif
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
#if defined(MODIFIER_NG)
        auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
#else
        auto cmd = std::make_unique<RSUpdatePropertyDrawCmdList>(
            nodeId, cmdList, propertyId, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
#endif
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
#if defined(MODIFIER_NG)
        auto cmd = std::make_unique<RSUpdatePropertyDrawCmdListNG>(nodeId, cmdList, propertyId);
#else
        auto cmd = std::make_unique<RSUpdatePropertyDrawCmdList>(
            nodeId, cmdList, propertyId, PropertyUpdateType::UPDATE_TYPE_OVERWRITE);
#endif
        transactionData->AddCommand(std::move(cmd), nodeId, FollowType::NONE);
    }
    auto renderThreadClient = CreateRenderThreadClientHybridSharedPtr();
    bool isNeedCommit = true;
    RSModifiersDrawThread::SetIsFirstFrame(false);
    RSModifiersDrawThread::Instance().PostSyncTask(
        [&]() { RSModifiersDrawThread::ConvertTransaction(transactionData, renderThreadClient, isNeedCommit); });
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
} // namespace OHOS::Rosen