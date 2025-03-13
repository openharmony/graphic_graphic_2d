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

#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_innovation.h"
#include "platform/common/rs_system_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_RATE = 1;
constexpr int32_t INVALID_VALUE = -1;
constexpr uint64_t REFRESH_PERIOD = 16666667;
constexpr uint64_t SKIP_COMMAND_FREQ_LIMIT = 30;
constexpr uint32_t MULTI_WINDOW_PERF_START_NUM = 2;
constexpr uint32_t MULTI_WINDOW_PERF_END_NUM = 4;
constexpr int32_t SIMI_VISIBLE_RATE = 2;
constexpr int32_t SYSTEM_ANIMATED_SCENES_RATE = 2;
constexpr int32_t INVISBLE_WINDOW_RATE = 10;
constexpr ScreenId DEFAULT_DISPLAY_SCREEN_ID = 0;
class RsMainThreadTest : public testing::Test {
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

void RsMainThreadTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RsMainThreadTest::TearDownTestCase() {}
void RsMainThreadTest::SetUp() {}
void RsMainThreadTest::TearDown()
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
void* RsMainThreadTest::CreateParallelSyncSignal(uint32_t count)
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
 * @tc.name: Start01
 * @tc.desc: Test RsMainThreadTest.Start
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, Start01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = nullptr;
    mainThread->Start();
}

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
 * @tc.name: ReleaseAllNodesBuffer
 * @tc.desc: ReleaseAllNodesBuffer002 Test preBuffer != nullptr;
 * @tc.type: FUNC
 * @tc.require: issueIAS924
 */
HWTEST_F(RSMainThreadTest, ReleaseAllNodesBuffer002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    // prepare nodemap
    mainThread->context_->GetMutableNodeMap().renderNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.clear();
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(config);
    node1->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node1->isHardwareEnabledNode_ = true;
    node1->isLastFrameHardwareEnabled_ = true;
    node1->isCurrentFrameHardwareEnabled_ = false;
    sptr<SurfaceBuffer> preBuffer = OHOS::SurfaceBuffer::Create();
    sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
    Rect damageRect = {0, 0, 0, 0};
    int64_t timestamp = 0;
    ASSERT_NE(node1->surfaceHandler_, nullptr);
    node1->surfaceHandler_->SetBuffer(preBuffer, acquireFence, damageRect, timestamp);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node1);
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
 * @tc.name: SendCommands001
 * @tc.desc: SendCommands Test
 * @tc.type: FUNC
 */
HWTEST_F(RSMainThreadTest, SendCommands001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 1;
    AnimationId animationId = 1;
    mainThread->context_->AddSyncFinishAnimationList(id, animationId);
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
 * @tc.name: Start02
 * @tc.desc: Test RsMainThreadTest.Start
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, Start02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->Start();
}

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

    NodeId childNodeId = childNode->GetId();
    NodeId parentNodeId = parentNode->GetId();
    pid_t childNodePid = ExtractPid(childNodeId);
    pid_t parentNodePid = ExtractPid(parentNodeId);
    mainThread->context_->nodeMap.renderNodeMap_[childNodePid][childNodeId] = childNode;
    mainThread->context_->nodeMap.renderNodeMap_[parentNodePid][parentNodeId] = parentNode;
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
    node2->GenerateFullChildrenList();
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
 * @tc.name: SkipCommandByNodeId003
 * @tc.desc: SkipCommandByNodeId test cacheCmdSkippedInfo_ is empty
 * @tc.type: FUNC
 * @tc.require: issueIAS924
 */
HWTEST_F(RSMainThreadTest, SkipCommandByNodeId003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<std::unique_ptr<RSTransactionData>> transactionVec;
    mainThread->cacheCmdSkippedInfo_.clear();
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
    rsSurfaceRenderNode1->GenerateFullChildrenList();
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
 * @tc.name: IsLastFrameUIFirstEnabled001
 * @tc.desc: IsLastFrameUIFirstEnabled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, IsLastFrameUIFirstEnabled001, TestSize.Level1)
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
 * @tc.name: IsLastFrameUIFirstEnabled002
 * @tc.desc: IsLastFrameUIFirstEnabled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, IsLastFrameUIFirstEnabled002, TestSize.Level1)
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
    node1->GenerateFullChildrenList();
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
 * @tc.name: CheckIfHardwareForcedDisabled
 * @tc.desc: CheckIfHardwareForcedDisabled002 test rootNode = nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAS924
 */
HWTEST_F(RSMainThreadTest, CheckIfHardwareForcedDisabled002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    mainThread->context_->globalRootRenderNode_ = nullptr;
    mainThread->CheckIfHardwareForcedDisabled();
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: CheckIfHardwareForcedDisabled
 * @tc.desc: CheckIfHardwareForcedDisabled003 test child = nullptr and type != DISPLAY_NODE
 * @tc.type: FUNC
 * @tc.require: issueIAS924
 */
HWTEST_F(RSMainThreadTest, CheckIfHardwareForcedDisabled003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    auto node1 = std::make_shared<RSRenderNode>(1, true);
    std::shared_ptr<RSRenderNode> node2 = nullptr;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(2, mainThread->context_);
    node3->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto node4 = std::make_shared<RSRenderNode>(3, true);
    node1->children_.emplace_back(node2);
    node1->AddChild(node3);
    node1->AddChild(node4);
    mainThread->context_->globalRootRenderNode_ = node1;
    mainThread->CheckIfHardwareForcedDisabled();
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: WaitUntilUnmarshallingTaskFinished001
 * @tc.desc: WaitUntilUnmarshallingTaskFinished test, divided render, the func call will return immediately.
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, WaitUntilUnmarshallingTaskFinished001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
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
    ASSERT_NE(mainThread, nullptr);
    TransactionDataMap tsDataMap;
    tsDataMap.emplace(0, std::vector<std::unique_ptr<RSTransactionData>>());
    auto data = std::make_unique<RSTransactionData>();
    data->SetIndex(1);
    tsDataMap[0].emplace_back(std::move(data));
    mainThread->MergeToEffectiveTransactionDataMap(tsDataMap);
    ASSERT_TRUE(tsDataMap.empty());
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
 * @tc.name: ProcessCommand01
 * @tc.desc: Test RsMainThreadTest.ProcessCommand
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, ProcessCommand01, TestSize.Level1)
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
 * @tc.name: RemoveRSEventDetector001
 * @tc.desc: Test RsMainThreadTest.RemoveRSEventDetector, with init
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, RemoveRSEventDetector001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->InitRSEventDetector();
    mainThread->RemoveRSEventDetector();
}

/**
 * @tc.name: RsEventParamDump
 * @tc.desc: Test RsMainThreadTest.RsEventParamDump
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, RsEventParamDump, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::string str = "";
    mainThread->RsEventParamDump(str);
    ASSERT_TRUE(str.empty());
}

/**
 * @tc.name: RemoveRSEventDetector02
 * @tc.desc: Test RsMainThreadTest.RemoveRSEventDetector, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, RemoveRSEventDetector02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->RemoveRSEventDetector();
}

/**
 * @tc.name: SetRSEventDetectorLoopStartTag01
 * @tc.desc: Test RsMainThreadTest.SetRSEventDetectorLoopStartTag, with init
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, SetRSEventDetectorLoopStartTag01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->InitRSEventDetector();
    mainThread->SetRSEventDetectorLoopStartTag();
}

/**
 * @tc.name: SetRSEventDetectorLoopStartTag02
 * @tc.desc: Test RsMainThreadTest.SetRSEventDetectorLoopStartTag, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, SetRSEventDetectorLoopStartTag02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->rsCompositionTimeoutDetector_ = nullptr;
    mainThread->SetRSEventDetectorLoopStartTag();
}

/**
 * @tc.name: InitRSEventDetector
 * @tc.desc: Test RsMainThreadTest.InitRSEventDetector, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, InitRSEventDetector, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->rsCompositionTimeoutDetector_ = nullptr;
    mainThread->InitRSEventDetector();
}

/**
 * @tc.name: SetRSEventDetectorLoopFinishTag01
 * @tc.desc: Test RsMainThreadTest.SetRSEventDetectorLoopFinishTag, with init
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, SetRSEventDetectorLoopFinishTag01, TestSize.Level1)
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
 * @tc.name: SetRSEventDetectorLoopFinishTag02
 * @tc.desc: Test RsMainThreadTest.SetRSEventDetectorLoopFinishTag, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, SetRSEventDetectorLoopFinishTag02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->rsCompositionTimeoutDetector_ = nullptr;
    mainThread->SetRSEventDetectorLoopFinishTag();
}


/**
 * @tc.name: ProcessCommandForDividedRender01
 * @tc.desc: Test RsMainThreadTest.ProcessCommandForDividedRender
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, ProcessCommandForDividedRender01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->ProcessCommandForDividedRender();
}

/**
 * @tc.name: CalcOcclusion
 * @tc.desc: Test RsMainThreadTest.CalcOcclusion, doWindowAnimate_ is false, isUniRender_ is true
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, CalcOcclusion, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->doWindowAnimate_ = false;
    mainThread->isUniRender_ = true;
    mainThread->CalcOcclusion();
}

/**
 * @tc.name: Animate01
 * @tc.desc: Test RsMainThreadTest.Animate, doWindowAnimate_ is false
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, Animate01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->doWindowAnimate_ = false;
    mainThread->Animate(0);
}

/**
 * @tc.name: Animate02
 * @tc.desc: Test RsMainThreadTest.Animate, doWindowAnimate_ is true
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, Animate02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->doWindowAnimate_ = true;
    mainThread->Animate(0);
}

/**
 * @tc.name: RenderServiceTreeDump
 * @tc.desc: Test RsMainThreadTest.RenderServiceTreeDump, str is an empty string
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, RenderServiceTreeDump, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::string str = "";
    mainThread->RenderServiceTreeDump(str);
    ASSERT_FALSE(str.size() == 0);
}

/**
 * @tc.name: UnRegisterOcclusionChangeCallback
 * @tc.desc: Test RsMainThreadTest.Animate
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, UnRegisterOcclusionChangeCallback, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->UnRegisterOcclusionChangeCallback(0);
}

/**
 * @tc.name: SetFocusAppInfo
 * @tc.desc: Test RsMainThreadTest.SetFocusAppInfo, input pid, uid is -1, str is an empty string
 * @tc.type: FUNC
 * @tc.require: issueI60QX1
 */
