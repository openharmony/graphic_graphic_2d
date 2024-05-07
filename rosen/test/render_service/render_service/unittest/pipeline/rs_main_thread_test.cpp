/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <parameter.h>
#include <parameters.h>

#include "gtest/gtest.h"
#include "limit_number.h"
#include "rs_test_util.h"

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_engine.h"
#include "pipeline/rs_uni_render_engine.h"
#include "platform/common/rs_innovation.h"
#include "platform/common/rs_system_properties.h"
#if defined(ACCESSIBILITY_ENABLE)
#include "accessibility_config.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr uint64_t REFRESH_PERIOD = 16666667;
constexpr uint64_t SKIP_COMMAND_FREQ_LIMIT = 30;
constexpr uint32_t MULTI_WINDOW_PERF_START_NUM = 2;
constexpr uint32_t MULTI_WINDOW_PERF_END_NUM = 4;
constexpr int32_t SIMI_VISIBLE_RATE = 2;
constexpr int32_t SYSTEM_ANIMATED_SECNES_RATE = 2;
constexpr int32_t INVISBLE_WINDOW_RATE = 10;
constexpr int32_t DEFAULT_RATE = 1;
constexpr int32_t INVALID_VALUE = -1;
constexpr ScreenId DEFAULT_DISPLAY_SCREEN_ID = 0;
class RSMainThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void* CreateParallelSyncSignal(uint32_t count);
};

void RSMainThreadTest::SetUpTestCase() {}
void RSMainThreadTest::TearDownTestCase() {}
void RSMainThreadTest::SetUp() {}
void RSMainThreadTest::TearDown() {}
void* RSMainThreadTest::CreateParallelSyncSignal(uint32_t count)
{
    (void)(count);
    return nullptr;
}

/**
 * @tc.name: Start001
 * @tc.desc: Test RSMainThreadTest.Start
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, Start001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->Start();
}

/**
 * @tc.name: Start002
 * @tc.desc: Test RSMainThreadTest.Start
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, Start002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = nullptr;
    mainThread->Start();
}

/**
 * @tc.name: ProcessCommand
 * @tc.desc: Test RSMainThreadTest.ProcessCommand
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, ProcessCommand, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->ProcessCommand();
    mainThread->isUniRender_ = true;
    mainThread->lastAnimateTimestamp_ = 0;
    mainThread->timestamp_ = REFRESH_PERIOD + 1;
    mainThread->context_->purgeType_ = RSContext::PurgeType::GENTLY;
    mainThread->ProcessCommand();
    mainThread->context_->purgeType_ = RSContext::PurgeType::STRONGLY;
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: RsEventParamDump
 * @tc.desc: Test RSMainThreadTest.RsEventParamDump
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, RsEventParamDump, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::string str = "";
    mainThread->RsEventParamDump(str);
    ASSERT_TRUE(str.empty());
}

/**
 * @tc.name: RemoveRSEventDetector001
 * @tc.desc: Test RSMainThreadTest.RemoveRSEventDetector, with init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, RemoveRSEventDetector001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->InitRSEventDetector();
    mainThread->RemoveRSEventDetector();
}

/**
 * @tc.name: RemoveRSEventDetector002
 * @tc.desc: Test RSMainThreadTest.RemoveRSEventDetector, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, RemoveRSEventDetector002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->RemoveRSEventDetector();
}

/**
 * @tc.name: InitRSEventDetector
 * @tc.desc: Test RSMainThreadTest.InitRSEventDetector, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, InitRSEventDetector, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->rsCompositionTimeoutDetector_ = nullptr;
    mainThread->InitRSEventDetector();
}

/**
 * @tc.name: SetRSEventDetectorLoopStartTag001
 * @tc.desc: Test RSMainThreadTest.SetRSEventDetectorLoopStartTag, with init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, SetRSEventDetectorLoopStartTag001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->InitRSEventDetector();
    mainThread->SetRSEventDetectorLoopStartTag();
}

/**
 * @tc.name: SetRSEventDetectorLoopStartTag002
 * @tc.desc: Test RSMainThreadTest.SetRSEventDetectorLoopStartTag, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, SetRSEventDetectorLoopStartTag002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->rsCompositionTimeoutDetector_ = nullptr;
    mainThread->SetRSEventDetectorLoopStartTag();
}

/**
 * @tc.name: SetRSEventDetectorLoopFinishTag001
 * @tc.desc: Test RSMainThreadTest.SetRSEventDetectorLoopFinishTag, with init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, SetRSEventDetectorLoopFinishTag001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->InitRSEventDetector();
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->SetRSEventDetectorLoopFinishTag();
    mainThread->isUniRender_ = true;
    mainThread->SetRSEventDetectorLoopFinishTag();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: SetRSEventDetectorLoopFinishTag002
 * @tc.desc: Test RSMainThreadTest.SetRSEventDetectorLoopFinishTag, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, SetRSEventDetectorLoopFinishTag002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->rsCompositionTimeoutDetector_ = nullptr;
    mainThread->SetRSEventDetectorLoopFinishTag();
}

/**
 * @tc.name: WaitUtilUniRenderFinished
 * @tc.desc: Test RSMainThreadTest.WaitUtilUniRenderFinished
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, WaitUtilUniRenderFinished, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->NotifyUniRenderFinish();
    mainThread->WaitUtilUniRenderFinished();
    ASSERT_EQ(mainThread->uniRenderFinished_, true);
}

/**
 * @tc.name: ProcessCommandForDividedRender001
 * @tc.desc: Test RSMainThreadTest.ProcessCommandForDividedRender
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, ProcessCommandForDividedRender001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->ProcessCommandForDividedRender();
}

/**
 * @tc.name: CalcOcclusion
 * @tc.desc: Test RSMainThreadTest.CalcOcclusion, doWindowAnimate_ is false, isUniRender_ is true
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, CalcOcclusion, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->doWindowAnimate_ = false;
    mainThread->isUniRender_ = true;
    mainThread->CalcOcclusion();
}

/**
 * @tc.name: Animate001
 * @tc.desc: Test RSMainThreadTest.Animate, doWindowAnimate_ is false
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, Animate001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->doWindowAnimate_ = false;
    mainThread->Animate(0);
}

/**
 * @tc.name: Animate002
 * @tc.desc: Test RSMainThreadTest.Animate, doWindowAnimate_ is true
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, Animate002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->doWindowAnimate_ = true;
    mainThread->Animate(0);
}

/**
 * @tc.name: UnRegisterOcclusionChangeCallback
 * @tc.desc: Test RSMainThreadTest.Animate
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, UnRegisterOcclusionChangeCallback, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->UnRegisterOcclusionChangeCallback(0);
}

/**
 * @tc.name: RenderServiceTreeDump
 * @tc.desc: Test RSMainThreadTest.RenderServiceTreeDump, str is an empty string
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, RenderServiceTreeDump, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::string str = "";
    mainThread->RenderServiceTreeDump(str);
}

/**
 * @tc.name: SetFocusAppInfo
 * @tc.desc: Test RSMainThreadTest.SetFocusAppInfo, input pid, uid is -1, str is an empty string
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, SetFocusAppInfo, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::string str = "";
    int32_t pid = INVALID_VALUE;
    int32_t uid = INVALID_VALUE;
    mainThread->SetFocusAppInfo(pid, uid, str, str, 0);
}

/**
 * @tc.name: SetFocusAppInfo002
 * @tc.desc: Test SetFocusAppInfo while change focus node
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSMainThreadTest, SetFocusAppInfo002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto oldFocusNode = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    auto newFocusNode = std::make_shared<RSSurfaceRenderNode>(id + 1, mainThread->context_);
    mainThread->focusNodeId_ = oldFocusNode->GetId();

    std::string str = "";
    int32_t pid = INVALID_VALUE;
    int32_t uid = INVALID_VALUE;
    mainThread->SetFocusAppInfo(pid, uid, str, str, newFocusNode->GetId());
    ASSERT_EQ(mainThread->GetFocusNodeId(), newFocusNode->GetId());
}

/**
 * @tc.name: ProcessSyncRSTransactionData001
 * @tc.desc: Test ProcessSyncRSTransactionData when TransactionData do not need sync
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, ProcessSyncRSTransactionData001, TestSize.Level1)
{
    // when IsNeedSync() is false
    auto mainThread = RSMainThread::Instance();
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    pid_t pid = 0;
    rsTransactionData->SetSyncId(1);
    mainThread->ProcessSyncRSTransactionData(rsTransactionData, pid);
    ASSERT_EQ(mainThread->syncTransactionData_.empty(), false);

    // when syncTransactionData_ is not empty and SyncId is larger
    rsTransactionData = std::make_unique<RSTransactionData>();
    rsTransactionData->MarkNeedSync();
    rsTransactionData->SetSyncId(0);
    mainThread->ProcessSyncRSTransactionData(rsTransactionData, pid);
    ASSERT_EQ(mainThread->syncTransactionData_.empty(), false);

    // when syncTransactionData_ is not empty and SyncId is equal or smaller
    rsTransactionData->SetSyncTransactionNum(1);
    rsTransactionData->SetSyncId(1);
    mainThread->syncTransactionCount_ = 1;
    mainThread->ProcessSyncRSTransactionData(rsTransactionData, pid);
    ASSERT_EQ(mainThread->syncTransactionData_.empty(), false);
}

/**
 * @tc.name: ProcessSyncRSTransactionData002
 * @tc.desc: Test ProcessSyncRSTransactionData when TransactionData do not need sync
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, ProcessSyncRSTransactionData002, TestSize.Level1)
{
    // when IsNeedSync() is true & syncTransactionData_ is empty & isNeedCloseSync is true
    auto mainThread = RSMainThread::Instance();
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    pid_t pid = 0;
    rsTransactionData->MarkNeedSync();
    rsTransactionData->MarkNeedCloseSync();
    rsTransactionData->SetSyncTransactionNum(1);
    mainThread->syncTransactionCount_ = 0;
    mainThread->ProcessSyncRSTransactionData(rsTransactionData, pid);
    ASSERT_EQ(mainThread->syncTransactionData_.empty(), false);
}

/**
 * @tc.name: GetContext
 * @tc.desc: Test if context has been initialized
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, GetContext, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto& context = mainThread->GetContext();
    uint64_t time = 0;
    ASSERT_EQ(context.GetTransactionTimestamp(), time);
}

/**
 * @tc.name: ClassifyRSTransactionData001
 * @tc.desc: Test ClassifyRSTransactionData when nodeId is 0
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, ClassifyRSTransactionData001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->pendingEffectiveCommands_.clear();
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    std::unique_ptr<RSCommand> command = nullptr;
    NodeId nodeId = 0;
    FollowType followType = FollowType::NONE;
    rsTransactionData->AddCommand(command, nodeId, followType);
    mainThread->ClassifyRSTransactionData(rsTransactionData);
    ASSERT_EQ(mainThread->pendingEffectiveCommands_.empty(), true);
}

/**
 * @tc.name: ClassifyRSTransactionData002
 * @tc.desc: Test ClassifyRSTransactionData when nodeId is 1
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSMainThreadTest, ClassifyRSTransactionData002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->pendingEffectiveCommands_.clear();
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    std::unique_ptr<RSCommand> command = nullptr;
    NodeId nodeId = 1;
    FollowType followType = FollowType::NONE;
    rsTransactionData->AddCommand(command, nodeId, followType);
    mainThread->ClassifyRSTransactionData(rsTransactionData);
    ASSERT_EQ(mainThread->pendingEffectiveCommands_.empty(), true);
}

/**
 * @tc.name: ClassifyRSTransactionData003
 * @tc.desc: Test ClassifyRSTransactionData when followType is FOLLOW_TO_PARENT
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSMainThreadTest, ClassifyRSTransactionData003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->pendingEffectiveCommands_.clear();
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    std::unique_ptr<RSCommand> command = nullptr;
    NodeId nodeId = 1;
    FollowType followType = FollowType::FOLLOW_TO_PARENT;
    rsTransactionData->AddCommand(command, nodeId, followType);
    mainThread->ClassifyRSTransactionData(rsTransactionData);
    ASSERT_EQ(mainThread->cachedCommands_[nodeId].empty(), true);
}

/**
 * @tc.name: ClassifyRSTransactionData004
 * @tc.desc: Test ClassifyRSTransactionData when followType is FOLLOW_TO_PARENT
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSMainThreadTest, ClassifyRSTransactionData004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    // build the NodeTree
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context = {};
    auto node = std::make_shared<RSBaseRenderNode>(nodeId, context);
    auto childNode = std::make_shared<RSBaseRenderNode>(nodeId + 1, context);
    int index = 0;
    node->SetIsOnTheTree(true);
    node->AddChild(node, index);
    ASSERT_EQ(static_cast<int>(node->GetChildrenCount()), 0);
    node->AddChild(childNode, index);
    ASSERT_EQ(static_cast<int>(node->GetChildrenCount()), 1);
    ASSERT_TRUE(childNode->IsOnTheTree());

    mainThread->cachedCommands_.clear();
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    std::unique_ptr<RSCommand> command = nullptr;
    FollowType followType = FollowType::FOLLOW_TO_SELF;
    rsTransactionData->AddCommand(command, nodeId, followType);
    mainThread->ClassifyRSTransactionData(rsTransactionData);
    ASSERT_EQ(mainThread->cachedCommands_[nodeId].empty(), true);

    mainThread->cachedCommands_.clear();
    rsTransactionData = std::make_unique<RSTransactionData>();
    command = nullptr;
    followType = FollowType::FOLLOW_TO_PARENT;
    rsTransactionData->AddCommand(command, nodeId + 1, followType);
    mainThread->ClassifyRSTransactionData(rsTransactionData);
    ASSERT_EQ(mainThread->cachedCommands_[nodeId + 1].empty(), true);
}

/**
 * @tc.name: AddActiveNode
 * @tc.desc: Test AddActiveNode, add invalid node id, check if fails
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, AddActiveNode, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->context_->activeNodesInRoot_.clear();
    // invalid nodeid
    NodeId id = INVALID_NODEID;
    auto node = std::make_shared<RSRenderNode>(id, mainThread->context_);
    mainThread->context_->AddActiveNode(node);
    ASSERT_EQ(static_cast<int>(mainThread->context_->activeNodesInRoot_.size()), 0);
}

/**
 * @tc.name: CheckAndUpdateInstanceContentStaticStatus01
 * @tc.desc: Test static instance(no dirty) would be classify as only basic geo transform
 * @tc.type: FUNC
 * @tc.require: issueI8IXTX
 */
