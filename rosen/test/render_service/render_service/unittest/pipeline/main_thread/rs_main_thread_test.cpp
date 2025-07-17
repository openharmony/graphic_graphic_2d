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
#include <if_system_ability_manager.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/rs_test_util.h"
#include "consumer_surface.h"

#include "command/rs_base_node_command.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "memory/rs_memory_track.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "platform/common/rs_innovation.h"
#include "platform/common/rs_system_properties.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "screen_manager/rs_screen.h"
#include "pipeline/rs_render_node_gc.h"
#if defined(ACCESSIBILITY_ENABLE)
#include "accessibility_config.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_RATE = 1;
constexpr int32_t INVALID_VALUE = -1;
constexpr int32_t INVISBLE_WINDOW_RATE = 10;
constexpr int32_t SCREEN_PHYSICAL_HEIGHT = 10;
constexpr int32_t SCREEN_PHYSICAL_WIDTH = 10;
constexpr int32_t SIMI_VISIBLE_RATE = 2;
constexpr int32_t SYSTEM_ANIMATED_SCENES_RATE = 2;
constexpr ScreenId DEFAULT_DISPLAY_SCREEN_ID = 0;
constexpr uint32_t MULTI_WINDOW_PERF_END_NUM = 4;
constexpr uint32_t MULTI_WINDOW_PERF_START_NUM = 2;
constexpr uint64_t REFRESH_PERIOD = 16666667;
constexpr uint64_t SKIP_COMMAND_FREQ_LIMIT = 30;
constexpr uint32_t DEFAULT_SCREEN_WIDTH = 480;
constexpr uint32_t DEFAULT_SCREEN_HEIGHT = 320;
class RSMainThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void* CreateParallelSyncSignal(uint32_t count);
    static std::shared_ptr<RSScreenRenderNode> GetAndInitScreenRenderNode();
    static void ChangeHardwareEnabledNodesBufferData(
        std::vector<std::shared_ptr<RSSurfaceRenderNode>>& hardwareEnabledNodes);

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
    static void SurfaceFlushBuffers(sptr<Surface> psurf1, const int32_t bufferNums,
        const int64_t& desiredPresentTimestamp);
};

void RSMainThreadTest::SurfaceFlushBuffers(sptr<Surface> psurf1, const int32_t bufferNums,
    const int64_t& desiredPresentTimestamp)
{
    for (int32_t i = 0; i <bufferNums; i++) {
        BufferFlushConfig flushConfigTmp = { .damage = { .w = 0x100, .h = 0x100, },
            .desiredPresentTimestamp = (bufferNums - i) * desiredPresentTimestamp - 200000,
        };
        sptr<SurfaceBuffer> buffer = nullptr;
        sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
        ASSERT_EQ(psurf1->RequestBuffer(buffer, requestFence, requestConfig), GSERROR_OK);
        ASSERT_EQ(psurf1->FlushBuffer(buffer, requestFence, flushConfigTmp), GSERROR_OK);
    }
}

void RSMainThreadTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = DEFAULT_SCREEN_WIDTH;
    uint32_t height = DEFAULT_SCREEN_HEIGHT;
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);
    screenManager->SetDefaultScreenId(id);
}

void RSMainThreadTest::TearDownTestCase()
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    screenManager->SetDefaultScreenId(INVALID_SCREEN_ID);
}

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
    renderNodeMap.screenNodeMap_.clear();
    renderNodeMap.canvasDrawingNodeMap_.clear();
    renderNodeMap.uiExtensionSurfaceNodes_.clear();
}
void* RSMainThreadTest::CreateParallelSyncSignal(uint32_t count)
{
    (void)(count);
    return nullptr;
}

std::shared_ptr<RSScreenRenderNode> RSMainThreadTest::GetAndInitScreenRenderNode()
{
    auto context = std::make_shared<RSContext>();
    ScreenId screenId = 0xFFFF;
    NodeId displayId = 1;
    auto screenNode = std::make_shared<RSScreenRenderNode>(displayId, screenId, context);
    auto screenManager = CreateOrGetScreenManager();
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    if (hdiOutput == nullptr) {
        return screenNode;
    }
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, false, hdiOutput, nullptr);
    if (rsScreen == nullptr) {
        return screenNode;
    }

    rsScreen->phyWidth_ = SCREEN_PHYSICAL_WIDTH;
    rsScreen->phyHeight_ = SCREEN_PHYSICAL_HEIGHT;
    screenManager->MockHdiScreenConnected(rsScreen);
    return screenNode;
}

void RSMainThreadTest::ChangeHardwareEnabledNodesBufferData(
    std::vector<std::shared_ptr<RSSurfaceRenderNode>>& hardwareEnabledNodes)
{
    if (hardwareEnabledNodes.empty()) {
        return;
    }
    for (auto& surfaceNode : hardwareEnabledNodes) {
        if (surfaceNode == nullptr || surfaceNode->GetRSSurfaceHandler() == nullptr) {
            continue;
        }
        if (surfaceNode->GetRSSurfaceHandler()->IsCurrentFrameBufferConsumed() &&
            surfaceNode->HwcSurfaceRecorder().GetLastFrameHasVisibleRegion()) {
            surfaceNode->GetRSSurfaceHandler()->ResetCurrentFrameBufferConsumed();
            surfaceNode->HwcSurfaceRecorder().SetLastFrameHasVisibleRegion(false);
        }
    }
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
    mainThread->ClassifyRSTransactionData(std::shared_ptr(std::move(rsTransactionData)));

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

    FocusAppInfo info = {
        .pid = pid,
        .uid = uid,
        .bundleName = str,
        .abilityName = str,
        .focusNodeId = 0};
    mainThread->SetFocusAppInfo(info);
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
    FocusAppInfo info = {
        .pid = pid,
        .uid = uid,
        .bundleName = str,
        .abilityName = str,
        .focusNodeId = newFocusNode->GetId()};
    mainThread->SetFocusAppInfo(info);
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
    mainThread->ClassifyRSTransactionData(std::shared_ptr(std::move(rsTransactionData)));
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
    mainThread->ClassifyRSTransactionData(std::shared_ptr(std::move(rsTransactionData)));
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
    mainThread->ClassifyRSTransactionData(std::shared_ptr(std::move(rsTransactionData)));
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
    mainThread->ClassifyRSTransactionData(std::shared_ptr(std::move(rsTransactionData)));
    ASSERT_EQ(mainThread->cachedCommands_[nodeId].empty(), true);

    mainThread->cachedCommands_.clear();
    rsTransactionData = std::make_unique<RSTransactionData>();
    command = nullptr;
    followType = FollowType::FOLLOW_TO_PARENT;
    rsTransactionData->AddCommand(command, nodeId + 1, followType);
    mainThread->ClassifyRSTransactionData(std::shared_ptr(std::move(rsTransactionData)));
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
 * @tc.name: ShowWatermark01
 * @tc.desc: ShowWatermark test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSMainThreadTest, ShowWatermark01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    Media::InitializationOptions opts;
    opts.size.width = DEFAULT_SCREEN_WIDTH * 2.5;
    opts.size.height = DEFAULT_SCREEN_HEIGHT * 2.5;
    std::unique_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelMap, nullptr);
    mainThread->watermarkFlag_ = false;
    mainThread->ShowWatermark(std::move(pixelMap), true);
    ASSERT_EQ(mainThread->GetWatermarkFlag(), false);
}

/**
 * @tc.name: ShowWatermark02
 * @tc.desc: ShowWatermark test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSMainThreadTest, ShowWatermark02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    std::unique_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelMap, nullptr);
    mainThread->ShowWatermark(std::move(pixelMap), true);
    ASSERT_EQ(mainThread->GetWatermarkFlag(), true);
}

/**
 * @tc.name: ShowWatermark03
 * @tc.desc: ShowWatermark test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSMainThreadTest, ShowWatermark03, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    Media::InitializationOptions opts;
    opts.size.width = DEFAULT_SCREEN_WIDTH;
    opts.size.height = DEFAULT_SCREEN_WIDTH;
    std::unique_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelMap, nullptr);
    mainThread->ShowWatermark(std::move(pixelMap), true);
    ASSERT_EQ(mainThread->GetWatermarkFlag(), true);
}

/**
 * @tc.name: ShowWatermark05
 * @tc.desc: ShowWatermark test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSMainThreadTest, ShowWatermark05, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    Media::InitializationOptions opts;
    opts.size.width = DEFAULT_SCREEN_WIDTH;
    opts.size.height = DEFAULT_SCREEN_WIDTH;
    std::unique_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelMap, nullptr);
    mainThread->watermarkFlag_ = false;
    mainThread->ShowWatermark(nullptr, true);
    ASSERT_EQ(mainThread->GetWatermarkFlag(), true);
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
    FocusAppInfo info = {
        .pid = -1,
        .uid = -1,
        .bundleName = str,
        .abilityName = str,
        .focusNodeId = node->GetId()};
    mainThread->SetFocusAppInfo(info);
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
    FocusAppInfo info = {
        .pid = -1,
        .uid = -1,
        .bundleName = str,
        .abilityName = str,
        .focusNodeId = childNode->GetId()};
    mainThread->SetFocusAppInfo(info);
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
    FocusAppInfo info = {
        .pid = -1,
        .uid = -1,
        .bundleName = str,
        .abilityName = str,
        .focusNodeId = childNode->GetId()};
    mainThread->SetFocusAppInfo(info);
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
 * @tc.name: IsSurfaceConsumerNeedSkip001
 * @tc.desc: IsSurfaceConsumerNeedSkip test
 * @tc.type: FUNC
 * @tc.require: issueICKWDL
 */