HWTEST_F(RsMainThreadTest, SetFocusAppInfo, TestSize.Level1)
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
 * @tc.name: SetFocusAppInfo02
 * @tc.desc: Test SetFocusAppInfo while change focus node
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RsMainThreadTest, SetFocusAppInfo02, TestSize.Level2)
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
 * @tc.name: ProcessSyncRSTransactionData01
 * @tc.desc: Test ProcessSyncRSTransactionData when TransactionData do not need sync
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A1
 */
HWTEST_F(RsMainThreadTest, ProcessSyncRSTransactionData01, TestSize.Level1)
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
 * @tc.name: ProcessSyncRSTransactionData02
 * @tc.desc: Test ProcessSyncRSTransactionData when TransactionData do not need sync
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A1
 */
HWTEST_F(RsMainThreadTest, ProcessSyncRSTransactionData02, TestSize.Level1)
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
 * @tc.name: GetContext
 * @tc.desc: Test if context has been initialized
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A1
 */
HWTEST_F(RsMainThreadTest, GetContext, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto& context = mainThread->GetContext();
    uint64_t time = 0;
    ASSERT_EQ(context.GetTransactionTimestamp(), time);
}

/**
 * @tc.name: ProcessSyncTransactionCount
 * @tc.desc: Test ProcessSyncTransactionCount
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A1
 */
HWTEST_F(RsMainThreadTest, ProcessSyncTransactionCount, TestSize.Level1)
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
 * @tc.name: ClassifyRSTransactionData01
 * @tc.desc: Test ClassifyRSTransactionData when nodeId is 0
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A1
 */
HWTEST_F(RsMainThreadTest, ClassifyRSTransactionData01, TestSize.Level1)
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
 * @tc.name: ClassifyRSTransactionData02
 * @tc.desc: Test ClassifyRSTransactionData when nodeId is 1
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RsMainThreadTest, ClassifyRSTransactionData02, TestSize.Level1)
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
 * @tc.name: ClassifyRSTransactionData03
 * @tc.desc: Test ClassifyRSTransactionData when followType is FOLLOW_TO_PARENT
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RsMainThreadTest, ClassifyRSTransactionData03, TestSize.Level1)
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
 * @tc.name: ClassifyRSTransactionData04
 * @tc.desc: Test ClassifyRSTransactionData when followType is FOLLOW_TO_PARENT
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RsMainThreadTest, ClassifyRSTransactionData04, TestSize.Level1)
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
 * @tc.name: AddActiveNode01
 * @tc.desc: Test AddActiveNode, add invalid node id, check if fails
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A1
 */
HWTEST_F(RsMainThreadTest, AddActiveNode01, TestSize.Level1)
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
HWTEST_F(RsMainThreadTest, CheckAndUpdateInstanceContentStaticStatus01, TestSize.Level1)
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
HWTEST_F(RsMainThreadTest, CheckAndUpdateInstanceContentStaticStatus02, TestSize.Level1)
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
 * @tc.name: IsNeedProcessBySingleFrameComposerTest01
 * @tc.desc: Test IsNeedProcessBySingleFrameComposerTest when TransactionData is null
 * @tc.type: FUNC
 * @tc.require: issueI9HPBS
 */
HWTEST_F(RsMainThreadTest, IsNeedProcessBySingleFrameComposerTest01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::unique_ptr<RSTransactionData> transactionData = nullptr;
    ASSERT_FALSE(mainThread->IsNeedProcessBySingleFrameComposer(transactionData));
    transactionData = std::make_unique<RSTransactionData>();
    mainThread->isUniRender_ = true;
    ASSERT_FALSE(mainThread->IsNeedProcessBySingleFrameComposer(transactionData));
}

/**
 * @tc.name: IsNeedProcessBySingleFrameComposerTest02
 * @tc.desc: Test IsNeedProcessBySingleFrameComposerTest when SingleFrameComposer enabled by app process
 * @tc.type: FUNC
 * @tc.require: issueI9HPBS
 */
HWTEST_F(RsMainThreadTest, IsNeedProcessBySingleFrameComposerTest02, TestSize.Level1)
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
 * @tc.name: IsNeedProcessBySingleFrameComposerTest03
 * @tc.desc: Test IsNeedProcessBySingleFrameComposerTest when animation node exists
 * @tc.type: FUNC
 * @tc.require: issueI9HPBS
 */
HWTEST_F(RsMainThreadTest, IsNeedProcessBySingleFrameComposerTest03, TestSize.Level1)
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
 * @tc.name: IsNeedProcessBySingleFrameComposerTest04
 * @tc.desc: Test IsNeedProcessBySingleFrameComposerTest when multi-window shown on screen
 * @tc.type: FUNC
 * @tc.require: issueI9HPBS
 */
HWTEST_F(RsMainThreadTest, IsNeedProcessBySingleFrameComposerTest04, TestSize.Level1)
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
 * @tc.name: RecvAndProcessRSTransactionDataImmediatelyTest01
 * @tc.desc: Test ecvAndProcessRSTransactionDataImmediately when transactionData is null
 * @tc.type: FUNC
 * @tc.require: issueI9HPBS
 */
HWTEST_F(RsMainThreadTest, RecvAndProcessRSTransactionDataImmediatelyTest01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::unique_ptr<RSTransactionData> transactionData = nullptr;
    mainThread->RecvAndProcessRSTransactionDataImmediately(transactionData);
    ASSERT_EQ(transactionData, nullptr);
}


/**
 * @tc.name: RecvRSTransactionData01
 * @tc.desc: Test RecvRSTransactionData, when TransactionData is null
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RsMainThreadTest, RecvRSTransactionData01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::unique_ptr<RSTransactionData> transactionData = nullptr;
    mainThread->RecvRSTransactionData(transactionData);
    ASSERT_EQ(transactionData, nullptr);
}

/**
 * @tc.name: PostSyncTask01
 * @tc.desc: Test PostSyncTask when handler is null or not
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RsMainThreadTest, PostSyncTask01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_EQ(mainThread->handler_, nullptr);
    RSTaskMessage::RSTask task = []() -> void { return; };
    mainThread->PostSyncTask(task);
}

/**
 * @tc.name: ShowWatermark01
 * @tc.desc: ShowWatermark test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsMainThreadTest, ShowWatermark01, TestSize.Level1)
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
 * @tc.name: MergeToEffectiveTransactionDataMap01
 * @tc.desc: Test RsMainThreadTest.MergeToEffectiveTransactionDataMap
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RsMainThreadTest, MergeToEffectiveTransactionDataMap01, TestSize.Level1)
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
 * @tc.name: ProcessCommandForUniRender01
 * @tc.desc: ProcessCommandForUniRender test with invalid data
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RsMainThreadTest, ProcessCommandForUniRender01, TestSize.Level1)
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
    mainThread->ProcessCommandForUniRender();
}

/**
 * @tc.name: GetWatermarkImg01
 * @tc.desc: GetWatermarkImg test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, GetWatermarkImg01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_EQ(mainThread->GetWatermarkImg(), nullptr);
}

/**
 * @tc.name: IsWatermarkFlagChanged01
 * @tc.desc: IsWatermarkFlagChanged test
 * @tc.type: FUNC
 * @tc.require: issuesIA8LNR
 */
HWTEST_F(RsMainThreadTest, IsWatermarkFlagChanged01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->IsWatermarkFlagChanged();
}

/**
 * @tc.name: DoParallelComposition01
 * @tc.desc: DoParallelComposition test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, DoParallelComposition01, TestSize.Level1)
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
    RSInnovation::_s_createParallelSyncSignal = (void*)RsMainThreadTest::CreateParallelSyncSignal;
    if (RSInnovation::GetParallelCompositionEnabled(mainThread->isUniRender_)) {
        mainThread->DoParallelComposition(node);
    }
}

/**
 * @tc.name: SetIdleTimerExpiredFlag01
 * @tc.desc: SetIdleTimerExpiredFlag test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SetIdleTimerExpiredFlag01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetIdleTimerExpiredFlag(true);
    ASSERT_TRUE(mainThread->idleTimerExpiredFlag_);
}

/**
 * @tc.name: SetFocusLeashWindowId01
 * @tc.desc: Test RsMainThreadTest.SetFocusLeashWindowId
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RsMainThreadTest, SetFocusLeashWindowId01, TestSize.Level1)
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
 * @tc.name: SetFocusLeashWindowId02
 * @tc.desc: Test SetFocusLeashWindowId while nodeMap is empty
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RsMainThreadTest, SetFocusLeashWindowId02, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    mainThread->SetFocusLeashWindowId();
    ASSERT_EQ(mainThread->GetFocusLeashWindowId(), INVALID_NODEID);
}

/**
 * @tc.name: SetFocusLeashWindowId03
 * @tc.desc: Test SetFocusLeashWindowId while focus node don't have parent
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RsMainThreadTest, SetFocusLeashWindowId03, TestSize.Level2)
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
    node2->GenerateFullChildrenList();
    mainThread->subThreadNodes_.push_back(node1);
    mainThread->subThreadNodes_.push_back(node2);
    mainThread->subThreadNodes_.push_back(node3);

    mainThread->cacheCmdSkippedInfo_.clear();
    mainThread->CheckParallelSubThreadNodesStatus();
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
    rsSurfaceRenderNode1->GenerateFullChildrenList();
    mainThread->subThreadNodes_.emplace_back(rsSurfaceRenderNode1);
    mainThread->CheckSubThreadNodeStatusIsDoing(rsSurfaceRenderNode2->GetId());
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
 * @tc.name: SkipCommandByNodeId003
 * @tc.desc: SkipCommandByNodeId test cacheCmdSkippedInfo_ is empty
 * @tc.type: FUNC
 * @tc.require: issueIAS924
 */
HWTEST_F(RSMainThreadTest, SkipCommandByNodeId003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<std::unique_ptr<RSTransactionData>> transactionVec;
    mainThread->cacheCmdSkippedInfo_.clear();
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
 * @tc.name: IsLastFrameUIFirstEnabled001
 * @tc.desc: IsLastFrameUIFirstEnabled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, IsLastFrameUIFirstEnabled001, TestSize.Level1)
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
 * @tc.name: CheckIfHardwareForcedDisabled
 * @tc.desc: CheckIfHardwareForcedDisabled002 test rootNode = nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAS924
 */