HWTEST_F(RSMainThreadTest, CheckAndUpdateInstanceContentStaticStatus01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->context_->activeNodesInRoot_.clear();
    // valid nodeid
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node, nullptr);
    mainThread->CheckAndUpdateInstanceContentStaticStatus(node);
    ASSERT_EQ(node->GetSurfaceCacheContentStatic(), true);
}

/**
 * @tc.name: CheckAndUpdateInstanceContentStaticStatus02
 * @tc.desc: Test new instance would not be classify as only basic geo transform
 * @tc.type: FUNC
 * @tc.require: issueI8IXTX
 */
HWTEST_F(RSMainThreadTest, CheckAndUpdateInstanceContentStaticStatus02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->context_->activeNodesInRoot_.clear();
    // valid nodeid
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node, nullptr);
    node->SetIsOnTheTree(true, id, id);
    node->SetContentDirty();
    mainThread->context_->AddActiveNode(node);
    ASSERT_EQ(static_cast<int>(mainThread->context_->activeNodesInRoot_.size()), 1);
    mainThread->CheckAndUpdateInstanceContentStaticStatus(node);
    ASSERT_EQ(node->GetSurfaceCacheContentStatic(), false);
}

/**
 * @tc.name: IsNeedProcessBySingleFrameComposerTest001
 * @tc.desc: Test IsNeedProcessBySingleFrameComposerTest when TransactionData is null
 * @tc.type: FUNC
 * @tc.require: issueI9HPBS
 */
HWTEST_F(RSMainThreadTest, IsNeedProcessBySingleFrameComposerTest001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::unique_ptr<RSTransactionData> transactionData = nullptr;
    ASSERT_FALSE(mainThread->IsNeedProcessBySingleFrameComposer(transactionData));
    transactionData = std::make_unique<RSTransactionData>();
    mainThread->isUniRender_ = true;
    ASSERT_FALSE(mainThread->IsNeedProcessBySingleFrameComposer(transactionData));
}

/**
 * @tc.name: IsNeedProcessBySingleFrameComposerTest002
 * @tc.desc: Test IsNeedProcessBySingleFrameComposerTest when SingleFrameComposer enabled by app process
 * @tc.type: FUNC
 * @tc.require: issueI9HPBS
 */
HWTEST_F(RSMainThreadTest, IsNeedProcessBySingleFrameComposerTest002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto transactionData = std::make_unique<RSTransactionData>();
    mainThread->isUniRender_ = true;
    pid_t pid = 1;
    transactionData->SetSendingPid(pid);
    RSSingleFrameComposer::AddOrRemoveAppPidToMap(true, pid);
    ASSERT_TRUE(mainThread->IsNeedProcessBySingleFrameComposer(transactionData));
}

/**
 * @tc.name: IsNeedProcessBySingleFrameComposerTest003
 * @tc.desc: Test IsNeedProcessBySingleFrameComposerTest when animation node exists
 * @tc.type: FUNC
 * @tc.require: issueI9HPBS
 */
HWTEST_F(RSMainThreadTest, IsNeedProcessBySingleFrameComposerTest003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto transactionData = std::make_unique<RSTransactionData>();
    mainThread->isUniRender_ = true;
    pid_t pid = 1;
    transactionData->SetSendingPid(pid);
    RSSingleFrameComposer::AddOrRemoveAppPidToMap(true, pid);
    
    NodeId id = 1;
    auto node = std::make_shared<RSRenderNode>(id, mainThread->context_);
    mainThread->context_->RegisterAnimatingRenderNode(node);
    ASSERT_FALSE(mainThread->IsNeedProcessBySingleFrameComposer(transactionData));
}

/**
 * @tc.name: IsNeedProcessBySingleFrameComposerTest004
 * @tc.desc: Test IsNeedProcessBySingleFrameComposerTest when multi-window shown on screen
 * @tc.type: FUNC
 * @tc.require: issueI9HPBS
 */
HWTEST_F(RSMainThreadTest, IsNeedProcessBySingleFrameComposerTest004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto transactionData = std::make_unique<RSTransactionData>();
    mainThread->isUniRender_ = true;
    pid_t pid = 1;
    transactionData->SetSendingPid(pid);
    RSSingleFrameComposer::AddOrRemoveAppPidToMap(true, pid);
    
    NodeId firstWindowNodeId = 2;
    auto firstWindowNode = std::make_shared<RSSurfaceRenderNode>(firstWindowNodeId, mainThread->context_);
    firstWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    NodeId firstWindowChildNodeId = 3;
    auto firstWindowChildNode = std::make_shared<RSSurfaceRenderNode>(firstWindowChildNodeId, mainThread->context_);
    firstWindowChildNode->MarkUIHidden(false);
    firstWindowNode->AddChild(firstWindowChildNode);
    firstWindowNode->GenerateFullChildrenList();
    mainThread->context_->nodeMap.RegisterRenderNode(firstWindowNode);
    
    NodeId secondWindowNodeId = 2;
    auto secondWindowNode = std::make_shared<RSSurfaceRenderNode>(secondWindowNodeId, mainThread->context_);
    secondWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    NodeId secondWindowChildNodeId = 3;
    auto secondWindowChildNode = std::make_shared<RSSurfaceRenderNode>(secondWindowChildNodeId, mainThread->context_);
    secondWindowChildNode->MarkUIHidden(false);
    secondWindowNode->AddChild(secondWindowChildNode);
    secondWindowNode->GenerateFullChildrenList();
    mainThread->context_->nodeMap.RegisterRenderNode(secondWindowNode);
    ASSERT_FALSE(mainThread->IsNeedProcessBySingleFrameComposer(transactionData));
}

/**
 * @tc.name: RecvAndProcessRSTransactionDataImmediatelyTest
 * @tc.desc: Test ecvAndProcessRSTransactionDataImmediately when transactionData is null
 * @tc.type: FUNC
 * @tc.require: issueI9HPBS
 */
HWTEST_F(RSMainThreadTest, RecvAndProcessRSTransactionDataImmediatelyTest, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::unique_ptr<RSTransactionData> transactionData = nullptr;
    mainThread->RecvAndProcessRSTransactionDataImmediately(transactionData);
    ASSERT_EQ(transactionData, nullptr);
}


