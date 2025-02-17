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
#include <parameter.h>
#include <parameters.h>
#include <if_system_ability_manager.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "gtest/gtest.h"
#include "limit_number.h"
#include "rs_test_util.h"

#include "command/rs_base_node_command.h"
#include "memory/rs_memory_track.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
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
constexpr int32_t SYSTEM_ANIMATED_SCENES_RATE = 2;
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

private:
    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_YCRCB_420_SP,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    static inline BufferFlushConfig flushConfig = {
        .damage = { .w = 0x100, .h = 0x100, },
    };
};

void RSMainThreadTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSMainThreadTest::TearDownTestCase() {}
void RSMainThreadTest::SetUp() {}
void RSMainThreadTest::TearDown()
{
    auto mainThread = RSMainThread::Instance();
    if (!mainThread || !mainThread->context_) {
        return;
    }
    auto& renderNodeMap = mainThread->context_->GetMutableNodeMap();
    renderNodeMap.renderNodeMap_.clear();
    renderNodeMap.surfaceNodeMap_.clear();
    renderNodeMap.residentSurfaceNodeMap_.clear();
    renderNodeMap.displayNodeMap_.clear();
    renderNodeMap.canvasDrawingNodeMap_.clear();
    renderNodeMap.uiExtensionSurfaceNodes_.clear();
}
void* RSMainThreadTest::CreateParallelSyncSignal(uint32_t count)
{
    (void)(count);
    return nullptr;
}

class ApplicationAgentImpl : public IRemoteStub<IApplicationAgent> {
public:
    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override
    {
        return 0;
    }
    void OnTransaction(std::shared_ptr<RSTransactionData> transactionData) override
    {
    }
};

/**
 * @tc.name: ProcessCommandForDividedRender
 * @tc.desc: Test RSMainThreadTest.ProcessCommandForDividedRender
 * @tc.type: FUNC
 * @tc.require: issueIB8HAQ
 */