HWTEST_F(RSMainThreadTest, CheckIfHardwareForcedDisabled002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    mainThread->context_->globalRootRenderNode_ = nullptr;
    mainThread->CheckIfHardwareForcedDisabled();
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: CheckIfHardwareForcedDisabled
 * @tc.desc: CheckIfHardwareForcedDisabled003 test child = nullptr and type != DISPLAY_NODE
 * @tc.type: FUNC
 * @tc.require: issueIAS924
 */
HWTEST_F(RSMainThreadTest, CheckIfHardwareForcedDisabled003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    auto node1 = std::make_shared<RSRenderNode>(1, true);
    std::shared_ptr<RSRenderNode> node2 = nullptr;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(2, mainThread->context_);
    node3->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto node4 = std::make_shared<RSRenderNode>(3, true);
    node1->children_.emplace_back(node2);
    node1->AddChild(node3);
    node1->AddChild(node4);
    mainThread->context_->globalRootRenderNode_ = node1;
    mainThread->CheckIfHardwareForcedDisabled();
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: WaitUntilUnmarshallingTaskFinished001
 * @tc.desc: WaitUntilUnmarshallingTaskFinished test, divided render, the func call will return immediately.
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, WaitUntilUnmarshallingTaskFinished001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
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
    ASSERT_NE(mainThread, nullptr);
    TransactionDataMap tsDataMap;
    tsDataMap.emplace(0, std::vector<std::unique_ptr<RSTransactionData>>());
    auto data = std::make_unique<RSTransactionData>();
    data->SetIndex(1);
    tsDataMap[0].emplace_back(std::move(data));
    mainThread->MergeToEffectiveTransactionDataMap(tsDataMap);
    ASSERT_TRUE(tsDataMap.empty());
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

    NodeId childNodeId = childNode->GetId();
    NodeId parentNodeId = parentNode->GetId();
    pid_t childNodePid = ExtractPid(childNodeId);
    pid_t parentNodePid = ExtractPid(parentNodeId);
    mainThread->context_->nodeMap.renderNodeMap_[childNodePid][childNodeId] = childNode;
    mainThread->context_->nodeMap.renderNodeMap_[parentNodePid][parentNodeId] = parentNode;
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
 * @tc.name: IsLastFrameUIFirstEnabled002
 * @tc.desc: IsLastFrameUIFirstEnabled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, IsLastFrameUIFirstEnabled002, TestSize.Level1)
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
    node1->GenerateFullChildrenList();
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
 * @tc.name: SetFocusLeashWindowId04
 * @tc.desc: Test SetFocusLeashWindowId while focus node's type don't match
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RsMainThreadTest, SetFocusLeashWindowId04, TestSize.Level2)
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

/**
 * @tc.name: SetFocusLeashWindowId05
 * @tc.desc: Test SetFocusLeashWindowId while focus node's parent's type don't match
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RsMainThreadTest, SetFocusLeashWindowId05, TestSize.Level2)
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

/**
 * @tc.name: CalcOcclusionImplementation001
 * @tc.desc: calculate occlusion when surfaces do not overlap
 * @tc.type: FUNC
 * @tc.require: issueIB7P9Z
 */
HWTEST_F(RSMainThreadTest, CalcOcclusionImplementation001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;

    NodeId idBottom = 0;
    auto nodeBottom = std::make_shared<RSSurfaceRenderNode>(idBottom, mainThread->context_);
    nodeBottom->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idBottom);
    RectI rectBottom = RectI(0, 0, 100, 100);
    nodeBottom->oldDirtyInSurface_ = rectBottom;
    nodeBottom->SetDstRect(rectBottom);
    nodeBottom->opaqueRegion_ = Occlusion::Region(rectBottom);

    NodeId idTop = 1;
    auto nodeTop = std::make_shared<RSSurfaceRenderNode>(idTop, mainThread->context_);
    nodeTop->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idTop);
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
 * @tc.require: issueIB7P9Z
 */
HWTEST_F(RSMainThreadTest, CalcOcclusionImplementation002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;

    NodeId idBottom = 0;
    auto nodeBottom = std::make_shared<RSSurfaceRenderNode>(idBottom, mainThread->context_);
    nodeBottom->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idBottom);
    RectI rectBottom = RectI(0, 0, 100, 100);
    nodeBottom->oldDirtyInSurface_ = rectBottom;
    nodeBottom->SetDstRect(rectBottom);
    nodeBottom->opaqueRegion_ = Occlusion::Region(rectBottom);

    NodeId idTop = 1;
    auto nodeTop = std::make_shared<RSSurfaceRenderNode>(idTop, mainThread->context_);
    nodeTop->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idTop);
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
 * @tc.require: issueIB7P9Z
 */
HWTEST_F(RSMainThreadTest, CalcOcclusionImplementation003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;

    NodeId idBottom = 0;
    auto nodeBottom = std::make_shared<RSSurfaceRenderNode>(idBottom, mainThread->context_);
    nodeBottom->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idBottom);
    RectI rectBottom = RectI(0, 0, 100, 100);
    nodeBottom->oldDirtyInSurface_ = rectBottom;
    nodeBottom->SetDstRect(rectBottom);
    nodeBottom->opaqueRegion_ = Occlusion::Region(rectBottom);

    NodeId idTop = 1;
    auto nodeTop = std::make_shared<RSSurfaceRenderNode>(idTop, mainThread->context_);
    nodeTop->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idTop);
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
 * @tc.require: issueIB7P9Z
 */
HWTEST_F(RSMainThreadTest, CalcOcclusionImplementation004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;

    NodeId idBottom = 0;
    auto nodeBottom = std::make_shared<RSSurfaceRenderNode>(idBottom, mainThread->context_);
    nodeBottom->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idBottom);
    RectI rectBottom = RectI(0, 0, 100, 100);
    nodeBottom->oldDirtyInSurface_ = rectBottom;
    nodeBottom->SetDstRect(rectBottom);
    nodeBottom->opaqueRegion_ = Occlusion::Region(rectBottom);

    NodeId idTop = 1;
    auto nodeTop = std::make_shared<RSSurfaceRenderNode>(idTop, mainThread->context_);
    nodeTop->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idTop);
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
 * @tc.require: issueIB7P9Z
 */
HWTEST_F(RSMainThreadTest, CalcOcclusionImplementation005, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;

    NodeId idBottom = 0;
    auto nodeBottom = std::make_shared<RSSurfaceRenderNode>(idBottom, mainThread->context_);
    nodeBottom->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idBottom);
    RectI rectBottom = RectI(0, 0, 100, 100);
    nodeBottom->oldDirtyInSurface_ = rectBottom;
    nodeBottom->SetDstRect(rectBottom);
    nodeBottom->opaqueRegion_ = Occlusion::Region(rectBottom);

    NodeId idTop = 1;
    auto nodeTop = std::make_shared<RSSurfaceRenderNode>(idTop, mainThread->context_);
    nodeTop->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idTop);
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
 * @tc.name: UiCaptureTasks
 * @tc.desc: test UiCaptureTasks processing
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSMainThreadTest, UiCaptureTasks, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    auto node1 = RSTestUtil::CreateSurfaceNode();
    auto node2 = RSTestUtil::CreateSurfaceNode();
    auto task = []() {};

    mainThread->ProcessUiCaptureTasks();
    ASSERT_EQ(mainThread->pendingUiCaptureTasks_.empty(), true);

    mainThread->context_->nodeMap.RegisterRenderNode(node1);
    mainThread->AddUiCaptureTask(node1->GetId(), task);
    mainThread->AddUiCaptureTask(node2->GetId(), task);
    ASSERT_EQ(mainThread->pendingUiCaptureTasks_.empty(), false);
    ASSERT_EQ(mainThread->uiCaptureTasks_.empty(), true);

    mainThread->PrepareUiCaptureTasks(nullptr);
    ASSERT_EQ(mainThread->pendingUiCaptureTasks_.empty(), true);
    ASSERT_EQ(mainThread->uiCaptureTasks_.empty(), false);

    mainThread->ProcessUiCaptureTasks();
    ASSERT_EQ(mainThread->pendingUiCaptureTasks_.empty(), true);
    ASSERT_EQ(mainThread->uiCaptureTasks_.empty(), true);

    mainThread->context_->nodeMap.UnregisterRenderNode(node1->GetId());
}

/**
 * @tc.name: CheckOnUIExtensionCallback001
 * @tc.desc: test CheckOnUIExtensionCallback
 * @tc.type: FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSMainThreadTest, CheckOnUIExtensionCallback001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->lastFrameUIExtensionDataEmpty_ = false;
    mainThread->uiExtensionCallbackData_.clear();
    ASSERT_TRUE(mainThread->CheckUIExtensionCallbackDataChanged());

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObj = samgr->GetSystemAbility(RENDER_SERVICE);
    ASSERT_NE(remoteObj, nullptr);
    auto callback = iface_cast<RSIUIExtensionCallback>(remoteObj);
    uint64_t userId = 0;
    pid_t pid = 0;
    mainThread->RegisterUIExtensionCallback(pid, userId, callback);
    mainThread->UIExtensionNodesTraverseAndCallback();
    mainThread->UnRegisterUIExtensionCallback(pid);
}

/**
 * @tc.name: CheckOnUIExtensionCallback002
 * @tc.desc: test RegisterUIExtensionCallback when callback is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSMainThreadTest, CheckOnUIExtensionCallback002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    uint64_t userId = 0;
    pid_t pid = 0;
    sptr<RSIUIExtensionCallback> callback = nullptr;
    mainThread->RegisterUIExtensionCallback(pid, userId, callback);
    mainThread->UIExtensionNodesTraverseAndCallback();
    mainThread->UnRegisterUIExtensionCallback(pid);
}

/**
 * @tc.name: CheckUIExtensionCallbackDataChanged001
 * @tc.desc: test CheckUIExtensionCallbackDataChanged, no need to callback (2 frames of empty callback data)
 * @tc.type: FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSMainThreadTest, UIExtensionNodesTraverseAndCallback001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->lastFrameUIExtensionDataEmpty_ = true;
    mainThread->uiExtensionCallbackData_.clear();
    ASSERT_FALSE(mainThread->CheckUIExtensionCallbackDataChanged());
}

/**
 * @tc.name: CheckUIExtensionCallbackDataChanged002
 * @tc.desc: test CheckUIExtensionCallbackDataChanged, first frame of empty callbackdata, need to callback
 * @tc.type: FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSMainThreadTest, CheckUIExtensionCallbackDataChanged002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->lastFrameUIExtensionDataEmpty_ = false;
    mainThread->uiExtensionCallbackData_.clear();
    ASSERT_TRUE(mainThread->CheckUIExtensionCallbackDataChanged());
}

/**
 * @tc.name: UpdateUIFirstSwitch001
 * @tc.desc: UpdateUIFirstSwitch Test, root node nullptr
 * @tc.type: FUNC
 * @tc.require: issueIB7P9Z
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
 * @tc.require: issueIB7P9Z
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
 * @tc.require: issueIB7P9Z
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
 * @tc.name: UpdateDisplayNodeScreenId001
 * @tc.desc: UpdateDisplayNodeScreenId, when rootnode is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIB7P9Z
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
 * @tc.name: CheckAndUpdateInstanceContentStaticStatus003
 * @tc.desc: CheckAndUpdateInstanceContentStaticStatus Test, nullptr
 * @tc.type: FUNC
 * @tc.require: issueIB7P9Z
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
 * @tc.require: issueIB7P9Z
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
 * @tc.name: UpdateDisplayNodeScreenId002
 * @tc.desc: UpdateDisplayNodeScreenId, root node has no child display node.
 * @tc.type: FUNC
 * @tc.require: issueIB7P9Z
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
    
    NodeId nodeId = node->GetId();
    pid_t pid = ExtractPid(nodeId);
    mainThread->context_->nodeMap.renderNodeMap_[pid][nodeId] = node;
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
    NodeId nodeId = node->GetId();
    pid_t pid = ExtractPid(nodeId);
    mainThread->context_->nodeMap.renderNodeMap_[pid][nodeId] = node;
    NodeId parentNodeId = parentNode->GetId();
    pid_t parentNodePid = ExtractPid(parentNodeId);
    mainThread->context_->nodeMap.renderNodeMap_[parentNodePid][parentNodeId] = parentNode;
    mainThread->UpdateNeedDrawFocusChange(id);
    ASSERT_TRUE(node->GetNeedDrawFocusChange());
}

/**
 * @tc.name: IsOcclusionNodesNeedSync001
 * @tc.desc: test IsOcclusionNodesNeedSync001
 * @tc.type: FUNC
 * @tc.require: issueIB9A0P
 */
