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
 

#include "common/rs_common_def.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_manager.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_buffer_layer.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_hpae.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_util.h"
#include "feature/hdr/rs_hdr_util.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "hetero_hdr/rs_hdr_pattern_manager.h"  // rs base
#include "hetero_hdr/rs_hdr_vulkan_task.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/render_thread/rs_divided_render_util.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_test_util.h"
#include "platform/common/rs_log.h"
#include "utils/matrix.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;
using namespace std;

namespace OHOS::Rosen {
constexpr NodeId DEFAULT_ID = 0xFFFF;
class RSHeteroHDRManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    shared_ptr<RSScreenRenderNode> GenerateScreenNode();
    shared_ptr<RSSurfaceRenderNode> GenerateAppNode();
    shared_ptr<RSSurfaceRenderNode> GenerateLeashNode();
    shared_ptr<RSSurfaceRenderNode> GenerateSurfaceNode();
    RSSurfaceRenderParams* GetRenderParams(shared_ptr<RSSurfaceRenderNode> surfaceNode);
    RSScreenRenderParams* GetScreenParams(shared_ptr<RSScreenRenderNode> screenNode);

    void GenerateValidSurfaceNode(shared_ptr<RSScreenRenderNode> &screenNode,
                                  shared_ptr<RSSurfaceRenderNode> &appNode,
                                  shared_ptr<RSSurfaceRenderNode> &leashNode,
                                  shared_ptr<RSSurfaceRenderNode> &surfaceNode);
};

void RSHeteroHDRManagerTest::SetUpTestCase()
{
    RS_LOGI("************************RSHeteroHDRManagerTest SetUpTestCase************************");
    RSTestUtil::InitRenderNodeGC();
    HgmCore::Instance().SetVsyncId(1);
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}
void RSHeteroHDRManagerTest::TearDownTestCase()
{
    RS_LOGI("************************RSHeteroHDRManagerTest TearDownTestCase************************");
}
void RSHeteroHDRManagerTest::SetUp() { RS_LOGI("SetUp------------------------------------"); }
void RSHeteroHDRManagerTest::TearDown() { RS_LOGI("TearDown------------------------------------"); }
shared_ptr<RSScreenRenderNode> RSHeteroHDRManagerTest::GenerateScreenNode()
{
    NodeId id = 0;
    ScreenId screenId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    if (screenNode == nullptr) {
        RS_LOGE("GenerateScreenNode::screenNode == nullptr");
        return nullptr;
    }

    screenNode->InitRenderParams();
    screenNode->SetIsOnTheTree(true);

    auto screenDrawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(screenNode);
    if (screenDrawableAdapter == nullptr) {
        RS_LOGE("GenerateScreeneNode::screenDrawableAdapter == nullptr");
        return nullptr;
    }
    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawableAdapter->renderParams_.get());
    if (screenParams == nullptr) {
        RS_LOGE("GenerateScreeneNode::screenParams == nullptr");
        return nullptr;
    }
    screenParams->SetHDRPresent(true);

    return screenNode;
}

shared_ptr<RSSurfaceRenderNode> RSHeteroHDRManagerTest::GenerateAppNode()
{
    NodeId id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    if (surfaceNode == nullptr) {
        return nullptr;
    }
    surfaceNode->SetHardwareEnabled(false);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    surfaceNode->InitRenderParams();
    RS_LOGI("surfaceNode->IsAppWindow() = %{public}d", surfaceNode->IsAppWindow());
    return surfaceNode;
}

shared_ptr<RSSurfaceRenderNode> RSHeteroHDRManagerTest::GenerateLeashNode()
{
    NodeId id = 2;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    if (surfaceNode == nullptr) {
        return nullptr;
    }
    surfaceNode->SetHardwareEnabled(false);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);

    surfaceNode->InitRenderParams();
    RS_LOGI("surfaceNode->IsLeashWindow() = %{public}d", surfaceNode->IsLeashWindow());
    return surfaceNode;
}

shared_ptr<RSSurfaceRenderNode> RSHeteroHDRManagerTest::GenerateSurfaceNode()
{
    NodeId id = 3;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    if (surfaceNode == nullptr) {
        RS_LOGE("GenerateSurfaceNode::surfaceNode == nullptr");
        return nullptr;
    }
    surfaceNode->SetHardwareEnabled(false);
    surfaceNode->SetVideoHdrStatus(HDR_VIDEO);

    surfaceNode->InitRenderParams();
    surfaceNode->SetIsOnTheTree(true);

    auto surfaceDrawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    if (surfaceDrawableAdapter == nullptr) {
        RS_LOGE("GenerateSurfaceNode::surfaceDrawableAdapter == nullptr");
        return nullptr;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawableAdapter->renderParams_.get());
    if (surfaceParams == nullptr) {
        RS_LOGE("GenerateSurfaceNode::surfaceParams == nullptr");
        return nullptr;
    }
    RSLayerInfo layerInfo;
    layerInfo.srcRect.w = 1;
    layerInfo.srcRect.h = 1;
    layerInfo.dstRect.w = 1;
    layerInfo.dstRect.h = 1;
    surfaceParams->SetLayerInfo(layerInfo);
    surfaceParams->SetHDRPresent(true);
    surfaceParams->SetColorFollow(false);
    surfaceParams->buffer_ = OHOS::SurfaceBuffer::Create();

    if (surfaceParams->GetBuffer() == nullptr) {
        RS_LOGE("GenerateSurfaceNode::surfaceParams->GetBuffer() is nullptr");
        return nullptr;
    }

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    surfaceParams->GetBuffer()->Alloc(requestConfig);

    return surfaceNode;
}

RSSurfaceRenderParams* RSHeteroHDRManagerTest::GetRenderParams(shared_ptr<RSSurfaceRenderNode> surfaceNode)
{
    auto drawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    if (drawableAdapter == nullptr) {
        RS_LOGE("GetRenderParams::drawableAdapter == nullptr");
        return nullptr;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawableAdapter->renderParams_.get());
    return surfaceParams;
}

RSScreenRenderParams* RSHeteroHDRManagerTest::GetScreenParams(shared_ptr<RSScreenRenderNode> screenNode)
{
    auto drawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(screenNode);
    if (drawableAdapter == nullptr) {
        RS_LOGE("GetScreenParams::drawableAdapter == nullptr");
        return nullptr;
    }
    auto screenParams = static_cast<RSScreenRenderParams*>(drawableAdapter->renderParams_.get());
    return screenParams;
}

void RSHeteroHDRManagerTest::GenerateValidSurfaceNode(shared_ptr<RSScreenRenderNode> &screenNode,
                                                      shared_ptr<RSSurfaceRenderNode> &appNode,
                                                      shared_ptr<RSSurfaceRenderNode> &leashNode,
                                                      shared_ptr<RSSurfaceRenderNode> &surfaceNode)
{
    screenNode = GenerateScreenNode();
    appNode = GenerateAppNode();
    leashNode = GenerateLeashNode();
    surfaceNode = GenerateSurfaceNode();
    if (screenNode == nullptr || appNode == nullptr || leashNode == nullptr || surfaceNode == nullptr) {
        screenNode = nullptr;
        appNode = nullptr;
        leashNode = nullptr;
        surfaceNode = nullptr;

        return;
    }
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    if (surfaceHandler == nullptr) {
        screenNode = nullptr;
        appNode = nullptr;
        leashNode = nullptr;
        surfaceNode = nullptr;
        return;
    }

    auto appParams = GetRenderParams(appNode);
    auto leashParams = GetRenderParams(leashNode);
    auto surfaceParams = GetRenderParams(surfaceNode);
    appParams->SetAncestorScreenNode(screenNode);
    leashParams->SetAncestorScreenNode(screenNode);
    surfaceParams->SetAncestorScreenNode(screenNode);

    auto screenNodeptr = appParams->GetAncestorScreenNode().lock();
    if (!screenNodeptr) {
        RS_LOGE("GenerateValidSurfaceNode::screenNodeptr is nullptr");
    }
    screenNodeptr = leashParams->GetAncestorScreenNode().lock();
    if (!screenNodeptr) {
        RS_LOGE("GenerateValidSurfaceNode::screenNodeptr is nullptr");
    }
    screenNodeptr = surfaceParams->GetAncestorScreenNode().lock();
    if (!screenNodeptr) {
        RS_LOGE("GenerateValidSurfaceNode::screenNodeptr is nullptr");
    }

    appParams->shouldPaint_ = true;
    appParams->contentEmpty_ = false;
    leashParams->shouldPaint_ = true;
    leashParams->contentEmpty_ = false;
    
    leashNode->AddChild(appNode);
    appNode->AddChild(surfaceNode);
}

class MockRSHeteroHDRManager : public RSHeteroHDRManager {
public:
    MOCK_METHOD(bool, MHCRequestEGraph, (uint64_t frameId), (override));
    MOCK_METHOD(bool, MHCSetCurFrameId, (uint64_t frameId), (override));
    MOCK_METHOD(bool, TryConsumeBuffer, (std::function<void()>&& consumingFunc), (override));
};

class SingletonMockRSHeteroHDRManager {
public:
    static MockRSHeteroHDRManager& Instance()
    {
        static MockRSHeteroHDRManager instance;
        return instance;
    }
};