HWTEST_F(RSMainThreadTest, IsSurfaceConsumerNeedSkip001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto distributor = mainThread->rsVSyncDistributor_;
    sptr<IConsumerSurface> cSurface = nullptr;
    auto res = mainThread->IsSurfaceConsumerNeedSkip(cSurface);
    ASSERT_EQ(res, false);
    cSurface = IConsumerSurface::Create();
    auto vsyncGenerator = CreateVSyncGenerator();
    auto vsyncController = new VSyncController(vsyncGenerator, 0);
    mainThread->rsVSyncDistributor_ = new VSyncDistributor(vsyncController, "rs");
    res = mainThread->IsSurfaceConsumerNeedSkip(cSurface);
    ASSERT_EQ(res, false);
    mainThread->rsVSyncDistributor_ = distributor;
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
    mainThread->context_->globalRootRenderNode_ = nullptr;
    mainThread->CheckIfHardwareForcedDisabled();
    NodeId id = 0;
    mainThread->context_->globalRootRenderNode_ = std::make_shared<RSRenderNode>(id++, true);

    std::vector<std::shared_ptr<OHOS::Rosen::RSRenderNode>> vec;
    vec.push_back(nullptr);
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode1 = std::make_shared<RSScreenRenderNode>(id++, 0, rsContext->weak_from_this());
    auto mirrorSourceNode = std::make_shared<RSScreenRenderNode>(id++, 0, rsContext->weak_from_this());
    displayNode1->mirrorSource_ = mirrorSourceNode;
    vec.push_back(displayNode1);
    auto displayNode2 = std::make_shared<RSScreenRenderNode>(id++, 0, rsContext->weak_from_this());
    vec.push_back(displayNode2);
    const std::vector<std::shared_ptr<OHOS::Rosen::RSRenderNode>> vec1 = vec;
    auto vec1Ptr = std::make_shared<std::vector<std::shared_ptr<OHOS::Rosen::RSRenderNode>>>(vec1);
    mainThread->context_->globalRootRenderNode_->fullChildrenList_ = vec1Ptr;

    mainThread->CheckIfHardwareForcedDisabled();
}

/**
 * @tc.name: CheckIfHardwareForcedDisabled
 * @tc.desc: CheckIfHardwareForcedDisabled002 test child = nullptr and type != SCREEN_NODE
 * @tc.type: FUNC
 * @tc.require: issueIAS924
 */
HWTEST_F(RSMainThreadTest, CheckIfHardwareForcedDisabled002, TestSize.Level1)
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
 * @tc.name: NotifyUnmarshalTask001
 * @tc.desc: NotifyUnmarshalTask test, divided render, the func call will return immediately.
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, NotifyUnmarshalTask001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    int64_t ts = 10000000;
    mainThread->NotifyUnmarshalTask(ts);
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
    auto rsContext = std::make_shared<RSContext>();
    auto childDisplayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
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
    auto rsContext = std::make_shared<RSContext>();
    auto childDisplayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
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
 * @tc.name: UniRender003
 * @tc.desc: UniRender test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMainThreadTest, UniRender003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    mainThread->renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    // prepare nodes
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    const std::shared_ptr<RSBaseRenderNode> rootNode = context->GetGlobalRootRenderNode();
    NodeId screenNodeId = 2;
    ScreenId screenId = 3;
    auto screenNode = std::shared_ptr<RSScreenRenderNode>(new RSScreenRenderNode(screenNodeId,
        screenId, context->weak_from_this()));
    rootNode->AddChild(screenNode);
    RSDisplayNodeConfig config;
    NodeId displayNodeId = 1;
    auto childDisplayNode = std::make_shared<RSLogicalDisplayRenderNode>(displayNodeId, config);
    screenNode->AddChild(childDisplayNode, 0);
    rootNode->InitRenderParams();
    screenNode->InitRenderParams();
    childDisplayNode->InitRenderParams();
    if (RSSystemProperties::GetSkipDisplayIfScreenOffEnabled()) {
        ScreenId screenId = 1;
        auto screenManager = CreateOrGetScreenManager();
        impl::RSScreenManager& screenManagerImpl =
            static_cast<impl::RSScreenManager&>(*screenManager);
        screenManagerImpl.powerOffNeedProcessOneFrame_ = false;
        screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_OFF;
    }
    mainThread->UniRender(rootNode);
    ASSERT_FALSE(mainThread->doDirectComposition_);
}

/**
 * @tc.name: UniRender004
 * @tc.desc: UniRender test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMainThreadTest, UniRender004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->isUniRender_ = true;
    mainThread->renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    
    auto rsContext = std::make_shared<RSContext>();
    auto rootNode = rsContext->GetGlobalRootRenderNode();
    NodeId id = 1;
    auto childDisplayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    rootNode->AddChild(childDisplayNode, 0);
    rootNode->InitRenderParams();
    childDisplayNode->InitRenderParams();

    NodeId nodeId = 2;
    RSUifirstManager::Instance().AddProcessSkippedNode(nodeId);

    mainThread->doDirectComposition_ = true;
    mainThread->isDirty_ = false;
    mainThread->isAccessibilityConfigChanged_ = false;
    mainThread->isCachedSurfaceUpdated_ = false;
    mainThread->isHardwareEnabledBufferUpdated_ = false;
    mainThread->UniRender(rootNode);
    ASSERT_TRUE(mainThread->doDirectComposition_);
}

/**
 * @tc.name: IsFirstFrameOfOverdrawSwitch
 * @tc.desc: test IsFirstFrameOfOverdrawSwitch
 * @tc.type: FUNC
 * @tc.require: issueIAKQC3
 */
HWTEST_F(RSMainThreadTest, IsFirstFrameOfOverdrawSwitch, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->isOverDrawEnabledOfCurFrame_ = true;
    ASSERT_TRUE(mainThread->IsFirstFrameOfOverdrawSwitch());
}

/**
 * @tc.name: GetFrontBufferDesiredPresentTimeStamp001
 * @tc.desc: test GetFrontBufferDesiredPresentTimeStamp
 * @tc.type: FUNC
 * @tc.require: issueIAKQC3
 */
HWTEST_F(RSMainThreadTest, GetFrontBufferDesiredPresentTimeStamp001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    const sptr<ConsumerSurface> consumer = new ConsumerSurface("test1");
    consumer->Init();
    int64_t resultValue = 100;
    int64_t desiredPresentTimestamp = resultValue;
    mainThread->GetFrontBufferDesiredPresentTimeStamp(consumer, desiredPresentTimestamp);
    ASSERT_EQ(desiredPresentTimestamp, 0);
}

/**
 * @tc.name: GetFrontBufferDesiredPresentTimeStamp002
 * @tc.desc: test GetFrontBufferDesiredPresentTimeStamp
 * @tc.type: FUNC
 * @tc.require: issueIAKQC3
 */
HWTEST_F(RSMainThreadTest, GetFrontBufferDesiredPresentTimeStamp002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    const sptr<ConsumerSurface> consumer = new ConsumerSurface("test2");
    consumer->Init();
    int64_t resultValue = 100;
    int64_t desiredPresentTimestamp = resultValue;
    uint32_t seqId = 1;
    consumer->consumer_->bufferQueue_->dirtyList_.clear();
    consumer->consumer_->bufferQueue_->dirtyList_.push_back(seqId);
    consumer->consumer_->bufferQueue_->bufferQueueCache_[seqId].isAutoTimestamp = true;
    mainThread->GetFrontBufferDesiredPresentTimeStamp(consumer, desiredPresentTimestamp);
    ASSERT_EQ(desiredPresentTimestamp, 0);
}

/**
 * @tc.name: GetFrontBufferDesiredPresentTimeStamp003
 * @tc.desc: test GetFrontBufferDesiredPresentTimeStamp
 * @tc.type: FUNC
 * @tc.require: issueIAKQC3
 */
HWTEST_F(RSMainThreadTest, GetFrontBufferDesiredPresentTimeStamp003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    int64_t desiredPresentTimestamp = -1;
    mainThread->GetFrontBufferDesiredPresentTimeStamp(nullptr, desiredPresentTimestamp);
    EXPECT_EQ(desiredPresentTimestamp, 0);
}

/**
 * @tc.name: GetFrontBufferDesiredPresentTimeStamp004
 * @tc.desc: test GetFrontBufferDesiredPresentTimeStamp
 * @tc.type: FUNC
 * @tc.require: issueIAKQC3
 */
HWTEST_F(RSMainThreadTest, GetFrontBufferDesiredPresentTimeStamp004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    const sptr<ConsumerSurface> consumer = new ConsumerSurface("test4");
    consumer->Init();
    int64_t desiredPresentTimestamp = 100000000;
    int64_t getDesiredPresentTimestamp = -1;
    uint32_t seqId = 1;
    consumer->consumer_->bufferQueue_->dirtyList_.clear();
    consumer->consumer_->bufferQueue_->dirtyList_.push_back(seqId);
    consumer->consumer_->bufferQueue_->bufferQueueCache_[seqId].isAutoTimestamp = false;
    consumer->consumer_->bufferQueue_->bufferQueueCache_[seqId].desiredPresentTimestamp = desiredPresentTimestamp;
    mainThread->GetFrontBufferDesiredPresentTimeStamp(consumer, getDesiredPresentTimestamp);
    EXPECT_EQ(getDesiredPresentTimestamp, desiredPresentTimestamp);
}

/**
 * @tc.name: GetFrontBufferDesiredPresentTimeStamp005
 * @tc.desc: test GetFrontBufferDesiredPresentTimeStamp
 * @tc.type: FUNC
 * @tc.require: issueIAKQC3
 */