HWTEST_F(RSMainThreadTest, IsOcclusionNodesNeedSync001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->nodeTreeDumpTasks_.clear();
    NodeId nodeId = 1;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    node->isFullChildrenListValid_ = false;
    RSRenderNodeMap& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);
    ASSERT_TRUE(mainThread->IsOcclusionNodesNeedSync(nodeId, true));
}

/**
 * @tc.name: IsOcclusionNodesNeedSync002
 * @tc.desc: test IsOcclusionNodesNeedSync002
 * @tc.type: FUNC
 * @tc.require: issueIB9A0P
 */
HWTEST_F(RSMainThreadTest, IsOcclusionNodesNeedSync002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->nodeTreeDumpTasks_.clear();
    NodeId nodeId = 1;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    RSRenderNodeMap& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);
    ASSERT_TRUE(mainThread->IsOcclusionNodesNeedSync(nodeId, true));
}

/**
 * @tc.name: GetDynamicRefreshRate002
 * @tc.desc: test GetDynamicRefreshRate, refreshRate = 0
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, GetDynamicRefreshRate_002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_EQ(mainThread->GetDynamicRefreshRate(), OHOS::Rosen::STANDARD_REFRESH_RATE);
}

/**
 * @tc.name: SetFrameIsRender
 * @tc.desc: test SetFrameIsRender001, rsVSyncDistributor_ = nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, SetFrameIsRender_001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto rsVSyncDistributor = mainThread->rsVSyncDistributor_;
    mainThread->rsVSyncDistributor_ = nullptr;
    mainThread->SetFrameIsRender(true);
    mainThread->rsVSyncDistributor_ = rsVSyncDistributor;
}

/**
 * @tc.name: SetFrameIsRender
 * @tc.desc: test SetFrameIsRender002, rsVSyncDistributor_ != nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, SetFrameIsRender_002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    auto vsyncGenerator = CreateVSyncGenerator();
    auto vsyncController = new VSyncController(vsyncGenerator, 0);
    mainThread->rsVSyncDistributor_ = new VSyncDistributor(vsyncController, "rs");
    mainThread->SetFrameIsRender(true);
}

/**
 * @tc.name: RSJankStatsOnVsyncStart
 * @tc.desc: test RSJankStatsOnVsyncStart001, isUnirender_ = false & true
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, RSJankStatsOnVsyncStart_001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->RSJankStatsOnVsyncStart(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->isUniRender_ = true;
    mainThread->RSJankStatsOnVsyncStart(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: RSJankStatsOnVsyncStart
 * @tc.desc: test RSJankStatsOnVsyncStart002, renderThreadParams_ = false & true
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, RSJankStatsOnVsyncStart_002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    mainThread->renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    mainThread->RSJankStatsOnVsyncStart(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->renderThreadParams_ = nullptr;
    mainThread->RSJankStatsOnVsyncStart(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: RSJankStatsOnVsyncEnd
 * @tc.desc: test RSJankStatsOnVsyncEnd001, isUniRender_ = true & doDirectComposition_ = true
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, RSJankStatsOnVsyncEnd_001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    auto doDirectComposition = mainThread->doDirectComposition_;
    mainThread->isUniRender_ = true;
    mainThread->doDirectComposition_ = true;
    mainThread->RSJankStatsOnVsyncEnd(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->isUniRender_ = isUniRender;
    mainThread->doDirectComposition_ = doDirectComposition;
}

/**
 * @tc.name: RSJankStatsOnVsyncEnd
 * @tc.desc: test RSJankStatsOnVsyncEnd002, isUniRender_ || doDirectComposition_ exist false
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, RSJankStatsOnVsyncEnd_002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    auto doDirectComposition = mainThread->doDirectComposition_;
    mainThread->isUniRender_ = false;
    mainThread->doDirectComposition_ = true;
    mainThread->RSJankStatsOnVsyncEnd(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->isUniRender_ = true;
    mainThread->doDirectComposition_ = false;
    mainThread->RSJankStatsOnVsyncEnd(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->isUniRender_ = false;
    mainThread->doDirectComposition_ = false;
    mainThread->RSJankStatsOnVsyncEnd(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->isUniRender_ = isUniRender;
    mainThread->doDirectComposition_ = doDirectComposition;
}

/**
 * @tc.name: OnUniRenderDraw
 * @tc.desc: test OnUniRenderDraw001, test isUniRender_ & doDirectComposition_ = false
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, OnUniRenderDraw001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    auto doDirectComposition = mainThread->doDirectComposition_;
    mainThread->isUniRender_ = false;
    mainThread->OnUniRenderDraw();
    mainThread->isUniRender_ = true;
    mainThread->doDirectComposition_ = false;
    mainThread->drawFrame_.rsParallelType_ = RsParallelType::RS_PARALLEL_TYPE_SYNC;
    mainThread->OnUniRenderDraw();
    mainThread->isUniRender_ = isUniRender;
    mainThread->doDirectComposition_ = doDirectComposition;
}

/**
 * @tc.name: OnUniRenderDraw
 * @tc.desc: test OnUniRenderDraw002, doDirectComposition_ = true
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, OnUniRenderDraw002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    auto doDirectComposition = mainThread->doDirectComposition_;
    mainThread->isUniRender_ = true;
    mainThread->doDirectComposition_ = true;
    mainThread->OnUniRenderDraw();
    mainThread->isUniRender_ = isUniRender;
    mainThread->doDirectComposition_ = doDirectComposition;
}

/**
 * @tc.name: ProcessDataBySingleFrameComposer
 * @tc.desc: test ProcessDataBySingleFrameComposer001, rsTransactionData != nullptr & isUniRender_ = ture & false
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, ProcessDataBySingleFrameComposer001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    mainThread->isUniRender_ = false;
    mainThread->ProcessDataBySingleFrameComposer(rsTransactionData);
    mainThread->isUniRender_ = true;
    mainThread->ProcessDataBySingleFrameComposer(rsTransactionData);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ProcessDataBySingleFrameComposer
 * @tc.desc: test ProcessDataBySingleFrameComposer002, rsTransactionData = nullptr & isUniRender_ = ture & false
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, ProcessDataBySingleFrameComposer002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    std::unique_ptr<RSTransactionData> rsTransactionData = nullptr;
    mainThread->isUniRender_ = false;
    mainThread->ProcessDataBySingleFrameComposer(rsTransactionData);
    mainThread->isUniRender_ = true;
    mainThread->ProcessDataBySingleFrameComposer(rsTransactionData);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: IsHardwareEnabledNodesNeedSync
 * @tc.desc: test IsHardwareEnabledNodesNeedSync
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, IsHardwareEnabledNodesNeedSync, TestSize.Level2)
{
    NodeId id = 1;
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    mainThread->hardwareEnabledNodes_.emplace_back(nullptr);
    ASSERT_EQ(mainThread->IsHardwareEnabledNodesNeedSync(), false);

    mainThread->hardwareEnabledNodes_.clear();
    mainThread->doDirectComposition_ = false;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    node1->stagingRenderParams_ = nullptr;
    mainThread->hardwareEnabledNodes_.emplace_back(node1);
    ASSERT_EQ(mainThread->IsHardwareEnabledNodesNeedSync(), false);

    mainThread->hardwareEnabledNodes_.clear();
    mainThread->doDirectComposition_ = false;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id + 1, mainThread->context_);
    node2->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id + 2);
    node2->stagingRenderParams_->SetNeedSync(true);
    mainThread->hardwareEnabledNodes_.emplace_back(node2);
    ASSERT_EQ(mainThread->IsHardwareEnabledNodesNeedSync(), true);

    mainThread->hardwareEnabledNodes_.clear();
    mainThread->doDirectComposition_ = true;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(id + 3, mainThread->context_);
    node3->SetHardwareForcedDisabledState(false);
    RectI dstRect{0, 0, 400, 600};
    node3->SetDstRect(dstRect);
    mainThread->hardwareEnabledNodes_.emplace_back(node3);
    ASSERT_EQ(mainThread->IsHardwareEnabledNodesNeedSync(), true);
}

/**
 * @tc.name: IsSingleDisplay
 * @tc.desc: test IsSingleDisplay, rootNode = nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, IsSingleDisplay, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    ASSERT_EQ(mainThread->IsSingleDisplay(), true);
    mainThread->context_->globalRootRenderNode_ = nullptr;
    ASSERT_EQ(mainThread->IsSingleDisplay(), false);
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: HasMirrorDisplay
 * @tc.desc: test HasMirrorDisplay001, rootNode = nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, HasMirrorDisplay001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    ASSERT_NE(rootNode, nullptr);
    mainThread->context_->globalRootRenderNode_ = nullptr;
    ASSERT_EQ(mainThread->HasMirrorDisplay(), false);
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: HasMirrorDisplay
 * @tc.desc: test HasMirrorDisplay002, rootNode = nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, HasMirrorDisplay002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    NodeId id = 1;
    auto node1 = std::make_shared<RSRenderNode>(id);
    id++;
    RSDisplayNodeConfig config;
    auto displayNode1 = std::make_shared<RSDisplayRenderNode>(id, config);
    id++;
    node1->AddChild(displayNode1);
    node1->GenerateFullChildrenList();
    ASSERT_TRUE(node1->GetChildrenCount() <= 1);
    mainThread->context_->globalRootRenderNode_ = node1;
    ASSERT_EQ(mainThread->HasMirrorDisplay(), false);
    auto displayNode2 = std::make_shared<RSDisplayRenderNode>(id, config);
    id++;
    auto mirrorSourceNode = std::make_shared<RSDisplayRenderNode>(id, config);
    displayNode2->isMirroredDisplay_ = true;
    displayNode2->SetMirrorSource(mirrorSourceNode);
    node1->AddChild(displayNode2);
    node1->GenerateFullChildrenList();
    ASSERT_TRUE(node1->GetChildrenCount() > 1);
    mainThread->context_->globalRootRenderNode_ = node1;
    ASSERT_EQ(mainThread->HasMirrorDisplay(), true);
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: ResetAnimateNodeFlag
 * @tc.desc: test ResetAnimateNodeFlag, context_ = nullptr & !nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, ResetAnimateNodeFlag, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    auto context = mainThread->context_;
    mainThread->ResetAnimateNodeFlag();
    mainThread->context_ = nullptr;
    mainThread->ResetAnimateNodeFlag();
    mainThread->context_ = context;
}

/**
 * @tc.name: OnDrawingCacheDfxSwitchCallback
 * @tc.desc: test OnDrawingCacheDfxSwitchCallback
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, OnDrawingCacheDfxSwitchCallback, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RSMainThread::OnDrawingCacheDfxSwitchCallback("persist", "1", nullptr);
    RSMainThread::OnDrawingCacheDfxSwitchCallback("rosen.drawingCache.enabledDfx", "1", nullptr);
}

/**
 * @tc.name: SendClientDumpNodeTreeCommands
 * @tc.desc: test SendClientDumpNodeTreeCommands
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, SendClientDumpNodeTreeCommands, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->nodeTreeDumpTasks_.clear();

    NodeId testId = 1;
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(testId++, RSDisplayNodeConfig {});
    rootNode->AddChild(displayNode);
    auto node1 = std::make_shared<RSRenderNode>(testId++);
    displayNode->AddChild(node1);
    rootNode->GenerateFullChildrenList();
    displayNode->GenerateFullChildrenList();

    auto node2 = std::make_shared<RSSurfaceRenderNode>(testId++, mainThread->context_);
    node1->AddChild(node2);
    auto node3 = std::make_shared<RSRootRenderNode>(testId++, mainThread->context_);
    node2->AddChild(node3);
    node1->GenerateFullChildrenList();
    node2->GenerateFullChildrenList();
    node3->SetIsOnTheTree(true);
    mainThread->context_->GetMutableNodeMap().FilterNodeByPid(0);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);

    uint32_t taskId = 0;
    sptr<ApplicationAgentImpl> agent = new ApplicationAgentImpl();
    mainThread->RegisterApplicationAgent(0, agent);
    mainThread->SendClientDumpNodeTreeCommands(taskId);
    ASSERT_TRUE(!mainThread->nodeTreeDumpTasks_.empty());
    ASSERT_TRUE(mainThread->nodeTreeDumpTasks_[taskId].count > 0);

    mainThread->SendClientDumpNodeTreeCommands(taskId);
    rootNode->RemoveChild(displayNode);
}

/**
 * @tc.name: CollectClientNodeTreeResult
 * @tc.desc: test CollectClientNodeTreeResult
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, CollectClientNodeTreeResult, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->nodeTreeDumpTasks_.clear();

    uint32_t taskId = 0;
    auto& task = mainThread->nodeTreeDumpTasks_[taskId];
    task.data[0] = "testData";
    task.count++;

    std::string out;
    mainThread->CollectClientNodeTreeResult(taskId, out, 1);
    ASSERT_TRUE(!out.empty());
    ASSERT_TRUE(mainThread->nodeTreeDumpTasks_.empty());
}

/**
 * @tc.name: OnCommitDumpClientNodeTree
 * @tc.desc: test OnCommitDumpClientNodeTree
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, OnCommitDumpClientNodeTree, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->nodeTreeDumpTasks_.clear();

    uint32_t taskId = 0;
    mainThread->OnCommitDumpClientNodeTree(0, 0, taskId, "testData");
    ASSERT_TRUE(mainThread->nodeTreeDumpTasks_.empty());

    auto& task = mainThread->nodeTreeDumpTasks_[taskId];
    task.count++;

    mainThread->OnCommitDumpClientNodeTree(0, 0, taskId, "testData");
    mainThread->OnCommitDumpClientNodeTree(0, 0, taskId, "testData");
    ASSERT_TRUE(!mainThread->nodeTreeDumpTasks_.empty());
    ASSERT_TRUE(!mainThread->nodeTreeDumpTasks_[taskId].data.empty());
}

/**
 * @tc.name: TraverseCanvasDrawingNodesNotOnTree
 * @tc.desc: test TraverseCanvasDrawingNodesNotOnTree
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, TraverseCanvasDrawingNodesNotOnTree, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->TraverseCanvasDrawingNodesNotOnTree();
}

/**
 * @tc.name: CheckIsAihdrSurface
 * @tc.desc: Test CheckIsAihdrSurface
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
#ifdef USE_VIDEO_PROCESSING_ENGINE
HWTEST_F(RSMainThreadTest, CheckIsAihdrSurface, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->context_->activeNodesInRoot_.clear();
    // valid nodeid
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node, nullptr);
    const auto& surfaceBuffer = node->GetRSSurfaceHandler()->GetBuffer();
    if (surfaceBuffer == nullptr) {
        return;
    }
    uint32_t hdrType = HDI::Display::Graphic::Common::V2_1::CM_VIDEO_AI_HDR;
    std::vector<uint8_t> metadataType;
    metadataType.resize(sizeof(hdrType));
    memcpy_s(metadataType.data(), metadataType.size(), &hdrType, sizeof(hdrType));
    surfaceBuffer->SetMetadata(Media::VideoProcessingEngine::ATTRKEY_HDR_METADATA_TYPE,
        metadataType);
    ASSERT_EQ(mainThread->CheckIsAihdrSurface(*node), true);
}
#endif

/**
 * @tc.name: RenderServiceAllNodeDump01
 * @tc.desc: RenderServiceAllNodeDump Test
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
HWTEST_F(RSMainThreadTest, RenderServiceAllNodeDump01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 1;
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
    DfxString log;
    mainThread->RenderServiceAllNodeDump(log);
}

/**
 * @tc.name: ConnectChipsetVsyncSer
 * @tc.desc: test ConnectChipsetVsyncSer
 * @tc.type: FUNC
 * @tc.require: issuesIBCE6Y
 */