HWTEST_F(RSMainThreadTest, ProcessCommandForDividedRender002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    int dataIndex = 1;
    rsTransactionData->SetIndex(dataIndex);
    int dataPayloadSize = 3;
    rsTransactionData->payload_.resize(dataPayloadSize);
    NodeId id = 0;
    rsTransactionData->payload_[id] = std::tuple<NodeId,
        FollowType, std::unique_ptr<RSCommand>>(id, FollowType::NONE, std::make_unique<RSBaseNodeAddChild>(0, 1, 3));
    id = 1;
    rsTransactionData->payload_[id] = std::tuple<NodeId,
        FollowType, std::unique_ptr<RSCommand>>(id, FollowType::FOLLOW_TO_SELF, nullptr);
    mainThread->ClassifyRSTransactionData(rsTransactionData);

    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    mainThread->context_->nodeMap.RegisterRenderNode(node);
    node->SetIsOnTheTree(true, 0, 1, 2);
    mainThread->ConsumeAndUpdateAllNodes();

    mainThread->ProcessCommandForDividedRender();
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
    ASSERT_FALSE(str.size() == 0);
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
    ASSERT_NE(mainThread, nullptr);
    std::string str = "";
    int32_t pid = INVALID_VALUE;
    int32_t uid = INVALID_VALUE;
    mainThread->SetFocusAppInfo(pid, uid, str, str, 0);
    ASSERT_EQ(mainThread->focusAppPid_, pid);
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
 * @tc.name: AddPidNeedDropFrame
 * @tc.desc: Test AddPidNeedDropFrame
 * @tc.type: FUNC
 * @tc.require: issueIB612L
 */
HWTEST_F(RSMainThreadTest, AddPidNeedDropFrame, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    mainThread->AddPidNeedDropFrame({ExtractPid(surfaceNode->GetId())});
    ASSERT_EQ(mainThread->surfacePidNeedDropFrame_.size(), 1);
}

/**
 * @tc.name: ClearNeedDropframePidList
 * @tc.desc: Test ClearNeedDropframePidList
 * @tc.type: FUNC
 * @tc.require: issueIB612L
 */
HWTEST_F(RSMainThreadTest, ClearNeedDropframePidList, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    mainThread->AddPidNeedDropFrame({ExtractPid(surfaceNode->GetId())});
    mainThread->ClearNeedDropframePidList();
    ASSERT_EQ(mainThread->surfacePidNeedDropFrame_.size(), 0);
}

/**
 * @tc.name: IsNeedDropFrameByPid001
 * @tc.desc: Test IsNeedDropFrameByPid while pid satisfy
 * @tc.type: FUNC
 * @tc.require: issueIB7PH1
 */
HWTEST_F(RSMainThreadTest, IsNeedDropFrameByPid001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    mainThread->AddPidNeedDropFrame({ExtractPid(surfaceNode->GetId())});
    ASSERT_TRUE(mainThread->IsNeedDropFrameByPid(surfaceNode->GetId()));

    mainThread->ClearNeedDropframePidList();
}

/**
 * @tc.name: IsNeedDropFrameByPid002
 * @tc.desc: Test IsNeedDropFrameByPid while pid not satisfy
 * @tc.type: FUNC
 * @tc.require: issueIB7PH1
 */
HWTEST_F(RSMainThreadTest, IsNeedDropFrameByPid002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    mainThread->AddPidNeedDropFrame({});
    ASSERT_FALSE(mainThread->IsNeedDropFrameByPid(surfaceNode->GetId()));

    mainThread->ClearNeedDropframePidList();
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
    mainThread->ProcessSyncRSTransactionData(rsTransactionData, pid);
    ASSERT_EQ(mainThread->syncTransactionData_.empty(), false);
}

/**
 * @tc.name: StartSyncTransactionFallbackTask001
 * @tc.desc: Test StartSyncTransactionFallbackTask
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, StartSyncTransactionFallbackTask001, TestSize.Level1)
{
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    rsTransactionData->SetSyncId(1);

    auto mainThread = RSMainThread::Instance();
    auto syncData = std::make_unique<RSTransactionData>();
    syncData->SetSyncId(0);
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    mainThread->syncTransactionData_[0] = std::vector<std::unique_ptr<RSTransactionData>>();
    mainThread->syncTransactionData_[0].push_back(std::move(syncData));
    mainThread->StartSyncTransactionFallbackTask(rsTransactionData);
    ASSERT_EQ(mainThread->syncTransactionData_.empty(), false);
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;

    auto mainThread2 = RSMainThread::Instance();
    auto syncData2 = std::make_unique<RSTransactionData>();
    syncData2->SetSyncId(1);
    mainThread2->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread2->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread2->runner_);
    mainThread2->syncTransactionData_[0] = std::vector<std::unique_ptr<RSTransactionData>>();
    mainThread2->syncTransactionData_[0].push_back(std::move(syncData2));
    mainThread2->StartSyncTransactionFallbackTask(rsTransactionData);
    ASSERT_EQ(mainThread2->syncTransactionData_.empty(), false);
    mainThread2->runner_ = nullptr;
    mainThread2->handler_ = nullptr;
}

/**
 * @tc.name: ProcessSyncTransactionCount
 * @tc.desc: Test ProcessSyncTransactionCount
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, ProcessSyncTransactionCount, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto rsTransactionData = std::make_unique<RSTransactionData>();

    rsTransactionData->SetParentPid(-1);
    mainThread->ProcessSyncTransactionCount(rsTransactionData);
    auto parentPid = rsTransactionData->GetParentPid();
    ASSERT_EQ(parentPid, -1);

    rsTransactionData->SetSyncTransactionNum(1);
    mainThread->ProcessSyncTransactionCount(rsTransactionData);
    ASSERT_EQ(rsTransactionData->GetSyncTransactionNum(), 1);

    rsTransactionData->MarkNeedCloseSync();
    mainThread->ProcessSyncTransactionCount(rsTransactionData);
    mainThread->StartSyncTransactionFallbackTask(rsTransactionData);
    ASSERT_EQ(rsTransactionData->IsNeedCloseSync(), true);
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
    ASSERT_EQ(mainThread->handler_, nullptr);
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
    if (mainThread->rsVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->rsVSyncDistributor_ = new VSyncDistributor(vsyncController, "rs");
        vsyncGenerator->SetRSDistributor(mainThread->rsVSyncDistributor_);
    }
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

    NodeId nodeId =1;
    std::weak_ptr<RSContext> context = {};
    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(nodeId, context);
    auto drawableNode = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsCanvasDrawingRenderNode);
    drawableNode->SetNeedDraw(true);
    mainThread->context_->nodeMap.RegisterRenderNode(rsCanvasDrawingRenderNode);
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
    ASSERT_EQ(mainThread->GetWatermarkImg(), nullptr);
}

/**
 * @tc.name: IsWatermarkFlagChanged
 * @tc.desc: IsWatermarkFlagChanged test
 * @tc.type: FUNC
 * @tc.require: issuesIA8LNR
 */
HWTEST_F(RSMainThreadTest, IsWatermarkFlagChanged, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->IsWatermarkFlagChanged();
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
    node->GenerateFullChildrenList();
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
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetIdleTimerExpiredFlag(true);
    ASSERT_TRUE(mainThread->idleTimerExpiredFlag_);
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
    mainThread->context_->nodeMap.renderNodeMap_[0][0] = node1;
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
    NodeId nodeId = node->GetId();
    pid_t pid = ExtractPid(nodeId);
    mainThread->context_->nodeMap.renderNodeMap_[pid][nodeId] = node;
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

    NodeId childNodeId = childNode->GetId();
    pid_t childNodePid = ExtractPid(childNodeId);
    mainThread->context_->nodeMap.renderNodeMap_[childNodePid][childNodeId] = childNode;
    NodeId parentNodeId = parentNode->GetId();
    pid_t parentNodePid = ExtractPid(parentNodeId);
    mainThread->context_->nodeMap.renderNodeMap_[parentNodePid][parentNodeId] = parentNode;
    std::string str = "";
    mainThread->SetFocusAppInfo(-1, -1, str, str, childNode->GetId());
    mainThread->SetFocusLeashWindowId();
    ASSERT_EQ(mainThread->GetFocusLeashWindowId(), INVALID_NODEID);
}
} // namespace OHOS::Rosen
