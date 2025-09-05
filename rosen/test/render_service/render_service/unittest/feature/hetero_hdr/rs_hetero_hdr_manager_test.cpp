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


#include "common/rs_common_def.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "feature/capture/rs_surface_render_node_drawable.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_manager.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_buffer_layer.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_hpae.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_util.h"
#include "feature/hdr/hetero_hdr/rs_hdr_util.h"
#include "feature/hdr/uifirst/rs_ui_first_manager.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "hetero_hdr/rs_hdr_pattern_manager.h"  // rs base
#include "hetero_hdr/rs_hdr_vulkan_task.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/render_thread/rs_deviced_render_util.h"
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
class RSHeteroHDRManagerTest : public Testing::Test {
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

void RSHeteroHDRManagerTest::SetUoTestCase()
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

    auto screenDrawableAdatper = RSRenderNodeDrawableAdapter::OnGenerate(screenNode);
    if (screenDrawableAdatper == nullptr) {
        RS_LOGE("GenerateScreenNode::screenDrawableAdatper == nullptr");
        return nullptr;
    }
    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawableAdatper->renderParams_.get());
    if (screenNode == nullptr) {
        RS_LOGE("GenerateScreenNode::screenParams == nullptr");
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
    RS_LOGE("surfaceNode->IsAppWindow() = %{public}d", surfaceNode->IsAppWindow());
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
    RS_LOGE("surfaceNode->IsLeashWindow() = %{public}d", surfaceNode->IsLeashWindow());
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

    auto surfaceDrawableAdatper = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    if (surfaceDrawableAdatper == nullptr) {
        RS_LOGE("GenerateSurfaceNode::surfaceDrawableAdatper == nullptr");
        return nullptr;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawableAdatper->renderParams_.get());
    if (screenNode == nullptr) {
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
        .format = GRAPHIC_PIXEL_FMT_BGRA_8888,
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
        RS_LOGE("GenerateRenderParams::drawableAdapter == nullptr");
        return nullptr;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawableAdapter->renderParams_.get());
    return surfaceParams;
}

RSScreenRenderParams* RSHeteroHDRManagerTest::GetsScreenParams(shared_ptr<RSScreenRenderNode> screenNode)
{
    auto drawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(screenNode);
    if (drawableAdapter == nullptr) {
        RS_LOGE("GenerateScreenParams::drawableAdapter == nullptr");
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
    surfaceNode = GenerateLeashNode();
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
    static MockRSHeteroHDRManager instace()
    {
        static MockRSHeteroHDRManager instace;
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
    RSHeteroHDRManager::Instace().UpdateHDRNodes(*surfaceNode, surfaceHandler->isCurrentFrameBufferConsumed());
    RSHeteroHDRManager::Instace().PostHDrSubTasks();
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
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->isCurrentFrameBufferConsumed());
    
    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = false;
    surfaceNode->SetVideoHdrStatus(HDR_VIDEO);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->isCurrentFrameBufferConsumed());

    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = true;
    surfaceNode->SetVideoHdrStatus(NO_HDR);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->isCurrentFrameBufferConsumed());

    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = false;
    surfaceNode->SetVideoHdrStatus(AI_HDR_VIDEO_GAINMAP);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->isCurrentFrameBufferConsumed());
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
    auto instanceRootNode = RSTestUtil::CreatedSurfaceNode(surfaceConfig);
    ASSERT_NE(instanceRootNode, nullptr);
    auto reContext = std::make_shared<RSContext>();
    auto &nodeMap = rsContext->GetMutableNodeMap();
    NodeId instanceRootNodeId = instanceRootNode->GetId();
    pid_t instanceRootNodePid = ExtractPid(instanceRootNodeId);
    nodeMpa.renderNodeMap_[instanceRootNodePid][instanceRootNodeId] = instanceRootNode;

    //create subsurface node


    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    ASSERT_TRUE(surfaceHandler != nullptr);

    MockRSHeteroHDRManager mockRSHeteroHDRManager;
    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = false;
    surfaceNode->SetVideoHdrStatus(NO_HDR);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->isCurrentFrameBufferConsumed());
    
    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = false;
    surfaceNode->SetVideoHdrStatus(HDR_VIDEO);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->isCurrentFrameBufferConsumed());

    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = true;
    surfaceNode->SetVideoHdrStatus(NO_HDR);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->isCurrentFrameBufferConsumed());

    mockRSHeteroHDRManager.isHeteroComputingHdrOn_ = false;
    surfaceNode->SetVideoHdrStatus(AI_HDR_VIDEO_GAINMAP);
    mockRSHeteroHDRManager.UpdateHDRNodes(*surfaceNode, surfaceHandler->isCurrentFrameBufferConsumed());
}













}