#if defined(RS_ENABLE_CHIPSET_VSYNC)
HWTEST_F(RSMainThreadTest, ConnectChipsetVsyncSer, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->initVsyncServiceFlag_ = false;
    mainThread->ConnectChipsetVsyncSer();
    ASSERT_EQ(mainThread->initVsyncServiceFlag_, false);
    mainThread->initVsyncServiceFlag_ = true;
    mainThread->ConnectChipsetVsyncSer();
    ASSERT_EQ(mainThread->initVsyncServiceFlag_, true);
}
#endif

/**
 * @tc.name: IsOcclusionNodesNeedSync003
 * @tc.desc: test IsOcclusionNodesNeedSync003
 * @tc.type: FUNC
 * @tc.require: issueIB9A0P
 */
HWTEST_F(RSMainThreadTest, IsOcclusionNodesNeedSync003, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->nodeTreeDumpTasks_.clear();
    NodeId nodeId = 1;
    ASSERT_FALSE(mainThread->IsOcclusionNodesNeedSync(nodeId, true));
}

/**
 * @tc.name: IsOcclusionNodesNeedSync004
 * @tc.desc: test IsOcclusionNodesNeedSync004
 * @tc.type: FUNC
 * @tc.require: issueIB9A0P
 */
HWTEST_F(RSMainThreadTest, IsOcclusionNodesNeedSync004, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->nodeTreeDumpTasks_.clear();
    NodeId nodeId = 1;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    RSRenderNodeMap& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);
    ASSERT_FALSE(mainThread->IsOcclusionNodesNeedSync(nodeId, false));
}

/**
 * @tc.name: ProcessHgmFrameRate001
 * @tc.desc: test ProcessHgmFrameRate
 * @tc.type: FUNC
 * @tc.require: issueIB9A0P
 */
HWTEST_F(RSMainThreadTest, ProcessHgmFrameRate001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    uint64_t timestamp = 0;
    mainThread->ProcessHgmFrameRate(timestamp);

    auto vsyncGenerator = CreateVSyncGenerator();
    auto vsyncController = new VSyncController(vsyncGenerator, 0);
    mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    FrameRateLinkerId id = 0;
    mainThread->rsFrameRateLinker_ = std::make_shared<RSRenderFrameRateLinker>(id);
    mainThread->ProcessHgmFrameRate(timestamp);

    mainThread->rsVSyncDistributor_ = nullptr;
    mainThread->rsFrameRateLinker_ = nullptr;
    mainThread->ProcessHgmFrameRate(timestamp);
    sleep(1);
}

/**
 * @tc.name: SetCurtainScreenUsingStatus001
 * @tc.desc: test SetCurtainScreenUsingStatus
 * @tc.type: FUNC
 * @tc.require: issueIB9A0P
 */
HWTEST_F(RSMainThreadTest, SetCurtainScreenUsingStatus001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isCurtainScreenOn = mainThread->isCurtainScreenOn_;

    mainThread->isCurtainScreenOn_ = true;
    mainThread->SetCurtainScreenUsingStatus(true);

    mainThread->isCurtainScreenOn_ = false;
    mainThread->SetCurtainScreenUsingStatus(true);
    ASSERT_EQ(mainThread->isCurtainScreenOn_, true);

    mainThread->SetCurtainScreenUsingStatus(isCurtainScreenOn);
}