HWTEST_F(RSMainThreadTest, GetFrontBufferDesiredPresentTimeStamp005, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    const sptr<ConsumerSurface> consumer = new ConsumerSurface("test5");
    consumer->Init();
    int64_t desiredPresentTimestamp = 100000000;
    int64_t getDesiredPresentTimestamp = -1;
    uint32_t seqId = 1;
    consumer->consumer_->bufferQueue_->dirtyList_.clear();
    consumer->consumer_->bufferQueue_->dirtyList_.push_back(seqId);
    consumer->consumer_->bufferQueue_->bufferQueueCache_[seqId].isAutoTimestamp = false;
    consumer->consumer_->bufferQueue_->bufferQueueCache_[seqId].desiredPresentTimestamp = desiredPresentTimestamp;

    uint64_t vsyncRsTimestamp = mainThread->vsyncRsTimestamp_.load(); // record
    mainThread->vsyncRsTimestamp_.store(desiredPresentTimestamp - 1);
    mainThread->GetFrontBufferDesiredPresentTimeStamp(consumer, getDesiredPresentTimestamp);
    EXPECT_EQ(getDesiredPresentTimestamp, desiredPresentTimestamp);

    mainThread->vsyncRsTimestamp_.store(desiredPresentTimestamp);
    mainThread->GetFrontBufferDesiredPresentTimeStamp(consumer, getDesiredPresentTimestamp);
    EXPECT_EQ(getDesiredPresentTimestamp, 0);

    mainThread->vsyncRsTimestamp_.store(desiredPresentTimestamp + 1);
    mainThread->GetFrontBufferDesiredPresentTimeStamp(consumer, getDesiredPresentTimestamp);
    EXPECT_EQ(getDesiredPresentTimestamp, 0);

    mainThread->vsyncRsTimestamp_.store(desiredPresentTimestamp + 1000000000); // add 1s
    mainThread->GetFrontBufferDesiredPresentTimeStamp(consumer, getDesiredPresentTimestamp);
    EXPECT_EQ(getDesiredPresentTimestamp, 0);

    mainThread->vsyncRsTimestamp_.store(desiredPresentTimestamp + 1000000001); // add 1+s
    mainThread->GetFrontBufferDesiredPresentTimeStamp(consumer, getDesiredPresentTimestamp);
    EXPECT_EQ(getDesiredPresentTimestamp, 0);

    mainThread->vsyncRsTimestamp_.store(vsyncRsTimestamp); // reset
}

/**
 * @tc.name: GetFrontBufferDesiredPresentTimeStamp006
 * @tc.desc: test GetFrontBufferDesiredPresentTimeStamp
 * @tc.type: FUNC
 * @tc.require: issueIAKQC3
 */
HWTEST_F(RSMainThreadTest, GetFrontBufferDesiredPresentTimeStamp006, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    const sptr<ConsumerSurface> consumer = new ConsumerSurface("test6");
    consumer->Init();
    int64_t getDesiredPresentTimestamp = -1;
    mainThread->GetFrontBufferDesiredPresentTimeStamp(consumer, getDesiredPresentTimestamp);
    EXPECT_EQ(getDesiredPresentTimestamp, 0);

    int64_t desiredPresentTimestamp = -1;
    uint32_t seqId = 1;
    consumer->consumer_->bufferQueue_->dirtyList_.clear();
    consumer->consumer_->bufferQueue_->dirtyList_.push_back(seqId);
    consumer->consumer_->bufferQueue_->bufferQueueCache_[seqId].isAutoTimestamp = false;
    consumer->consumer_->bufferQueue_->bufferQueueCache_[seqId].desiredPresentTimestamp = desiredPresentTimestamp;
    mainThread->GetFrontBufferDesiredPresentTimeStamp(consumer, getDesiredPresentTimestamp);
    EXPECT_EQ(getDesiredPresentTimestamp, 0);
}

/**
 * @tc.name: GetFrontBufferDesiredPresentTimeStamp007
 * @tc.desc: test GetFrontBufferDesiredPresentTimeStamp
 * @tc.type: FUNC
 * @tc.require: issueIAKQC3
 */
HWTEST_F(RSMainThreadTest, GetFrontBufferDesiredPresentTimeStamp007, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    const sptr<ConsumerSurface> consumer = new ConsumerSurface("test7");
    consumer->Init();
    uint32_t seqId = 1;
    int64_t desiredPresentTimestamp = 999999999; // 0.999ms
    consumer->consumer_->bufferQueue_->dirtyList_.clear();
    consumer->consumer_->bufferQueue_->dirtyList_.push_back(seqId);
    consumer->consumer_->bufferQueue_->bufferQueueCache_[seqId].isAutoTimestamp = false;
    consumer->consumer_->bufferQueue_->bufferQueueCache_[seqId].desiredPresentTimestamp = desiredPresentTimestamp;

    int64_t getDesiredPresentTimestamp = -1;
    uint64_t vsyncRsTimestamp = mainThread->vsyncRsTimestamp_.load(); // record

    mainThread->vsyncRsTimestamp_.store(desiredPresentTimestamp / 2);
    mainThread->GetFrontBufferDesiredPresentTimeStamp(consumer, getDesiredPresentTimestamp);
    EXPECT_EQ(getDesiredPresentTimestamp, desiredPresentTimestamp);

    desiredPresentTimestamp = 1100000000; // 1.1ms
    consumer->consumer_->bufferQueue_->bufferQueueCache_[seqId].desiredPresentTimestamp = desiredPresentTimestamp;
    mainThread->vsyncRsTimestamp_.store(0);
    mainThread->GetFrontBufferDesiredPresentTimeStamp(consumer, getDesiredPresentTimestamp);
    EXPECT_EQ(getDesiredPresentTimestamp, 0);
    mainThread->vsyncRsTimestamp_.store(vsyncRsTimestamp); // reset
}

/**
 * @tc.name: GetRealTimeOffsetOfDvsync
 * @tc.desc: test GetRealTimeOffsetOfDvsync
 * @tc.type: FUNC
 * @tc.require: issueIAXG6P
 */
HWTEST_F(RSMainThreadTest, GetRealTimeOffsetOfDvsync, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    int64_t time = 1000;
    uint64_t offset = mainThread->GetRealTimeOffsetOfDvsync(time);
    ASSERT_EQ(offset, 0);
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
    auto rsContext = std::make_shared<RSContext>();
    auto childDisplayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
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
 * @tc.desc: Render002 test rootNode = nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAS924
 */
HWTEST_F(RSMainThreadTest, Render002, TestSize.Level1)
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
    NodeId displayNodeId = 0;
    auto displayNode = std::make_shared<RSScreenRenderNode>(displayNodeId, 0, mainThread->context_->weak_from_this());

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
 * @tc.name: SurfaceOcclusionCallback003
 * @tc.desc: SurfaceOcclusionCallback for appwindow
 * @tc.type: FUNC
 * @tc.require: issuesIBE1C8
 */
HWTEST_F(RSMainThreadTest, SurfaceOcclusionCallback003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RectI rectBottom = RectI(-1000000, -1000000, 999999, 999999);

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
    ASSERT_NE(node1, nullptr);
    node1->SetIsOnTheTree(true);
    config.id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node2, nullptr);
    node2->SetIsOnTheTree(true);
    node2->instanceRootNodeId_ = INVALID_NODEID;
    config.id = 3;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node3, nullptr);
    node3->SetIsOnTheTree(true);
    node3->instanceRootNodeId_ = 1;

    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node1);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);
    mainThread->CheckSurfaceOcclusionNeedProcess(1);
    mainThread->CheckSurfaceOcclusionNeedProcess(2);
    mainThread->CheckSurfaceOcclusionNeedProcess(3);
    for (auto &listener : mainThread->savedAppWindowNode_) {
        auto& property = listener.second.second->GetRenderProperties();
        listener.second.second->SetVisibleRegion(Occlusion::Region(rectBottom));
        auto& geoPtr = property.GetBoundsGeometry();
        geoPtr->SetAbsRect();
    }

    // run
    mainThread->SurfaceOcclusionCallback();
    mainThread->surfaceOcclusionListeners_.clear();
    ASSERT_TRUE(mainThread->surfaceOcclusionListeners_.empty());
}

/**
 * @tc.name: SurfaceOcclusionCallback004
 * @tc.desc: SurfaceOcclusionCallback with vector of surfaceOcclusionListeners_
 * @tc.type: FUNC
 * @tc.require: issuesIBE1C8
 */
HWTEST_F(RSMainThreadTest, SurfaceOcclusionCallback004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RectI rectBottom = RectI(-1000000, -1000000, 999999, 999999);
    // prepare listeners
    std::tuple<pid_t, sptr<RSISurfaceOcclusionChangeCallback>,
        std::vector<float>, uint8_t> info(0, nullptr, {1, 2, 3}, 0);
    mainThread->surfaceOcclusionListeners_.insert({1, info});
    mainThread->surfaceOcclusionListeners_.insert({2, info});
    mainThread->surfaceOcclusionListeners_.insert({3, info});
    //prepare nodemap
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node1, nullptr);
    node1->SetIsOnTheTree(true);
    config.id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node2, nullptr);
    node2->SetIsOnTheTree(true);
    node2->instanceRootNodeId_ = INVALID_NODEID;
    config.id = 3;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node3, nullptr);
    node3->SetIsOnTheTree(true);
    node3->instanceRootNodeId_ = 1;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node1);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);
    mainThread->CheckSurfaceOcclusionNeedProcess(1);
    mainThread->CheckSurfaceOcclusionNeedProcess(2);
    mainThread->CheckSurfaceOcclusionNeedProcess(3);
    for (auto &listener : mainThread->savedAppWindowNode_) {
        auto& property = listener.second.second->GetRenderProperties();
        listener.second.second->SetVisibleRegion(Occlusion::Region(rectBottom));
        auto& geoPtr = property.GetBoundsGeometry();
        geoPtr->SetAbsRect();
    }

    // run
    mainThread->SurfaceOcclusionCallback();
    mainThread->surfaceOcclusionListeners_.clear();
    ASSERT_TRUE(mainThread->surfaceOcclusionListeners_.empty());
}

/**
 * @tc.name: SurfaceOcclusionCallback005
 * @tc.desc: SurfaceOcclusionCallback with visibleAreaRatio on rectBottom
 * @tc.type: FUNC
 * @tc.require: issuesIBE1C8
 */
HWTEST_F(RSMainThreadTest, SurfaceOcclusionCallback005, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RectI rectBottom = RectI(-1000000, -1000000, 599999, 599999);

    // prepare listeners
    std::tuple<pid_t, sptr<RSISurfaceOcclusionChangeCallback>,
        std::vector<float>, uint8_t> info(0, nullptr, {1, 2, 3}, 0);
    mainThread->surfaceOcclusionListeners_.insert({1, info});
    mainThread->surfaceOcclusionListeners_.insert({2, info});
    mainThread->surfaceOcclusionListeners_.insert({3, info});

    //prepare nodemap
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node1, nullptr);
    node1->SetIsOnTheTree(true);
    config.id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node2, nullptr);
    node2->SetIsOnTheTree(true);
    node2->instanceRootNodeId_ = INVALID_NODEID;
    config.id = 3;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node3, nullptr);
    node3->SetIsOnTheTree(true);
    node3->instanceRootNodeId_ = 1;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node1);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);
    mainThread->CheckSurfaceOcclusionNeedProcess(1);
    mainThread->CheckSurfaceOcclusionNeedProcess(2);
    mainThread->CheckSurfaceOcclusionNeedProcess(3);
    for (auto &listener : mainThread->savedAppWindowNode_) {
        auto& property = listener.second.second->GetRenderProperties();
        listener.second.second->SetVisibleRegion(Occlusion::Region(rectBottom));
        auto& geoPtr = property.GetBoundsGeometry();
        geoPtr->SetAbsRect();
    }

    // run
    mainThread->SurfaceOcclusionCallback();
    mainThread->surfaceOcclusionListeners_.clear();
    ASSERT_TRUE(mainThread->surfaceOcclusionListeners_.empty());
}

