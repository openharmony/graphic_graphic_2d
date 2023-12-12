/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "pipeline/rs_context.h"
#include "pipeline/parallel_render/rs_node_cost_manager.h"
#include "limit_number.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_system_properties.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsNodeCostManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsNodeCostManagerTest::SetUpTestCase() {}
void RsNodeCostManagerTest::TearDownTestCase() {}
void RsNodeCostManagerTest::SetUp() {}
void RsNodeCostManagerTest::TearDown() {}

/**
 * @tc.name: GetDirtyNodeCostTest
 * @tc.desc: Test RsNodeCostManagerTest.GetDirtyNodeCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RsNodeCostManagerTest, GetDirtyNodeCostTest, TestSize.Level1)
{
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
    rsNodeCostManager->AddNodeCost(5);
    result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(5, result);
}

/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RsNodeCostManagerTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RsNodeCostManagerTest, IsSkipProcessingTest1, TestSize.Level1)
{
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->SetSecurityLayer(1);
    auto result = rsNodeCostManager->IsSkipProcessing(*rsSurfaceRenderNode);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RsNodeCostManagerTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RsNodeCostManagerTest, IsSkipProcessingTest2, TestSize.Level1)
{
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(0.0f);
    rsSurfaceRenderNode->shouldPaint_ = false;
    auto result = rsNodeCostManager->IsSkipProcessing(*rsSurfaceRenderNode);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RsNodeCostManagerTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RsNodeCostManagerTest, IsSkipProcessingTest3, TestSize.Level1)
{
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(false, 1, false);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(2.0f);
    rsSurfaceRenderNode->SetOcclusionVisible(false);
    rsNodeCostManager->isOcclusionEnabled_ = true;
    auto result = rsNodeCostManager->IsSkipProcessing(*rsSurfaceRenderNode);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RsNodeCostManagerTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RsNodeCostManagerTest, IsSkipProcessingTest4, TestSize.Level1)
{
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(false, 1, false);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.nodeType = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(2.0f);
    rsSurfaceRenderNode->SetOcclusionVisible(false);
    rsNodeCostManager->isOcclusionEnabled_ = false;
    rsSurfaceRenderNode->dstRect_.Clear();
    auto result = rsNodeCostManager->IsSkipProcessing(*rsSurfaceRenderNode);
    ASSERT_TRUE(result);
}
#ifdef RS_ENABLE_EGLQUERYSURFACE
/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RsNodeCostManagerTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RsNodeCostManagerTest, IsSkipProcessingTest5, TestSize.Level1)
{
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(true, 1, false);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    rsNodeCostManager->isOpDropped_ = true;
    config.nodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(2.0f);
    rsSurfaceRenderNode->SetOcclusionVisible(false);
    rsSurfaceRenderNode->SetSecurityLayer(false);
    rsNodeCostManager->isOcclusionEnabled_ = false;
    rsSurfaceRenderNode->oldDirtyInSurface_ = RectI(0, 0, 100, 100);
    rsSurfaceRenderNode->visibleRegion_.rects_.emplace_back(Occlusion::Rect(200, 200, 200, 200));
    rsNodeCostManager->partialRenderType_ = PartialRenderType::SET_DAMAGE_AND_DROP_OP_OCCLUSION;
    rsSurfaceRenderNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    rsSurfaceRenderNode->globalDirtyRegionIsEmpty_ = true;
    auto result = rsNodeCostManager->IsSkipProcessing(*rsSurfaceRenderNode);
    ASSERT_TRUE(result);
}
#endif

#ifdef RS_ENABLE_EGLQUERYSURFACE
/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RsNodeCostManagerTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RsNodeCostManagerTest, IsSkipProcessingTest6, TestSize.Level1)
{
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(true, 1, false);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    rsNodeCostManager->isOpDropped_ = true;
    config.nodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(2.0f);
    rsSurfaceRenderNode->SetOcclusionVisible(false);
    rsSurfaceRenderNode->SetSecurityLayer(false);
    rsNodeCostManager->isOcclusionEnabled_ = false;
    rsSurfaceRenderNode->oldDirtyInSurface_ = RectI(10000, 90000, -1000, -1000);
    rsNodeCostManager->partialRenderType_ = PartialRenderType::DISABLED;
    rsSurfaceRenderNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    rsSurfaceRenderNode->globalDirtyRegionIsEmpty_ = true;
    auto result = rsNodeCostManager->IsSkipProcessing(*rsSurfaceRenderNode);
    ASSERT_FALSE(result);
}
#endif

/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RsNodeCostManagerTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RsNodeCostManagerTest, IsSkipProcessingTest7, TestSize.Level1)
{
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(true, 1, false);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    rsNodeCostManager->isOpDropped_ = false;
    config.nodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(2.0f);
    rsSurfaceRenderNode->SetOcclusionVisible(false);
    rsSurfaceRenderNode->SetSecurityLayer(false);
    rsNodeCostManager->isOcclusionEnabled_ = false;
    rsSurfaceRenderNode->oldDirtyInSurface_ = RectI(10000, 90000, -1000, -1000);
    rsNodeCostManager->partialRenderType_ = PartialRenderType::DISABLED;
    rsSurfaceRenderNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    rsSurfaceRenderNode->globalDirtyRegionIsEmpty_ = true;
    auto result = rsNodeCostManager->IsSkipProcessing(*rsSurfaceRenderNode);
    ASSERT_FALSE(result);
}

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
/**
 * @tc.name: CalcBaseRenderNodeCostTest
 * @tc.desc: Test RsNodeCostManagerTest.CalcBaseRenderNodeCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RsNodeCostManagerTest, CalcBaseRenderNodeCostTest1, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto rightNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->AddChild(node);
    rsSurfaceRenderNode->AddChild(rightNode);
    rsNodeCostManager->CalcBaseRenderNodeCost(*rsSurfaceRenderNode);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
}
#endif

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
/**
 * @tc.name: CalcBaseRenderNodeCostTest
 * @tc.desc: Test RsNodeCostManagerTest.CalcBaseRenderNodeCostTest2
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RsNodeCostManagerTest, CalcBaseRenderNodeCostTest2, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    auto rightNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->AddChild(rightNode);
    rsNodeCostManager->CalcBaseRenderNodeCost(*rsSurfaceRenderNode);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
}
#endif

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
/**
 * @tc.name: CalcBaseRenderNodeCostTest
 * @tc.desc: Test RsNodeCostManagerTest.CalcBaseRenderNodeCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RsNodeCostManagerTest, CalcBaseRenderNodeCostTest3, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSCanvasRenderNode node(nodeId);
    node.GetMutableRenderProperties().SetAlpha(0.0f);
    rsNodeCostManager->CalcCanvasRenderNodeCost(node);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
}
#endif

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
/**
 * @tc.name: CalcCanvasRenderNodeCostTest
 * @tc.desc: Test RsNodeCostManagerTest.CalcCanvasRenderNodeCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RsNodeCostManagerTest, CalcCanvasRenderNodeCostTest1, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsContext = std::make_shared<RSContext>();
    RSCanvasRenderNode node(nodeId);
    node.GetMutableRenderProperties().SetAlpha(0.0f);
    node.GetMutableRenderProperties().SetVisible(true);
    rsNodeCostManager->isOpDropped_ = false;
    rsNodeCostManager->costSurfaceNode_ =nullptr;
    node.UpdateChildrenOutOfRectFlag(false);
    rsNodeCostManager->CalcCanvasRenderNodeCost(node);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
}
#endif

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
#ifdef RS_ENABLE_EGLQUERYSURFACE
/**
 * @tc.name: CalcCanvasRenderNodeCostTest
 * @tc.desc: Test RsNodeCostManagerTest.CalcCanvasRenderNodeCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RsNodeCostManagerTest, CalcCanvasRenderNodeCostTest2, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.nodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    RSCanvasRenderNode node(nodeId);
    node.GetMutableRenderProperties().SetAlpha(2.0f);
    node.GetMutableRenderProperties().SetVisible(true);
    rsNodeCostManager->isOpDropped_ = true;
    rsNodeCostManager->costSurfaceNode_ = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsNodeCostManager->costSurfaceNode_->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    rsNodeCostManager->costSurfaceNode_->oldDirtyInSurface_ = RectI(0, 0, 100, 100);
    rsNodeCostManager->costSurfaceNode_->visibleRegion_.rects_.emplace_back(Occlusion::Rect(200, 200, 200, 200));
    rsNodeCostManager->partialRenderType_ = PartialRenderType::SET_DAMAGE_AND_DROP_OP_OCCLUSION;
    node.UpdateChildrenOutOfRectFlag(false);
    rsNodeCostManager->CalcCanvasRenderNodeCost(node);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
}
#endif
#endif

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
/**
 * @tc.name: CalcCanvasRenderNodeCostTest
 * @tc.desc: Test RsNodeCostManagerTest.CalcCanvasRenderNodeCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RsNodeCostManagerTest, CalcCanvasRenderNodeCostTest3, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsContext = std::make_shared<RSContext>();
    RSCanvasRenderNode node(nodeId);
    node.GetMutableRenderProperties().SetAlpha(2.0f);
    node.GetMutableRenderProperties().SetVisible(true);
    rsNodeCostManager->isOpDropped_ = false;
    rsNodeCostManager->costSurfaceNode_ =nullptr;
    node.UpdateChildrenOutOfRectFlag(false);
    rsNodeCostManager->CalcCanvasRenderNodeCost(node);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
}
#endif

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
/**
 * @tc.name: CalcCanvasRenderNodeCostTest
 * @tc.desc: Test RsNodeCostManagerTest.CalcCanvasRenderNodeCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RsNodeCostManagerTest, CalcCanvasRenderNodeCostTest4, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsContext = std::make_shared<RSContext>();
    RSCanvasRenderNode node(nodeId);
    node.GetMutableRenderProperties().SetAlpha(2.0f);
    node.GetMutableRenderProperties().SetVisible(true);
    rsNodeCostManager->isOpDropped_ = false;
    node.UpdateChildrenOutOfRectFlag(true);
    rsNodeCostManager->CalcCanvasRenderNodeCost(node);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
}
#endif
/**
 * @tc.name: AddNodeCostTest
 * @tc.desc: Test RsNodeCostManagerTest.AddNodeCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RsNodeCostManagerTest, AddNodeCostTest, TestSize.Level1)
{
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    rsNodeCostManager->AddNodeCost(5);
    auto result = rsNodeCostManager->dirtyNodeCost_;
    ASSERT_EQ(5, result);
}

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
/**
 * @tc.name: CalcSurfaceRenderNodeCost
 * @tc.desc: Test RsNodeCostManagerTest.CalcSurfaceRenderNodeCost
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RsNodeCostManagerTest, CalcSurfaceRenderNodeCostTest1, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(0.0f);
    rsNodeCostManager->CalcSurfaceRenderNodeCost(*rsSurfaceRenderNode);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
}
#endif

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
/**
 * @tc.name: CalcSurfaceRenderNodeCost
 * @tc.desc: Test RsNodeCostManagerTest.CalcSurfaceRenderNodeCost
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RsNodeCostManagerTest, CalcSurfaceRenderNodeCostTest2, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(2.0f);
    rsSurfaceRenderNode->GetMutableRenderProperties().visible_ = true;
    rsSurfaceRenderNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    rsNodeCostManager->isOpDropped_ = false;
    rsNodeCostManager->CalcSurfaceRenderNodeCost(*rsSurfaceRenderNode);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(1, result);
}
#endif

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
/**
 * @tc.name: CalcSurfaceRenderNodeCost
 * @tc.desc: Test RsNodeCostManagerTest.CalcSurfaceRenderNodeCost
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RsNodeCostManagerTest, CalcSurfaceRenderNodeCostTest3, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(2.0f);
    rsSurfaceRenderNode->GetMutableRenderProperties().visible_ = true;
    rsSurfaceRenderNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    rsNodeCostManager->isOpDropped_ = false;
    rsNodeCostManager->CalcSurfaceRenderNodeCost(*rsSurfaceRenderNode);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
}
#endif

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
/**
 * @tc.name: CalcSurfaceRenderNodeCost
 * @tc.desc: Test RsNodeCostManagerTest.CalcSurfaceRenderNodeCost
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RsNodeCostManagerTest, CalcSurfaceRenderNodeCostTest4, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(2.0f);
    rsSurfaceRenderNode->GetMutableRenderProperties().visible_ = true;
    rsSurfaceRenderNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    rsNodeCostManager->isOpDropped_ = false;
    rsNodeCostManager->CalcSurfaceRenderNodeCost(*rsSurfaceRenderNode);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
}
#endif

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
/**
 * @tc.name: CalcSurfaceRenderNodeCost
 * @tc.desc: Test RsNodeCostManagerTest.CalcSurfaceRenderNodeCost
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RsNodeCostManagerTest, CalcSurfaceRenderNodeCostTest5, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(2.0f);
    rsSurfaceRenderNode->GetMutableRenderProperties().visible_ = true;
    rsSurfaceRenderNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    rsNodeCostManager->isOpDropped_ = false;
    rsNodeCostManager->CalcSurfaceRenderNodeCost(*rsSurfaceRenderNode);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
}
#endif

} // namespace OHOS::Rosen