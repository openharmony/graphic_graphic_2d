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

/**
 * @tc.name: PostHDRSubTasksTest002
 * @tc.desc: Test PostHDRSubTasks app and leash node condition
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest002, TestSize.Level1)
{
    // test skip and map
    auto screenNode = GenerateScreenNode();
    auto appNode = GenerateAppNode();
    auto leashNode = GenerateLeashNode();
    auto surfaceNode = GenerateSurfaceNode();
    ASSERT_TRUE(screenNode != nullptr && appNode != nullptr && leashNode != nullptr && surfaceNode != nullptr);
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    ASSERT_TRUE(surfaceHandler != nullptr);

    auto appParams = GetRenderParams(appNode);
    auto leashParams = GetRenderParams(leashNode);
    auto surfaceParams = GetRenderParams(surfaceNode);
    surfaceParams->SetAncestorScreenNode(screenNode);

    appParams->shouldPaint_ = true;
    appParams->contentEmpty_ = false;
    leashParams->shouldPaint_ = true;
    leashParams->contentEmpty_ = false;

    // all return true
    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = true;
    EXPECT_CALL(mockRSHeteroHDRManager, MHCRequestEGraph(_)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockRSHeteroHDRManager, MHCSetCurFrameId(_)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockRSHeteroHDRManager, TryConsumeBuffer(_)).WillRepeatedly(testing::Return(true));
    RSHeteroHDRHpae::GetInstance().MDCExistedStatus_.store(true);
    mockRSHeteroHDRManager.curHandleStatus_ = RSHeteroHDRHpae::GetInstance().existedChannelStatus_;

    // test app and leash node
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    appNode->AddChild(surfaceNode);
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    NodeId instanceRootNodeId = appNode->GetId();
    pid_t instanceRootNodePid = ExtractPid(instanceRootNodeId);
    nodeMap.renderNodeMap_[instanceRootNodePid][instanceRootNodeId] = appNode;
    surfaceNode->context_ = rsContext;
    surfaceNode->instanceRootNodeId_ = appNode->GetId();

    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    leashNode->AddChild(appNode);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    appParams->SetAncestorScreenNode(screenNode);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    leashParams->SetAncestorScreenNode(screenNode);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    // test pendingNodes
    auto pendingNodes = RSUifirstManager::Instance().GetPendingPostNodes();
    std::weak_ptr<RSRenderNode> ancestorScreenNode;
    leashParams->SetAncestorScreenNode(ancestorScreenNode);
    pendingNodes.insert({surfaceNode->GetId(), surfaceNode});
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    pendingNodes.erase({surfaceNode->GetId()});
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    leashParams->SetAncestorScreenNode(screenNode);
    pendingNodes.insert({surfaceNode->GetId(), surfaceNode});
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    pendingNodes.erase({surfaceNode->GetId()});
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    // test skiped
    leashParams->shouldPaint_ = false;
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    appParams->shouldPaint_ = false;
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    // test hardware enable
    surfaceParams->SetHardwareEnabled(true);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();
    surfaceParams->SetHardwareEnabled(false);
}

/**
 * @tc.name: PostHDRSubTasksTest003
 * @tc.desc: Test PostHDRSubTasks rect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PostHDRSubTasksTest003, TestSize.Level1)
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
    RSHeteroHDRHpae::GetInstance().MDCExistedStatus_.store(true);
    mockRSHeteroHDRManager.curHandleStatus_ = RSHeteroHDRHpae::GetInstance().existedChannelStatus_;

    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();
    
    RSLayerInfo layerInfo = surfaceParams->GetLayerInfo();
    layerInfo.transformType = GRAPHIC_ROTATE_90;
    surfaceParams->SetLayerInfo(layerInfo);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    layerInfo.transformType = GRAPHIC_ROTATE_270;
    surfaceParams->SetLayerInfo(layerInfo);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    // rect test
    layerInfo.dstRect.w = 0;
    layerInfo.dstRect.h = 1;
    surfaceParams->SetLayerInfo(layerInfo);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    layerInfo.dstRect.w = 1;
    layerInfo.dstRect.h = 0;
    surfaceParams->SetLayerInfo(layerInfo);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();
    layerInfo.dstRect.w = 1;
    layerInfo.dstRect.h = 1;

    layerInfo.srcRect.w = 1;
    layerInfo.srcRect.h = 0;
    surfaceParams->SetLayerInfo(layerInfo);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();

    layerInfo.srcRect.w = 0;
    layerInfo.srcRect.h = 1;
    surfaceParams->SetLayerInfo(layerInfo);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->IsCurrentFrameBufferConsumed());
    mockRSHeteroHDRManager.PostHDRSubTasks();
    layerInfo.srcRect.w = 1;
    layerInfo.srcRect.h = 1;
}

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
 * @tc.name: PrepareAndSubmitHDRTaskTest010
 * @tc.desc: Test PrepareAndSubmitHDRTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PrepareAndSubmitHDRTaskTest010, TestSize.Level1)
{
    MockRSHeteroHDRManager mockRSHeteroHDRManager;

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(surfaceDrawable, nullptr);
    auto surfaceParams = GetRenderParams(surfaceNode);
    ASSERT_NE(surfaceParams, nullptr);

    RSLayerInfo layerInfo;
    layerInfo.srcRect.w = 400;
    layerInfo.srcRect.h = 400;
    layerInfo.dstRect.w = 400;
    layerInfo.dstRect.h = 400;

    layerInfo.matrix.Set(Drawing::Matrix::SKEW_X, 1.0);
    layerInfo.matrix.Set(Drawing::Matrix::SKEW_Y, 1.0);

    surfaceParams->SetLayerInfo(layerInfo);
    surfaceParams->buffer_ = OHOS::SurfaceBuffer::Create();
    BufferRequestConfig requestConfig = {
        .width = 0x190,
        .height = 0x190,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    surfaceParams->GetBuffer()->Alloc(requestConfig);
    surfaceParams->GetBuffer()->SetSurfaceBufferScalingMode(ScalingMode::SCALING_MODE_FREEZE);

    auto nodeId = surfaceNode->GetId();

    RectI tempRect = { 0, 0, 400, 400 };
    mockRSHeteroHDRManager.src_ = tempRect;
    mockRSHeteroHDRManager.nodeSrcRectMap_[nodeId] = tempRect;
    mockRSHeteroHDRManager.PrepareAndSubmitHDRTask(surfaceDrawable, surfaceParams, nodeId, 1);
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

/**
 * @tc.name: GenerateHpaeRectTest001
 * @tc.desc: Test GenerateHpaeRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, GenerateHpaeRectTest001, TestSize.Level1)
{
    MockRSHeteroHDRManager mockRSHeteroHDRManager;

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(surfaceDrawable, nullptr);
    auto surfaceParams = GetRenderParams(surfaceNode);
    ASSERT_NE(surfaceParams, nullptr);

    RSLayerInfo layerInfo;
    layerInfo.srcRect.w = 400;
    layerInfo.srcRect.h = 400;
    layerInfo.dstRect.w = 400;
    layerInfo.dstRect.h = 400;

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

    RectI hpaeSrcRect = {};
    RectI validHpaeDstRect = {};

    mockRSHeteroHDRManager.isFixedDstBuffer_ = true;
    mockRSHeteroHDRManager.GenerateHpaeRect(surfaceParams, hpaeSrcRect, validHpaeDstRect);

    mockRSHeteroHDRManager.isFixedDstBuffer_ = false;
    mockRSHeteroHDRManager.GenerateHpaeRect(surfaceParams, hpaeSrcRect, validHpaeDstRect);
}

/**
 * @tc.name: PrepareHpaeTaskTest001
 * @tc.desc: Test PrepareHpaeTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, PrepareHpaeTaskTest001, TestSize.Level1)
{
    MockRSHeteroHDRManager mockRSHeteroHDRManager;

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(surfaceDrawable, nullptr);
    auto surfaceParams = GetRenderParams(surfaceNode);
    ASSERT_NE(surfaceParams, nullptr);

    RSLayerInfo layerInfo;
    layerInfo.srcRect.w = 400;
    layerInfo.srcRect.h = 400;
    layerInfo.dstRect.w = 400;
    layerInfo.dstRect.h = 400;

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

    auto temp = mockRSHeteroHDRManager.src_.width_;
    mockRSHeteroHDRManager.PrepareHpaeTask(surfaceDrawable, surfaceParams, 1);

    mockRSHeteroHDRManager.src_.width_ = temp + 1;
    mockRSHeteroHDRManager.PrepareHpaeTask(surfaceDrawable, surfaceParams, 1);
    mockRSHeteroHDRManager.src_.width_ = temp;
}

/**
 * @tc.name: ValidateSurfaceTest001
 * @tc.desc: Test ValidateSurface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, ValidateSurfaceTest001, TestSize.Level1)
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
    surfaceParams->SetCacheSize(100, 100);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, true);

    layerInfo.srcRect.w = 400;
    layerInfo.srcRect.h = 400;
    layerInfo.dstRect.w = 400;
    layerInfo.dstRect.h = 400;
    surfaceParams->SetLayerInfo(layerInfo);
    surfaceParams->SetCacheSize(0, 100);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, true);

    layerInfo.srcRect.w = 400;
    layerInfo.srcRect.h = 400;
    layerInfo.dstRect.w = 400;
    layerInfo.dstRect.h = 400;
    surfaceParams->SetLayerInfo(layerInfo);
    surfaceParams->SetCacheSize(0, 0);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, true);

    layerInfo.srcRect.w = 400;
    layerInfo.srcRect.h = 400;
    layerInfo.dstRect.w = 400;
    layerInfo.dstRect.h = 400;
    surfaceParams->SetLayerInfo(layerInfo);
    surfaceParams->SetCacheSize(100, 0);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, true);

    layerInfo.srcRect.w = 0;
    layerInfo.srcRect.h = 400;
    layerInfo.dstRect.w = 400;
    layerInfo.dstRect.h = 400;
    surfaceParams->SetLayerInfo(layerInfo);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, false);

    layerInfo.srcRect.w = 5000;
    layerInfo.srcRect.h = 400;
    layerInfo.dstRect.w = 400;
    layerInfo.dstRect.h = 400;
    surfaceParams->SetLayerInfo(layerInfo);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, false);

    layerInfo.srcRect.w = 400;
    layerInfo.srcRect.h = 0;
    layerInfo.dstRect.w = 400;
    layerInfo.dstRect.h = 400;
    surfaceParams->SetLayerInfo(layerInfo);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, false);

    layerInfo.srcRect.w = 400;
    layerInfo.srcRect.h = 5000;
    layerInfo.dstRect.w = 400;
    layerInfo.dstRect.h = 400;
    surfaceParams->SetLayerInfo(layerInfo);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, false);

    layerInfo.srcRect.w = 400;
    layerInfo.srcRect.h = 400;
    layerInfo.dstRect.w = 0;
    layerInfo.dstRect.h = 400;
    surfaceParams->SetLayerInfo(layerInfo);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, false);

    layerInfo.srcRect.w = 400;
    layerInfo.srcRect.h = 400;
    layerInfo.dstRect.w = 5000;
    layerInfo.dstRect.h = 400;
    surfaceParams->SetLayerInfo(layerInfo);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, false);

    layerInfo.srcRect.w = 400;
    layerInfo.srcRect.h = 400;
    layerInfo.dstRect.w = 400;
    layerInfo.dstRect.h = 0;
    surfaceParams->SetLayerInfo(layerInfo);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, false);

    layerInfo.srcRect.w = 400;
    layerInfo.srcRect.h = 400;
    layerInfo.dstRect.w = 400;
    layerInfo.dstRect.h = 5000;
    surfaceParams->SetLayerInfo(layerInfo);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, false);


    layerInfo.srcRect.w = 400;
    layerInfo.srcRect.h = 400;
    layerInfo.dstRect.w = 400;
    layerInfo.dstRect.h = 400;
    surfaceParams->SetLayerInfo(layerInfo);

    surfaceParams->GetBuffer()->SetSurfaceBufferWidth(1);
    surfaceParams->GetBuffer()->SetSurfaceBufferHeight(0);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, false);

    surfaceParams->GetBuffer()->SetSurfaceBufferWidth(0);
    surfaceParams->GetBuffer()->SetSurfaceBufferHeight(1);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, false);

    surfaceParams->GetBuffer()->SetSurfaceBufferWidth(1);
    surfaceParams->GetBuffer()->SetSurfaceBufferHeight(1);
    ret = RSHeteroHDRUtil::ValidateSurface(surfaceParams);
    EXPECT_EQ(ret, true);
}

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
    surfaceParams->SetCacheSize(100, 100);

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
 * @tc.name: ProcessPendingNodeDrawableTest
 * @tc.desc: Test ProcessPendingNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, ProcessPendingNodeDrawableTest, TestSize.Level1)
{
    RS_LOGI("************************RSHeteroHDRManagerTest ProcessPendingNodeDrawableTest************************");
    NodeId id = 4;
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = id;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);

    NodeId id3 = 3;
    std::shared_ptr<const RSRenderNode> node = std::make_shared<const RSRenderNode>(id3);
    auto adapter = std::make_shared<RSRenderNodeDrawable>(std::move(node));
    std::weak_ptr<RSRenderNodeDrawable> drawableAdapter = adapter;
    auto drawableCache = DrawableV2::RSRenderNodeDrawableAdapter::RenderNodeDrawableCache_;
    DrawableV2::RSRenderNodeDrawableAdapter::RenderNodeDrawableCache_.clear();
    DrawableV2::RSRenderNodeDrawableAdapter::RenderNodeDrawableCache_.insert(std::make_pair(id3, drawableAdapter));

    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    bool ret = mockRSHeteroHDRManager.ProcessPendingNode(surfaceNode, 0);
    EXPECT_EQ(ret, false);
    DrawableV2::RSRenderNodeDrawableAdapter::RenderNodeDrawableCache_ = drawableCache;
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
 * @tc.name: UpdateHDRHeteroParamsTest
 * @tc.desc: Test UpdateHDRHeteroParams
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, UpdateHDRHeteroParamsTest002, TestSize.Level1)
{
    RsVulkanContext::GetSingleton().InitVulkanContextForUniRender("");
    auto interfaceTypeTmp = RsVulkanContext::GetSingleton().vulkanInterfaceType_;

    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = true;
    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceDrawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceNode);
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    BufferDrawParam drawableParams;

    RsVulkanContext::GetSingleton().vulkanInterfaceType_ = VulkanInterfaceType::BASIC_RENDER;
    auto ret = RSHDRVulkanTask::IsInterfaceTypeBasicRender();
    mockRSHeteroHDRManager.UpdateHDRHeteroParams(canvas, *surfaceDrawable, drawableParams);
    EXPECT_EQ(ret, true);

    RsVulkanContext::GetSingleton().vulkanInterfaceType_ = VulkanInterfaceType::UNPROTECTED_REDRAW;
    auto ret1 = RSHDRVulkanTask::IsInterfaceTypeBasicRender();
    mockRSHeteroHDRManager.UpdateHDRHeteroParams(canvas, *surfaceDrawable, drawableParams);
    EXPECT_EQ(ret1, false);
    RsVulkanContext::GetSingleton().vulkanInterfaceType_ = interfaceTypeTmp;
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

/**
 * @tc.name: GenDrawHDRBufferParamsTest
 * @tc.desc: Test GenDrawHDRBufferParams
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRManagerTest, GenDrawHDRBufferParamsTest, TestSize.Level1)
{
    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(surfaceNode, nullptr);

    auto surfaceDrawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceNode);

    // surfaceParams test
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    bool isFixedDstBuffer = false;
    MDCRectT hpaeDstRect {0, 0, 100, 100};
    BufferDrawParam param;
    BufferDrawParam drawableParams;
    uint32_t hdrHeteroType = RSHeteroHDRUtilConst::HDR_HETERO_NO;
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);

    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable->nodeId_);
    surfaceDrawable->renderParams_ = std::move(drawableParam);
    auto surfaceParams = static_cast<RSSurfaceRenderParams *>(surfaceDrawable->GetRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);

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
    auto hdrSurfaceHandler = RSHeteroHDRManager::Instance().GetHDRSurfaceHandler();
    hdrSurfaceHandler->SetConsumer(IConsumerSurface::Create("test consumer"));
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();

    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);

    // matrix
    layerInfo.matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    surfaceParams->SetLayerInfo(layerInfo);
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);

    // isFixedDstBuffer and scalingMode
    isFixedDstBuffer = true;
    drawableParams.srcRect.SetRight(0.0);
    drawableParams.srcRect.SetBottom(0.0);
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);
    drawableParams.srcRect.SetRight(0.0);
    drawableParams.srcRect.SetBottom(1.0);
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);
    drawableParams.srcRect.SetRight(1.0);
    drawableParams.srcRect.SetBottom(0.0);
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);
    drawableParams.srcRect.SetRight(1.0);
    drawableParams.srcRect.SetBottom(1.0);
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);
    
    surfaceParams->GetBuffer()->SetSurfaceBufferScalingMode(ScalingMode::SCALING_MODE_SCALE_FIT);
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);

    // bufferTransform
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);
    surfaceParams->GetBuffer()->SetSurfaceBufferTransform(GRAPHIC_ROTATE_90);
    surfaceParams->SetLayerInfo(layerInfo);
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);
    surfaceParams->GetBuffer()->SetSurfaceBufferTransform(GRAPHIC_ROTATE_180);
    surfaceParams->SetLayerInfo(layerInfo);
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);
    surfaceParams->GetBuffer()->SetSurfaceBufferTransform(GRAPHIC_ROTATE_270);
    surfaceParams->SetLayerInfo(layerInfo);
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);

    // hpaeDstRect
    isFixedDstBuffer = true;
    const Rosen::Drawing::Rect boundsRect {0, 0, 100, 100};
    surfaceParams->SetBoundsRect(boundsRect);
    hpaeDstRect = MDCRectT {0, 0, 120, 20};
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);
    hpaeDstRect = MDCRectT {0, 0, 120, 20};
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);
    hpaeDstRect = MDCRectT {0, 0, 20, 20};
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);
    hpaeDstRect = MDCRectT {0, 0, 100, 100};
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);
    hpaeDstRect = MDCRectT {0, 0, 0, 100};
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);
    hpaeDstRect = MDCRectT {0, 0, 100, 0};
    RSHeteroHDRUtil::GenDrawHDRBufferParams(*surfaceDrawable, hpaeDstRect, isFixedDstBuffer, drawableParams);
}

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