/**
 * @tc.name: CheckSurfaceOcclusionNeedProcess
 * @tc.desc: CheckSurfaceOcclusionNeedProcess Test while node out of appWindow
 * @tc.type: FUNC
 * @tc.require: issuesIBE1C8
 */
HWTEST_F(RSMainThreadTest, CheckSurfaceOcclusionNeedProcess001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RSSurfaceRenderNodeConfig config;

    config.id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node, nullptr);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node);
    auto appNode = mainThread->savedAppWindowNode_.find(1);
    appNode->second = {};

    bool result = mainThread->CheckSurfaceOcclusionNeedProcess(1);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: CheckSurfaceOcclusionNeedProcess
 * @tc.desc: CheckSurfaceOcclusionNeedProcess Test for appId invalid
 * @tc.type: FUNC
 * @tc.require: issuesIBE1C8
 */
HWTEST_F(RSMainThreadTest, CheckSurfaceOcclusionNeedProcess002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RSSurfaceRenderNodeConfig config;

    config.id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node1, nullptr);
    node1->instanceRootNodeId_ = INSTANCE_ID_UNDEFINED;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node1);

    config.id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node2, nullptr);
    node2->instanceRootNodeId_ = INSTANCE_ID_UNDEFINED;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);
    mainThread->savedAppWindowNode_[2] = {node1, node2};

    bool result = mainThread->CheckSurfaceOcclusionNeedProcess(2);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: CheckSurfaceOcclusionNeedProcess
 * @tc.desc: CheckSurfaceOcclusionNeedProcess Test while node is empty
 * @tc.type: FUNC
 * @tc.require: issuesIBE1C8
 */
HWTEST_F(RSMainThreadTest, CheckSurfaceOcclusionNeedProcess003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RSSurfaceRenderNodeConfig config;

    config.id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node1, nullptr);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node1);

    config.id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node2, nullptr);
    node2->SetIsOnTheTree(true);
    node2->instanceRootNodeId_ = INVALID_NODEID;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);

    mainThread->savedAppWindowNode_.clear();
    bool result1 = mainThread->CheckSurfaceOcclusionNeedProcess(1);
    ASSERT_FALSE(result1);
    bool result2 = mainThread->CheckSurfaceOcclusionNeedProcess(2);
    ASSERT_FALSE(result2);
}

/**
 * @tc.name: GetVsyncRefreshRate
 * @tc.desc: GetVsyncRefreshRate Test
 * @tc.type: FUNC
 * @tc.require: issueICAANX
 */
