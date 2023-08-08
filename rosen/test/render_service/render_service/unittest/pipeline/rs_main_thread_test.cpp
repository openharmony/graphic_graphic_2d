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
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_qos_thread.h"
#include "pipeline/rs_render_engine.h"
#include "platform/common/rs_innovation.h"
#include "platform/common/rs_system_properties.h"
#include "rs_test_util.h"
#if defined(ACCESSIBILITY_ENABLE)
#include "accessibility_config.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

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
    mainThread->isUniRender_ = false;
    mainThread->ProcessCommand();
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
    mainThread->InitRSEventDetector();
    mainThread->SetRSEventDetectorLoopFinishTag();
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
    mainThread->doWindowAnimate_ = false;
    mainThread->isUniRender_ = true;
    mainThread->CalcOcclusion();
}

/**
 * @tc.name: CheckQosVisChanged001
 * @tc.desc: Test RSMainThreadTest.CheckQosVisChanged, pidVisMap is empty
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, CheckQosVisChanged001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::map<uint32_t, bool> pidVisMap;
    auto isVisibleChanged = mainThread->CheckQosVisChanged(pidVisMap);
    ASSERT_EQ(false, isVisibleChanged);
}

/**
 * @tc.name: CheckQosVisChanged002
 * @tc.desc: Test RSMainThreadTest.CheckQosVisChanged, pidVisMap is not empty
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, CheckQosVisChanged002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::map<uint32_t, bool> pidVisMap;
    pidVisMap[0] = true;
    mainThread->lastPidVisMap_[0] = false;
    auto isVisibleChanged = mainThread->CheckQosVisChanged(pidVisMap);
    ASSERT_EQ(true, isVisibleChanged);
}

/**
 * @tc.name: CheckQosVisChanged003
 * @tc.desc: Test RSMainThreadTest.CheckQosVisChanged, pidVisMap is not empty, lastPidVisMap_ equals to pidVisMap
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, CheckQosVisChanged003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::map<uint32_t, bool> pidVisMap;
    pidVisMap[0] = true;
    mainThread->lastPidVisMap_[0] = true;
    auto isVisibleChanged = mainThread->CheckQosVisChanged(pidVisMap);
    ASSERT_EQ(false, isVisibleChanged);
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
    mainThread->UnRegisterOcclusionChangeCallback(0);
}

/**
 * @tc.name: QosStateDump001
 * @tc.desc: When qosCal_ is false, QosStateDump's dump string is "QOS is disabled\n"
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, QosStateDump001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    RSQosThread::GetInstance()->SetQosCal(false);
    std::string str = "";
    mainThread->QosStateDump(str);
    ASSERT_EQ(str, "QOS is disabled\n");
}

/**
 * @tc.name: QosStateDump002
 * @tc.desc: When qosCal_ is true, QosStateDump's dump string is "QOS is enabled\n"
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, QosStateDump002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    RSQosThread::GetInstance()->SetQosCal(true);
    std::string str = "";
    mainThread->QosStateDump(str);
    ASSERT_EQ(str, "QOS is enabled\n");
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
    mainThread->SetFocusAppInfo(-1, -1, str, str, 0);
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

    // when syncTransactionData_ is not empty and SyncId is equal or smaller
    rsTransactionData->SetSyncTransactionNum(1);
    rsTransactionData->SetSyncId(1);
    mainThread->syncTransactionCount_ = 1;
    mainThread->ProcessSyncRSTransactionData(rsTransactionData, pid);
    ASSERT_EQ(mainThread->syncTransactionData_.empty(), true);
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
    ASSERT_EQ(mainThread->pendingEffectiveCommands_.empty(), false);
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
    ASSERT_EQ(mainThread->pendingEffectiveCommands_.empty(), false);
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
    ASSERT_EQ(mainThread->cachedCommands_[nodeId].empty(), false);
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
    ASSERT_EQ(mainThread->cachedCommands_[nodeId].empty(), false);

    mainThread->cachedCommands_.clear();
    rsTransactionData = std::make_unique<RSTransactionData>();
    command = nullptr;
    followType = FollowType::FOLLOW_TO_PARENT;
    rsTransactionData->AddCommand(command, nodeId + 1, followType);
    mainThread->ClassifyRSTransactionData(rsTransactionData);
    ASSERT_EQ(mainThread->cachedCommands_[nodeId + 1].empty(), false);
}

/**
 * @tc.name: AddActiveNodeId
 * @tc.desc: Test AddActiveNodeId, add fake nodeid info, check if fails
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, AddActiveNodeId, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->activeAppsInProcess_.clear();
    mainThread->activeProcessNodeIds_.clear();
    pid_t pid = 1;
    NodeId id = ((NodeId)pid << 32) + 1;
    mainThread->AddActiveNodeId(pid, id);
    ASSERT_NE(static_cast<int>(mainThread->activeAppsInProcess_[pid].size()), 0);
    for (auto iter : mainThread->activeAppsInProcess_[pid]) {
        ASSERT_EQ(iter, INVALID_NODEID);
    }
}

/**
 * @tc.name: WaitUtilDrivenRenderFinished
 * @tc.desc: Test WaitUtilDrivenRenderFinished, check if drivenRenderFinished_ is valid
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSMainThreadTest, WaitUtilDrivenRenderFinished, TestSize.Level1)
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    auto mainThread = RSMainThread::Instance();
    mainThread->NotifyDrivenRenderFinish();
    mainThread->WaitUtilDrivenRenderFinished();
    ASSERT_EQ(mainThread->drivenRenderFinished_, true);
#endif
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
 * @tc.name: ProcessCommandForUniRender
 * @tc.desc: ProcessCommandForUniRender test with invalid data
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSMainThreadTest, ProcessCommandForUniRender, TestSize.Level1)
{
    constexpr uint64_t REFRESH_PERIOD = 16666667;
    constexpr uint64_t SKIP_COMMAND_FREQ_LIMIT = 30;
    auto mainThread = RSMainThread::Instance();
    ASSERT_EQ(mainThread->effectiveTransactionDataIndexMap_.empty(), true);

    // // let lastindex same as timeout index and test timeout case at first
    mainThread->transactionDataLastWaitTime_[0] = 0;
    mainThread->timestamp_ = REFRESH_PERIOD * SKIP_COMMAND_FREQ_LIMIT + 1;
    mainThread->effectiveTransactionDataIndexMap_[0].first = 0;
    // default data with index 0
    auto data = std::make_unique<RSTransactionData>();
    mainThread->effectiveTransactionDataIndexMap_[0].second.emplace_back(std::move(data));
    // empty data
    mainThread->effectiveTransactionDataIndexMap_[0].second.emplace_back(nullptr);
    mainThread->ProcessCommandForUniRender();
}

/**
 * @tc.name: ObserverConfigChangeColorFilter
 * @tc.desc: AccessibilityObserver changes different config of case ColorFilter
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
#if defined(ACCESSIBILITY_ENABLE)
HWTEST_F(RSMainThreadTest, ObserverConfigChangeColorFilter, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto observer = mainThread->accessibilityObserver_;
    ASSERT_NE(observer, nullptr);

    CONFIG_ID id = CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER;
    std::vector<DALTONIZATION_TYPE> dalTypes = { DALTONIZATION_TYPE::Normal, DALTONIZATION_TYPE::Protanomaly,
        DALTONIZATION_TYPE::Deuteranomaly, DALTONIZATION_TYPE::Tritanomaly };
    ConfigValue value;
    for (auto dalType : dalTypes) {
        value.daltonizationColorFilter = dalType;
        observer->OnConfigChanged(id, value);
    }
}
#endif

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
    mainThread->DoParallelComposition(node);
}
} // namespace OHOS::Rosen