/**
 * @tc.name: CountMem001
 * @tc.desc: test CountMem
 * @tc.type: FUNC
 * @tc.require: issueIB9A0P
 */
HWTEST_F(RSMainThreadTest, CountMem001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    auto context = mainThread->context_;
    mainThread->context_ = nullptr;
    std::vector<MemoryGraphic> memoryGraphic;

    mainThread->CountMem(memoryGraphic);
    mainThread->context_ = context;
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
    NodeId nodeId = node->GetId();
    pid_t pid = ExtractPid(nodeId);
    mainThread->context_->nodeMap.renderNodeMap_[pid][nodeId] = node;
    NodeId parentNodeId = parentNode->GetId();
    pid_t parentNodePid = ExtractPid(parentNodeId);
    mainThread->context_->nodeMap.renderNodeMap_[parentNodePid][parentNodeId] = parentNode;
    mainThread->UpdateNeedDrawFocusChange(id);
    ASSERT_FALSE(node->GetNeedDrawFocusChange());
}

/**
 * @tc.name: UpdateDisplayNodeScreenId003
 * @tc.desc: UpdateDisplayNodeScreenId, root node has one child display node.
 * @tc.type: FUNC
 * @tc.require: issueIB7P9Z
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
 * @tc.name: SetIsCachedSurfaceUpdated01
 * @tc.desc: Test RsMainThreadTest.SetIsCachedSurfaceUpdated
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RsMainThreadTest, SetIsCachedSurfaceUpdated01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetIsCachedSurfaceUpdated(true);
}


/**
 * @tc.name: RSJankStatsOnVsyncStart
 * @tc.desc: test RSJankStatsOnVsyncStart001, isUnirender_ = false & true
 * @tc.type: FUNC
 * @tc.require: issueIAIZ2Y
 */