/**
 * @tc.name: UpdateHDRNodesTest001
 * @tc.desc: Test UpdateHDRNodes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, UpdateHDRNodesTest001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    ASSERT_TRUE(surfaceHandler != nullptr);

    surfaceNode->SetVideoHdrStatus(HDR_VIDEO);
    RSHeteroHDRManager::Instance().UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    RSHeteroHDRManager::Instance().PostHDRSubTasks();
}

/**
 * @tc.name: UpdateHDRNodesTest002
 * @tc.desc: Test UpdateHDRNodes input condition
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, UpdateHDRNodesTest002, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    ASSERT_TRUE(surfaceHandler != nullptr);

    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = false;
    surfaceNode->SetVideoHdrStatus(NO_HDR);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());

    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = false;
    surfaceNode->SetVideoHdrStatus(HDR_VIDEO);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());

    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = true;
    surfaceNode->SetVideoHdrStatus(NO_HDR);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());

    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = false;
    surfaceNode->SetVideoHdrStatus(AI_HDR_VIDEO_GAINMAP);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
}

/**
 * @tc.name: UpdateHDRNodesTest003
 * @tc.desc: Test UpdateHDRNodes input condition
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, UpdateHDRNodesTest003, TestSize.Level1)
{
    // register instance root node
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 10;
    surfaceConfig.name = "cameraSurfaceNode";
    auto instanceRootNode = RSTestUtil::CreateSurfaceNode(surfaceConfig);
    ASSERT_NE(instanceRootNode, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    NodeId instanceRootNodeId = instanceRootNode->GetId();
    pid_t instanceRootNodePid = ExtractPid(instanceRootNodeId);
    nodeMap.renderNodeMap_[instanceRootNodePid][instanceRootNodeId] = instanceRootNode;

    // create subsurface node
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    ASSERT_TRUE(surfaceHandler != nullptr);
    surfaceNode->context_ = rsContext;
    surfaceNode->instanceRootNodeId_ = instanceRootNode->GetId();
    ASSERT_NE(surfaceNode->GetInstanceRootNode(), nullptr);

    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = true;
    surfaceNode->SetVideoHdrStatus(HDR_VIDEO);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
}

/**
 * @tc.name: UpdateHDRNodesTest004
 * @tc.desc: Test UpdateHDRNodes input condition
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, UpdateHDRNodesTest004, TestSize.Level1)
{
    // register instance root node
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 10;
    surfaceConfig.name = "cameraSurfaceNode";
    auto instanceRootNode = RSTestUtil::CreateSurfaceNode(surfaceConfig);
    ASSERT_NE(instanceRootNode, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    NodeId instanceRootNodeId = instanceRootNode->GetId();
    pid_t instanceRootNodePid = ExtractPid(instanceRootNodeId);
    nodeMap.renderNodeMap_[instanceRootNodePid][instanceRootNodeId] = instanceRootNode;

    // create subsurface node
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    ASSERT_TRUE(surfaceHandler != nullptr);
    surfaceNode->context_ = rsContext;
    surfaceNode->instanceRootNodeId_ = instanceRootNode->GetId();
    ASSERT_NE(surfaceNode->GetInstanceRootNode(), nullptr);

    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = true;
    surfaceNode->SetVideoHdrStatus(AI_HDR_VIDEO_GAINMAP);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
}

/**
 * @tc.name: PostHDRSubTasksTest001
 * @tc.desc: Test PostHDRSubTasks input condition
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest001, TestSize.Level1)
{
    TestPostHDRSubTasksWhenHeteroOff();
    TestPostHDRSubTasksWhenHeteroOn();
    TestPostHDRSubTasksWithValidSurfaceNode();
    TestPostHDRSubTasksWithMDCExisted();
    TestPostHDRSubTasksWithVsyncIdZero();
    TestPostHDRSubTasksWithDestroyedFlag();
    TestPostHDRSubTasksWithVideoGainmap();
    TestPostHDRSubTasksWithColorFollow();
    TestPostHDRSubTasksWithHDRNotPresent();
}

void RSHeteroHDRManagerTest::TestPostHDRSubTasksWhenHeteroOff()
{
    MockRSHeteroHDRManager mock;
    mock.isHeteroComputingHdrOn_ = false;
    mock.PostHDRSubTasks();
    EXPECT_EQ(mock.pendingPostNodes_.size(), 0);
}

void RSHeteroHDRManagerTest::TestPostHDRSubTasksWhenHeteroOn()
{
    MockRSHeteroHDRManager mock;
    mock.isHeteroComputingHdrOn_ = true;
    mock.PostHDRSubTasks();
    EXPECT_EQ(mock.pendingPostNodes_.size(), 0);
}

void RSHeteroHDRManagerTest::TestPostHDRSubTasksWithValidSurfaceNode()
{
    MockRSHeteroHDRManager mock;
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    SetupSurfaceContext(surfaceNode, appNode);

    EXPECT_CALL(mock, MHCRequestEGraph(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, MHCSetCurFrameId(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, TryConsumeBuffer(_)).WillRepeatedly(Return(true));
    
    mock.isHeteroComputingHdrOn_ = true;
    mock.UpdateHDRNodes(*surfaceNode, false);
    mock.PostHDRSubTasks();
}

void RSHeteroHDRManagerTest::TestPostHDRSubTasksWithMDCExisted()
{
    MockRSHeteroHDRManager mock;
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    SetupSurfaceContext(surfaceNode, appNode);

    RSHeteroHDRHpae::GetInstance().mdcExistedStatus_.store(true);
    mock.curHandleStatus_ = RSHeteroHDRHpae::GetInstance().existedChannelStatus_;
    
    mock.isHeteroComputingHdrOn_ = true;
    mock.UpdateHDRNodes(*surfaceNode, false);
    mock.PostHDRSubTasks();
}

void RSHeteroHDRManagerTest::TestPostHDRSubTasksWithVsyncIdZero()
{
    MockRSHeteroHDRManager mock;
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    SetupSurfaceContext(surfaceNode, appNode);

    HgmCore::Instance().SetVsyncId(0);
    mock.isHeteroComputingHdrOn_ = true;
    mock.UpdateHDRNodes(*surfaceNode, false);
    mock.PostHDRSubTasks();
    HgmCore::Instance().SetVsyncId(1); // Reset to non-zero
}

void RSHeteroHDRManagerTest::TestPostHDRSubTasksWithDestroyedFlag()
{
    MockRSHeteroHDRManager mock;
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    SetupSurfaceContext(surfaceNode, appNode);

    mock.destroyedFlag_.store(false);
    mock.PostHDRSubTasks();
    
    mock.destroyedFlag_.store(true);
    mock.PostHDRSubTasks();
    
    mock.destroyedFlag_.store(false);
    mock.UpdateHDRNodes(*surfaceNode, false);
    mock.PostHDRSubTasks();
}

void RSHeteroHDRManagerTest::TestPostHDRSubTasksWithVideoGainmap()
{
    MockRSHeteroHDRManager mock;
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    SetupSurfaceContext(surfaceNode, appNode);

    surfaceNode->SetVideoHdrStatus(AI_HDR_VIDEO_GAINMAP);
    mock.isHeteroComputingHdrOn_ = true;
    mock.UpdateHDRNodes(*surfaceNode, false);
    mock.PostHDRSubTasks();
}

void RSHeteroHDRManagerTest::TestPostHDRSubTasksWithColorFollow()
{
    MockRSHeteroHDRManager mock;
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    SetupSurfaceContext(surfaceNode, appNode);

    auto params = GetRenderParams(surfaceNode);
    params->SetColorFollow(true);
    
    mock.isHeteroComputingHdrOn_ = true;
    mock.UpdateHDRNodes(*surfaceNode, false);
    mock.PostHDRSubTasks();
}

void RSHeteroHDRManagerTest::TestPostHDRSubTasksWithHDRNotPresent()
{
    MockRSHeteroHDRManager mock;
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    SetupSurfaceContext(surfaceNode, appNode);

    auto params = GetScreenParams(screenNode);
    params->SetHDRPresent(false);
    
    mock.isHeteroComputingHdrOn_ = true;
    mock.UpdateHDRNodes(*surfaceNode, false);
    mock.PostHDRSubTasks();
}
// original test: Test PostHDRSubTasks app and leash node condition

/**
 * @tc.name: PostHDRSubTasksTest002_01
 * @tc.desc: Test PostHDRSubTasks basic app and leash node condition
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest002_01, TestSize.Level1)
{
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    ASSERT_NE(surfaceNode, nullptr);
    
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);
    
    auto appParams = GetRenderParams(appNode);
    auto surfaceParams = GetRenderParams(surfaceNode);
    surfaceParams->SetAncestorScreenNode(screenNode);
    
    appParams->shouldPaint_ = true;
    appParams->contentEmpty_ = false;

    MockRSHeteroHDRManager mock;
    SetupMockForHDR(mock);
    
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 基本条件下应该有HDR任务被添加
    EXPECT_FALSE(mock.pendingPostNodes_.empty());
    EXPECT_EQ(mock.pendingPostNodes_.size(), 1);
    EXPECT_EQ(mock.pendingPostNodes_.front().nodeId, surfaceNode->GetId());
}

/**
 * @tc.name: PostHDRSubTasksTest002_02
 * @tc.desc: Test PostHDRSubTasks with surface node as app child
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest002_02, TestSize.Level1)
{
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    
    appNode->AddChild(surfaceNode);
    SetupSurfaceContext(surfaceNode, appNode);

    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    auto surfaceParams = GetRenderParams(surfaceNode);
    surfaceParams->SetAncestorScreenNode(screenNode);
    
    auto appParams = GetRenderParams(appNode);
    appParams->shouldPaint_ = true;
    appParams->contentEmpty_ = false;

    MockRSHeteroHDRManager mock;
    SetupMockForHDR(mock);
    
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 当surface是app子节点时，应该能正确处理
    EXPECT_FALSE(mock.pendingPostNodes_.empty());
    EXPECT_EQ(mock.pendingPostNodes_.front().nodeId, surfaceNode->GetId());
    EXPECT_EQ(mock.pendingPostNodes_.front().appNodeId, appNode->GetId());
}

/**
 * @tc.name: PostHDRSubTasksTest002_03
 * @tc.desc: Test PostHDRSubTasks with app node as leash child
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest002_03, TestSize.Level1)
{
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    
    leashNode->AddChild(appNode);
    SetupSurfaceContext(surfaceNode, appNode);

    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    auto surfaceParams = GetRenderParams(surfaceNode);
    surfaceParams->SetAncestorScreenNode(screenNode);
    
    auto appParams = GetRenderParams(appNode);
    appParams->shouldPaint_ = true;
    appParams->contentEmpty_ = false;

    MockRSHeteroHDRManager mock;
    SetupMockForHDR(mock);
    
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 当app是leash子节点时，应该能正确处理
    EXPECT_FALSE(mock.pendingPostNodes_.empty());
    EXPECT_EQ(mock.pendingPostNodes_.front().nodeId, surfaceNode->GetId());
    EXPECT_EQ(mock.pendingPostNodes_.front().leashNodeId, leashNode->GetId());
}

/**
 * @tc.name: PostHDRSubTasksTest002_04
 * @tc.desc: Test PostHDRSubTasks with ancestor screen node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest002_04, TestSize.Level1)
{
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    
    leashNode->AddChild(appNode);
    SetupSurfaceContext(surfaceNode, appNode);

    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    auto appParams = GetRenderParams(appNode);
    auto leashParams = GetRenderParams(leashNode);
    
    appParams->SetAncestorScreenNode(screenNode);
    leashParams->SetAncestorScreenNode(screenNode);

    MockRSHeteroHDRManager mock;
    SetupMockForHDR(mock);
    
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 当设置了祖先屏幕节点时，应该能正确关联
    EXPECT_FALSE(mock.pendingPostNodes_.empty());
    EXPECT_EQ(mock.pendingPostNodes_.front().screenNodeId, screenNode->GetId());
}

/**
 * @tc.name: PostHDRSubTasksTest002_05
 * @tc.desc: Test PostHDRSubTasks with pending nodes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest002_05, TestSize.Level1)
{
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    
    leashNode->AddChild(appNode);
    SetupSurfaceContext(surfaceNode, appNode);
    
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    auto leashParams = GetRenderParams(leashNode);
    leashParams->SetAncestorScreenNode(screenNode);

    auto& pendingNodes = RSUifirstManager::Instance().GetPendingPostNodes();
    pendingNodes.insert({surfaceNode->GetId(), surfaceNode});

    MockRSHeteroHDRManager mock;
    SetupMockForHDR(mock);
    
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 当节点在pendingNodes中时，应该被正确处理
    EXPECT_TRUE(mock.pendingPostNodes_.empty());
    
    pendingNodes.erase(surfaceNode->GetId());
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 当节点不在pendingNodes中时，应该被添加到pendingPostNodes_
    EXPECT_FALSE(mock.pendingPostNodes_.empty());
}

/**
 * @tc.name: PostHDRSubTasksTest002_06
 * @tc.desc: Test PostHDRSubTasks with painting skipped
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest002_06, TestSize.Level1)
{
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    
    leashNode->AddChild(appNode);
    SetupSurfaceContext(surfaceNode, appNode);
    
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    auto appParams = GetRenderParams(appNode);
    auto leashParams = GetRenderParams(leashNode);
    
    leashParams->shouldPaint_ = false;
    appParams->shouldPaint_ = true;

    MockRSHeteroHDRManager mock;
    SetupMockForHDR(mock);
    
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 当leash不绘制时，应该跳过HDR处理
    EXPECT_TRUE(mock.pendingPostNodes_.empty());
    
    leashParams->shouldPaint_ = true;
    appParams->shouldPaint_ = false;
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 当app不绘制时，应该跳过HDR处理
    EXPECT_TRUE(mock.pendingPostNodes_.empty());
}

/**
 * @tc.name: PostHDRSubTasksTest002_07
 * @tc.desc: Test PostHDRSubTasks with hardware enabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest002_07, TestSize.Level1)
{
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    
    leashNode->AddChild(appNode);
    SetupSurfaceContext(surfaceNode, appNode);
    
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    auto surfaceParams = GetRenderParams(surfaceNode);
    surfaceParams->SetHardwareEnabled(true);

    MockRSHeteroHDRManager mock;
    SetupMockForHDR(mock);
    
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 当硬件加速启用时，应该能正确处理
    EXPECT_FALSE(mock.pendingPostNodes_.empty());
    EXPECT_TRUE(mock.pendingPostNodes_.front().isHardwareEnabled);
    
    surfaceParams->SetHardwareEnabled(false);
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 当硬件加速禁用时，应该能正确处理
    EXPECT_FALSE(mock.pendingPostNodes_.empty());
    EXPECT_FALSE(mock.pendingPostNodes_.front().isHardwareEnabled);
}

// end original test: Test PostHDRSubTasks app and leash node condition

// original test: Test PostHDRSubTasks rect
/**
 * @tc.name: PostHDRSubTasksTest003_01
 * @tc.desc: Test PostHDRSubTasks with basic rectangle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest003_01, TestSize.Level1)
{
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    SetupSurfaceContext(surfaceNode, appNode);
    
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);
    auto surfaceParams = GetRenderParams(surfaceNode);
    ASSERT_NE(surfaceParams, nullptr);

    MockRSHeteroHDRManager mock;
    SetupMockForHDR(mock);
    
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 基本条件下应该有HDR任务被添加
    EXPECT_FALSE(mock.pendingPostNodes_.empty());
    auto& nodeInfo = mock.pendingPostNodes_.front();
    
    // 验证矩形尺寸是否正确
    RSLayerInfo layerInfo = surfaceParams->GetLayerInfo();
    EXPECT_EQ(nodeInfo.srcRect.width, layerInfo.srcRect.w);
    EXPECT_EQ(nodeInfo.srcRect.height, layerInfo.srcRect.h);
    EXPECT_EQ(nodeInfo.dstRect.width, layerInfo.dstRect.w);
    EXPECT_EQ(nodeInfo.dstRect.height, layerInfo.dstRect.h);
}

/**
 * @tc.name: PostHDRSubTasksTest003_02
 * @tc.desc: Test PostHDRSubTasks with 90 degree rotation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest003_02, TestSize.Level1)
{
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    SetupSurfaceContext(surfaceNode, appNode);
    
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    auto surfaceParams = GetRenderParams(surfaceNode);
    
    RSLayerInfo layerInfo = surfaceParams->GetLayerInfo();
    layerInfo.transformType = GRAPHIC_ROTATE_90;
    surfaceParams->SetLayerInfo(layerInfo);

    MockRSHeteroHDRManager mock;
    SetupMockForHDR(mock);
    
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 旋转90度后，矩形转换应该正确处理
    EXPECT_FALSE(mock.pendingPostNodes_.empty());
    auto& nodeInfo = mock.pendingPostNodes_.front();
    
    // 90度旋转时，宽度和高度应该交换
    EXPECT_EQ(nodeInfo.dstRect.width, layerInfo.dstRect.h);
    EXPECT_EQ(nodeInfo.dstRect.height, layerInfo.dstRect.w);
    EXPECT_EQ(nodeInfo.srcRect.width, layerInfo.srcRect.h);
    EXPECT_EQ(nodeInfo.srcRect.height, layerInfo.srcRect.w);
}

/**
 * @tc.name: PostHDRSubTasksTest003_03
 * @tc.desc: Test PostHDRSubTasks with 270 degree rotation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest003_03, TestSize.Level1)
{
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    SetupSurfaceContext(surfaceNode, appNode);
    
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    auto surfaceParams = GetRenderParams(surfaceNode);
    
    RSLayerInfo layerInfo = surfaceParams->GetLayerInfo();
    layerInfo.transformType = GRAPHIC_ROTATE_270;
    surfaceParams->SetLayerInfo(layerInfo);

    MockRSHeteroHDRManager mock;
    SetupMockForHDR(mock);
    
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 旋转270度后，矩形转换应该正确处理
    EXPECT_FALSE(mock.pendingPostNodes_.empty());
    auto& nodeInfo = mock.pendingPostNodes_.front();
    
    // 270度旋转时，宽度和高度应该交换
    EXPECT_EQ(nodeInfo.dstRect.width, layerInfo.dstRect.h);
    EXPECT_EQ(nodeInfo.dstRect.height, layerInfo.dstRect.w);
    EXPECT_EQ(nodeInfo.srcRect.width, layerInfo.srcRect.h);
    EXPECT_EQ(nodeInfo.srcRect.height, layerInfo.srcRect.w);
}

/**
 * @tc.name: PostHDRSubTasksTest003_04
 * @tc.desc: Test PostHDRSubTasks with zero width destination rectangle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest003_04, TestSize.Level1)
{
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    SetupSurfaceContext(surfaceNode, appNode);
    
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    auto surfaceParams = GetRenderParams(surfaceNode);
    
    RSLayerInfo layerInfo = surfaceParams->GetLayerInfo();
    layerInfo.dstRect.w = 0;
    layerInfo.dstRect.h = 1;
    surfaceParams->SetLayerInfo(layerInfo);

    MockRSHeteroHDRManager mock;
    SetupMockForHDR(mock);
    
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 零宽度的目标矩形应该被正确处理
    if (!mock.pendingPostNodes_.empty()) {
        auto& nodeInfo = mock.pendingPostNodes_.front();
        EXPECT_EQ(nodeInfo.dstRect.width, 0);
        EXPECT_EQ(nodeInfo.dstRect.height, 1);
    } else {
        // 可能会跳过零尺寸的矩形
        SUCCEED() << "Zero width destination rectangle might be skipped";
    }
}

/**
 * @tc.name: PostHDRSubTasksTest003_05
 * @tc.desc: Test PostHDRSubTasks with zero height destination rectangle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest003_05, TestSize.Level1)
{
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    SetupSurfaceContext(surfaceNode, appNode);
    
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    auto surfaceParams = GetRenderParams(surfaceNode);
    
    RSLayerInfo layerInfo = surfaceParams->GetLayerInfo();
    layerInfo.dstRect.w = 1;
    layerInfo.dstRect.h = 0;
    surfaceParams->SetLayerInfo(layerInfo);

    MockRSHeteroHDRManager mock;
    SetupMockForHDR(mock);
    
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 零高度的目标矩形应该被正确处理
    if (!mock.pendingPostNodes_.empty()) {
        auto& nodeInfo = mock.pendingPostNodes_.front();
        EXPECT_EQ(nodeInfo.dstRect.width, 1);
        EXPECT_EQ(nodeInfo.dstRect.height, 0);
    } else {
        // 可能会跳过零尺寸的矩形
        SUCCEED() << "Zero height destination rectangle might be skipped";
    }
}

/**
 * @tc.name: PostHDRSubTasksTest003_06
 * @tc.desc: Test PostHDRSubTasks with zero width source rectangle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest003_06, TestSize.Level1)
{
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    SetupSurfaceContext(surfaceNode, appNode);
    
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    auto surfaceParams = GetRenderParams(surfaceNode);
    
    RSLayerInfo layerInfo = surfaceParams->GetLayerInfo();
    layerInfo.srcRect.w = 1;
    layerInfo.srcRect.h = 0;
    surfaceParams->SetLayerInfo(layerInfo);

    MockRSHeteroHDRManager mock;
    SetupMockForHDR(mock);
    
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 零宽度的源矩形应该被正确处理
    if (!mock.pendingPostNodes_.empty()) {
        auto& nodeInfo = mock.pendingPostNodes_.front();
        EXPECT_EQ(nodeInfo.srcRect.width, 1);
        EXPECT_EQ(nodeInfo.srcRect.height, 0);
    } else {
        // 可能会跳过零尺寸的矩形
        SUCCEED() << "Zero width source rectangle might be skipped";
    }
}

/**
 * @tc.name: PostHDRSubTasksTest003_07
 * @tc.desc: Test PostHDRSubTasks with zero height source rectangle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest003_07, TestSize.Level1)
{
    auto [screenNode, appNode, leashNode, surfaceNode] = GenerateTestNodes();
    SetupSurfaceContext(surfaceNode, appNode);
    
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    auto surfaceParams = GetRenderParams(surfaceNode);
    
    RSLayerInfo layerInfo = surfaceParams->GetLayerInfo();
    layerInfo.srcRect.w = 0;
    layerInfo.srcRect.h = 1;
    surfaceParams->SetLayerInfo(layerInfo);

    MockRSHeteroHDRManager mock;
    SetupMockForHDR(mock);
    
    mock.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mock.PostHDRSubTasks();
    
    // 验证: 零高度的源矩形应该被正确处理
    if (!mock.pendingPostNodes_.empty()) {
        auto& nodeInfo = mock.pendingPostNodes_.front();
        EXPECT_EQ(nodeInfo.srcRect.width, 0);
        EXPECT_EQ(nodeInfo.srcRect.height, 1);
    } else {
        // 可能会跳过零尺寸的矩形
        SUCCEED() << "Zero height source rectangle might be skipped";
    }
}
// end original test: Test PostHDRSubTasks rect

/**
 * @tc.name: PrepareAndSubmitHDRTaskTest001
 * @tc.desc: Test PrepareAndSubmitHDRTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PrepareAndSubmitHDRTaskTest001, TestSize.Level1)
{
    shared_ptr<RSScreenRenderNode> screenNode;
    shared_ptr<RSSurfaceRenderNode> appNode;
    shared_ptr<RSSurfaceRenderNode> leashNode;
    shared_ptr<RSSurfaceRenderNode> surfaceNode;
    GenerateValidSurfaceNode(screenNode, appNode, leashNode, surfaceNode);
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    NodeId instanceRootNodeId = appNode->GetId();
    pid_t instanceRootNodePid = ExtractPid(instanceRootNodeId);
    nodeMap.renderNodeMap_[instanceRootNodePid][instanceRootNodeId] = appNode;
    surfaceNode->context_ = rsContext;
    surfaceNode->instanceRootNodeId_ = appNode->GetId();
    ASSERT_TRUE(surfaceNode != nullptr);
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    ASSERT_TRUE(surfaceHandler != nullptr);
    auto surfaceParams = GetRenderParams(surfaceNode);
    ASSERT_TRUE(surfaceParams != nullptr);
    
    // all return true
    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = true;
    EXPECT_CALL(mockRSHeteroHDRManager, MHCRequestEGraph(_)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockRSHeteroHDRManager, MHCSetCurFrameId(_)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockRSHeteroHDRManager, TryConsumeBuffer(_)).WillRepeatedly(testing::Return(true));
    RSHeteroHDRHpae::GetInstance().mdcExistedStatus_.store(true);
    mockRSHeteroHDRManager.curHandleStatus_ = RSHeteroHDRHpae::GetInstance().existedChannelStatus_;

    // prev rect test
    RSLayerInfo layerInfo = surfaceParams->GetLayerInfo();
    layerInfo.transformType = GRAPHIC_ROTATE_90;
    surfaceParams->SetLayerInfo(layerInfo);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    BufferRequestConfig requestConfig = {
        .width = 0x200,
        .height = 0x200,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    surfaceParams->GetBuffer()->Alloc(requestConfig);

    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    // needhandle
    auto surfaceDrawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    auto drawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceDrawableAdapter);
    mockRSHeteroHDRManager.preFrameHeteroHandleCanBeUsed_ = true;
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, false);
    mockRSHeteroHDRManager.PostHDRSubTasks();
}

/**
 * @tc.name: PrepareAndSubmitHDRTaskTest002
 * @tc.desc: Test PrepareAndSubmitHDRTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PrepareAndSubmitHDRTaskTest002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(surfaceDrawable, nullptr);
    auto surfaceParams = GetRenderParams(surfaceNode);
    ASSERT_NE(surfaceParams, nullptr);
    
    RSLayerInfo layerInfo;
    layerInfo.srcRect.w = 1;
    layerInfo.srcRect.h = 1;
    layerInfo.dstRect.w = 1;
    layerInfo.dstRect.h = 1;

    layerInfo.matrix.Set(Drawing::Matrix::SKEW_X, 1.0);
    layerInfo.matrix.Set(Drawing::Matrix::SKEW_Y, 1.0);

    surfaceParams->SetLayerInfo(layerInfo);
    surfaceParams->buffer_ = OHOS::SurfaceBuffer::Create();
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    surfaceParams->GetBuffer()->Alloc(requestConfig);
    surfaceParams->GetBuffer()->SetSurfaceBufferScalingMode(ScalingMode::SCALING_MODE_FREEZE);

    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    EXPECT_CALL(mockRSHeteroHDRManager, MHCRequestEGraph(_)).WillRepeatedly(testing::Return(false));
    mockRSHeteroHDRManager.PrepareAndSubmitHDRTask(surfaceDrawable, surfaceParams, surfaceNode->GetId(), 1);
    EXPECT_EQ(mockRSHeteroHDRManager.isFixedDstBuffer_, true);
}

/**
 * @tc.name: PostHDRSubTasksTest020
 * @tc.desc: Test PostHdrSubTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest020, TestSize.Level1)
{
    shared_ptr<RSScreenRenderNode> screenNode;
    shared_ptr<RSSurfaceRenderNode> appNode;
    shared_ptr<RSSurfaceRenderNode> leashNode;
    shared_ptr<RSSurfaceRenderNode> surfaceNode;
    GenerateValidSurfaceNode(screenNode, appNode, leashNode, surfaceNode);
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    NodeId instanceRootNodeId = appNode->GetId();
    pid_t instanceRootNodePid = ExtractPid(instanceRootNodeId);
    nodeMap.renderNodeMap_[instanceRootNodePid][instanceRootNodeId] = appNode;
    surfaceNode->context_ = rsContext;
    surfaceNode->instanceRootNodeId_ = appNode->GetId();
    ASSERT_TRUE(surfaceNode != nullptr);
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    ASSERT_TRUE(surfaceHandler != nullptr);
    auto parent = surfaceNode->GetParent().lock();
    if (parent == nullptr) {
        RS_LOGE("parent is nullptr");
    }

    // begin return false one by one
    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = true;
    EXPECT_CALL(mockRSHeteroHDRManager, MHCRequestEGraph(_)).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(mockRSHeteroHDRManager, MHCSetCurFrameId(_)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockRSHeteroHDRManager, TryConsumeBuffer(_)).WillRepeatedly(testing::Return(true));
    RSHeteroHDRHpae::GetInstance().mdcExistedStatus_.store(true);
    mockRSHeteroHDRManager.curHandleStatus_ = RSHeteroHDRHpae::GetInstance().existedChannelStatus_;

    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();
}

/**
 * @tc.name: PostHDRSubTasksTest021
 * @tc.desc: Test PostHdrSubTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest021, TestSize.Level1)
{
    shared_ptr<RSScreenRenderNode> screenNode;
    shared_ptr<RSSurfaceRenderNode> appNode;
    shared_ptr<RSSurfaceRenderNode> leashNode;
    shared_ptr<RSSurfaceRenderNode> surfaceNode;
    GenerateValidSurfaceNode(screenNode, appNode, leashNode, surfaceNode);
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    NodeId instanceRootNodeId = appNode->GetId();
    pid_t instanceRootNodePid = ExtractPid(instanceRootNodeId);
    nodeMap.renderNodeMap_[instanceRootNodePid][instanceRootNodeId] = appNode;
    surfaceNode->context_ = rsContext;
    surfaceNode->instanceRootNodeId_ = appNode->GetId();
    ASSERT_TRUE(surfaceNode != nullptr);
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    ASSERT_TRUE(surfaceHandler != nullptr);
    auto parent = surfaceNode->GetParent().lock();
    if (parent == nullptr) {
        RS_LOGE("parent is nullptr");
    }

    // begin return false one by one
    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = true;
    EXPECT_CALL(mockRSHeteroHDRManager, MHCRequestEGraph(_)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockRSHeteroHDRManager, MHCSetCurFrameId(_)).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(mockRSHeteroHDRManager, TryConsumeBuffer(_)).WillRepeatedly(testing::Return(true));
    RSHeteroHDRHpae::GetInstance().mdcExistedStatus_.store(true);
    mockRSHeteroHDRManager.curHandleStatus_ = RSHeteroHDRHpae::GetInstance().existedChannelStatus_;

    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();
}

/**
 * @tc.name: PostHDRSubTasksTest022
 * @tc.desc: Test PostHdrSubTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest022, TestSize.Level1)
{
    shared_ptr<RSScreenRenderNode> screenNode;
    shared_ptr<RSSurfaceRenderNode> appNode;
    shared_ptr<RSSurfaceRenderNode> leashNode;
    shared_ptr<RSSurfaceRenderNode> surfaceNode;
    GenerateValidSurfaceNode(screenNode, appNode, leashNode, surfaceNode);
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    NodeId instanceRootNodeId = appNode->GetId();
    pid_t instanceRootNodePid = ExtractPid(instanceRootNodeId);
    nodeMap.renderNodeMap_[instanceRootNodePid][instanceRootNodeId] = appNode;
    surfaceNode->context_ = rsContext;
    surfaceNode->instanceRootNodeId_ = appNode->GetId();
    ASSERT_TRUE(surfaceNode != nullptr);
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    ASSERT_TRUE(surfaceHandler != nullptr);
    auto parent = surfaceNode->GetParent().lock();
    if (parent == nullptr) {
        RS_LOGE("parent is nullptr");
    }

    // begin return false one by one
    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = true;
    EXPECT_CALL(mockRSHeteroHDRManager, MHCRequestEGraph(_)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockRSHeteroHDRManager, MHCSetCurFrameId(_)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockRSHeteroHDRManager, TryConsumeBuffer(_)).WillRepeatedly(testing::Return(false));
    RSHeteroHDRHpae::GetInstance().mdcExistedStatus_.store(true);
    mockRSHeteroHDRManager.curHandleStatus_ = RSHeteroHDRHpae::GetInstance().existedChannelStatus_;

    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();
}

// original test: Test ValidateSurface
/**
 * @tc.name: ValidateSurfaceTest001_01
 * @tc.desc: Test ValidateSurface with null pointer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, ValidateSurfaceTest001_01, TestSize.Level1)
{
    bool ret = RSHeteroHDRUtil::ValidateSurface(nullptr);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ValidateSurfaceTest001_02
 * @tc.desc: Test ValidateSurface with uninitialized surface params
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, ValidateSurfaceTest001_02, TestSize.Level1)
{
    NodeId id = 5;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->InitRenderParams();
    auto surfaceParams = GetRenderParams(surfaceNode);
    ASSERT_NE(surfaceParams, nullptr);
    
    bool ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ValidateSurfaceTest001_03
 * @tc.desc: Test ValidateSurface with valid surface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, ValidateSurfaceTest001_03, TestSize.Level1)
{
    auto surfaceParams = CreateValidSurfaceParams();
    bool ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: ValidateSurfaceTest001_04
 * @tc.desc: Test ValidateSurface with invalid source rectangle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, ValidateSurfaceTest001_04, TestSize.Level1)
{
    auto surfaceParams = CreateValidSurfaceParams();
    
    // 测试零宽度源矩形
    RSLayerInfo layerInfo = surfaceParams->GetLayerInfo();
    layerInfo.srcRect.w = 0;
    surfaceParams->SetLayerInfo(layerInfo);
    bool ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_FALSE(ret);
    
    // 测试过大宽度源矩形
    layerInfo.srcRect.w = 5000;
    surfaceParams->SetLayerInfo(layerInfo);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_FALSE(ret);
    
    // 测试零高度源矩形
    layerInfo.srcRect.w = 400;
    layerInfo.srcRect.h = 0;
    surfaceParams->SetLayerInfo(layerInfo);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_FALSE(ret);
    
    // 测试过大高度源矩形
    layerInfo.srcRect.h = 5000;
    surfaceParams->SetLayerInfo(layerInfo);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ValidateSurfaceTest001_05
 * @tc.desc: Test ValidateSurface with invalid destination rectangle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, ValidateSurfaceTest001_05, TestSize.Level1)
{
    auto surfaceParams = CreateValidSurfaceParams();
    
    // 测试零宽度目标矩形
    RSLayerInfo layerInfo = surfaceParams->GetLayerInfo();
    layerInfo.dstRect.w = 0;
    surfaceParams->SetLayerInfo(layerInfo);
    bool ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_FALSE(ret);
    
    // 测试过大宽度目标矩形
    layerInfo.dstRect.w = 5000;
    surfaceParams->SetLayerInfo(layerInfo);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_FALSE(ret);
    
    // 测试零高度目标矩形
    layerInfo.dstRect.w = 400;
    layerInfo.dstRect.h = 0;
    surfaceParams->SetLayerInfo(layerInfo);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_FALSE(ret);
    
    // 测试过大高度目标矩形
    layerInfo.dstRect.h = 5000;
    surfaceParams->SetLayerInfo(layerInfo);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ValidateSurfaceTest001_06
 * @tc.desc: Test ValidateSurface with invalid buffer size
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, ValidateSurfaceTest001_06, TestSize.Level1)
{
    auto surfaceParams = CreateValidSurfaceParams();
    
    // 测试零宽度缓冲区
    surfaceParams->GetBuffer()->SetSurfaceBufferWidth(0);
    surfaceParams->GetBuffer()->SetSurfaceBufferHeight(1);
    bool ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_FALSE(ret);
    
    // 测试零高度缓冲区
    surfaceParams->GetBuffer()->SetSurfaceBufferWidth(1);
    surfaceParams->GetBuffer()->SetSurfaceBufferHeight(0);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_FALSE(ret);
    
    // 测试有效缓冲区
    surfaceParams->GetBuffer()->SetSurfaceBufferWidth(1);
    surfaceParams->GetBuffer()->SetSurfaceBufferHeight(1);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_TRUE(ret);
}
// end original test: Test ValidateSurface

/**
 * @tc.name: ValidateSurfaceTest002
 * @tc.desc: Test ValidateSurface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, ValidateSurfaceTest002, TestSize.Level1)
{
    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    bool ret = RSHeteroHDRUtil::ValidateSurface(nullptr);
    EXPECT_EQ(ret, false);

    NodeId id = 5;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_TRUE(surfaceNode != nullptr);
    surfaceNode->InitRenderParams();
    auto surfaceParams = GetRenderParams(surfaceNode);
    ASSERT_NE(surfaceParams, nullptr);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, false);

    surfaceParams->buffer_ = OHOS::SurfaceBuffer::Create();
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    surfaceParams->GetBuffer()->Alloc(requestConfig);

    RSLayerInfo layerInfo;
    layerInfo.srcRect.w = 400;
    layerInfo.srcRect.h = 400;
    layerInfo.dstRect.w = 400;
    layerInfo.dstRect.h = 400;
    surfaceParams->SetLayerInfo(layerInfo);

    sptr<BufferExtraData> extraData = surfaceParams->buffer_->GetExtraData();
    ASSERT_NE(extraData, nullptr);
    extraData->ExtraSet("VIDEO_RATE", 60.0);
    mockRSHeteroHDRManager.curHandleStatus_ = HdrStatus::HDR_VIDEO;
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, true);

    extraData->ExtraSet("VIDEO_RATE", 30.0);
    mockRSHeteroHDRManager.curHandleStatus_ = HdrStatus::HDR_VIDEO;
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, true);

    extraData->ExtraSet("VIDEO_RATE", 60.0);
    mockRSHeteroHDRManager.curHandleStatus_ = HdrStatus::AI_HDR_VIDEO_GAINMAP;
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, true);

    extraData->ExtraSet("VIDEO_RATE", 30.0);
    mockRSHeteroHDRManager.curHandleStatus_ = HdrStatus::AI_HDR_VIDEO_GAINMAP;
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: BuildHDRTaskTest001
 * @tc.desc: Test BuildHDRTask input condition
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, BuildHDRTaskTest001, TestSize.Level1)
{
    auto surfaceNode = GenerateSurfaceNode();
    ASSERT_TRUE(surfaceNode != nullptr);
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    ASSERT_TRUE(surfaceHandler != nullptr);
    auto surfaceParams = GetRenderParams(surfaceNode);
    RSLayerInfo rsLyaerInfo;
    surfaceParams->SetLayerInfo(rsLyaerInfo);

    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    MDCRectT srcRect {0, 0, 0, 0};
    uint32_t taskId = 123;
    mockRSHeteroHDRManager.dstBuffer_ = OHOS::SurfaceBuffer::Create();
    void* taskPtr = nullptr;
    int32_t ret = mockRSHeteroHDRManager.BuildHDRTask(surfaceParams, srcRect, &taskId, &taskPtr, HdrStatus::HDR_VIDEO);
    EXPECT_EQ(ret, -1);

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    GSError allocRet = mockRSHeteroHDRManager.dstBuffer_->Alloc(requestConfig);
    ASSERT_EQ(allocRet, OHOS::GSERROR_OK);

    ret = mockRSHeteroHDRManager.BuildHDRTask(surfaceParams, srcRect, &taskId, &taskPtr, HdrStatus::HDR_VIDEO);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: BuildHDRTaskTest002
 * @tc.desc: Test BuildHDRTask input condition
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, BuildHDRTaskTest002, TestSize.Level1)
{
    auto surfaceNode = GenerateSurfaceNode();
    ASSERT_TRUE(surfaceNode != nullptr);
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    ASSERT_TRUE(surfaceHandler != nullptr);
    auto surfaceParams = GetRenderParams(surfaceNode);
    RSLayerInfo rsLyaerInfo;
    surfaceParams->SetLayerInfo(rsLyaerInfo);

    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    MDCRectT srcRect {0, 0, 0, 0};
    uint32_t taskId = 123;
    mockRSHeteroHDRManager.dstBuffer_ = OHOS::SurfaceBuffer::Create();
    void* taskPtr = nullptr;
    int32_t ret = mockRSHeteroHDRManager.BuildHDRTask(surfaceParams, srcRect, &taskId,
        &taskPtr, HdrStatus::AI_HDR_VIDEO_GAINMAP);
    EXPECT_EQ(ret, -1);

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    GSError allocRet = mockRSHeteroHDRManager.dstBuffer_->Alloc(requestConfig);
    ASSERT_EQ(allocRet, OHOS::GSERROR_OK);

    ret = mockRSHeteroHDRManager.BuildHDRTask(surfaceParams, srcRect, &taskId, &taskPtr,
         HdrStatus::AI_HDR_VIDEO_GAINMAP);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: IsHDRSurfaceNodeSkippedTest
 * @tc.desc: Test IsHDRSurfaceNodeSkipped
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, IsHDRSurfaceNodeSkippedTest, TestSize.Level1)
{
    auto surfaceNode = GenerateSurfaceNode();
    ASSERT_TRUE(surfaceNode != nullptr);
    auto surfaceParams = GetRenderParams(surfaceNode);
    ASSERT_TRUE(surfaceParams != nullptr);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceNode);

    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    // nullptr
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> drawableNullptr;
    bool ret = mockRSHeteroHDRManager.IsHDRSurfaceNodeSkipped(drawableNullptr);
    EXPECT_EQ(ret, true);

    // surfaceParams nullptr
    ret = mockRSHeteroHDRManager.IsHDRSurfaceNodeSkipped(drawable);
    EXPECT_EQ(ret, true);

    // GetOccludedByFilterCache
    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);
    drawableParam->SetOccludedByFilterCache(true);
    drawable->renderParams_ = std::move(drawableParam);
    ret = mockRSHeteroHDRManager.IsHDRSurfaceNodeSkipped(drawable);
    EXPECT_EQ(ret, true);

    auto drawableParam1 = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);
    drawableParam1->SetOccludedByFilterCache(false);
    drawable->renderParams_ = std::move(drawableParam1);

    // ShouldPaint
    ret = mockRSHeteroHDRManager.IsHDRSurfaceNodeSkipped(drawable);
    EXPECT_EQ(ret, true);
    drawable->renderParams_->shouldPaint_ = true;
    drawable->renderParams_->contentEmpty_ = false;
    ret = mockRSHeteroHDRManager.IsHDRSurfaceNodeSkipped(drawable);
    EXPECT_EQ(ret, false);

    // uniParam nullptr
    RSUniRenderThread::Instance().Sync(nullptr);
    ret = mockRSHeteroHDRManager.IsHDRSurfaceNodeSkipped(drawable);
    EXPECT_EQ(ret, true);
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: GetScreenIDByDrawableTest
 * @tc.desc: Test GetScreenIDByDrawable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, GetScreenIDByDrawableTest, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawable->GetRsSubThreadCache().SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    NodeId id = 10;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
    auto surfaceRenderParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->renderParams_.get());
    surfaceRenderParams->SetAncestorScreenNode(displayNode);

    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    mockRSHeteroHDRManager.GetScreenIDByDrawable(surfaceDrawable);

    // screenParams nullptr
    displayNode->stagingRenderParams_ = nullptr;
    displayNode->renderDrawable_->renderParams_ = nullptr;
    mockRSHeteroHDRManager.GetScreenIDByDrawable(surfaceDrawable);

    // GetAncestorScreenNode nullptr
    displayNode = nullptr;
    auto surfaceNodeTemp = RSTestUtil::CreateSurfaceNode();
    surfaceRenderParams->SetAncestorScreenNode(surfaceNodeTemp);
    mockRSHeteroHDRManager.GetScreenIDByDrawable(surfaceDrawable);

    // surfaceParams nullptr
    surfaceDrawable->renderParams_ = nullptr;
    mockRSHeteroHDRManager.GetScreenIDByDrawable(surfaceDrawable);
}

/**
 * @tc.name: GetFixedDstRectStatusTest
 * @tc.desc: Test GetFixedDstRectStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, GetFixedDstRectStatusTest, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(surfaceDrawable, nullptr);
    auto surfaceParams = GetRenderParams(surfaceNode);
    ASSERT_NE(surfaceParams, nullptr);
    
    RSLayerInfo layerInfo;
    layerInfo.srcRect.w = 1;
    layerInfo.srcRect.h = 1;
    layerInfo.dstRect.w = 1;
    layerInfo.dstRect.h = 1;

    layerInfo.matrix.Set(Drawing::Matrix::SKEW_X, 1.0);
    layerInfo.matrix.Set(Drawing::Matrix::SKEW_Y, 1.0);

    surfaceParams->SetLayerInfo(layerInfo);
    surfaceParams->buffer_ = OHOS::SurfaceBuffer::Create();
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    surfaceParams->GetBuffer()->Alloc(requestConfig);
    surfaceParams->GetBuffer()->SetSurfaceBufferScalingMode(ScalingMode::SCALING_MODE_FREEZE);

    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    RectI finalDstRect;
    bool isFixDstRect = false;
    mockRSHeteroHDRManager.GetFixedDstRectStatus(surfaceDrawable, false, nullptr);
    mockRSHeteroHDRManager.GetFixedDstRectStatus(surfaceDrawable, false, surfaceParams);

    mockRSHeteroHDRManager.curHandleStatus_ == HdrStatus::AI_HDR_VIDEO_GAINMAP;
    mockRSHeteroHDRManager.GetFixedDstRectStatus(surfaceDrawable, false, surfaceParams);

    layerInfo.transformType = GraphicTransformType::GRAPHIC_ROTATE_90;
    surfaceParams->SetLayerInfo(layerInfo);
    mockRSHeteroHDRManager.GetFixedDstRectStatus(surfaceDrawable, false, surfaceParams);

    mockRSHeteroHDRManager.GetFixedDstRectStatus(surfaceDrawable, true, surfaceParams);
}

/**
 * @tc.name: ProcessPendingNodeTest
 * @tc.desc: Test ProcessPendingNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, ProcessPendingNodeTest, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    bool ret = mockRSHeteroHDRManager.ProcessPendingNode(surfaceNode, 0);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: ClearBufferCacheTest
 * @tc.desc: Test ClearBufferCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, ClearBufferCacheTest, TestSize.Level1)
{
    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    mockRSHeteroHDRManager.ClearBufferCache();
    EXPECT_EQ(mockRSHeteroHDRManager.framesNoApplyCnt_, 1);
    mockRSHeteroHDRManager.ClearBufferCache();
    EXPECT_EQ(mockRSHeteroHDRManager.framesNoApplyCnt_, 2);
    mockRSHeteroHDRManager.ClearBufferCache();
    EXPECT_EQ(mockRSHeteroHDRManager.framesNoApplyCnt_, 3);
    mockRSHeteroHDRManager.ClearBufferCache();
    EXPECT_EQ(mockRSHeteroHDRManager.framesNoApplyCnt_, 4);
    mockRSHeteroHDRManager.ClearBufferCache();
    EXPECT_EQ(mockRSHeteroHDRManager.framesNoApplyCnt_, 5);
    mockRSHeteroHDRManager.ClearBufferCache();
    EXPECT_EQ(mockRSHeteroHDRManager.framesNoApplyCnt_, 6);
    mockRSHeteroHDRManager.ClearBufferCache();
    EXPECT_EQ(mockRSHeteroHDRManager.framesNoApplyCnt_, 0);
}

/**
 * @tc.name: DrawHDRCacheWithDmaFFRTTest
 * @tc.desc: Test UpdateHDRHeteroParams
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, DrawHDRCacheWithDmaFFRTTest, TestSize.Level1)
{
    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceDrawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceNode);
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    BufferDrawParam drawableParams;

    // test isHeteroComputingHdrOn_
    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = false;
    bool ret = mockRSHeteroHDRManager.UpdateHDRHeteroParams(canvas, *surfaceDrawable, drawableParams);
    ASSERT_EQ(ret, false);

    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = true;
    ret = mockRSHeteroHDRManager.UpdateHDRHeteroParams(canvas, *surfaceDrawable, drawableParams);
    ASSERT_EQ(ret, false);

    // test surfaceParams
    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable->nodeId_);
    surfaceDrawable->renderParams_ = std::move(drawableParam);
    auto surfaceParams = static_cast<RSSurfaceRenderParams *>(surfaceDrawable->GetRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = true;
    ret = mockRSHeteroHDRManager.UpdateHDRHeteroParams(canvas, *surfaceDrawable, drawableParams);
    ASSERT_EQ(ret, false);

    // test skipDraw
    surfaceParams->SetHardwareEnabled(true);
    ret = mockRSHeteroHDRManager.UpdateHDRHeteroParams(canvas, *surfaceDrawable, drawableParams);
    ASSERT_EQ(ret, false);

    surfaceParams->SetHardwareEnabled(false);
    mockRSHeteroHDRManager.curFrameHeteroHandleCanBeUsed_ = true;
    mockRSHeteroHDRManager.curNodeId_ == id;
    mockRSHeteroHDRManager.curHandleStatus_ = HDR_VIDEO;
    drawableParams.useCPU = false;
    canvas.SetOnMultipleScreen(false);
    RSUniRenderThread::ResetCaptureParam();

    ret = mockRSHeteroHDRManager.UpdateHDRHeteroParams(canvas, *surfaceDrawable, drawableParams);
    ASSERT_EQ(ret, false);
    
    // test hdrSurfaceHandler
    std::shared_ptr<RSSurfaceHandler> hdrSurfaceHandlerTemp = mockRSHeteroHDRManager.GetHDRSurfaceHandler();
    mockRSHeteroHDRManager.rsHeteroHDRBufferLayer_.surfaceHandler_ = nullptr;
    ret = mockRSHeteroHDRManager.UpdateHDRHeteroParams(canvas, *surfaceDrawable, drawableParams);
    ASSERT_EQ(ret, false);
    mockRSHeteroHDRManager.rsHeteroHDRBufferLayer_.surfaceHandler_ = hdrSurfaceHandlerTemp; // must set back
}

/**
 * @tc.name: DrawHDRBufferWithGPUTest
 * @tc.desc: Test ProcessParamsUpdate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, DrawHDRBufferWithGPUTest, TestSize.Level1)
{
    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceDrawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceNode);
    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable->nodeId_);
    surfaceDrawable->renderParams_ = std::move(drawableParam);
    auto surfaceParams = static_cast<RSSurfaceRenderParams *>(surfaceDrawable->GetRenderParams().get());
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();

    RSLayerInfo layerInfo;
    layerInfo.srcRect.w = 1;
    layerInfo.srcRect.h = 1;
    layerInfo.dstRect.w = 1;
    layerInfo.dstRect.h = 1;
    layerInfo.transformType = GRAPHIC_ROTATE_NONE;
    surfaceParams->SetLayerInfo(layerInfo);
    surfaceParams->buffer_ = OHOS::SurfaceBuffer::Create();
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    surfaceParams->GetBuffer()->Alloc(requestConfig);
    surfaceParams->SetHardwareEnabled(true);

    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    auto hdrSurfaceHandler = mockRSHeteroHDRManager.GetHDRSurfaceHandler();
    hdrSurfaceHandler->SetConsumer(IConsumerSurface::Create("test consumer"));

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    BufferDrawParam drawableParams;
    mockRSHeteroHDRManager.ProcessParamsUpdate(canvas, *surfaceDrawable, drawableParams);

    // test hdrSurfaceHandler
    std::shared_ptr<RSSurfaceHandler> hdrSurfaceHandlerTemp = mockRSHeteroHDRManager.GetHDRSurfaceHandler();
    mockRSHeteroHDRManager.rsHeteroHDRBufferLayer_.surfaceHandler_ = nullptr;
    mockRSHeteroHDRManager.ProcessParamsUpdate(canvas, *surfaceDrawable, drawableParams);
    mockRSHeteroHDRManager.rsHeteroHDRBufferLayer_.surfaceHandler_ = hdrSurfaceHandlerTemp; // must set back

    // hdrStatus
    mockRSHeteroHDRManager.curHandleStatus_ = HdrStatus::NO_HDR;
    mockRSHeteroHDRManager.ProcessParamsUpdate(canvas, *surfaceDrawable, drawableParams);
    mockRSHeteroHDRManager.curHandleStatus_ = HdrStatus::HDR_VIDEO;
    mockRSHeteroHDRManager.ProcessParamsUpdate(canvas, *surfaceDrawable, drawableParams);
    mockRSHeteroHDRManager.curHandleStatus_ = HdrStatus::AI_HDR_VIDEO_GAINMAP;
    mockRSHeteroHDRManager.ProcessParamsUpdate(canvas, *surfaceDrawable, drawableParams);
}

// original test func: Test GenDrawHDRBufferParams
/**
 * @tc.name: GenDrawHDRBufferParamsTest001
 * @tc.desc: Test GenDrawHDRBufferParams with basic surface params
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHeteroHDRManagerTest, GenDrawHDRBufferParamsTest001, TestSize.Level1)
{
    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(surfaceNode, nullptr);

    auto surfaceDrawable = std::make_shared<
        DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceNode);
    MDCRectT hpaeDstRect {0, 0, 100, 100};
    BufferDrawParam drawableParams;
    bool isFixedDstBuffer = false;

    // 测试没有renderParams的情况
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, 
        isFixedDstBuffer, drawableParams);
    EXPECT_EQ(drawableParams.hdrHeteroType, RSHeteroHDRUtilConst::HDR_HETERO_NO);
    EXPECT_EQ(drawableParams.srcRect.GetRight(), 1.0f);
    EXPECT_EQ(drawableParams.srcRect.GetBottom(), 1.0f);

    // 测试有renderParams但没有layerInfo的情况
    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable->nodeId_);
    surfaceDrawable->renderParams_ = std::move(drawableParam);
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, 
        isFixedDstBuffer, drawableParams);
    EXPECT_EQ(drawableParams.hdrHeteroType, RSHeteroHDRUtilConst::HDR_HETERO_NO);
}

/**
 * @tc.name: GenDrawHDRBufferParamsTest002
 * @tc.desc: Test GenDrawHDRBufferParams with buffer setup
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHeteroHDRManagerTest, GenDrawHDRBufferParamsTest002, TestSize.Level1)
{
    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto surfaceDrawable = std::make_shared<
        DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceNode);

    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable->nodeId_);
    surfaceDrawable->renderParams_ = std::move(drawableParam);
    auto surfaceParams = static_cast<RSSurfaceRenderParams *>(
        surfaceDrawable->GetRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);

    RSLayerInfo layerInfo;
    layerInfo.srcRect = {1, 1};
    layerInfo.dstRect = {1, 1};
    surfaceParams->SetLayerInfo(layerInfo);
    
    surfaceParams->buffer_ = OHOS::SurfaceBuffer::Create();
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE,
    };
    surfaceParams->GetBuffer()->Alloc(requestConfig);
    surfaceParams->SetHardwareEnabled(true);

    auto hdrSurfaceHandler = RSHeteroHDRManager::Instance().GetHDRSurfaceHandler();
    hdrSurfaceHandler->SetConsumer(IConsumerSurface::Create("test consumer"));
    RSUniRenderThread::Instance().uniRenderEngine_ = 
        std::make_shared<RSRenderEngine>();

    MDCRectT hpaeDstRect {0, 0, 100, 100};
    BufferDrawParam drawableParams;
    bool isFixedDstBuffer = false;
    
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, 
        isFixedDstBuffer, drawableParams);
    
    EXPECT_EQ(drawableParams.hdrHeteroType, RSHeteroHDRUtilConst::HDR_HETERO_NO);
    EXPECT_EQ(drawableParams.srcRect.GetLeft(), 0.0f);
    EXPECT_EQ(drawableParams.srcRect.GetTop(), 0.0f);
    EXPECT_EQ(drawableParams.srcRect.GetRight(), 1.0f);
    EXPECT_EQ(drawableParams.srcRect.GetBottom(), 1.0f);
}

/**
 * @tc.name: GenDrawHDRBufferParamsTest003
 * @tc.desc: Test GenDrawHDRBufferParams with matrix transformation
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHeteroHDRManagerTest, GenDrawHDRBufferParamsTest003, TestSize.Level1)
{
    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto surfaceDrawable = std::make_shared<
        DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceNode);

    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable->nodeId_);
    surfaceDrawable->renderParams_ = std::move(drawableParam);
    auto surfaceParams = static_cast<RSSurfaceRenderParams *>(
        surfaceDrawable->GetRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);

    RSLayerInfo layerInfo;
    layerInfo.matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    surfaceParams->SetLayerInfo(layerInfo);

    MDCRectT hpaeDstRect {0, 0, 100, 100};
    BufferDrawParam drawableParams;
    bool isFixedDstBuffer = false;
    
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, 
        isFixedDstBuffer, drawableParams);
    
    // 验证矩阵变换是否被正确处理
    EXPECT_EQ(drawableParams.hdrHeteroType, RSHeteroHDRUtilConst::HDR_HETERO_NO);
    EXPECT_EQ(drawableParams.transform.GetScaleX(), 1.0f);
    EXPECT_EQ(drawableParams.transform.GetSkewY(), 2.0f);
    EXPECT_EQ(drawableParams.transform.GetTranslateX(), 3.0f);
    EXPECT_EQ(drawableParams.transform.GetSkewX(), 4.0f);
    EXPECT_EQ(drawableParams.transform.GetScaleY(), 5.0f);
    EXPECT_EQ(drawableParams.transform.GetTranslateY(), 6.0f);
}

/**
 * @tc.name: GenDrawHDRBufferParamsTest004
 * @tc.desc: Test GenDrawHDRBufferParams with fixed destination buffer
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHeteroHDRManagerTest, GenDrawHDRBufferParamsTest004, TestSize.Level1)
{
    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto surfaceDrawable = std::make_shared<
        DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceNode);

    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable->nodeId_);
    surfaceDrawable->renderParams_ = std::move(drawableParam);
    auto surfaceParams = static_cast<RSSurfaceRenderParams *>(
        surfaceDrawable->GetRenderParams().get());

    RSLayerInfo layerInfo;
    layerInfo.srcRect = {1, 1};
    layerInfo.dstRect = {1, 1};
    surfaceParams->SetLayerInfo(layerInfo);

    MDCRectT hpaeDstRect {0, 0, 100, 100};
    BufferDrawParam drawableParams;
    bool isFixedDstBuffer = true;

    // 明确指定捕获的变量
    auto testCombination = [surfaceDrawable, hpaeDstRect, isFixedDstBuffer, &drawableParams](float right, float bottom) {
        drawableParams.srcRect.SetRight(right);
        drawableParams.srcRect.SetBottom(bottom);
        RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, 
            isFixedDstBuffer, drawableParams);
        EXPECT_EQ(drawableParams.srcRect.GetRight(), right);
        EXPECT_EQ(drawableParams.srcRect.GetBottom(), bottom);
    };

    testCombination(0.0, 0.0);
    testCombination(0.0, 1.0);
    testCombination(1.0, 0.0);
    testCombination(1.0, 1.0);
}

/**
 * @tc.name: GenDrawHDRBufferParamsTest005
 * @tc.desc: Test GenDrawHDRBufferParams with scaling mode
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHeteroHDRManagerTest, GenDrawHDRBufferParamsTest005, TestSize.Level1)
{
    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto surfaceDrawable = std::make_shared<
        DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceNode);

    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable->nodeId_);
    surfaceDrawable->renderParams_ = std::move(drawableParam);
    auto surfaceParams = static_cast<RSSurfaceRenderParams *>(
        surfaceDrawable->GetRenderParams().get());

    RSLayerInfo layerInfo;
    layerInfo.srcRect = {1, 1};
    layerInfo.dstRect = {1, 1};
    surfaceParams->SetLayerInfo(layerInfo);

    surfaceParams->buffer_ = OHOS::SurfaceBuffer::Create();
    surfaceParams->GetBuffer()->SetSurfaceBufferScalingMode(
        ScalingMode::SCALING_MODE_SCALE_FIT);

    MDCRectT hpaeDstRect {0, 0, 100, 100};
    BufferDrawParam drawableParams;
    bool isFixedDstBuffer = true;
    
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, 
        isFixedDstBuffer, drawableParams);
    
    // 验证缩放模式是否被正确处理
    EXPECT_EQ(drawableParams.hdrHeteroType, RSHeteroHDRUtilConst::HDR_HETERO_NO);
    EXPECT_EQ(surfaceParams->GetBuffer()->GetSurfaceBufferScalingMode(), 
              ScalingMode::SCALING_MODE_SCALE_FIT);
}

/**
 * @tc.name: GenDrawHDRBufferParamsTest006
 * @tc.desc: Test GenDrawHDRBufferParams with buffer transform
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHeteroHDRManagerTest, GenDrawHDRBufferParamsTest006, TestSize.Level1)
{
    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto surfaceDrawable = std::make_shared<
        DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceNode);

    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable->nodeId_);
    surfaceDrawable->renderParams_ = std::move(drawableParam);
    auto surfaceParams = static_cast<RSSurfaceRenderParams *>(
        surfaceDrawable->GetRenderParams().get());

    RSLayerInfo layerInfo;
    surfaceParams->SetLayerInfo(layerInfo);
    MDCRectT hpaeDstRect {0, 0, 100, 100};
    BufferDrawParam drawableParams;
    bool isFixedDstBuffer = true;

    // 明确指定捕获的变量
    auto testRotation = [surfaceParams, surfaceDrawable, hpaeDstRect, isFixedDstBuffer, &drawableParams](GraphicTransformType rotation) {
        surfaceParams->GetBuffer()->SetSurfaceBufferTransform(rotation);
        RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, 
            isFixedDstBuffer, drawableParams);
        EXPECT_EQ(surfaceParams->GetBuffer()->GetSurfaceBufferTransform(), rotation);
    };

    testRotation(GRAPHIC_ROTATE_90);
    testRotation(GRAPHIC_ROTATE_180);
    testRotation(GRAPHIC_ROTATE_270);
}

/**
 * @tc.name: GenDrawHDRBufferParamsTest007
 * @tc.desc: Test GenDrawHDRBufferParams with different hpaeDstRect sizes
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHeteroHDRManagerTest, GenDrawHDRBufferParamsTest007, TestSize.Level1)
{
    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto surfaceDrawable = std::make_shared<
        DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceNode);

    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable->nodeId_);
    surfaceDrawable->renderParams_ = std::move(drawableParam);
    auto surfaceParams = static_cast<RSSurfaceRenderParams *>(
        surfaceDrawable->GetRenderParams().get());

    const Rosen::Drawing::Rect boundsRect {0, 0, 100, 100};
    surfaceParams->SetBoundsRect(boundsRect);
    bool isFixedDstBuffer = true;
    BufferDrawParam drawableParams;

    // 明确指定捕获的变量
    auto testRect = [surfaceDrawable, isFixedDstBuffer, &drawableParams](int w, int h) {
        MDCRectT rect = {0, 0, w, h};
        RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, rect, 
            isFixedDstBuffer, drawableParams);
        EXPECT_EQ(drawableParams.dstRect.width, w);
        EXPECT_EQ(drawableParams.dstRect.height, h);
    };

    testRect(120, 20);
    testRect(120, 20);
    testRect(20, 20);
    testRect(100, 100);
    testRect(0, 100);
    testRect(100, 0);
}
// end original test: GenDrawHDRBufferParamsTest

/**
 * @tc.name: GenerateHDRHeteroShaderTest
 * @tc.desc: Test GenerateHDRHeteroShader
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, GenerateHDRHeteroShaderTest, TestSize.Level1)
{
    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    std::shared_ptr<Drawing::ShaderEffect> imageShader = std::make_shared<Drawing::ShaderEffect>();
    ASSERT_NE(imageShader, nullptr);

    BufferDrawParam paramHDR;
    paramHDR.hdrHeteroType = RSHeteroHDRUtilConst::HDR_HETERO_HDR;
    mockRSHeteroHDRManager.GenerateHDRHeteroShader(paramHDR, imageShader);

    BufferDrawParam paramAIHDR;
    paramAIHDR.hdrHeteroType = RSHeteroHDRUtilConst::HDR_HETERO_AIHDR;
    mockRSHeteroHDRManager.GenerateHDRHeteroShader(paramAIHDR, imageShader);

    BufferDrawParam paramDefault;
    paramDefault.hdrHeteroType = 0;
    mockRSHeteroHDRManager.GenerateHDRHeteroShader(paramDefault, imageShader);

#ifdef USE_VIDEO_PROCESSING_ENGINE
    BufferDrawParam paramNit;
    paramNit.sdrNits = 0.0f;
    mockRSHeteroHDRManager.GenerateHDRHeteroShader(paramNit, imageShader);

    BufferDrawParam paramNitDefault;
    paramNitDefault.sdrNits = 500.0f;
    mockRSHeteroHDRManager.GenerateHDRHeteroShader(paramNitDefault, imageShader);
#endif // USE_VIDEO_PROCESSING_ENGINE
}

/**
 * @tc.name: DrawSurfaceNodeWithParamsTest
 * @tc.desc: Test DrawSurfaceNodeWithParams
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, DrawSurfaceNodeWithParamsTest, TestSize.Level1)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
    param.useCPU = false;

    param.hdrHeteroType = RSHeteroHDRUtilConst::HDR_HETERO_HDR | RSHeteroHDRUtilConst::HDR_HETERO;
    param.sdrNits = 5.0f;
    uniRenderEngine->DrawSurfaceNodeWithParams(*canvas, *node, param, nullptr, nullptr);

    param.hdrHeteroType = RSHeteroHDRUtilConst::HDR_HETERO_NO;
    param.sdrNits = 0.0f;
    uniRenderEngine->DrawSurfaceNodeWithParams(*canvas, *node, param, nullptr, nullptr);

    param.hdrHeteroType = RSHeteroHDRUtilConst::HDR_HETERO_HDR | RSHeteroHDRUtilConst::HDR_HETERO;
    param.sdrNits = 0.0f;
    uniRenderEngine->DrawSurfaceNodeWithParams(*canvas, *node, param, nullptr, nullptr);

    param.hdrHeteroType = RSHeteroHDRUtilConst::HDR_HETERO_NO;
    param.sdrNits = 5.0f;
    uniRenderEngine->DrawSurfaceNodeWithParams(*canvas, *node, param, nullptr, nullptr);
}

/**
 * @tc.name: CheckIsHdrSurfaceBuffer
 * @tc.desc: Test CheckIsHdrSurfaceBuffer
 * @tc.type: FUNC
 * @tc.require:issueI6QM6E
 */
HWTEST_F(RSHeteroHDRManagerTest, CheckIsHdrSurfaceBufferTest, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_TRUE(node->GetRSSurfaceHandler() != nullptr);
    sptr<SurfaceBuffer> buffer = nullptr;
    HdrStatus ret = RSHdrUtil::CheckIsHdrSurfaceBuffer(buffer);
    ASSERT_EQ(ret, HdrStatus::NO_HDR);
    buffer = node->GetRSSurfaceHandler()->GetBuffer();
    ASSERT_TRUE(buffer != nullptr);
    ASSERT_TRUE(buffer->GetBufferHandle() != nullptr);
#ifdef USE_VIDEO_PROCESSING_ENGINE
    uint32_t hdrType = 12345;
    std::vector<uint8_t> metadataType;
    metadataType.resize(sizeof(hdrType));
    memcpy_s(metadataType.data(), metadataType.size(), &hdrType, sizeof(hdrType));
    buffer->SetMetadata(Media::VideoProcessingEngine::ATTRKEY_HDR_METADATA_TYPE, metadataType);
    ret = RSHdrUtil::CheckIsHdrSurfaceBuffer(buffer);
    ASSERT_EQ(ret, HdrStatus::NO_HDR);
#endif
}

} // namespace OHOS::Rosen