/**
 * @tc.name: RecvRSTransactionData
 * @tc.desc: Test RecvRSTransactionData, when TransactionData is null
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSMainThreadTest, RecvRSTransactionData, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::unique_ptr<RSTransactionData> transactionData = nullptr;
    mainThread->RecvRSTransactionData(transactionData);
    ASSERT_EQ(transactionData, nullptr);
}

/**
 * @tc.name: PostSyncTask
 * @tc.desc: Test PostSyncTask when handler is null or not
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSMainThreadTest, PostSyncTask, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    RSTaskMessage::RSTask task = []() -> void { return; };
    mainThread->PostSyncTask(task);
}

/**
 * @tc.name: ShowWatermark
 * @tc.desc: ShowWatermark test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSMainThreadTest, ShowWatermark, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    const uint32_t color[8] = { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 };
    uint32_t colorLength = sizeof(color) / sizeof(color[0]);
    const int32_t offset = 0;
    Media::InitializationOptions opts;
    int32_t stride = 3;
    std::unique_ptr<Media::PixelMap> pixelMap1 = Media::PixelMap::Create(color, colorLength, offset, stride, opts);
    mainThread->ShowWatermark(std::move(pixelMap1), true);
    ASSERT_EQ(mainThread->GetWatermarkFlag(), true);
    mainThread->ShowWatermark(nullptr, false);
    ASSERT_EQ(mainThread->GetWatermarkFlag(), false);
}

/**
 * @tc.name: MergeToEffectiveTransactionDataMap001
 * @tc.desc: Test RSMainThreadTest.MergeToEffectiveTransactionDataMap
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, MergeToEffectiveTransactionDataMap001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->Start();
    ASSERT_EQ(mainThread->effectiveTransactionDataIndexMap_.empty(), true);
    mainThread->effectiveTransactionDataIndexMap_[0].first = 0;
    TransactionDataMap dataMap;
    auto data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(1);
    dataMap[0].emplace_back(std::move(data));
    data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(3);
    dataMap[0].emplace_back(std::move(data));
    dataMap[0].emplace_back(nullptr);
    mainThread->MergeToEffectiveTransactionDataMap(dataMap);
    mainThread->effectiveTransactionDataIndexMap_.clear();
}

/**
 * @tc.name: ProcessCommandForUniRender
 * @tc.desc: ProcessCommandForUniRender test with invalid data
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSMainThreadTest, ProcessCommandForUniRender, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_EQ(mainThread->effectiveTransactionDataIndexMap_.empty(), true);

    // // let lastindex same as timeout index and test timeout case at first
    mainThread->transactionDataLastWaitTime_[0] = 0;
    mainThread->timestamp_ = REFRESH_PERIOD * SKIP_COMMAND_FREQ_LIMIT + 1;
    mainThread->effectiveTransactionDataIndexMap_[0].first = 0;
    // default data with index 0
    auto data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(1);
    mainThread->effectiveTransactionDataIndexMap_[0].second.emplace_back(std::move(data));
    data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(3);
    mainThread->effectiveTransactionDataIndexMap_[0].second.emplace_back(std::move(data));
    data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(2);
    mainThread->effectiveTransactionDataIndexMap_[0].second.emplace_back(std::move(data));
    // empty data
    mainThread->effectiveTransactionDataIndexMap_[0].second.emplace_back(nullptr);
    mainThread->ProcessCommandForUniRender();
}

/**
 * @tc.name: GetWatermarkImg
 * @tc.desc: GetWatermarkImg test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, GetWatermarkImg, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->GetWatermarkImg();
}

/**
 * @tc.name: DoParallelComposition
 * @tc.desc: DoParallelComposition test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, DoParallelComposition, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context = {};
    auto node = std::make_shared<RSBaseRenderNode>(nodeId, context);
    auto childNode = std::make_shared<RSBaseRenderNode>(nodeId + 1, context);
    int index = 0;
    node->SetIsOnTheTree(true);
    node->AddChild(childNode, index);
    ASSERT_EQ(static_cast<int>(node->GetChildrenCount()), 1);
    ASSERT_TRUE(childNode->IsOnTheTree());

    auto mainThread = RSMainThread::Instance();
    RSInnovation::_s_createParallelSyncSignal = (void*)RSMainThreadTest::CreateParallelSyncSignal;
    if (RSInnovation::GetParallelCompositionEnabled(mainThread->isUniRender_)) {
        mainThread->DoParallelComposition(node);
    }
}

/**
 * @tc.name: SetIdleTimerExpiredFlag
 * @tc.desc: SetIdleTimerExpiredFlag test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetIdleTimerExpiredFlag, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->SetIdleTimerExpiredFlag(true);
}

/**
 * @tc.name: SetFocusLeashWindowId
 * @tc.desc: Test RSMainThreadTest.SetFocusLeashWindowId
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, SetFocusLeashWindowId, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->Start();
    NodeId id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node1, nullptr);
    RSSurfaceRenderNodeConfig config;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node2, nullptr);
    node1->SetParent(node2);
    node1->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node2->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;

    mainThread->context_ = std::make_shared<RSContext>();
    mainThread->context_->nodeMap.renderNodeMap_[0] = node1;
    mainThread->focusNodeId_ = 0;
    mainThread->SetFocusLeashWindowId();
}

/**
 * @tc.name: SetFocusLeashWindowId002
 * @tc.desc: Test SetFocusLeashWindowId while nodeMap is empty
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSMainThreadTest, SetFocusLeashWindowId002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    mainThread->SetFocusLeashWindowId();
    ASSERT_EQ(mainThread->GetFocusLeashWindowId(), INVALID_NODEID);
}

/**
 * @tc.name: SetFocusLeashWindowId003
 * @tc.desc: Test SetFocusLeashWindowId while focus node don't have parent
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSMainThreadTest, SetFocusLeashWindowId003, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(node, nullptr);

    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->nodeMap.renderNodeMap_[node->GetId()] = node;
    std::string str = "";
    mainThread->SetFocusAppInfo(-1, -1, str, str, node->GetId());
    mainThread->SetFocusLeashWindowId();
    ASSERT_EQ(mainThread->GetFocusLeashWindowId(), INVALID_NODEID);
}

/**
 * @tc.name: SetFocusLeashWindowId004
 * @tc.desc: Test SetFocusLeashWindowId while focus node's type don't match
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSMainThreadTest, SetFocusLeashWindowId004, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    auto childNode = RSTestUtil::CreateSurfaceNode();
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(childNode, nullptr);
    ASSERT_NE(parentNode, nullptr);

    parentNode->AddChild(parentNode);
    childNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);

    mainThread->context_->nodeMap.renderNodeMap_[childNode->GetId()] = childNode;
    mainThread->context_->nodeMap.renderNodeMap_[parentNode->GetId()] = parentNode;
    std::string str = "";
    mainThread->SetFocusAppInfo(-1, -1, str, str, childNode->GetId());
    mainThread->SetFocusLeashWindowId();
    ASSERT_EQ(mainThread->GetFocusLeashWindowId(), INVALID_NODEID);
}

/**
 * @tc.name: SetFocusLeashWindowId005
 * @tc.desc: Test SetFocusLeashWindowId while focus node's parent's type don't match
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSMainThreadTest, SetFocusLeashWindowId005, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    auto childNode = RSTestUtil::CreateSurfaceNode();
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(childNode, nullptr);
    ASSERT_NE(parentNode, nullptr);

    parentNode->AddChild(parentNode);
    childNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    mainThread->context_->nodeMap.renderNodeMap_[childNode->GetId()] = childNode;
    mainThread->context_->nodeMap.renderNodeMap_[parentNode->GetId()] = parentNode;
    std::string str = "";
    mainThread->SetFocusAppInfo(-1, -1, str, str, childNode->GetId());
    mainThread->SetFocusLeashWindowId();
    ASSERT_EQ(mainThread->GetFocusLeashWindowId(), INVALID_NODEID);
}

/**
 * @tc.name: SetIsCachedSurfaceUpdated
 * @tc.desc: Test RSMainThreadTest.SetIsCachedSurfaceUpdated
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, SetIsCachedSurfaceUpdated, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetIsCachedSurfaceUpdated(true);
}

/**
 * @tc.name: PrintCurrentStatus
 * @tc.desc: Test RSMainThreadTest.PrintCurrentStatus
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, PrintCurrentStatus, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->PrintCurrentStatus();
}

/**
 * @tc.name: SetDeviceType
 * @tc.desc: Test RSMainThreadTest.SetDeviceType
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, SetDeviceType, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    system::SetParameter("const.product.devicetype", "pc");
    mainThread->SetDeviceType();
    system::SetParameter("const.product.devicetype", "tablet");
    mainThread->SetDeviceType();
    system::SetParameter("const.product.devicetype", "others");
    mainThread->SetDeviceType();
    system::SetParameter("const.product.devicetype", "phone");
    mainThread->SetDeviceType();
}

/**
 * @tc.name: CacheCommands
 * @tc.desc: Test RSMainThreadTest.CacheCommands
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, CacheCommands, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(1);
    mainThread->cachedSkipTransactionDataMap_[0].emplace_back(std::move(data));
    data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(2);
    mainThread->cachedSkipTransactionDataMap_[0].emplace_back(std::move(data));
    mainThread->CacheCommands();
}

/**
 * @tc.name: CheckIfNodeIsBundle
 * @tc.desc: Test RSMainThreadTest.CheckIfNodeIsBundle
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, CheckIfNodeIsBundle, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RSSurfaceRenderNodeConfig config;
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    node->name_ = "WallpaperView";
    mainThread->CheckIfNodeIsBundle(node);
    ASSERT_TRUE(mainThread->noBundle_);
}

/**
 * @tc.name: InformHgmNodeInfo
 * @tc.desc: Test RSMainThreadTest.InformHgmNodeInfo
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, InformHgmNodeInfo, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->currentBundleName_ = "test";
    mainThread->InformHgmNodeInfo();
    mainThread->currentBundleName_ = "";
    mainThread->noBundle_ = true;
    mainThread->InformHgmNodeInfo();
}

/**
 * @tc.name: CheckParallelSubThreadNodesStatus
 * @tc.desc: Test RSMainThreadTest.CheckParallelSubThreadNodesStatus
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, CheckParallelSubThreadNodesStatus, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->subThreadNodes_.clear();
    mainThread->subThreadNodes_.push_back(nullptr);
    mainThread->isUiFirstOn_ = false;
    mainThread->CheckParallelSubThreadNodesStatus();

    auto node1 = std::make_shared<RSSurfaceRenderNode>(0xFFFFFFFFFFFFFFFF);
    node1->cacheProcessStatus_ = CacheProcessStatus::DOING;
    node1->name_ = "node1";
    node1->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node1->hasAbilityComponent_ = true;
    node1->abilityNodeIds_.emplace(10);
    node1->abilityNodeIds_.emplace(11);
    auto node2 = std::make_shared<RSSurfaceRenderNode>(2);
    node2->cacheProcessStatus_ = CacheProcessStatus::DOING;
    node2->name_ = "node2";
    node2->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(0xFFFFFFFFFFFFFFF0);
    node3->cacheProcessStatus_ = CacheProcessStatus::DOING;
    node3->name_ = "node3";
    node3->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node3->hasAbilityComponent_ = false;
    // create child nodes
    auto childNode1 = std::make_shared<RSSurfaceRenderNode>(3);
    childNode1->name_ = "childNode1";
    childNode1->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto childNode2 = std::make_shared<RSSurfaceRenderNode>(4);
    childNode2->name_ = "childNode2";
    childNode2->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto childNode3 = std::make_shared<RSSurfaceRenderNode>(5);
    childNode3->name_ = "childNode3";

    node2->AddChild(childNode1);
    node2->AddChild(childNode2);
    node2->AddChild(childNode3);
    mainThread->subThreadNodes_.push_back(node1);
    mainThread->subThreadNodes_.push_back(node2);
    mainThread->subThreadNodes_.push_back(node3);

    mainThread->cacheCmdSkippedInfo_.clear();
    mainThread->CheckParallelSubThreadNodesStatus();
}

/**
 * @tc.name: IsNeedSkip
 * @tc.desc: IsNeedSkip test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, IsNeedSkip, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->IsNeedSkip(0, 0);
}

/**
 * @tc.name: SkipCommandByNodeId001
 * @tc.desc: SkipCommandByNodeId test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SkipCommandByNodeId001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<std::unique_ptr<RSTransactionData>> transactionVec;
    mainThread->SkipCommandByNodeId(transactionVec, -1);
}

/**
 * @tc.name: SkipCommandByNodeId002
 * @tc.desc: SkipCommandByNodeId test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SkipCommandByNodeId002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare transactionVec
    std::vector<std::unique_ptr<RSTransactionData>> transactionVec;
    auto data = std::make_unique<RSTransactionData>();
    data->SetIndex(1);
    transactionVec.emplace_back(std::move(data));
    data = std::make_unique<RSTransactionData>();
    data->SetIndex(2);
    data->payload_.resize(2);
    data->payload_[0] = std::tuple<NodeId, FollowType, std::unique_ptr<RSCommand>>(0, FollowType::NONE, nullptr);
    transactionVec.emplace_back(std::move(data));
    std::pair<std::vector<NodeId>, bool> info = {{0}, true};
    mainThread->cacheCmdSkippedInfo_.insert({0, info});
    mainThread->SkipCommandByNodeId(transactionVec, 0);
}

/**
 * @tc.name: ConsumeAndUpdateAllNodes001
 * @tc.desc: ConsumeAndUpdateAllNodes test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ConsumeAndUpdateAllNodes001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->ConsumeAndUpdateAllNodes();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ConsumeAndUpdateAllNodes002
 * @tc.desc: ConsumeAndUpdateAllNodes test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ConsumeAndUpdateAllNodes002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->ConsumeAndUpdateAllNodes();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: CheckSubThreadNodeStatusIsDoing001
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CheckSubThreadNodeStatusIsDoing001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->subThreadNodes_.clear();
    auto rsSurfaceRenderNode1 = RSTestUtil::CreateSurfaceNode();
    rsSurfaceRenderNode1->SetCacheSurfaceProcessedStatus(CacheProcessStatus::WAITING);
    auto rsSurfaceRenderNode2 = RSTestUtil::CreateSurfaceNode();
    rsSurfaceRenderNode2->SetCacheSurfaceProcessedStatus(CacheProcessStatus::DOING);
    mainThread->subThreadNodes_.emplace_back(rsSurfaceRenderNode1);
    mainThread->subThreadNodes_.emplace_back(rsSurfaceRenderNode2);
    mainThread->CheckSubThreadNodeStatusIsDoing(rsSurfaceRenderNode2->GetId());
}

/**
 * @tc.name: CheckSubThreadNodeStatusIsDoing002
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CheckSubThreadNodeStatusIsDoing002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->subThreadNodes_.clear();
    auto rsSurfaceRenderNode1 = RSTestUtil::CreateSurfaceNode();
    rsSurfaceRenderNode1->SetCacheSurfaceProcessedStatus(CacheProcessStatus::DOING);
    auto rsSurfaceRenderNode2 = RSTestUtil::CreateSurfaceNode();
    rsSurfaceRenderNode1->AddChild(rsSurfaceRenderNode2);
    mainThread->subThreadNodes_.emplace_back(rsSurfaceRenderNode1);
    mainThread->CheckSubThreadNodeStatusIsDoing(rsSurfaceRenderNode2->GetId());
}

/**
 * @tc.name: CollectInfoForHardwareComposer001
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CollectInfoForHardwareComposer001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->CollectInfoForHardwareComposer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: CollectInfoForHardwareComposer002
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CollectInfoForHardwareComposer002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    mainThread->CollectInfoForHardwareComposer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: IsLastFrameUIFirstEnbaled001
 * @tc.desc: IsLastFrameUIFirstEnbaled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, IsLastFrameUIFirstEnbaled001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->subThreadNodes_.clear();
    // not app window
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    mainThread->subThreadNodes_.push_back(node);
    mainThread->IsLastFrameUIFirstEnabled(0);
    // app window
    id = 2;
    node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    node->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    mainThread->subThreadNodes_.push_back(node);
    mainThread->IsLastFrameUIFirstEnabled(2);
}


/**
 * @tc.name: IsLastFrameUIFirstEnbaled002
 * @tc.desc: IsLastFrameUIFirstEnbaled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, IsLastFrameUIFirstEnbaled002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->subThreadNodes_.clear();
    // parent
    NodeId id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    node1->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    // child
    id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    node2->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node1->AddChild(node2);
    mainThread->subThreadNodes_.push_back(node1);
    mainThread->IsLastFrameUIFirstEnabled(2);
}

/**
 * @tc.name: CheckIfHardwareForcedDisabled
 * @tc.desc: CheckIfHardwareForcedDisabled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CheckIfHardwareForcedDisabled, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->CheckIfHardwareForcedDisabled();
}

/**
 * @tc.name: WaitUntilDisplayNodeBufferReleased
 * @tc.desc: WaitUntilDisplayNodeBufferReleased test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, WaitUntilDisplayNodeBufferReleased, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    sptr<IConsumerSurface> csurf = IConsumerSurface::Create();
    node->SetConsumer(csurf);
    mainThread->WaitUntilDisplayNodeBufferReleased(*node);
}


/**
 * @tc.name: WaitUntilUnmarshallingTaskFinished001
 * @tc.desc: WaitUntilUnmarshallingTaskFinished test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, WaitUntilUnmarshallingTaskFinished001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->WaitUntilUnmarshallingTaskFinished();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: MergeToEffectiveTransactionDataMap
 * @tc.desc: MergeToEffectiveTransactionDataMap test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, MergeToEffectiveTransactionDataMap, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    TransactionDataMap tsDataMap;
    tsDataMap.emplace(0, std::vector<std::unique_ptr<RSTransactionData>>());
    auto data = std::make_unique<RSTransactionData>();
    data->SetIndex(1);
    tsDataMap[0].emplace_back(std::move(data));
    mainThread->MergeToEffectiveTransactionDataMap(tsDataMap);
}

/**
 * @tc.name: UniRender001
 * @tc.desc: UniRender test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, UniRender001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    mainThread->renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    // prepare nodes
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    const std::shared_ptr<RSBaseRenderNode> rootNode = context->GetGlobalRootRenderNode();
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto childDisplayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    rootNode->AddChild(childDisplayNode, 0);
    rootNode->InitRenderParams();
    childDisplayNode->InitRenderParams();
    mainThread->UniRender(rootNode);
}

/**
 * @tc.name: UniRender002
 * @tc.desc: UniRender test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, UniRender002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    mainThread->renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    // prepare nodes
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    const std::shared_ptr<RSBaseRenderNode> rootNode = context->GetGlobalRootRenderNode();
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto childDisplayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    rootNode->AddChild(childDisplayNode, 0);
    // prepare status
    bool doDirectComposition = mainThread->doDirectComposition_;
    bool isDirty = mainThread->isDirty_;
    bool isAccessibilityConfigChanged = mainThread->isAccessibilityConfigChanged_;
    bool isCachedSurfaceUpdated = mainThread->isCachedSurfaceUpdated_;
    bool isHardwareEnabledBufferUpdated = mainThread->isHardwareEnabledBufferUpdated_;
    mainThread->doDirectComposition_ = true;
    mainThread->isDirty_ = false;
    mainThread->isAccessibilityConfigChanged_ = false;
    mainThread->isCachedSurfaceUpdated_ = false;
    mainThread->isHardwareEnabledBufferUpdated_ = true;
    rootNode->InitRenderParams();
    childDisplayNode->InitRenderParams();
    mainThread->UniRender(rootNode);
    // status recover
    mainThread->doDirectComposition_ = doDirectComposition;
    mainThread->isDirty_ = isDirty;
    mainThread->isAccessibilityConfigChanged_ = isAccessibilityConfigChanged;
    mainThread->isCachedSurfaceUpdated_ = isCachedSurfaceUpdated;
    mainThread->isHardwareEnabledBufferUpdated_ = isHardwareEnabledBufferUpdated;
}

/**
 * @tc.name: Render
 * @tc.desc: Render test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, Render, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    mainThread->renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    const std::shared_ptr<RSBaseRenderNode> rootNode = mainThread->context_->globalRootRenderNode_;
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto childDisplayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    rootNode->AddChild(childDisplayNode);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    // uni render
    mainThread->isUniRender_ = true;
    rootNode->InitRenderParams();
    childDisplayNode->InitRenderParams();
    mainThread->Render();
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: CallbackDrawContextStatusToWMS
 * @tc.desc: CallbackDrawContextStatusToWMS test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CallbackDrawContextStatusToWMS, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->curDrawStatusVec_.push_back(0);
    mainThread->curDrawStatusVec_.push_back(1);
    mainThread->lastDrawStatusMap_.insert({1, 0});
    mainThread->occlusionListeners_.insert({0, nullptr});
    mainThread->CallbackDrawContextStatusToWMS();
    mainThread->curDrawStatusVec_.clear();
    mainThread->lastDrawStatusMap_.clear();
    mainThread->occlusionListeners_.clear();
}

/**
 * @tc.name: CheckSurfaceNeedProcess
 * @tc.desc: CheckSurfaceNeedProcess test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CheckSurfaceNeedProcess, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    OcclusionRectISet occRectISet;
    // not focus node
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    mainThread->CheckSurfaceNeedProcess(occRectISet, node);
    // focus node
    id = mainThread->focusNodeId_;
    node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    mainThread->CheckSurfaceNeedProcess(occRectISet, node);
}

/**
 * @tc.name: GetRegionVisibleLevel001
 * @tc.desc: GetRegionVisibleLevel test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, GetRegionVisibleLevel001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // empty region
    Occlusion::Region occRegion;
    mainThread->GetRegionVisibleLevel(occRegion, occRegion);
}

/**
 * @tc.name: GetRegionVisibleLevel002
 * @tc.desc: GetRegionVisibleLevel test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, GetRegionVisibleLevel002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // equal region
    Occlusion::Region occRegion;
    occRegion.rects_.emplace_back(Occlusion::Rect(0, 1, 1, 0));
    mainThread->GetRegionVisibleLevel(occRegion, occRegion);
}

/**
 * @tc.name: GetRegionVisibleLevel003
 * @tc.desc: GetRegionVisibleLevel test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, GetRegionVisibleLevel003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    Occlusion::Region occRegion1;
    occRegion1.rects_.emplace_back(Occlusion::Rect(0, 1, 1, 0));
    Occlusion::Region occRegion2;
    occRegion2.rects_.emplace_back(Occlusion::Rect(0, 2, 2, 0));
    mainThread->GetRegionVisibleLevel(occRegion1, occRegion2);
}

/**
 * @tc.name: CalcOcclusionImplementation
 * @tc.desc: CalcOcclusionImplementation test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CalcOcclusionImplementation, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RSDisplayNodeConfig config;
    NodeId displayNodeId = 0;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayNodeId, config, mainThread->context_);

    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    NodeId surfaceNodeId = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId, mainThread->context_);
    curAllSurfaces.emplace_back(nullptr);
    curAllSurfaces.emplace_back(surfaceNode);
    VisibleData dstCurVisVec;
    std::map<NodeId, RSVisibleLevel> dstPidVisMap;
    mainThread->CalcOcclusionImplementation(displayNode, curAllSurfaces, dstCurVisVec, dstPidVisMap);
}

/**
 * @tc.name: CallbackToWMS001
 * @tc.desc: CallbackToWMS test visible not changed
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CallbackToWMS001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    VisibleData curVisVec;
    curVisVec.push_back({0, WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE});
    mainThread->lastVisVec_ = curVisVec;
    mainThread->CallbackToWMS(curVisVec);
    mainThread->lastVisVec_.clear();
}

/**
 * @tc.name: CallbackToWMS002
 * @tc.desc: CallbackToWMS test visible changed
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CallbackToWMS002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    VisibleData curVisVec;
    curVisVec.push_back({0, WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE});
    curVisVec.push_back({1, WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE});
    mainThread->lastVisVec_.push_back({0, WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE});
    mainThread->lastVisVec_.push_back({1, WINDOW_LAYER_INFO_TYPE::INVISIBLE});
    mainThread->occlusionListeners_.insert({0, nullptr});
    mainThread->CallbackToWMS(curVisVec);
    mainThread->occlusionListeners_.clear();
    mainThread->lastVisVec_.clear();
}

/**
 * @tc.name: SurfaceOcclusionCallback001
 * @tc.desc: SurfaceOcclusionCallback with empty nodemap
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SurfaceOcclusionCallback001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare listeners
    std::tuple<pid_t, sptr<RSISurfaceOcclusionChangeCallback>,
        std::vector<float>, uint8_t> info(0, nullptr, {}, 0);
    mainThread->surfaceOcclusionListeners_.insert({0, info});
    // run
    mainThread->SurfaceOcclusionCallback();
    mainThread->surfaceOcclusionListeners_.clear();
}

/**
 * @tc.name: SurfaceOcclusionCallback002
 * @tc.desc: SurfaceOcclusionCallback with corresponding nodemap
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SurfaceOcclusionCallback002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare listeners
    std::tuple<pid_t, sptr<RSISurfaceOcclusionChangeCallback>,
        std::vector<float>, uint8_t> info(0, nullptr, {}, 0);
    mainThread->surfaceOcclusionListeners_.insert({1, info});
    mainThread->surfaceOcclusionListeners_.insert({2, info});
    mainThread->surfaceOcclusionListeners_.insert({3, info});
    //prepare nodemap
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(config);
    node1->SetIsOnTheTree(true);
    config.id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    node2->SetIsOnTheTree(true);
    node2->instanceRootNodeId_ = INVALID_NODEID;
    config.id = 3;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(config);
    node3->SetIsOnTheTree(true);
    node3->instanceRootNodeId_ = 1;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node1);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);
    // run
    mainThread->SurfaceOcclusionCallback();
    mainThread->surfaceOcclusionListeners_.clear();
}

/**
 * @tc.name: CalcOcclusion002
 * @tc.desc: CalcOcclusion Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CalcOcclusion002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare states
    bool doWindowAnimate = mainThread->doWindowAnimate_;
    mainThread->doWindowAnimate_ = false;
    auto globalRootRenderNode = mainThread->context_->globalRootRenderNode_;
    // run with nullptr
    mainThread->context_->globalRootRenderNode_ = nullptr;
    mainThread->CalcOcclusion();
    // run with one child node
    auto node1 = std::make_shared<RSRenderNode>(0, true);
    auto node2 = std::make_shared<RSRenderNode>(1, true);
    node1->AddChild(node2);
    mainThread->context_->globalRootRenderNode_ = node1;
    mainThread->CalcOcclusion();
    // run with more than one node
    auto node3 = std::make_shared<RSRenderNode>(0, true);
    RSDisplayNodeConfig config1;
    auto node4 = std::make_shared<RSDisplayRenderNode>(1, config1);
    RSSurfaceRenderNodeConfig config2;
    auto node5 = std::make_shared<RSSurfaceRenderNode>(config2);
    node3->AddChild(node4);
    node4->curAllSurfaces_.push_back(nullptr);
    node4->curAllSurfaces_.push_back(node5);
    mainThread->context_->globalRootRenderNode_ = node3;
    mainThread->CalcOcclusion();
    // recover
    mainThread->doWindowAnimate_ = doWindowAnimate;
    mainThread->context_->globalRootRenderNode_ = globalRootRenderNode;
}

/**
 * @tc.name: Aniamte
 * @tc.desc: Aniamte Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, Aniamte, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 0;
    auto node = std::make_shared<RSRenderNode>(id);
    mainThread->context_->RegisterAnimatingRenderNode(node);
    mainThread->Animate(mainThread->timestamp_);
}

/**
 * @tc.name: RecvRSTransactionData002
 * @tc.desc: RecvRSTransactionData002 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, RecvRSTransactionData002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    auto transactionData1 = std::make_unique<RSTransactionData>();
    mainThread->isUniRender_ = true;
    mainThread->RecvRSTransactionData(transactionData1);
    auto transactionData2 = std::make_unique<RSTransactionData>();
    mainThread->isUniRender_ = false;
    mainThread->RecvRSTransactionData(transactionData2);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ConsumeAndUpdateAllNodes003
 * @tc.desc: ConsumeAndUpdateAllNodes003 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ConsumeAndUpdateAllNodes003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    // prepare nodemap
    mainThread->context_->GetMutableNodeMap().renderNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.emplace(1, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 2;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(config);
    node1->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node1);
    config.id = 3;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    node2->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);
    config.id = 4;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(config);
    node3->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node3->isHardwareEnabledNode_ = true;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);
    mainThread->ConsumeAndUpdateAllNodes();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: CollectInfoForHardwareComposer003
 * @tc.desc: CollectInfoForHardwareComposer003 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CollectInfoForHardwareComposer003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    // prepare nodemap
    mainThread->context_->GetMutableNodeMap().renderNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.emplace(1, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);
    config.id = 3;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(config);
    node3->SetIsOnTheTree(true);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);
    auto node4 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node4->SetIsOnTheTree(true);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node4);
    auto node5 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node5->SetIsOnTheTree(true);
    node5->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node5->isHardwareEnabledNode_ = true;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node5);
    mainThread->CollectInfoForHardwareComposer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ClassifyRSTransactionData005
 * @tc.desc: ClassifyRSTransactionData005 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ClassifyRSTransactionData005, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    // prepare nodemap
    mainThread->context_->GetMutableNodeMap().renderNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.emplace(1, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);
    // prepare transactionData
    auto data = std::make_unique<RSTransactionData>();
    int dataIndex = 1;
    data->SetIndex(dataIndex);
    int dataPayloadSize = 3;
    data->payload_.resize(dataPayloadSize);
    NodeId id = 0;
    data->payload_[id] = std::tuple<NodeId,
        FollowType, std::unique_ptr<RSCommand>>(id, FollowType::NONE, nullptr);
    id = 1;
    data->payload_[id] = std::tuple<NodeId,
        FollowType, std::unique_ptr<RSCommand>>(id, FollowType::FOLLOW_TO_SELF, nullptr);
    id = 2;
    data->payload_[id] = std::tuple<NodeId,
        FollowType, std::unique_ptr<RSCommand>>(id, FollowType::FOLLOW_TO_PARENT, nullptr);
    mainThread->ClassifyRSTransactionData(data);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ReleaseAllNodesBuffer
 * @tc.desc: ReleaseAllNodesBuffer Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ReleaseAllNodesBuffer, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    // prepare nodemap
    mainThread->context_->GetMutableNodeMap().renderNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.emplace(1, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);
    config.id = 3;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(config);
    node3->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node3->isHardwareEnabledNode_ = true;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);
    config.id = 4;
    auto node4 = std::make_shared<RSSurfaceRenderNode>(config);
    node4->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node4->isHardwareEnabledNode_ = true;
    node4->isLastFrameHardwareEnabled_ = true;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node4);
    config.id = 5;
    auto node5 = std::make_shared<RSSurfaceRenderNode>(config);
    node5->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node5->isHardwareEnabledNode_ = true;
    node5->isLastFrameHardwareEnabled_ = true;
    node5->isCurrentFrameHardwareEnabled_ = false;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node5);
    mainThread->ReleaseAllNodesBuffer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: PostTask001
 * @tc.desc: PostTask Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, PostTask001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    RSTaskMessage::RSTask task = []() -> void { return; };
    mainThread->PostTask(task);
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
}

/**
 * @tc.name: PostTask002
 * @tc.desc: PostTask Test with IMMEDIATE Priority
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, PostTask002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    RSTaskMessage::RSTask task = []() -> void { return; };
    std::string name = "test";
    int64_t delayTime = 0;
    AppExecFwk::EventQueue::Priority priority = AppExecFwk::EventQueue::Priority::IMMEDIATE;
    mainThread->PostTask(task, name, delayTime, priority);
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
}

/**
 * @tc.name: RemoveTask
 * @tc.desc: RemoveTask Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, RemoveTask, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    std::string name = "test";
    mainThread->RemoveTask(name);
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
}

/**
 * @tc.name: PostSyncTask002
 * @tc.desc: PostSyncTask Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, PostSyncTask002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    RSTaskMessage::RSTask task = []() -> void { return; };
    mainThread->PostSyncTask(task);
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
}

/**
 * @tc.name: RegisterSurfaceOcclusionChangeCallBack001
 * @tc.desc: RegisterSurfaceOcclusionChangeCallBack001 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, RegisterSurfaceOcclusionChangeCallBack001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 0;
    pid_t pid = 0;
    sptr<RSISurfaceOcclusionChangeCallback> callback = nullptr;
    std::vector<float> partitionPoints = {};
    mainThread->RegisterSurfaceOcclusionChangeCallback(id, pid, callback, partitionPoints);
    ASSERT_NE(mainThread->surfaceOcclusionListeners_.size(), 0);
    mainThread->surfaceOcclusionListeners_.clear();
}

/**
 * @tc.name: RegisterSurfaceOcclusionChangeCallBack002
 * @tc.desc: RegisterSurfaceOcclusionChangeCallBack002 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, RegisterSurfaceOcclusionChangeCallBack002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 0;
    pid_t pid = 0;
    sptr<RSISurfaceOcclusionChangeCallback> callback = nullptr;
    std::vector<float> partitionPoints = {1.0f};
    mainThread->RegisterSurfaceOcclusionChangeCallback(id, pid, callback, partitionPoints);
    ASSERT_NE(mainThread->surfaceOcclusionListeners_.size(), 0);
    mainThread->surfaceOcclusionListeners_.clear();
}

/**
 * @tc.name: ClearSurfaceOcclusionChangeCallBack
 * @tc.desc: RegisterSurfaceOcclusionChangeCallBack Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ClearSurfaceOcclusionChangeCallback, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare listeners
    pid_t pid = 1;
    int level = 0;
    NodeId id = 1;
    std::tuple<pid_t, sptr<RSISurfaceOcclusionChangeCallback>,
        std::vector<float>, uint8_t> info1(pid, nullptr, {}, level);
    mainThread->surfaceOcclusionListeners_.insert({id, info1});
    id = 2;
    pid = 2;
    std::tuple<pid_t, sptr<RSISurfaceOcclusionChangeCallback>,
        std::vector<float>, uint8_t> info2(pid, nullptr, {}, level);
    mainThread->surfaceOcclusionListeners_.insert({id, info2});
    mainThread->ClearSurfaceOcclusionChangeCallback(pid);
    mainThread->surfaceOcclusionListeners_.clear();
}

/**
 * @tc.name: SendCommands
 * @tc.desc: SendCommands Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SendCommands, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SendCommands();
}

/**
 * @tc.name: ClearTransactionDataPidInfo001
 * @tc.desc: ClearTransactionDataPidInfo Test, remotePid = 0
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ClearTransactionDataPidInfo001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    int remotePid = 0;
    mainThread->ClearTransactionDataPidInfo(remotePid);
}

/**
 * @tc.name: ClearTransactionDataPidInfo002
 * @tc.desc: ClearTransactionDataPidInfo Test, remotePid > 0
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ClearTransactionDataPidInfo002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    int remotePid = 1;
    mainThread->ClearTransactionDataPidInfo(remotePid);
}

/**
 * @tc.name: AddTransactionDataPidInfo001
 * @tc.desc: AddTransactionDataPidInfo Test, no UniRender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, AddTransactionDataPidInfo001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    pid_t pid = 0;
    mainThread->AddTransactionDataPidInfo(pid);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: AddTransactionDataPidInfo002
 * @tc.desc: AddTransactionDataPidInfo Test, UniRender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, AddTransactionDataPidInfo002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    pid_t pid = 0;
    mainThread->AddTransactionDataPidInfo(pid);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: PerfAfterAnim001
 * @tc.desc: PerfAfterAnim Test, not UniRender, needRequestNextVsync
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, PerfAfterAnim001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    bool needRequestNextVsync = true;
    mainThread->PerfAfterAnim(needRequestNextVsync);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: PerfAfterAnim002
 * @tc.desc: PerfAfterAnim Test, UniRender, needRequestNextVsync
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, PerfAfterAnim002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    bool needRequestNextVsync = true;
    mainThread->PerfAfterAnim(needRequestNextVsync);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: PerfAfterAnim003
 * @tc.desc: PerfAfterAnim Test, UniRender, not needRequestNextVsync
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, PerfAfterAnim003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    bool needRequestNextVsync = false;
    mainThread->PerfAfterAnim(needRequestNextVsync);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ForceRefreshForUni001
 * @tc.desc: ForceRefreshForUni Test, UniRender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ForceRefreshForUni001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    mainThread->ForceRefreshForUni();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ForceRefreshForUni002
 * @tc.desc: ForceRefreshForUni Test, without UniRender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ForceRefreshForUni002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->ForceRefreshForUni();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: PerfForBlurIfNeeded
 * @tc.desc: PerfForBlurIfNeeded Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, PerfForBlurIfNeeded, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    mainThread->PerfForBlurIfNeeded();
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
}

/**
 * @tc.name: PerfMultiWindow001
 * @tc.desc: PerfMultiWindow Test, not unirender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, PerfMultiWindow001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->PerfMultiWindow();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: PerfMultiWindow002
 * @tc.desc: PerfMultiWindow Test, unirender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, PerfMultiWindow002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    auto appWindowNum = mainThread->appWindowNum_;
    mainThread->appWindowNum_ = MULTI_WINDOW_PERF_START_NUM - 1;
    mainThread->PerfMultiWindow();
    mainThread->appWindowNum_ = MULTI_WINDOW_PERF_START_NUM;
    mainThread->PerfMultiWindow();
    mainThread->appWindowNum_ = MULTI_WINDOW_PERF_END_NUM + 1;
    mainThread->PerfMultiWindow();
    mainThread->isUniRender_ = isUniRender;
    mainThread->appWindowNum_ = appWindowNum;
}

/**
 * @tc.name: RenderFrameStart
 * @tc.desc: RenderFrameStart Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, RenderFrameStart, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->RenderFrameStart(mainThread->timestamp_);
}

/**
 * @tc.name: SetSystemAnimatedScenes001
 * @tc.desc: SetSystemAnimatedScenes Test, case set 1
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetSystemAnimatedScenes001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    SystemAnimatedScenes scenesInit = mainThread->systemAnimatedScenes_;
    SystemAnimatedScenes scenes = SystemAnimatedScenes::ENTER_MISSION_CENTER;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::EXIT_MISSION_CENTER;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::ENTER_TFS_WINDOW;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::EXIT_TFU_WINDOW;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::ENTER_WINDOW_FULL_SCREEN;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::EXIT_WINDOW_FULL_SCREEN;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::ENTER_MAX_WINDOW;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::EXIT_MAX_WINDOW;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    mainThread->systemAnimatedScenes_ = scenesInit;
}

/**
 * @tc.name: SetSystemAnimatedScenes002
 * @tc.desc: SetSystemAnimatedScenes Test, case set 2
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetSystemAnimatedScenes002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    SystemAnimatedScenes scenesInit = mainThread->systemAnimatedScenes_;
    SystemAnimatedScenes scenes = SystemAnimatedScenes::ENTER_SPLIT_SCREEN;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::EXIT_SPLIT_SCREEN;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::ENTER_APP_CENTER;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::EXIT_APP_CENTER;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::APPEAR_MISSION_CENTER;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::ENTER_WIND_CLEAR;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::ENTER_WIND_RECOVER;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::OTHERS;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    mainThread->systemAnimatedScenes_ = scenesInit;
}

/**
 * @tc.name: SetSystemAnimatedScenes003
 * @tc.desc: SetSystemAnimatedScenes Test, System Animated Scenes Disabled
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetSystemAnimatedScenes003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto systemAnimatedScenesEnabled = mainThread->systemAnimatedScenesEnabled_;
    mainThread->systemAnimatedScenesEnabled_ = false;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::OTHERS));
    mainThread->systemAnimatedScenesEnabled_ = systemAnimatedScenesEnabled;
}

/**
 * @tc.name: CheckNodeHasToBePreparedByPid001
 * @tc.desc: CheckNodeHasToBePreparedByPid Test, Classify By Root
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CheckNodeHasToBePreparedByPid001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 1;
    bool isClassifyByRoot = true;
    mainThread->CheckNodeHasToBePreparedByPid(id, isClassifyByRoot);
}

/**
 * @tc.name: CheckNodeHasToBePreparedByPid002
 * @tc.desc: CheckNodeHasToBePreparedByPid Test, not Classify By Root
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CheckNodeHasToBePreparedByPid002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 1;
    bool isClassifyByRoot = false;
    mainThread->CheckNodeHasToBePreparedByPid(id, isClassifyByRoot);
}

/**
 * @tc.name: SetVSyncRateByVisibleLevel001
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when RSVisibleLevel is RS_SEMI_DEFAULT_VISIBLE
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetVSyncRateByVisibleLevel001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    if (vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid) == VSYNC_ERROR_OK) {
        vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
        std::map<NodeId, RSVisibleLevel> pidVisMap;
        pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE;
        std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
        mainThread->lastVisMapForVsyncRate_.clear();
        mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
        ASSERT_NE(connection->highPriorityRate_, (int32_t)SIMI_VISIBLE_RATE);
    }
}

/**
 * @tc.name: SetVSyncRateByVisibleLevel002
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when RSVisibleLevel is RS_SYSTEM_ANIMATE_SCENE
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetVSyncRateByVisibleLevel002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    if (vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid) == VSYNC_ERROR_OK) {
        vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
        std::map<NodeId, RSVisibleLevel> pidVisMap;
        pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_SYSTEM_ANIMATE_SCENE;
        std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
        mainThread->lastVisMapForVsyncRate_.clear();
        mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
        ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SECNES_RATE);
    }
}

/**
 * @tc.name: SetVSyncRateByVisibleLevel003
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when RSVisibleLevel is RS_INVISIBLE
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetVSyncRateByVisibleLevel003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    if (vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid) == VSYNC_ERROR_OK) {
        vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
        std::map<NodeId, RSVisibleLevel> pidVisMap;
        pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_INVISIBLE;
        std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
        mainThread->lastVisMapForVsyncRate_.clear();
        mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
        ASSERT_NE(connection->highPriorityRate_, (int32_t)INVISBLE_WINDOW_RATE);
    }
}

/**
 * @tc.name: SetVSyncRateByVisibleLevel004
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when RSVisibleLevel is RS_UNKNOW_VISIBLE_LEVEL
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetVSyncRateByVisibleLevel004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    if (vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid) == VSYNC_ERROR_OK) {
        vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
        std::map<NodeId, RSVisibleLevel> pidVisMap;
        pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL;
        std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
        mainThread->lastVisMapForVsyncRate_.clear();
        mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
        ASSERT_NE(connection->highPriorityRate_, (int32_t)DEFAULT_RATE);
    }
}

/**
 * @tc.name: SetSystemAnimatedScenes004
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when SystemAnimatedScenes is ENTER_MISSION_CENTER
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetSystemAnimatedScenes004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_MISSION_CENTER);
    ASSERT_NE(mainThread->systemAnimatedScenesList_.empty(), true);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    ASSERT_EQ(vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid), VSYNC_ERROR_OK);
    vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
    std::map<NodeId, RSVisibleLevel> pidVisMap;
    pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    mainThread->lastVisMapForVsyncRate_.clear();
    mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SECNES_RATE);
}

/**
 * @tc.name: SetSystemAnimatedScenes005
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when SystemAnimatedScenes is ENTER_TFS_WINDOW
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetSystemAnimatedScenes005, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_TFS_WINDOW);
    ASSERT_NE(mainThread->systemAnimatedScenesList_.empty(), true);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    ASSERT_EQ(vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid), VSYNC_ERROR_OK);
    vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
    std::map<NodeId, RSVisibleLevel> pidVisMap;
    pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    mainThread->lastVisMapForVsyncRate_.clear();
    mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SECNES_RATE);
}

/**
 * @tc.name: SetSystemAnimatedScenes006
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when SystemAnimatedScenes is ENTER_WINDOW_FULL_SCREEN
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetSystemAnimatedScenes006, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_WINDOW_FULL_SCREEN);
    ASSERT_NE(mainThread->systemAnimatedScenesList_.empty(), true);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    ASSERT_EQ(vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid), VSYNC_ERROR_OK);
    vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
    std::map<NodeId, RSVisibleLevel> pidVisMap;
    pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    mainThread->lastVisMapForVsyncRate_.clear();
    mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SECNES_RATE);
}

/**
 * @tc.name: SetSystemAnimatedScenes007
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when SystemAnimatedScenes is ENTER_MAX_WINDOW
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetSystemAnimatedScenes007, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_MAX_WINDOW);
    ASSERT_NE(mainThread->systemAnimatedScenesList_.empty(), true);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    ASSERT_EQ(vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid), VSYNC_ERROR_OK);
    vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
    std::map<NodeId, RSVisibleLevel> pidVisMap;
    pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    mainThread->lastVisMapForVsyncRate_.clear();
    mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SECNES_RATE);
}

/**
 * @tc.name: SetSystemAnimatedScenes008
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when SystemAnimatedScenes is ENTER_SPLIT_SCREEN
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetSystemAnimatedScenes008, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_SPLIT_SCREEN);
    ASSERT_NE(mainThread->systemAnimatedScenesList_.empty(), true);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    ASSERT_EQ(vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid), VSYNC_ERROR_OK);
    vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
    std::map<NodeId, RSVisibleLevel> pidVisMap;
    pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    mainThread->lastVisMapForVsyncRate_.clear();
    mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SECNES_RATE);
}

/**
 * @tc.name: SetSystemAnimatedScenes009
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when SystemAnimatedScenes is ENTER_APP_CENTER
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetSystemAnimatedScenes009, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_APP_CENTER);
    ASSERT_NE(mainThread->systemAnimatedScenesList_.empty(), true);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    ASSERT_EQ(vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid), VSYNC_ERROR_OK);
    vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
    std::map<NodeId, RSVisibleLevel> pidVisMap;
    pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    mainThread->lastVisMapForVsyncRate_.clear();
    mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SECNES_RATE);
}

/**
 * @tc.name: IsDrawingGroupChanged
 * @tc.desc: IsDrawingGroupChanged Test, not Classify By Root
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, IsDrawingGroupChanged, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 1;
    auto node = std::make_shared<RSRenderNode>(id);
    mainThread->IsDrawingGroupChanged(*node);
}

/**
 * @tc.name: CheckAndUpdateInstanceContentStaticStatus003
 * @tc.desc: CheckAndUpdateInstanceContentStaticStatus Test, nullptr
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CheckAndUpdateInstanceContentStaticStatus003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->CheckAndUpdateInstanceContentStaticStatus(nullptr);
}

/**
 * @tc.name: UpdateRogSizeIfNeeded
 * @tc.desc: UpdateRogSizeIfNeeded Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, UpdateRogSizeIfNeeded, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare context
    auto contextInit = mainThread->context_;
    auto context = std::make_shared<RSContext>();
    auto rootNode = context->GetGlobalRootRenderNode();
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto childDisplayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    rootNode->AddChild(childDisplayNode);
    mainThread->context_ = context;
    mainThread->UpdateRogSizeIfNeeded();
    mainThread->context_ = contextInit;
}

/**
 * @tc.name: UpdateUIFirstSwitch001
 * @tc.desc: UpdateUIFirstSwitch Test, root node nullptr
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, UpdateUIFirstSwitch001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    mainThread->context_->globalRootRenderNode_ = nullptr;
    mainThread->UpdateUIFirstSwitch();
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: UpdateUIFirstSwitch002
 * @tc.desc: UpdateUIFirstSwitch Test, with surfacenode child
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, UpdateUIFirstSwitch002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    // one child
    NodeId id = 1;
    auto node1 = std::make_shared<RSRenderNode>(id);
    id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id);
    node1->AddChild(node2);
    mainThread->context_->globalRootRenderNode_ = node1;
    mainThread->UpdateUIFirstSwitch();
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: UpdateUIFirstSwitch003
 * @tc.desc: UpdateUIFirstSwitch Test, with displaynode child
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, UpdateUIFirstSwitch003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    // one child
    NodeId id = 1;
    auto node1 = std::make_shared<RSRenderNode>(id);
    id = 2;
    RSDisplayNodeConfig config;
    auto node2 = std::make_shared<RSDisplayRenderNode>(id, config);
    node1->AddChild(node2);
    mainThread->context_->globalRootRenderNode_ = node1;
    mainThread->UpdateUIFirstSwitch();
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: ReleaseSurface
 * @tc.desc: ReleaseSurface Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ReleaseSurface, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->tmpSurfaces_.push(nullptr);
    mainThread->ReleaseSurface();
}

/**
 * @tc.name: SetCurtainScreenUsingStatus
 * @tc.desc: SetCurtainScreenUsingStatus Test
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSMainThreadTest, SetCurtainScreenUsingStatus, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetCurtainScreenUsingStatus(true);
    ASSERT_EQ(mainThread->IsCurtainScreenOn(), true);

    // restore curtain screen status
    mainThread->SetCurtainScreenUsingStatus(false);
}

/**
 * @tc.name: CalcOcclusionImplementation001
 * @tc.desc: calculate occlusion when surfaces do not overlap
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, CalcOcclusionImplementation001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;

    NodeId idBottom = 0;
    auto nodeBottom = std::make_shared<RSSurfaceRenderNode>(idBottom, mainThread->context_);
    RectI rectBottom = RectI(0, 0, 100, 100);
    nodeBottom->oldDirtyInSurface_ = rectBottom;
    nodeBottom->SetDstRect(rectBottom);
    nodeBottom->opaqueRegion_ = Occlusion::Region(rectBottom);

    NodeId idTop = 1;
    auto nodeTop = std::make_shared<RSSurfaceRenderNode>(idTop, mainThread->context_);
    RectI rectTop = RectI(100, 100, 100, 100);
    nodeTop->oldDirtyInSurface_ = rectTop;
    nodeTop->SetDstRect(rectTop);
    nodeTop->opaqueRegion_ = Occlusion::Region(rectTop);
    
    curAllSurfaces.emplace_back(nodeBottom);
    curAllSurfaces.emplace_back(nodeTop);
    VisibleData dstCurVisVec;
    std::map<NodeId, RSVisibleLevel> dstPidVisMap;
    mainThread->CalcOcclusionImplementation(nullptr, curAllSurfaces, dstCurVisVec, dstPidVisMap);
    ASSERT_EQ(nodeBottom->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeTop->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeBottom->GetVisibleRegion().Size(), 1);
    ASSERT_EQ(nodeTop->GetVisibleRegion().Size(), 1);
}

/**
 * @tc.name: CalcOcclusionImplementation002
 * @tc.desc: calculate occlusion when surfaces partially overlap
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, CalcOcclusionImplementation002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;

    NodeId idBottom = 0;
    auto nodeBottom = std::make_shared<RSSurfaceRenderNode>(idBottom, mainThread->context_);
    RectI rectBottom = RectI(0, 0, 100, 100);
    nodeBottom->oldDirtyInSurface_ = rectBottom;
    nodeBottom->SetDstRect(rectBottom);
    nodeBottom->opaqueRegion_ = Occlusion::Region(rectBottom);

    NodeId idTop = 1;
    auto nodeTop = std::make_shared<RSSurfaceRenderNode>(idTop, mainThread->context_);
    RectI rectTop = RectI(50, 50, 100, 100);
    nodeTop->oldDirtyInSurface_ = rectTop;
    nodeTop->SetDstRect(rectTop);
    nodeTop->opaqueRegion_ = Occlusion::Region(rectTop);
    
    curAllSurfaces.emplace_back(nodeBottom);
    curAllSurfaces.emplace_back(nodeTop);
    VisibleData dstCurVisVec;
    std::map<NodeId, RSVisibleLevel> dstPidVisMap;
    mainThread->CalcOcclusionImplementation(nullptr, curAllSurfaces, dstCurVisVec, dstPidVisMap);
    ASSERT_EQ(nodeBottom->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeTop->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeBottom->GetVisibleRegion().Size(), 2);
    ASSERT_EQ(nodeTop->GetVisibleRegion().Size(), 1);
}

/**
 * @tc.name: CalcOcclusionImplementation003
 * @tc.desc: calculate occlusion when the bottom node is occluded completely
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, CalcOcclusionImplementation003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;

    NodeId idBottom = 0;
    auto nodeBottom = std::make_shared<RSSurfaceRenderNode>(idBottom, mainThread->context_);
    RectI rectBottom = RectI(0, 0, 100, 100);
    nodeBottom->oldDirtyInSurface_ = rectBottom;
    nodeBottom->SetDstRect(rectBottom);
    nodeBottom->opaqueRegion_ = Occlusion::Region(rectBottom);

    NodeId idTop = 1;
    auto nodeTop = std::make_shared<RSSurfaceRenderNode>(idTop, mainThread->context_);
    RectI rectTop = RectI(0, 0, 100, 100);
    nodeTop->oldDirtyInSurface_ = rectTop;
    nodeTop->SetDstRect(rectTop);
    nodeTop->opaqueRegion_ = Occlusion::Region(rectTop);
    
    curAllSurfaces.emplace_back(nodeBottom);
    curAllSurfaces.emplace_back(nodeTop);
    VisibleData dstCurVisVec;
    std::map<NodeId, RSVisibleLevel> dstPidVisMap;
    mainThread->CalcOcclusionImplementation(nullptr, curAllSurfaces, dstCurVisVec, dstPidVisMap);
    ASSERT_EQ(nodeBottom->GetOcclusionVisible(), false);
    ASSERT_EQ(nodeTop->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeBottom->GetVisibleRegion().Size(), 0);
    ASSERT_EQ(nodeTop->GetVisibleRegion().Size(), 1);
}

/**
 * @tc.name: CalcOcclusionImplementation004
 * @tc.desc: calculate occlusion when the bottom node is occluded completely, and the top node is transparent
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, CalcOcclusionImplementation004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;

    NodeId idBottom = 0;
    auto nodeBottom = std::make_shared<RSSurfaceRenderNode>(idBottom, mainThread->context_);
    RectI rectBottom = RectI(0, 0, 100, 100);
    nodeBottom->oldDirtyInSurface_ = rectBottom;
    nodeBottom->SetDstRect(rectBottom);
    nodeBottom->opaqueRegion_ = Occlusion::Region(rectBottom);

    NodeId idTop = 1;
    auto nodeTop = std::make_shared<RSSurfaceRenderNode>(idTop, mainThread->context_);
    RectI rectTop = RectI(0, 0, 100, 100);
    nodeTop->oldDirtyInSurface_ = rectTop;
    nodeTop->SetDstRect(rectTop);
    // The top node is transparent
    nodeTop->SetGlobalAlpha(0.0f);
    
    curAllSurfaces.emplace_back(nodeBottom);
    curAllSurfaces.emplace_back(nodeTop);
    VisibleData dstCurVisVec;
    std::map<NodeId, RSVisibleLevel> dstPidVisMap;
    mainThread->CalcOcclusionImplementation(nullptr, curAllSurfaces, dstCurVisVec, dstPidVisMap);
    ASSERT_EQ(nodeBottom->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeTop->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeBottom->GetVisibleRegion().Size(), 1);
    ASSERT_EQ(nodeTop->GetVisibleRegion().Size(), 1);
}

/**
 * @tc.name: CalcOcclusionImplementation005
 * @tc.desc: calculate occlusion when the bottom node is occluded completely,
 * And the top node is transparent and filter cache valid
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, CalcOcclusionImplementation005, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;

    NodeId idBottom = 0;
    auto nodeBottom = std::make_shared<RSSurfaceRenderNode>(idBottom, mainThread->context_);
    RectI rectBottom = RectI(0, 0, 100, 100);
    nodeBottom->oldDirtyInSurface_ = rectBottom;
    nodeBottom->SetDstRect(rectBottom);
    nodeBottom->opaqueRegion_ = Occlusion::Region(rectBottom);

    NodeId idTop = 1;
    auto nodeTop = std::make_shared<RSSurfaceRenderNode>(idTop, mainThread->context_);
    RectI rectTop = RectI(0, 0, 100, 100);
    nodeTop->oldDirtyInSurface_ = rectTop;
    nodeTop->SetDstRect(rectTop);
    // The top node is transparent
    nodeTop->SetGlobalAlpha(0.0f);
    nodeTop->isFilterCacheValidForOcclusion_ = true;
    
    curAllSurfaces.emplace_back(nodeBottom);
    curAllSurfaces.emplace_back(nodeTop);
    VisibleData dstCurVisVec;
    std::map<NodeId, RSVisibleLevel> dstPidVisMap;
    mainThread->CalcOcclusionImplementation(nullptr, curAllSurfaces, dstCurVisVec, dstPidVisMap);
    ASSERT_EQ(nodeBottom->GetOcclusionVisible(), false);
    ASSERT_EQ(nodeTop->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeBottom->GetVisibleRegion().Size(), 0);
    ASSERT_EQ(nodeTop->GetVisibleRegion().Size(), 1);
}

/**
 * @tc.name: UpdateDisplayNodeScreenId001
 * @tc.desc: UpdateDisplayNodeScreenId, when rootnode is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, UpdateDisplayNodeScreenId001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->globalRootRenderNode_ = nullptr;
    mainThread->UpdateDisplayNodeScreenId();
    ASSERT_EQ(mainThread->displayNodeScreenId_, DEFAULT_DISPLAY_SCREEN_ID);
}

/**
 * @tc.name: UpdateDisplayNodeScreenId002
 * @tc.desc: UpdateDisplayNodeScreenId, root node has no child display node.
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, UpdateDisplayNodeScreenId002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    NodeId id = 1;
    mainThread->context_->globalRootRenderNode_ = std::make_shared<RSRenderNode>(id);
    mainThread->UpdateDisplayNodeScreenId();
    ASSERT_EQ(mainThread->displayNodeScreenId_, DEFAULT_DISPLAY_SCREEN_ID);
}

/**
 * @tc.name: UpdateDisplayNodeScreenId003
 * @tc.desc: UpdateDisplayNodeScreenId, root node has one child display node.
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, UpdateDisplayNodeScreenId003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId rootId = 0;
    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->globalRootRenderNode_ = std::make_shared<RSRenderNode>(rootId);
    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);
    uint64_t screenId = 1;
    displayNode->SetScreenId(screenId);
    mainThread->context_->globalRootRenderNode_->AddChild(displayNode);
    ASSERT_FALSE(mainThread->context_->globalRootRenderNode_->children_.empty());
    mainThread->UpdateDisplayNodeScreenId();
}

/**
 * @tc.name: ProcessScreenHotPlugEvents
 * @tc.desc: Test ProcessScreenHotPlugEvents
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, ProcessScreenHotPlugEvents, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->ProcessScreenHotPlugEvents();
}

/**
 * @tc.name: CheckSurfaceVisChanged001
 * @tc.desc: Test CheckSurfaceVisChanged, systemAnimatedScenesList is empty
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, CheckSurfaceVisChanged001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::map<NodeId, RSVisibleLevel> visMapForVsyncRate;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    mainThread->systemAnimatedScenesList_.clear();
    mainThread->CheckSurfaceVisChanged(visMapForVsyncRate, curAllSurfaces);
}

/**
 * @tc.name: CheckSystemSceneStatus001
 * @tc.desc: Test CheckSystemSceneStatus, APPEAR_MISSION_CENTER
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, CheckSystemSceneStatus001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::APPEAR_MISSION_CENTER);
    mainThread->CheckSystemSceneStatus();
}

/**
 * @tc.name: CheckSystemSceneStatus002
 * @tc.desc: Test CheckSystemSceneStatus, ENTER_TFS_WINDOW
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, CheckSystemSceneStatus002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_TFS_WINDOW);
    mainThread->CheckSystemSceneStatus();
}

/**
 * @tc.name: DoDirectComposition
 * @tc.desc: Test DoDirectComposition
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, DoDirectComposition, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId rootId = 0;
    auto rootNode = std::make_shared<RSBaseRenderNode>(rootId);
    NodeId displayId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);
    rootNode->AddChild(displayNode);
    mainThread->DoDirectComposition(rootNode, false);
}

/**
 * @tc.name: UpdateNeedDrawFocusChange001
 * @tc.desc: test UpdateNeedDrawFocusChange while node don't has parent
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSMainThreadTest, UpdateNeedDrawFocusChange001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node, nullptr);

    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->nodeMap.renderNodeMap_[node->GetId()] = node;
    mainThread->UpdateNeedDrawFocusChange(id);
    ASSERT_TRUE(node->GetNeedDrawFocusChange());
}

/**
 * @tc.name: UpdateNeedDrawFocusChange002
 * @tc.desc: test UpdateNeedDrawFocusChange while node's parent isn't leash window
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSMainThreadTest, UpdateNeedDrawFocusChange002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id + 1, mainThread->context_);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(parentNode, nullptr);
    parentNode->AddChild(node);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->nodeMap.renderNodeMap_[node->GetId()] = node;
    mainThread->context_->nodeMap.renderNodeMap_[parentNode->GetId()] = parentNode;
    mainThread->UpdateNeedDrawFocusChange(id);
    ASSERT_TRUE(node->GetNeedDrawFocusChange());
}

/**
 * @tc.name: UpdateNeedDrawFocusChange003
 * @tc.desc: test UpdateNeedDrawFocusChange while node's parent is leash window
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSMainThreadTest, UpdateNeedDrawFocusChange003, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id + 1, mainThread->context_);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(parentNode, nullptr);
    parentNode->AddChild(node);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);

    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->nodeMap.renderNodeMap_[node->GetId()] = node;
    mainThread->context_->nodeMap.renderNodeMap_[parentNode->GetId()] = parentNode;
    mainThread->UpdateNeedDrawFocusChange(id);
    ASSERT_FALSE(node->GetNeedDrawFocusChange());
}

/**
 * @tc.name: UpdateFocusNodeId001
 * @tc.desc: test UpdateFocusNodeId while focusNodeId don't change
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSMainThreadTest, UpdateFocusNodeId001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node, nullptr);

    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->nodeMap.renderNodeMap_[node->GetId()] = node;
    mainThread->focusNodeId_ = id;
    mainThread->UpdateFocusNodeId(id);
    ASSERT_EQ(mainThread->GetFocusNodeId(), id);
}

/**
 * @tc.name: UpdateFocusNodeId002
 * @tc.desc: test UpdateFocusNodeId while newfocusNodeId is invalid
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSMainThreadTest, UpdateFocusNodeId002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node, nullptr);

    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->nodeMap.renderNodeMap_[node->GetId()] = node;
    mainThread->focusNodeId_ = id;
    mainThread->UpdateFocusNodeId(INVALID_NODEID);
    ASSERT_EQ(mainThread->GetFocusNodeId(), id);
}

/**
 * @tc.name: UpdateFocusNodeId003
 * @tc.desc: test UpdateFocusNodeId while focus node change
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSMainThreadTest, UpdateFocusNodeId003, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto oldFocusNode = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    auto newFocusNode = std::make_shared<RSSurfaceRenderNode>(id + 1, mainThread->context_);
    ASSERT_NE(oldFocusNode, nullptr);
    ASSERT_NE(newFocusNode, nullptr);

    mainThread->context_->nodeMap.renderNodeMap_[oldFocusNode->GetId()] = oldFocusNode;
    mainThread->context_->nodeMap.renderNodeMap_[newFocusNode->GetId()] = newFocusNode;
    mainThread->focusNodeId_ = oldFocusNode->GetId();
    mainThread->UpdateFocusNodeId(newFocusNode->GetId());
    ASSERT_EQ(mainThread->GetFocusNodeId(), newFocusNode->GetId());
}
} // namespace OHOS::Rosen