HWTEST_F(RSMainThreadTest, RSJankStatsOnVsyncStart001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->RSJankStatsOnVsyncStart(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->isUniRender_ = true;
    mainThread->RSJankStatsOnVsyncStart(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: RSJankStatsOnVsyncStart
 * @tc.desc: test RSJankStatsOnVsyncStart002, renderThreadParams_ = false & true
 * @tc.type: FUNC
 * @tc.require: issueIAIZ2Y
 */
HWTEST_F(RSMainThreadTest, RSJankStatsOnVsyncStart002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    mainThread->renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    mainThread->RSJankStatsOnVsyncStart(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->renderThreadParams_ = nullptr;
    mainThread->RSJankStatsOnVsyncStart(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: RSJankStatsOnVsyncEnd
 * @tc.desc: test RSJankStatsOnVsyncEnd001, isUniRender_ = true & doDirectComposition_ = true
 * @tc.type: FUNC
 * @tc.require: issueIAIZ2Y
 */
HWTEST_F(RSMainThreadTest, RSJankStatsOnVsyncEnd001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    auto doDirectComposition = mainThread->doDirectComposition_;
    mainThread->isUniRender_ = true;
    mainThread->doDirectComposition_ = true;
    mainThread->RSJankStatsOnVsyncEnd(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->isUniRender_ = isUniRender;
    mainThread->doDirectComposition_ = doDirectComposition;
}

/**
 * @tc.name: RSJankStatsOnVsyncEnd
 * @tc.desc: test RSJankStatsOnVsyncEnd002, isUniRender_ || doDirectComposition_ exist false
 * @tc.type: FUNC
 * @tc.require: issueIAIZ2Y
 */
HWTEST_F(RSMainThreadTest, RSJankStatsOnVsyncEnd002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    auto doDirectComposition = mainThread->doDirectComposition_;
    mainThread->isUniRender_ = false;
    mainThread->doDirectComposition_ = true;
    mainThread->RSJankStatsOnVsyncEnd(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->isUniRender_ = true;
    mainThread->doDirectComposition_ = false;
    mainThread->RSJankStatsOnVsyncEnd(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->isUniRender_ = false;
    mainThread->doDirectComposition_ = false;
    mainThread->RSJankStatsOnVsyncEnd(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    mainThread->isUniRender_ = isUniRender;
    mainThread->doDirectComposition_ = doDirectComposition;
}

/**
 * @tc.name: ProcessDataBySingleFrameComposer
 * @tc.desc: test ProcessDataBySingleFrameComposer001, rsTransactionData != nullptr & isUniRender_ = ture & false
 * @tc.type: FUNC
 * @tc.require: issueIB9A0P
 */
HWTEST_F(RSMainThreadTest, ProcessDataBySingleFrameComposer001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    mainThread->isUniRender_ = false;
    mainThread->ProcessDataBySingleFrameComposer(rsTransactionData);
    mainThread->isUniRender_ = true;
    mainThread->ProcessDataBySingleFrameComposer(rsTransactionData);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ProcessDataBySingleFrameComposer
 * @tc.desc: test ProcessDataBySingleFrameComposer002, rsTransactionData = nullptr & isUniRender_ = ture & false
 * @tc.type: FUNC
 * @tc.require: issueIB9A0P
 */
HWTEST_F(RSMainThreadTest, ProcessDataBySingleFrameComposer002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    std::unique_ptr<RSTransactionData> rsTransactionData = nullptr;
    mainThread->isUniRender_ = false;
    mainThread->ProcessDataBySingleFrameComposer(rsTransactionData);
    mainThread->isUniRender_ = true;
    mainThread->ProcessDataBySingleFrameComposer(rsTransactionData);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: PrintCurrentStatus01
 * @tc.desc: Test RsMainThreadTest.PrintCurrentStatus
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RsMainThreadTest, PrintCurrentStatus01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->PrintCurrentStatus();
}

/**
 * @tc.name: SetDeviceType01
 * @tc.desc: Test RsMainThreadTest.SetDeviceType
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RsMainThreadTest, SetDeviceType01, TestSize.Level1)
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
 * @tc.name: CacheCommands01
 * @tc.desc: Test RsMainThreadTest.CacheCommands
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RsMainThreadTest, CacheCommands01, TestSize.Level1)
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
 * @tc.name: CheckParallelSubThreadNodesStatus01
 * @tc.desc: Test RsMainThreadTest.CheckParallelSubThreadNodesStatus
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RsMainThreadTest, CheckParallelSubThreadNodesStatus01, TestSize.Level1)
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
 * @tc.name: IsNeedSkip01
 * @tc.desc: IsNeedSkip test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, IsNeedSkip01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->IsNeedSkip(0, 0);
}

/**
 * @tc.name: UpdateFocusNodeId001
 * @tc.desc: test UpdateFocusNodeId while focusNodeId don't change
 * @tc.type: FUNC
 * @tc.require: issueIB9A0P
 */
HWTEST_F(RSMainThreadTest, UpdateFocusNodeId001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node, nullptr);

    ASSERT_NE(mainThread->context_, nullptr);
    NodeId nodeId = node->GetId();
    pid_t pid = ExtractPid(nodeId);
    mainThread->context_->nodeMap.renderNodeMap_[pid][nodeId] = node;
    mainThread->focusNodeId_ = id;
    mainThread->UpdateFocusNodeId(id);
    ASSERT_EQ(mainThread->GetFocusNodeId(), id);
}

/**
 * @tc.name: UpdateFocusNodeId002
 * @tc.desc: test UpdateFocusNodeId while newfocusNodeId is invalid
 * @tc.type: FUNC
 * @tc.require: issueIB9A0P
 */
HWTEST_F(RSMainThreadTest, UpdateFocusNodeId002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node, nullptr);

    ASSERT_NE(mainThread->context_, nullptr);
    NodeId nodeId = node->GetId();
    pid_t pid = ExtractPid(nodeId);
    mainThread->context_->nodeMap.renderNodeMap_[pid][nodeId] = node;
    mainThread->focusNodeId_ = id;
    mainThread->UpdateFocusNodeId(INVALID_NODEID);
    ASSERT_EQ(mainThread->GetFocusNodeId(), id);
}

/**
 * @tc.name: UpdateFocusNodeId003
 * @tc.desc: test UpdateFocusNodeId while focus node change
 * @tc.type: FUNC
 * @tc.require: issueIB9A0P
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

    NodeId oldFocusNodeId = oldFocusNode->GetId();
    pid_t oldFocusNodePid = ExtractPid(oldFocusNodeId);
    mainThread->context_->nodeMap.renderNodeMap_[oldFocusNodePid][oldFocusNodeId] = oldFocusNode;
    NodeId newFocusNodeId = newFocusNode->GetId();
    pid_t newFocusNodePid = ExtractPid(newFocusNodeId);
    mainThread->context_->nodeMap.renderNodeMap_[newFocusNodePid][newFocusNodeId] = newFocusNode;
    
    mainThread->focusNodeId_ = oldFocusNode->GetId();
    mainThread->UpdateFocusNodeId(newFocusNode->GetId());
    ASSERT_EQ(mainThread->GetFocusNodeId(), newFocusNode->GetId());
}

/**
 * @tc.name: UiCaptureTasks002
 * @tc.desc: test UiCaptureTasks processing
 * @tc.type: FUNC
 * @tc.require: issueIB9A0P
 */
HWTEST_F(RSMainThreadTest, UiCaptureTasks002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    auto node1 = RSTestUtil::CreateSurfaceNode();
    auto node2 = RSTestUtil::CreateSurfaceNode();
    auto task = []() {};

    mainThread->ProcessUiCaptureTasks();
    ASSERT_EQ(mainThread->pendingUiCaptureTasks_.empty(), true);

    mainThread->context_->nodeMap.RegisterRenderNode(node1);
    mainThread->AddUiCaptureTask(node1->GetId(), task);
    mainThread->AddUiCaptureTask(node2->GetId(), task);
    ASSERT_EQ(mainThread->pendingUiCaptureTasks_.empty(), false);
    ASSERT_EQ(mainThread->uiCaptureTasks_.empty(), true);

    mainThread->PrepareUiCaptureTasks(nullptr);
    ASSERT_EQ(mainThread->pendingUiCaptureTasks_.empty(), true);
    ASSERT_EQ(mainThread->uiCaptureTasks_.empty(), false);

    mainThread->ProcessUiCaptureTasks();
    ASSERT_EQ(mainThread->pendingUiCaptureTasks_.empty(), true);
    ASSERT_EQ(mainThread->uiCaptureTasks_.empty(), true);

    mainThread->context_->nodeMap.UnregisterRenderNode(node1->GetId());
}

/**
 * @tc.name: SkipCommandByNodeId01
 * @tc.desc: SkipCommandByNodeId test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SkipCommandByNodeId01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<std::unique_ptr<RSTransactionData>> transactionVec;
    mainThread->SkipCommandByNodeId(transactionVec, -1);
}

/**
 * @tc.name: SkipCommandByNodeId02
 * @tc.desc: SkipCommandByNodeId test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SkipCommandByNodeId02, TestSize.Level1)
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
 * @tc.name: SkipCommandByNodeId03
 * @tc.desc: SkipCommandByNodeId test cacheCmdSkippedInfo_ is empty
 * @tc.type: FUNC
 * @tc.require: issueIAS924
 */
HWTEST_F(RsMainThreadTest, SkipCommandByNodeId03, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<std::unique_ptr<RSTransactionData>> transactionVec;
    mainThread->cacheCmdSkippedInfo_.clear();
    mainThread->SkipCommandByNodeId(transactionVec, 0);
}

/**
 * @tc.name: ConsumeAndUpdateAllNodes01
 * @tc.desc: ConsumeAndUpdateAllNodes test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, ConsumeAndUpdateAllNodes01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->ConsumeAndUpdateAllNodes();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ConsumeAndUpdateAllNodes02
 * @tc.desc: ConsumeAndUpdateAllNodes test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, ConsumeAndUpdateAllNodes02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->ConsumeAndUpdateAllNodes();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: CheckSubThreadNodeStatusIsDoing01
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, CheckSubThreadNodeStatusIsDoing01, TestSize.Level1)
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
 * @tc.name: CheckSubThreadNodeStatusIsDoing02
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, CheckSubThreadNodeStatusIsDoing02, TestSize.Level1)
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
 * @tc.name: CollectInfoForHardwareComposer01
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, CollectInfoForHardwareComposer01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->CollectInfoForHardwareComposer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: CollectInfoForHardwareComposer02
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, CollectInfoForHardwareComposer02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    mainThread->CollectInfoForHardwareComposer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: IsLastFrameUIFirstEnbaled01
 * @tc.desc: IsLastFrameUIFirstEnbaled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, IsLastFrameUIFirstEnbaled01, TestSize.Level1)
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
 * @tc.name: IsLastFrameUIFirstEnbaled02
 * @tc.desc: IsLastFrameUIFirstEnbaled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, IsLastFrameUIFirstEnbaled02, TestSize.Level1)
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
 * @tc.name: CheckIfHardwareForcedDisabled01
 * @tc.desc: CheckIfHardwareForcedDisabled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, CheckIfHardwareForcedDisabled01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->CheckIfHardwareForcedDisabled();
}

/**
 * @tc.name: CheckIfHardwareForcedDisabled02
 * @tc.desc: CheckIfHardwareForcedDisabled02 test rootNode = nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAS924
 */
HWTEST_F(RsMainThreadTest, CheckIfHardwareForcedDisabled02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    mainThread->context_->globalRootRenderNode_ = nullptr;
    mainThread->CheckIfHardwareForcedDisabled();
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: CheckIfHardwareForcedDisabled03
 * @tc.desc: CheckIfHardwareForcedDisabled03 test child = nullptr and type != DISPLAY_NODE
 * @tc.type: FUNC
 * @tc.require: issueIAS924
 */
HWTEST_F(RsMainThreadTest, CheckIfHardwareForcedDisabled03, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    auto node1 = std::make_shared<RSRenderNode>(1, true);
    std::shared_ptr<RSRenderNode> node2 = nullptr;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(2, mainThread->context_);
    node3->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto node4 = std::make_shared<RSRenderNode>(3, true);
    node1->children_.emplace_back(node2);
    node1->AddChild(node3);
    node1->AddChild(node4);
    mainThread->context_->globalRootRenderNode_ = node1;
    mainThread->CheckIfHardwareForcedDisabled();
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: WaitUntilUnmarshallingTaskFinished01
 * @tc.desc: WaitUntilUnmarshallingTaskFinished test, divided render, the func call will return immediately.
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, WaitUntilUnmarshallingTaskFinished01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->WaitUntilUnmarshallingTaskFinished();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: MergeToEffectiveTransactionDataMap01
 * @tc.desc: MergeToEffectiveTransactionDataMap test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, MergeToEffectiveTransactionDataMap01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    TransactionDataMap tsDataMap;
    tsDataMap.emplace(0, std::vector<std::unique_ptr<RSTransactionData>>());
    auto data = std::make_unique<RSTransactionData>();
    data->SetIndex(1);
    tsDataMap[0].emplace_back(std::move(data));
    mainThread->MergeToEffectiveTransactionDataMap(tsDataMap);
    ASSERT_TRUE(tsDataMap.empty());
}

/**
 * @tc.name: UniRender01
 * @tc.desc: UniRender test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, UniRender01, TestSize.Level1)
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
 * @tc.name: UniRender02
 * @tc.desc: UniRender test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, UniRender02, TestSize.Level1)
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
 * @tc.name: IsFirstFrameOfOverdrawSwitch01
 * @tc.desc: test IsFirstFrameOfOverdrawSwitch
 * @tc.type: FUNC
 * @tc.require: issueIAKQC3
 */
HWTEST_F(RsMainThreadTest, IsFirstFrameOfOverdrawSwitch01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->isOverDrawEnabledOfCurFrame_ = true;
    ASSERT_TRUE(mainThread->IsFirstFrameOfOverdrawSwitch());
}

/**
 * @tc.name: GetRealTimeOffsetOfDvsync01
 * @tc.desc: test GetRealTimeOffsetOfDvsync
 * @tc.type: FUNC
 * @tc.require: issueIAXG6P
 */
HWTEST_F(RsMainThreadTest, GetRealTimeOffsetOfDvsync01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    int64_t time = 1000;
    mainThread->GetRealTimeOffsetOfDvsync(time);
}

/**
 * @tc.name: Render01
 * @tc.desc: Render test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, Render01, TestSize.Level1)
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
 * @tc.name: Render
 * @tc.desc: Render02 test rootNode = nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAS924
 */
HWTEST_F(RsMainThreadTest, Render02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    mainThread->context_->globalRootRenderNode_ = nullptr;
    mainThread->Render();
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: CallbackDrawContextStatusToWMS01
 * @tc.desc: CallbackDrawContextStatusToWMS test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, CallbackDrawContextStatusToWMS01, TestSize.Level1)
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
 * @tc.name: CheckSurfaceNeedProcess01
 * @tc.desc: CheckSurfaceNeedProcess test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, CheckSurfaceNeedProcess01, TestSize.Level1)
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
 * @tc.name: GetRegionVisibleLevel01
 * @tc.desc: GetRegionVisibleLevel test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, GetRegionVisibleLevel01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // empty region
    Occlusion::Region occRegion;
    mainThread->GetRegionVisibleLevel(occRegion, occRegion);
}

/**
 * @tc.name: GetRegionVisibleLevel02
 * @tc.desc: GetRegionVisibleLevel test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, GetRegionVisibleLevel02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // equal region
    Occlusion::Region occRegion;
    occRegion.rects_.emplace_back(Occlusion::Rect(0, 1, 1, 0));
    mainThread->GetRegionVisibleLevel(occRegion, occRegion);
}

/**
 * @tc.name: GetRegionVisibleLevel03
 * @tc.desc: GetRegionVisibleLevel test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, GetRegionVisibleLevel03, TestSize.Level1)
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
 * @tc.name: CalcOcclusionImplementation01
 * @tc.desc: CalcOcclusionImplementation test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, CalcOcclusionImplementation01, TestSize.Level1)
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
 * @tc.name: CallbackToWMS01
 * @tc.desc: CallbackToWMS test visible not changed
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, CallbackToWMS01, TestSize.Level1)
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
 * @tc.name: CallbackToWMS02
 * @tc.desc: CallbackToWMS test visible changed
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, CallbackToWMS02, TestSize.Level1)
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
 * @tc.name: SurfaceOcclusionCallback01
 * @tc.desc: SurfaceOcclusionCallback with empty nodemap
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SurfaceOcclusionCallback01, TestSize.Level1)
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
 * @tc.name: SurfaceOcclusionCallback02
 * @tc.desc: SurfaceOcclusionCallback with corresponding nodemap
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SurfaceOcclusionCallback02, TestSize.Level1)
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
 * @tc.name: CalcOcclusion02
 * @tc.desc: CalcOcclusion Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, CalcOcclusion02, TestSize.Level1)
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
 * @tc.name: Aniamte01
 * @tc.desc: Aniamte Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, Aniamte01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 0;
    auto node = std::make_shared<RSRenderNode>(id);
    mainThread->context_->RegisterAnimatingRenderNode(node);
    mainThread->Animate(mainThread->timestamp_);
}

/**
 * @tc.name: RecvRSTransactionData02
 * @tc.desc: RecvRSTransactionData02 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, RecvRSTransactionData02, TestSize.Level1)
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
 * @tc.name: ConsumeAndUpdateAllNodes03
 * @tc.desc: ConsumeAndUpdateAllNodes03 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, ConsumeAndUpdateAllNodes03, TestSize.Level1)
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
 * @tc.name: ConsumeAndUpdateAllNodes04
 * @tc.desc: ConsumeAndUpdateAllNodes04 Test
 * @tc.type: FUNC
 * @tc.require: issueIANQPF
 */
HWTEST_F(RsMainThreadTest, ConsumeAndUpdateAllNodes04, TestSize.Level1)
{
#ifndef ROSEN_CROSS_PLATFORM
    if (!RSSystemProperties::GetUniRenderEnabled()) {
        return;
    }
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    mainThread->timestamp_ = 1000;
    // prepare nodemap
    mainThread->context_->GetMutableNodeMap().renderNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.clear();
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    rsSurfaceRenderNode->isHardwareEnabledNode_ = true;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(rsSurfaceRenderNode);
    auto rsSurfaceHandlerPtr_ = rsSurfaceRenderNode->GetRSSurfaceHandler();
    ASSERT_NE(rsSurfaceHandlerPtr_, nullptr);
    auto surfaceConsumer = rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer();
    ASSERT_NE(surfaceConsumer, nullptr);
    auto producer = surfaceConsumer->GetProducer();
    ASSERT_NE(producer, nullptr);
    sptr<Surface> psurf = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurf, nullptr);
    psurf->SetQueueSize(5);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    GSError ret = psurf->RequestBuffer(buffer, requestFence, requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);
    sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
    ret = psurf->FlushBuffer(buffer, flushFence, flushConfig);
    ASSERT_EQ(ret, GSERROR_OK);
    RSSurfaceHandler::SurfaceBufferEntry bufferEntry;
    bufferEntry.timestamp = 0;
    ret = psurf->RequestBuffer(bufferEntry.buffer, requestFence, requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);
    rsSurfaceHandlerPtr_->SetBufferTransformTypeChanged(true);
    ASSERT_TRUE(rsSurfaceHandlerPtr_->GetBufferTransformTypeChanged());
    mainThread->ConsumeAndUpdateAllNodes();
    mainThread->isUniRender_ = isUniRender;
#endif
}

/**
 * @tc.name: CollectInfoForHardwareComposer03
 * @tc.desc: CollectInfoForHardwareComposer03 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, CollectInfoForHardwareComposer03, TestSize.Level1)
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
 * @tc.name: ClassifyRSTransactionData05
 * @tc.desc: ClassifyRSTransactionData05 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, ClassifyRSTransactionData05, TestSize.Level1)
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
 * @tc.name: ReleaseAllNodesBuffer01
 * @tc.desc: ReleaseAllNodesBuffer Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, ReleaseAllNodesBuffer01, TestSize.Level1)
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
 * @tc.name: ReleaseAllNodesBuffer02
 * @tc.desc: ReleaseAllNodesBuffer002 Test preBuffer != nullptr;
 * @tc.type: FUNC
 * @tc.require: issueIAS924
 */
HWTEST_F(RsMainThreadTest, ReleaseAllNodesBuffer02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    // prepare nodemap
    mainThread->context_->GetMutableNodeMap().renderNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.clear();
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(config);
    node1->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node1->isHardwareEnabledNode_ = true;
    node1->isLastFrameHardwareEnabled_ = true;
    node1->isCurrentFrameHardwareEnabled_ = false;
    sptr<SurfaceBuffer> preBuffer = OHOS::SurfaceBuffer::Create();
    sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
    Rect damageRect = {0, 0, 0, 0};
    int64_t timestamp = 0;
    ASSERT_NE(node1->surfaceHandler_, nullptr);
    node1->surfaceHandler_->SetBuffer(preBuffer, acquireFence, damageRect, timestamp);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node1);
    mainThread->ReleaseAllNodesBuffer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: PostTask01
 * @tc.desc: PostTask Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, PostTask01, TestSize.Level1)
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
 * @tc.name: PostTask02
 * @tc.desc: PostTask Test with IMMEDIATE Priority
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, PostTask02, TestSize.Level1)
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
 * @tc.name: RemoveTask01
 * @tc.desc: RemoveTask Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, RemoveTask01, TestSize.Level1)
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
 * @tc.name: PostSyncTask02
 * @tc.desc: PostSyncTask Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, PostSyncTask02, TestSize.Level1)
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
 * @tc.name: RegisterSurfaceOcclusionChangeCallBack01
 * @tc.desc: RegisterSurfaceOcclusionChangeCallBack01 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, RegisterSurfaceOcclusionChangeCallBack01, TestSize.Level1)
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
 * @tc.name: RegisterSurfaceOcclusionChangeCallBack02
 * @tc.desc: RegisterSurfaceOcclusionChangeCallBack02 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, RegisterSurfaceOcclusionChangeCallBack02, TestSize.Level1)
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
 * @tc.name: ClearSurfaceOcclusionChangeCallBack01
 * @tc.desc: RegisterSurfaceOcclusionChangeCallBack Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, ClearSurfaceOcclusionChangeCallback01, TestSize.Level1)
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
 * @tc.name: SendCommands01
 * @tc.desc: SendCommands Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SendCommands01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SendCommands();
}