HWTEST_F(RSMainThreadTest, GetVsyncRefreshRate001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->vsyncGenerator_ = nullptr;
    uint32_t refreshRate = mainThread->GetVsyncRefreshRate();
    ASSERT_EQ(refreshRate, 0);
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
    node1->GenerateFullChildrenList();
    mainThread->context_->globalRootRenderNode_ = node1;
    mainThread->CalcOcclusion();
    // run with more than one node
    auto node3 = std::make_shared<RSRenderNode>(0, true);
    auto rsContext = std::make_shared<RSContext>();
    auto node4 = std::make_shared<RSScreenRenderNode>(1, 0, rsContext->weak_from_this());
    RSSurfaceRenderNodeConfig config2;
    auto node5 = std::make_shared<RSSurfaceRenderNode>(config2);
    node3->AddChild(node4);
    node3->GenerateFullChildrenList();
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
 * @tc.name: ConsumeAndUpdateAllNodes004
 * @tc.desc: ConsumeAndUpdateAllNodes004 Test
 * @tc.type: FUNC
 * @tc.require: issueIANQPF
 */
HWTEST_F(RSMainThreadTest, ConsumeAndUpdateAllNodes004, TestSize.Level1)
{
#ifndef ROSEN_CROSS_PLATFORM
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
 * @tc.name: ConsumeAndUpdateAllNodes005
 * @tc.desc: ConsumeAndUpdateAllNodes005 Test
 * @tc.type: FUNC
 * @tc.require: issueIANQPF
 */
HWTEST_F(RSMainThreadTest, ConsumeAndUpdateAllNodes005, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->isUniRender_ = true;
    mainThread->timestamp_ = 1000;
    // prepare nodemap
    mainThread->context_->GetMutableNodeMap().renderNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.clear();

    int64_t desiredPresentTimestamp = 1000000000;
    uint64_t vsyncRsTimestamp = mainThread->vsyncRsTimestamp_.load(); // record
    mainThread->vsyncRsTimestamp_.store(desiredPresentTimestamp);

    auto rsSurfaceRenderNode1 = RSTestUtil::CreateSurfaceNode();
    EXPECT_EQ(mainThread->context_->GetMutableNodeMap().RegisterRenderNode(rsSurfaceRenderNode1), true);
    auto rsSurfaceRenderNode2 = RSTestUtil::CreateSurfaceNode();
    EXPECT_EQ(mainThread->context_->GetMutableNodeMap().RegisterRenderNode(rsSurfaceRenderNode2), true);

    auto surfaceConsumer1 = rsSurfaceRenderNode1->GetRSSurfaceHandler()->GetConsumer();
    ASSERT_NE(surfaceConsumer1, nullptr);
    sptr<IBufferConsumerListener> listener1 = new RSRenderServiceListener(rsSurfaceRenderNode1);
    EXPECT_EQ(surfaceConsumer1->RegisterConsumerListener(listener1), SURFACE_ERROR_OK);
    auto producer1 = surfaceConsumer1->GetProducer();
    ASSERT_NE(producer1, nullptr);
    sptr<Surface> psurf1 = Surface::CreateSurfaceAsProducer(producer1);
    ASSERT_NE(psurf1, nullptr);
    psurf1->SetQueueSize(5);
    SurfaceFlushBuffers(psurf1, 5, desiredPresentTimestamp);

    auto surfaceConsumer2 = rsSurfaceRenderNode2->GetRSSurfaceHandler()->GetConsumer();
    ASSERT_NE(surfaceConsumer2, nullptr);
    sptr<IBufferConsumerListener> listener2 = new RSRenderServiceListener(rsSurfaceRenderNode2);
    EXPECT_EQ(surfaceConsumer2->RegisterConsumerListener(listener2), SURFACE_ERROR_OK);
    auto producer2 = surfaceConsumer2->GetProducer();
    ASSERT_NE(producer2, nullptr);
    sptr<Surface> psurf2 = Surface::CreateSurfaceAsProducer(producer2);
    ASSERT_NE(psurf2, nullptr);
    psurf2->SetQueueSize(5);
    SurfaceFlushBuffers(psurf2, 5, desiredPresentTimestamp);
    mainThread->ConsumeAndUpdateAllNodes();
    mainThread->ConsumeAndUpdateAllNodes();
    mainThread->vsyncRsTimestamp_.store(vsyncRsTimestamp);
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
    mainThread->ClassifyRSTransactionData(std::shared_ptr(std::move(data)));
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
    uint64_t token = 1;
    mainThread->context_->AddSyncFinishAnimationList(id, animationId, token);
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
 * @tc.name: ForceRefreshForUni003
 * @tc.desc: ForceRefreshForUni Test, with fastcompose
 * @tc.type: FUNC
 * @tc.require: issueIBGV2W
 */
HWTEST_F(RSMainThreadTest, ForceRefreshForUni003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    mainThread->ForceRefreshForUni(true);
    ASSERT_EQ(mainThread->isForceRefresh_, false);
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
        mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
        mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
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
        mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
        mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
        ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
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
        mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
        mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
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
        mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
        mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
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
    mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
    mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
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
    mainThread->GetRSVsyncRateReduceManager().ClearLastVisMapForVsyncRate();
    mainThread->GetRSVsyncRateReduceManager().SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
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
 * @tc.name: SetLuminanceChangingStatus
 * @tc.desc: SetLuminanceChangingStatus Test
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSMainThreadTest, SetLuminanceChangingStatus, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    for (int i = 0; i < 10; i++) {
        auto id = static_cast<ScreenId>(i);
        mainThread->SetLuminanceChangingStatus(id, true);
        ASSERT_EQ(mainThread->displayLuminanceChanged_[id], true);
        mainThread->SetLuminanceChangingStatus(id, false);
        ASSERT_EQ(mainThread->displayLuminanceChanged_[id], false);
    }
}

/**
 * @tc.name: ExchangeLuminanceChangingStatus
 * @tc.desc: ExchangeLuminanceChangingStatus Test
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSMainThreadTest, ExchangeLuminanceChangingStatus, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    for (int i = 0; i < 10; i++) {
        auto id = static_cast<ScreenId>(i);
        mainThread->SetLuminanceChangingStatus(id, true);
        ASSERT_EQ(mainThread->ExchangeLuminanceChangingStatus(id), true);
        ASSERT_EQ(mainThread->ExchangeLuminanceChangingStatus(id), false);
        mainThread->SetLuminanceChangingStatus(id, false);
        ASSERT_EQ(mainThread->ExchangeLuminanceChangingStatus(id), false);
    }
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
 * @tc.require: issueI97LXT
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
 * @tc.require: issueI97LXT
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
 * @tc.require: issueI97LXT
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
 * @tc.require: issueI97LXT
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
 * @tc.name: UpdateScreenNodeScreenId001
 * @tc.desc: UpdateScreenNodeScreenId, when rootnode is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, UpdateScreenNodeScreenId001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->globalRootRenderNode_ = nullptr;
    mainThread->UpdateScreenNodeScreenId();
    ASSERT_EQ(mainThread->screenNodeScreenId_, DEFAULT_DISPLAY_SCREEN_ID);
}

/**
 * @tc.name: UpdateScreenNodeScreenId002
 * @tc.desc: UpdateScreenNodeScreenId, root node has no child display node.
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, UpdateScreenNodeScreenId002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    NodeId id = 1;
    mainThread->context_->globalRootRenderNode_ = std::make_shared<RSRenderNode>(id);
    mainThread->UpdateScreenNodeScreenId();
    ASSERT_EQ(mainThread->screenNodeScreenId_, DEFAULT_DISPLAY_SCREEN_ID);
}

/**
 * @tc.name: UpdateScreenNodeScreenId003
 * @tc.desc: UpdateScreenNodeScreenId, root node has one child display node.
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, UpdateScreenNodeScreenId003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId rootId = 0;
    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->globalRootRenderNode_ = std::make_shared<RSRenderNode>(rootId);
    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(displayId, config);
    uint64_t screenId = 1;
    displayNode->SetScreenId(screenId);
    mainThread->context_->globalRootRenderNode_->AddChild(displayNode);
    ASSERT_FALSE(mainThread->context_->globalRootRenderNode_->children_.empty());
    mainThread->UpdateScreenNodeScreenId();
}

/**
 * @tc.name: UpdateScreenNodeScreenId004
 * @tc.desc: UpdateScreenNodeScreenId, root node has a display child node and a canvas child node
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadTest, UpdateScreenNodeScreenId004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId rootId = 0;
    ASSERT_NE(mainThread->context_, nullptr);
    auto backUpRootNode = mainThread->context_->globalRootRenderNode_;
    mainThread->context_->globalRootRenderNode_ = std::make_shared<RSRenderNode>(rootId);

    std::weak_ptr<RSContext> weakContext = {};
    NodeId canvasId = 1;
    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(canvasId, weakContext);
    mainThread->context_->globalRootRenderNode_->AddChild(rsCanvasDrawingRenderNode);

    NodeId displayId = 2;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(displayId, 0, rsContext->weak_from_this());
    std::shared_ptr<RSRenderNode> tempNode = nullptr;

    displayNode->children_.emplace_back(tempNode);
    mainThread->context_->globalRootRenderNode_->AddChild(displayNode);
    mainThread->context_->globalRootRenderNode_->GenerateFullChildrenList();

    ASSERT_FALSE(mainThread->context_->globalRootRenderNode_->children_.empty());
    mainThread->UpdateScreenNodeScreenId();
    mainThread->context_->globalRootRenderNode_ = backUpRootNode;
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
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(displayId, 0, rsContext->weak_from_this());
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
 * @tc.name: UpdateLuminanceAndColorTempTest
 * @tc.desc: test UpdateLuminanceAndColorTemp
 * @tc.type: FUNC
 * @tc.require: issueIAIIEP
 */
HWTEST_F(RSMainThreadTest, UpdateLuminanceAndColorTempTest, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    auto backUpNode = mainThread->context_->globalRootRenderNode_;
    NodeId id = 1;
    auto node1 = std::make_shared<RSRenderNode>(id);
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode1 = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    id++;
    node1->AddChild(displayNode1);
    auto displayNode2 = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    id++;
    node1->AddChild(displayNode2);
    std::weak_ptr<RSContext> weakContext = {};
    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(id, weakContext);
    node1->AddChild(rsCanvasDrawingRenderNode);
    node1->GenerateFullChildrenList();
    mainThread->context_->globalRootRenderNode_ = node1;

    mainThread->UpdateLuminanceAndColorTemp();

    mainThread->context_->globalRootRenderNode_ = backUpNode;
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
 * @tc.name: GetDynamicRefreshRate002
 * @tc.desc: test GetDynamicRefreshRate, refreshRate = 0
 * @tc.type: FUNC
 * @tc.require: issueIAIPI3
 */
HWTEST_F(RSMainThreadTest, GetDynamicRefreshRate002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_EQ(mainThread->GetDynamicRefreshRate(), OHOS::Rosen::STANDARD_REFRESH_RATE);
}

/**
 * @tc.name: SetFrameIsRender
 * @tc.desc: test SetFrameIsRender001, rsVSyncDistributor_ = nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIPI3
 */
HWTEST_F(RSMainThreadTest, SetFrameIsRender001, TestSize.Level2)
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
 * @tc.require: issueIAIPI3
 */
HWTEST_F(RSMainThreadTest, SetFrameIsRender002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    auto vsyncGenerator = CreateVSyncGenerator();
    auto vsyncController = new VSyncController(vsyncGenerator, 0);
    mainThread->rsVSyncDistributor_ = new VSyncDistributor(vsyncController, "rs");
    mainThread->SetFrameIsRender(true);
}

/**
 * @tc.name: OnUniRenderDraw
 * @tc.desc: test OnUniRenderDraw001, test isUniRender_ & doDirectComposition_ = false
 * @tc.type: FUNC
 * @tc.require: issueIAIPI3
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
 * @tc.require: issueIAIPI3
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
 * @tc.require: issueIAIZ2Y
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
 * @tc.require: issueIAIZ2Y
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
 * @tc.require:
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
 * @tc.require: issueIAS924
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
 * @tc.require: issueIB5EAA
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
 * @tc.require: issueIB5EAA
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
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode1 = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    id++;
    node1->AddChild(displayNode1);
    node1->GenerateFullChildrenList();
    ASSERT_TRUE(node1->GetChildrenCount() <= 1);
    mainThread->context_->globalRootRenderNode_ = node1;
    ASSERT_EQ(mainThread->HasMirrorDisplay(), false);
    auto displayNode2 = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    id++;
    auto mirrorSourceNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    displayNode2->isMirroredScreen_ = true;
    displayNode2->SetMirrorSource(mirrorSourceNode);
    node1->AddChild(displayNode2);
    node1->GenerateFullChildrenList();
    ASSERT_TRUE(node1->GetChildrenCount() > 1);
    mainThread->context_->globalRootRenderNode_ = node1;
    ASSERT_EQ(mainThread->HasMirrorDisplay(), true);
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: HasMirrorDisplay003
 * @tc.desc: test HasMirrorDisplay, rootNode != nullptr, and childNode is canvas node
 * @tc.type: FUNC
 * @tc.require: issueIB5EAA
 */
HWTEST_F(RSMainThreadTest, HasMirrorDisplay003, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    NodeId id = 1;
    auto node1 = std::make_shared<RSRenderNode>(id);
    id++;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode1 = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    id++;
    node1->AddChild(displayNode1);
    node1->GenerateFullChildrenList();
    ASSERT_TRUE(node1->GetChildrenCount() <= 1);
    mainThread->context_->globalRootRenderNode_ = node1;
    ASSERT_EQ(mainThread->HasMirrorDisplay(), false);
    auto displayNode2 = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    id++;
    auto mirrorSourceNode1 = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    id++;
    displayNode2->isMirroredScreen_ = true;
    displayNode2->SetMirrorSource(mirrorSourceNode1);
    displayNode2->compositeType_ = CompositeType::UNI_RENDER_COMPOSITE;
    node1->AddChild(displayNode2);
    node1->GenerateFullChildrenList();

    auto displayNode3 = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    id++;
    auto mirrorSourceNode2 = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    id++;
    displayNode3->isMirroredScreen_ = true;
    displayNode3->SetMirrorSource(mirrorSourceNode2);
    displayNode2->compositeType_ = CompositeType::UNI_RENDER_MIRROR_COMPOSITE;
    node1->AddChild(displayNode3);
    node1->GenerateFullChildrenList();

    std::weak_ptr<RSContext> weakContext = {};
    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(id, weakContext);
    node1->AddChild(rsCanvasDrawingRenderNode);
    node1->GenerateFullChildrenList();

    mainThread->context_->globalRootRenderNode_ = node1;
    EXPECT_TRUE(mainThread->HasMirrorDisplay());
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: ResetAnimateNodeFlag
 * @tc.desc: test ResetAnimateNodeFlag, context_ = nullptr & !nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIZ2Y
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
 * @tc.name: SendClientDumpNodeTreeCommands
 * @tc.desc: test SendClientDumpNodeTreeCommands
 * @tc.type: FUNC
 * @tc.require: issueIAKME2
 */
HWTEST_F(RSMainThreadTest, SendClientDumpNodeTreeCommands, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->nodeTreeDumpTasks_.clear();

    NodeId testId = 1;
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    auto displayNode = std::make_shared<RSScreenRenderNode>(testId++, 0, std::make_shared<RSContext>());
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
 * @tc.require: issueIAKME2
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
 * @tc.name: OnDrawingCacheDfxSwitchCallback
 * @tc.desc: test OnDrawingCacheDfxSwitchCallback
 * @tc.type: FUNC
 * @tc.require: issueIALU2Y
 */
HWTEST_F(RSMainThreadTest, OnDrawingCacheDfxSwitchCallback, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RSMainThread::OnDrawingCacheDfxSwitchCallback("persist", "1", nullptr);
    RSMainThread::OnDrawingCacheDfxSwitchCallback("rosen.drawingCache.enabledDfx", "1", nullptr);
}

/**
 * @tc.name: OnCommitDumpClientNodeTree
 * @tc.desc: test OnCommitDumpClientNodeTree
 * @tc.type: FUNC
 * @tc.require: issueIAKME2
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
 * @tc.require: issueIB56EL
 */
HWTEST_F(RSMainThreadTest, TraverseCanvasDrawingNodesNotOnTree, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->TraverseCanvasDrawingNodesNotOnTree();
}

/**
 * @tc.name: RenderServiceAllNodeDump01
 * @tc.desc: RenderServiceAllNodeDump Test
 * @tc.type: FUNC
 * @tc.require: issueIB57QP
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
 * @tc.require:issuesIB6292
 */
#if defined(RS_ENABLE_CHIPSET_VSYNC)
HWTEST_F(RSMainThreadTest, ConnectChipsetVsyncSer, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->initVsyncServiceFlag_ = false;
    mainThread->ConnectChipsetVsyncSer();
}
#endif

/**
 * @tc.name: SetVsyncInfo
 * @tc.desc: test SetVsyncInfo
 * @tc.type: FUNC
 * @tc.require:issuesIB6292
 */
#if defined(RS_ENABLE_CHIPSET_VSYNC)
HWTEST_F(RSMainThreadTest, SetVsyncInfo, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    auto receiver = mainThread->receiver_;
    if (mainThread->rsVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->rsVSyncDistributor_ = new VSyncDistributor(vsyncController, "rs");
    }
    sptr<VSyncConnection> conn = new VSyncConnection(mainThread->rsVSyncDistributor_, "rs");
    mainThread->receiver_ = std::make_shared<VSyncReceiver>(conn);
    mainThread->SetVsyncInfo(0);
    mainThread->receiver_ = receiver;
}
#endif

/**
 * @tc.name: IsOcclusionNodesNeedSync001
 * @tc.desc: test IsOcclusionNodesNeedSync001
 * @tc.type: FUNC
 * @tc.require:
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
 * @tc.require:
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
 * @tc.name: IsOcclusionNodesNeedSync003
 * @tc.desc: test IsOcclusionNodesNeedSync003
 * @tc.type: FUNC
 * @tc.require:
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
 * @tc.require:
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
    node->SetIsOnTheTree(true);
    RSRenderNodeMap& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);
    ASSERT_FALSE(mainThread->IsOcclusionNodesNeedSync(nodeId, false));
}

/**
 * @tc.name: ProcessHgmFrameRate
 * @tc.desc: test ProcessHgmFrameRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMainThreadTest, ProcessHgmFrameRate, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    uint64_t timestamp = 0;
    FrameRateLinkerId id = 0;
    mainThread->hgmContext_.rsFrameRateLinker_ = std::make_shared<RSRenderFrameRateLinker>(id);
    mainThread->ProcessHgmFrameRate(timestamp);

    auto vsyncGenerator = CreateVSyncGenerator();
    auto vsyncController = new VSyncController(vsyncGenerator, 0);
    mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    mainThread->ProcessHgmFrameRate(timestamp);

    mainThread->rsVSyncDistributor_ = nullptr;
    mainThread->ProcessHgmFrameRate(timestamp);
    sleep(1);
}

/**
 * @tc.name: SetCurtainScreenUsingStatus
 * @tc.desc: test SetCurtainScreenUsingStatus
 * @tc.type: FUNC
 * @tc.require: issueIB5RAM
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
 * @tc.name: CountMem
 * @tc.desc: test CountMem
 * @tc.type: FUNC
 * @tc.require: issueIB5RAM
 */
HWTEST_F(RSMainThreadTest, CountMem, TestSize.Level2)
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
 * @tc.name: UpdateSubSurfaceCnt001
 * @tc.desc: test UpdateSubSurfaceCnt when info empty
 * @tc.type: FUNC
 * @tc.require: issueIBBUDG
 */
HWTEST_F(RSMainThreadTest, UpdateSubSurfaceCnt001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->UpdateSubSurfaceCnt();
}

/**
 * @tc.name: UpdateSubSurfaceCnt002
 * @tc.desc: test UpdateSubSurfaceCnt when addChild
 * @tc.type: FUNC
 * @tc.require: issueIBBUDG
 */
HWTEST_F(RSMainThreadTest, UpdateSubSurfaceCnt002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto context = mainThread->context_;
    ASSERT_NE(context, nullptr);
    const int cnt = 0;
    const int id = 100;
    auto rootNode = std::make_shared<RSRenderNode>(id, context);

    auto leashNode = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    leashNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    rootNode->AddChild(leashNode);

    auto appNode = std::make_shared<RSSurfaceRenderNode>(id + 2, context);
    appNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    leashNode->AddChild(appNode);

    context->nodeMap.RegisterRenderNode(rootNode);
    context->nodeMap.RegisterRenderNode(leashNode);
    context->nodeMap.RegisterRenderNode(appNode);

    mainThread->UpdateSubSurfaceCnt();
    // cnt + 2: rootNode contain 2 subSurfaceNodes(leash and app)
    ASSERT_EQ(rootNode->subSurfaceCnt_, cnt + 2);
}

/**
 * @tc.name: UpdateSubSurfaceCnt003
 * @tc.desc: test UpdateSubSurfaceCnt when removeChild
 * @tc.type: FUNC
 * @tc.require: issueIBBUDG
 */
HWTEST_F(RSMainThreadTest, UpdateSubSurfaceCnt003, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto context = mainThread->context_;
    ASSERT_NE(context, nullptr);
    const int cnt = 0;
    const int id = 100;
    auto rootNode = std::make_shared<RSRenderNode>(id, context);

    auto leashNode = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    leashNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    rootNode->AddChild(leashNode);

    auto appNode = std::make_shared<RSSurfaceRenderNode>(id + 2, context);
    appNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    leashNode->AddChild(appNode);

    leashNode->RemoveChild(appNode);
    rootNode->RemoveChild(leashNode);

    context->nodeMap.RegisterRenderNode(rootNode);
    context->nodeMap.RegisterRenderNode(leashNode);
    context->nodeMap.RegisterRenderNode(appNode);

    mainThread->UpdateSubSurfaceCnt();
    ASSERT_EQ(rootNode->subSurfaceCnt_, cnt);
}

/**
 * @tc.name: ClearMemoryCache
 * @tc.desc: test ClearMemoryCache
 * @tc.type: FUNC
 * @tc.require: issueIB8HAQ
 */
HWTEST_F(RSMainThreadTest, ClearMemoryCache, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    mainThread->ClearMemoryCache(PROCESS_EXIT, true, -1);
}

/**
 * @tc.name: DoDirectComposition
 * @tc.desc: Test DoDirectComposition
 * @tc.type: FUNC
 * @tc.require: issueIB8HAQ
 */
HWTEST_F(RSMainThreadTest, DoDirectComposition001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId rootId = 0;
    auto rootNode = std::make_shared<RSBaseRenderNode>(rootId);
    NodeId displayId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(displayId, 0, rsContext->weak_from_this());
    rootNode->AddChild(displayNode);
    rootNode->GenerateFullChildrenList();

    bool doDirectComposition = mainThread->DoDirectComposition(rootNode, false);
    ASSERT_FALSE(doDirectComposition);
}

/**
 * @tc.name: DoDirectComposition
 * @tc.desc: Test DoDirectComposition
 * @tc.type: FUNC
 * @tc.require: issueIB8HAQ
 */
HWTEST_F(RSMainThreadTest, DoDirectComposition002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId rootId = 0;
    auto rootNode = std::make_shared<RSBaseRenderNode>(rootId);
    NodeId displayId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(displayId, 0, rsContext->weak_from_this());
    rootNode->AddChild(displayNode);
    rootNode->GenerateFullChildrenList();
    auto childNode = RSRenderNode::ReinterpretCast<RSScreenRenderNode>(rootNode->GetChildren()->front());
    childNode->SetCompositeType(CompositeType::UNI_RENDER_COMPOSITE);

    bool doDirectComposition = mainThread->DoDirectComposition(rootNode, false);
    ASSERT_FALSE(doDirectComposition);
}

/**
 * @tc.name: MultiDisplayChangeTest
 * @tc.desc: test MultiDisplayChangeTest
 * @tc.type: FUNC
 * @tc.require: issueIBF9OU
 */
HWTEST_F(RSMainThreadTest, MultiDisplayChangeTest, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->isMultiDisplayPre_ = false;
    auto getMultiDisplayStatus = mainThread->GetMultiDisplayStatus();
    EXPECT_FALSE(getMultiDisplayStatus);
    mainThread->isMultiDisplayChange_ = false;
    EXPECT_FALSE(mainThread->GetMultiDisplayChange());
    mainThread->MultiDisplayChange(getMultiDisplayStatus);
}

/**
 * @tc.name: RegisterScreenNodeListenerTest
 * @tc.desc: test RegisterScreenNodeListenerTest
 * @tc.type: FUNC
 * @tc.require: issueIBF9OU
 */
HWTEST_F(RSMainThreadTest, RegisterScreenNodeListenerTest, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    mainThread->RegisterScreenNodeListener();
    auto screenManagerPtr = impl::RSScreenManager::GetInstance();
    auto* screenManager = static_cast<impl::RSScreenManager*>(screenManagerPtr.GetRefPtr());
    EXPECT_NE(screenManager->screenNodeListener_, nullptr);

    screenManager->screens_.insert(std::make_pair(100, nullptr));
    mainThread->RegisterScreenNodeListener();

    screenManager->NotifyScreenNodeChange(0, true);
    screenManager->NotifyScreenNodeChange(0, false);
}

/**
 * @tc.name: OnScreenConnectTest
 * @tc.desc: test OnScreenDisConnect
 * @tc.type: FUNC
 * @tc.require: issueIBF9OU
 */
HWTEST_F(RSMainThreadTest, OnScreenConnectTest, TestSize.Level2)
{
    auto mainThread1 = RSMainThread::Instance();
    ASSERT_NE(mainThread1, nullptr);

    bool backUpState = mainThread1->isRunning_;
    mainThread1->isRunning_ = false;
    RSDisplayNodeConfig config;
    NodeId displayNodeId1 = 1;
    auto displayNode1 = std::make_shared<RSLogicalDisplayRenderNode>(displayNodeId1, config);
    uint64_t screenId = 1;
    displayNode1->SetScreenId(screenId);
    displayNode1->waitToSetOnTree_ = true;
    mainThread1->context_->nodeMap.logicalDisplayNodeMap_.insert({displayNodeId1, displayNode1});

    NodeId displayNodeId2 = 2;
    auto displayNode2 = std::make_shared<RSLogicalDisplayRenderNode>(displayNodeId2, config);
    mainThread1->context_->nodeMap.logicalDisplayNodeMap_.insert({displayNodeId2, displayNode2});

    auto screenManagerPtr = impl::RSScreenManager::GetInstance();
    auto* screenManager = static_cast<impl::RSScreenManager*>(screenManagerPtr.GetRefPtr());
    screenManager->screenNodeListener_->OnScreenConnect(1);
    mainThread1->isRunning_ = backUpState;
}

/**
 * @tc.name: OnScreenDisconnectTest
 * @tc.desc: test OnScreenDisconnectTest
 * @tc.type: FUNC
 * @tc.require: issueIBF9OU
 */
HWTEST_F(RSMainThreadTest, OnScreenDisconnectTest, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    auto screenManagerPtr = impl::RSScreenManager::GetInstance();
    auto* screenManager = static_cast<impl::RSScreenManager*>(screenManagerPtr.GetRefPtr());
    screenManager->screenNodeListener_->OnScreenDisconnect(0);
}
/**
 * @tc.name: IsFastComposeVsyncTimeSync001
 * @tc.desc: test IsFastComposeVsyncTimeSync input value error condition
 * @tc.type: FUNC
 * @tc.require: issueICGGHY
 */
HWTEST_F(RSMainThreadTest, IsFastComposeVsyncTimeSync001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    uint64_t unsignedVsyncPeriod = 0;
    bool nextVsyncRequested = false;
    uint64_t unsignedNowTime = 1000;
    uint64_t lastVsyncTime = 500;
    int64_t vsyncTimeStamp = -100;
    uint64_t timestamp = mainThread->timestamp_;
    bool result = mainThread->IsFastComposeVsyncTimeSync(unsignedVsyncPeriod, nextVsyncRequested,
        unsignedNowTime, lastVsyncTime, vsyncTimeStamp);
    ASSERT_EQ(result, false);
    unsignedVsyncPeriod = 16666666;
    result = mainThread->IsFastComposeVsyncTimeSync(unsignedVsyncPeriod, nextVsyncRequested,
        unsignedNowTime, lastVsyncTime, vsyncTimeStamp);
    ASSERT_EQ(result, false);
    vsyncTimeStamp = 16666666;
    mainThread->timestamp_ = 1000;
    result = mainThread->IsFastComposeVsyncTimeSync(unsignedVsyncPeriod, nextVsyncRequested,
        unsignedNowTime, lastVsyncTime, vsyncTimeStamp);
    ASSERT_EQ(result, false);
    mainThread->timestamp_ = 15666666;
    result = mainThread->IsFastComposeVsyncTimeSync(unsignedVsyncPeriod, nextVsyncRequested,
        unsignedNowTime, lastVsyncTime, vsyncTimeStamp);
    mainThread->timestamp_ = 17666666;
    result = mainThread->IsFastComposeVsyncTimeSync(unsignedVsyncPeriod, nextVsyncRequested,
        unsignedNowTime, lastVsyncTime, vsyncTimeStamp);
    ASSERT_EQ(result, true);
    nextVsyncRequested = true;
    result = mainThread->IsFastComposeVsyncTimeSync(unsignedVsyncPeriod, nextVsyncRequested,
        unsignedNowTime, lastVsyncTime, vsyncTimeStamp);
    ASSERT_EQ(result, true);
    mainThread->timestamp_ = timestamp;
}

/**
 * @tc.name: IsFastComposeVsyncTimeSync002
 * @tc.desc: test IsFastComposeVsyncTimeSync at time near vsync time or not condition
 * @tc.type: FUNC
 * @tc.require: issueICGGHY
 */
HWTEST_F(RSMainThreadTest, IsFastComposeVsyncTimeSync002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    uint64_t unsignedVsyncPeriod = 0;
    bool nextVsyncRequested = false;
    uint64_t unsignedNowTime = 1000;
    uint64_t lastVsyncTime = 500;
    int64_t vsyncTimeStamp = 16666666;
    uint64_t timestamp = mainThread->timestamp_;
    mainThread->timestamp_ = 16666666;
    bool result = mainThread->IsFastComposeVsyncTimeSync(unsignedVsyncPeriod, nextVsyncRequested,
        unsignedNowTime, lastVsyncTime, vsyncTimeStamp);
    ASSERT_EQ(result, true);
    unsignedNowTime = 1000;
    result = mainThread->IsFastComposeVsyncTimeSync(unsignedVsyncPeriod, nextVsyncRequested,
        unsignedNowTime, lastVsyncTime, vsyncTimeStamp);
    ASSERT_EQ(result, true);
    nextVsyncRequested = true;
    result = mainThread->IsFastComposeVsyncTimeSync(unsignedVsyncPeriod, nextVsyncRequested,
        unsignedNowTime, lastVsyncTime, vsyncTimeStamp);
    ASSERT_EQ(result, true);
    mainThread->timestamp_ = timestamp;
}

/**
 * @tc.name: CheckFastCompose
 * @tc.desc: test CheckFastCompose
 * @tc.type: FUNC
 * @tc.require: issueIBGV2W
 */
HWTEST_F(RSMainThreadTest, CheckFastCompose001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto receiver = mainThread->receiver_;
    if (mainThread->rsVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->rsVSyncDistributor_ = new VSyncDistributor(vsyncController, "rs");
    }
    sptr<VSyncConnection> conn = new VSyncConnection(mainThread->rsVSyncDistributor_, "rs");
    mainThread->receiver_ = nullptr;
    mainThread->CheckFastCompose(0);
    mainThread->receiver_ = std::make_shared<VSyncReceiver>(conn);
    mainThread->lastFastComposeTimeStamp_ = 0;
    mainThread->CheckFastCompose(mainThread->timestamp_ - 1);
    mainThread->lastFastComposeTimeStamp_ = mainThread->timestamp_;
    mainThread->CheckFastCompose(mainThread->timestamp_ - 1);
    ASSERT_NE(mainThread->requestNextVsyncNum_.load(), 0);
    mainThread->receiver_ = receiver;
}

/**
 * @tc.name: CheckFastCompose
 * @tc.desc: test CheckFastCompose
 * @tc.type: FUNC
 * @tc.require: issueICGGHY
 */
HWTEST_F(RSMainThreadTest, CheckFastCompose002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    uint64_t timestamp = mainThread->timestamp_;
    mainThread->timestamp_ = mainThread->timestamp_ - 16666666;
    mainThread->CheckFastCompose(mainThread->timestamp_ - 1);
    ASSERT_NE(mainThread->requestNextVsyncNum_.load(), 0);
    mainThread->timestamp_ = timestamp;
}

/**
 * @tc.name: InitVulkanErrorCallback001Test
 * @tc.desc: test InitVulkanErrorCallback001Test
 * @tc.type: FUNC
 * @tc.require: issueIBOLWU
 */
HWTEST_F(RSMainThreadTest, InitVulkanErrorCallback001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    Drawing::GPUContext gpuContext;
    mainThread->InitVulkanErrorCallback(&gpuContext);
}

/**
 * @tc.name: RenderServiceAllSurafceDump01
 * @tc.desc: RenderServiceAllSurafceDump Test
 * @tc.type: FUNC
 * @tc.require: issueIB57QP
 */
HWTEST_F(RSMainThreadTest, RenderServiceAllSurafceDump01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    //prepare nodemap
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(config);
    node1->SetIsOnTheTree(true);
    config.id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    node2->SetIsOnTheTree(true);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node1);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);

    DfxString log;
    mainThread->RenderServiceAllSurafceDump(log);
}

/**
 * @tc.name: ExistBufferIsVisibleAndUpdateTest
 * @tc.desc: RenderServiceAllSurafceDump Test
 * @tc.type: FUNC
 * @tc.require: issueIC0AQO
 */
HWTEST_F(RSMainThreadTest, ExistBufferIsVisibleAndUpdateTest, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    EXPECT_FALSE(mainThread->ExistBufferIsVisibleAndUpdate());
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes;
    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    sptr<SurfaceBuffer> preBuffer = SurfaceBuffer::Create();
    sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
    Rect damageRect = {0, 0, 0, 0};
    int64_t timestamp = 0;
    ASSERT_NE(surfaceNode->surfaceHandler_, nullptr);
    surfaceNode->surfaceHandler_->SetBuffer(preBuffer, acquireFence, damageRect, timestamp);
    surfaceNode->surfaceHandler_->isCurrentFrameBufferConsumed_ = true;
    surfaceNode->HwcSurfaceRecorder().SetLastFrameHasVisibleRegion(true);
    hardwareEnabledNodes.push_back(surfaceNode);
    mainThread->hardwareEnabledNodes_ = hardwareEnabledNodes;
    EXPECT_TRUE(mainThread->ExistBufferIsVisibleAndUpdate());
}

/**
 * @tc.name: GetForceCommitReasonTest
 * @tc.desc: GetForceCommitReasonTest
 * @tc.type: FUNC
 * @tc.require: issueIC0AQO
 */
HWTEST_F(RSMainThreadTest, GetForceCommitReasonTest, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->isHardwareEnabledBufferUpdated_ = false;
    mainThread->forceUpdateUniRenderFlag_ = false;
    EXPECT_EQ(mainThread->GetForceCommitReason(), 0);
    uint32_t forceCommitReason = 0;
    forceCommitReason |= ForceCommitReason::FORCED_BY_HWC_UPDATE;
    mainThread->isHardwareEnabledBufferUpdated_ = true;
    EXPECT_EQ(mainThread->GetForceCommitReason(), forceCommitReason);
    mainThread->forceUpdateUniRenderFlag_ = true;
    forceCommitReason |= ForceCommitReason::FORCED_BY_UNI_RENDER_FLAG;
    EXPECT_EQ(mainThread->GetForceCommitReason(), forceCommitReason);
}

/**
 * @tc.name: HandleTunnelLayerId001
 * @tc.desc: HandleTunnelLayerId001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMainThreadTest, HandleTunnelLayerId001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0, mainThread->context_);
    auto surfaceHandler = surfaceNode->surfaceHandler_;

    mainThread->HandleTunnelLayerId(surfaceHandler, surfaceNode);
    EXPECT_EQ(surfaceNode->GetTunnelLayerId(), 0);
}

/**
 * @tc.name: HandleTunnelLayerId002
 * @tc.desc: HandleTunnelLayerId002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMainThreadTest, HandleTunnelLayerId002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0, mainThread->context_);
    auto surfaceHandler = surfaceNode->surfaceHandler_;
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->consumer_ = nullptr;

    EXPECT_EQ(surfaceHandler->sourceType_, 0);

    mainThread->HandleTunnelLayerId(surfaceHandler, surfaceNode);
    EXPECT_EQ(surfaceNode->GetTunnelLayerId(), 0);
}

/**
 * @tc.name: HandleTunnelLayerId003
 * @tc.desc: HandleTunnelLayerId003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMainThreadTest, HandleTunnelLayerId003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0, mainThread->context_);
    auto surfaceHandler = surfaceNode->surfaceHandler_;
    ASSERT_NE(surfaceHandler, nullptr);
    auto consumer = surfaceHandler->GetConsumer();
    ASSERT_NE(consumer, nullptr);

    EXPECT_EQ(surfaceHandler->sourceType_, 0);

    mainThread->HandleTunnelLayerId(surfaceHandler, surfaceNode);
    EXPECT_EQ(surfaceNode->GetTunnelLayerId(), 0);
}

/**
 * @tc.name: HandleTunnelLayerId004
 * @tc.desc: HandleTunnelLayerId004
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMainThreadTest, HandleTunnelLayerId004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0, mainThread->context_);
    auto surfaceHandler = surfaceNode->surfaceHandler_;
    ASSERT_NE(surfaceHandler, nullptr);
    auto consumer = surfaceHandler->GetConsumer();
    ASSERT_NE(consumer, nullptr);

    EXPECT_EQ(surfaceHandler->sourceType_, 0);

    surfaceHandler->sourceType_ = 5;
    EXPECT_EQ(surfaceHandler->GetSourceType(), 5);
    mainThread->HandleTunnelLayerId(surfaceHandler, surfaceNode);
}

/**
 * @tc.name: DoDirectComposition003
 * @tc.desc: Test DoDirectComposition
 * @tc.type: FUNC
 * @tc.require: icc3sm
 */
HWTEST_F(RSMainThreadTest, DoDirectComposition003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId rootId = 0;
    auto rootNode = std::make_shared<RSBaseRenderNode>(rootId);
    ASSERT_NE(rootNode, nullptr);
    auto displayNode = GetAndInitScreenRenderNode();
    ASSERT_NE(displayNode, nullptr);
    auto otherNode = std::make_shared<RSRenderNode>(2);
    displayNode->renderDrawable_ = std::make_shared<DrawableV2::RSRenderNodeDrawable>(otherNode);

    auto drawable = DrawableV2::RSScreenRenderNodeDrawable::OnGenerate(otherNode);
    auto displayDrawable = static_cast<DrawableV2::RSScreenRenderNodeDrawable*>(drawable);
    ASSERT_NE(displayDrawable, nullptr);
    ASSERT_NE(displayDrawable->surfaceHandler_, nullptr);
    displayDrawable->surfaceHandler_->buffer_.buffer = SurfaceBuffer::Create();
    auto handle = new BufferHandle();
    handle->usage = BUFFER_USAGE_CPU_READ;
    displayDrawable->surfaceHandler_->buffer_.buffer->SetBufferHandle(handle);
    displayNode->renderDrawable_.reset(displayDrawable);

    rootNode->AddChild(displayNode);
    rootNode->GenerateFullChildrenList();
    auto childNode = RSRenderNode::ReinterpretCast<RSScreenRenderNode>(rootNode->GetChildren()->front());
    childNode->SetCompositeType(CompositeType::UNI_RENDER_COMPOSITE);
    auto type = system::GetParameter("persist.sys.graphic.anco.disableHebc", "-1");
    system::SetParameter("persist.sys.graphic.anco.disableHebc", "1");
    RSSurfaceRenderNode::SetAncoForceDoDirect(true);
    ASSERT_FALSE(mainThread->DoDirectComposition(rootNode, false));

    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes = mainThread->hardwareEnabledNodes_;
    ChangeHardwareEnabledNodesBufferData(hardwareEnabledNodes);

    NodeId displayId2 = 2;
    auto context = std::make_shared<RSContext>();
    auto displayNode2 = std::make_shared<RSScreenRenderNode>(displayId2, 0, context);
    rootNode->AddChild(displayNode2);
    rootNode->GenerateFullChildrenList();

    ASSERT_FALSE(mainThread->DoDirectComposition(rootNode, false));
    system::SetParameter("persist.sys.graphic.anco.disableHebc", type);
    delete handle;
}

/**
 * @tc.name: DoDirectComposition004
 * @tc.desc: Test DoDirectComposition For HwcNodes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMainThreadTest, DoDirectComposition004_BufferSync, TestSize.Level1)
{
    // INIT SCREEN
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    auto rsScreen = std::make_shared<impl::RSScreen>(5, false, HdiOutput::CreateHdiOutput(5), nullptr);
    ASSERT_NE(rsScreen, nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);

    // INIT DISPLAY
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId rootId = 0;
    auto rootNode = std::make_shared<RSBaseRenderNode>(rootId);
    NodeId displayId = 1;
    RSDisplayNodeConfig config;
    config.screenId = 5; // screeId is 5 for test

    // INIT CHILDLIST
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(displayId, config.screenId, rsContext->weak_from_this());

    auto displayNode2 = std::make_shared<RSScreenRenderNode>(displayId, 2, rsContext->weak_from_this());

    rootNode->AddChild(displayNode);
    rootNode->AddChild(displayNode2);
    rootNode->GenerateFullChildrenList();
    auto childNode = RSRenderNode::ReinterpretCast<RSScreenRenderNode>(rootNode->GetChildren()->front());
    childNode->SetCompositeType(CompositeType::UNI_RENDER_COMPOSITE);

    // INIT NodeList
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config.screenId, mainThread->context_);
    surfaceNode->InitRenderParams();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_NE(surfaceNode->surfaceHandler_, nullptr);
    surfaceNode->SetHardwareForcedDisabledState(false);
    surfaceNode->HwcSurfaceRecorder().SetLastFrameHasVisibleRegion(true);
    mainThread->hardwareEnabledNodes_.clear();
    mainThread->hardwareEnabledNodes_.emplace_back(surfaceNode);

    // true true
    mainThread->isUniRender_ = true;
    displayNode->HwcDisplayRecorder().hasVisibleHwcNodes_ = true;
    surfaceNode->surfaceHandler_->SetCurrentFrameBufferConsumed();
    ASSERT_TRUE(mainThread->DoDirectComposition(rootNode, false));

    // true false
    displayNode->HwcDisplayRecorder().hasVisibleHwcNodes_ = true;
    surfaceNode->surfaceHandler_->ResetCurrentFrameBufferConsumed();
    ASSERT_TRUE(mainThread->DoDirectComposition(rootNode, false));

    std::shared_ptr<RSBaseRenderEngine> renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    mainThread->renderEngine_ = renderEngine;
    // false true
    displayNode->HwcDisplayRecorder().hasVisibleHwcNodes_ = true;
    mainThread->isUniRender_ = false;
    surfaceNode->surfaceHandler_->SetCurrentFrameBufferConsumed();
    ASSERT_TRUE(mainThread->DoDirectComposition(rootNode, false));

    // false false
    displayNode->HwcDisplayRecorder().hasVisibleHwcNodes_ = true;
    mainThread->isUniRender_ = false;
    surfaceNode->surfaceHandler_->ResetCurrentFrameBufferConsumed();
    ASSERT_TRUE(mainThread->DoDirectComposition(rootNode, false));

    // RESET
    mainThread->renderEngine_ = nullptr;
}

/**
 * @tc.name: SetTaskEndWithTime001
 * @tc.desc: Test SetTaskEndWithTime
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMainThreadTest, SetTaskEndWithTime001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto rsVSyncDistributor = mainThread->rsVSyncDistributor_;
    mainThread->SetTaskEndWithTime(0);
}

/**
 * @tc.name: CheckAdaptiveCompose001
 * @tc.desc: Test CheckAdaptiveCompose
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMainThreadTest, CheckAdaptiveCompose001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto context = mainThread->context_;
    mainThread->context_ = nullptr;
    ASSERT_FALSE(mainThread->CheckAdaptiveCompose());
    //reset
    mainThread->context_ = context;
}

/**
 * @tc.name: CheckAdaptiveCompose002
 * @tc.desc: Test CheckAdaptiveCompose
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMainThreadTest, CheckAdaptiveCompose002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto &hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    if (frameRateMgr != nullptr) {
        std::atomic<int32_t> status = frameRateMgr->isAdaptive_.load();
        frameRateMgr->isAdaptive_.store(SupportASStatus::NOT_SUPPORT);
        ASSERT_FALSE(mainThread->CheckAdaptiveCompose());
        frameRateMgr->isAdaptive_.store(SupportASStatus::SUPPORT_AS);
        ASSERT_FALSE(mainThread->CheckAdaptiveCompose());
        //reset
        frameRateMgr->isAdaptive_.store(status.load());
    }
}

/**
 * @tc.name: DumpMem001
 * @tc.desc: Test DumpMem
 * @tc.type: FUNC
 */
HWTEST_F(RSMainThreadTest, DumpMem001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    mainThread->renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    std::unordered_set<std::u16string> args;
    std::string dumpString;
    std::string type = "";
    pid_t pid = 0;

    // prepare nodes
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    const std::shared_ptr<RSBaseRenderNode> rootNode = context->GetGlobalRootRenderNode();
    RSRenderNodeMap& nodeMap = context->GetMutableNodeMap();
    ASSERT_NE(rootNode, nullptr);

    //prepare nodemap
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node1, nullptr);
    node1->SetIsOnTheTree(true);
    nodeMap.renderNodeMap_[pid].insert({ config.id, node1 });

    config.id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node2, nullptr);
    node2->SetIsOnTheTree(false);
    node2->instanceRootNodeId_ = INVALID_NODEID;
    nodeMap.renderNodeMap_[pid].insert({ config.id, node2 });

    config.id = 3;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node3, nullptr);
    node3->SetIsOnTheTree(true);
    node3->instanceRootNodeId_ = 1;
    nodeMap.renderNodeMap_[pid].insert({ config.id, node3 });
    nodeMap.renderNodeMap_[pid].insert({ 4, nullptr });
    mainThread->DumpMem(args, dumpString, type, pid);

    // rootNode == nullptr
    context->globalRootRenderNode_ = nullptr;
    mainThread->DumpMem(args, dumpString, type, pid);
}
} // namespace OHOS::Rosen