/**
 * @tc.name: SendCommands01
 * @tc.desc: SendCommands Test
 * @tc.type: FUNC
 */
HWTEST_F(RsMainThreadTest, SendCommands01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 1;
    AnimationId animationId = 1;
    mainThread->context_->AddSyncFinishAnimationList(id, animationId);
    mainThread->SendCommands();
}

/**
 * @tc.name: ClearTransactionDataPidInfo01
 * @tc.desc: ClearTransactionDataPidInfo Test, remotePid = 0
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, ClearTransactionDataPidInfo01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    int remotePid = 0;
    mainThread->ClearTransactionDataPidInfo(remotePid);
}

/**
 * @tc.name: ClearTransactionDataPidInfo02
 * @tc.desc: ClearTransactionDataPidInfo Test, remotePid > 0
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, ClearTransactionDataPidInfo02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    int remotePid = 1;
    mainThread->ClearTransactionDataPidInfo(remotePid);
}

/**
 * @tc.name: AddTransactionDataPidInfo01
 * @tc.desc: AddTransactionDataPidInfo Test, no UniRender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, AddTransactionDataPidInfo01, TestSize.Level1)
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
 * @tc.name: AddTransactionDataPidInfo02
 * @tc.desc: AddTransactionDataPidInfo Test, UniRender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, AddTransactionDataPidInfo02, TestSize.Level1)
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
 * @tc.name: PerfAfterAnim01
 * @tc.desc: PerfAfterAnim Test, not UniRender, needRequestNextVsync
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, PerfAfterAnim01, TestSize.Level1)
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
 * @tc.name: PerfAfterAnim02
 * @tc.desc: PerfAfterAnim Test, UniRender, needRequestNextVsync
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, PerfAfterAnim02, TestSize.Level1)
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
 * @tc.name: PerfAfterAnim03
 * @tc.desc: PerfAfterAnim Test, UniRender, not needRequestNextVsync
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, PerfAfterAnim03, TestSize.Level1)
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
 * @tc.name: ForceRefreshForUni01
 * @tc.desc: ForceRefreshForUni Test, UniRender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, ForceRefreshForUni01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    mainThread->ForceRefreshForUni();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ForceRefreshForUni02
 * @tc.desc: ForceRefreshForUni Test, without UniRender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, ForceRefreshForUni02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->ForceRefreshForUni();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: PerfForBlurIfNeeded01
 * @tc.desc: PerfForBlurIfNeeded Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, PerfForBlurIfNeeded01, TestSize.Level1)
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
 * @tc.name: PerfMultiWindow01
 * @tc.desc: PerfMultiWindow Test, not unirender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, PerfMultiWindow01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->PerfMultiWindow();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: PerfMultiWindow02
 * @tc.desc: PerfMultiWindow Test, unirender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, PerfMultiWindow02, TestSize.Level1)
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
 * @tc.name: RenderFrameStart01
 * @tc.desc: RenderFrameStart Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, RenderFrameStart01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->RenderFrameStart(mainThread->timestamp_);
}

/**
 * @tc.name: SetSystemAnimatedScenes01
 * @tc.desc: SetSystemAnimatedScenes Test, case set 1
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SetSystemAnimatedScenes01, TestSize.Level1)
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
 * @tc.name: SetSystemAnimatedScenes02
 * @tc.desc: SetSystemAnimatedScenes Test, case set 2
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SetSystemAnimatedScenes02, TestSize.Level1)
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
 * @tc.name: SetSystemAnimatedScenes03
 * @tc.desc: SetSystemAnimatedScenes Test, System Animated Scenes Disabled
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SetSystemAnimatedScenes03, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto systemAnimatedScenesEnabled = mainThread->systemAnimatedScenesEnabled_;
    mainThread->systemAnimatedScenesEnabled_ = false;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::OTHERS));
    mainThread->systemAnimatedScenesEnabled_ = systemAnimatedScenesEnabled;
}

/**
 * @tc.name: CheckNodeHasToBePreparedByPid01
 * @tc.desc: CheckNodeHasToBePreparedByPid Test, Classify By Root
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, CheckNodeHasToBePreparedByPid01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 1;
    bool isClassifyByRoot = true;
    mainThread->CheckNodeHasToBePreparedByPid(id, isClassifyByRoot);
}

/**
 * @tc.name: CheckNodeHasToBePreparedByPid02
 * @tc.desc: CheckNodeHasToBePreparedByPid Test, not Classify By Root
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, CheckNodeHasToBePreparedByPid02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 1;
    bool isClassifyByRoot = false;
    mainThread->CheckNodeHasToBePreparedByPid(id, isClassifyByRoot);
}

/**
 * @tc.name: SetVSyncRateByVisibleLevel01
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when RSVisibleLevel is RS_SEMI_DEFAULT_VISIBLE
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SetVSyncRateByVisibleLevel01, TestSize.Level1)
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
        mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
        mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
        ASSERT_NE(connection->highPriorityRate_, (int32_t)SIMI_VISIBLE_RATE);
    }
}

/**
 * @tc.name: SetVSyncRateByVisibleLevel02
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when RSVisibleLevel is RS_SYSTEM_ANIMATE_SCENE
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SetVSyncRateByVisibleLevel02, TestSize.Level1)
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
        mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
        mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
        ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
    }
}

/**
 * @tc.name: SetVSyncRateByVisibleLevel03
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when RSVisibleLevel is RS_INVISIBLE
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SetVSyncRateByVisibleLevel03, TestSize.Level1)
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
        mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
        mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
        ASSERT_NE(connection->highPriorityRate_, (int32_t)INVISBLE_WINDOW_RATE);
    }
}

/**
 * @tc.name: SetVSyncRateByVisibleLevel04
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when RSVisibleLevel is RS_UNKNOW_VISIBLE_LEVEL
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SetVSyncRateByVisibleLevel04, TestSize.Level1)
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
        mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
        mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
        ASSERT_NE(connection->highPriorityRate_, (int32_t)DEFAULT_RATE);
    }
}

/**
 * @tc.name: SetSystemAnimatedScenes04
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when SystemAnimatedScenes is ENTER_MISSION_CENTER
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SetSystemAnimatedScenes04, TestSize.Level1)
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
    mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
    mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
}

/**
 * @tc.name: SetSystemAnimatedScenes05
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when SystemAnimatedScenes is ENTER_TFS_WINDOW
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SetSystemAnimatedScenes05, TestSize.Level1)
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
    mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
    mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
}

/**
 * @tc.name: SetSystemAnimatedScenes06
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when SystemAnimatedScenes is ENTER_WINDOW_FULL_SCREEN
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RsMainThreadTest, SetSystemAnimatedScenes06, TestSize.Level1)
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
    mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
    mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
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
    mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
    mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
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
    mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
    mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
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
    mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
    mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
}
} // namespace OHOS::Rosen
