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

#include "gtest/gtest.h"
#include "limit_number.h"

#include "pipeline/parallel_render/rs_parallel_pack_visitor.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_visitor.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSParallelPackVisitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSParallelPackVisitorTest::SetUpTestCase() {}
void RSParallelPackVisitorTest::TearDownTestCase() {}
void RSParallelPackVisitorTest::SetUp() {}
void RSParallelPackVisitorTest::TearDown() {}

/**
 * @tc.name: ProcessPackVisitorTest
 * @tc.desc: Test RSParallelPackVisitorTest.ProcessPackVisitorTest
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */

HWTEST_F(RSParallelPackVisitorTest, ProcessPackVisitorTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto rsUniRenderPackVisitor = std::make_shared<RSParallelPackVisitor>(*rsUniRenderVisitor);

    rsSurfaceRenderNode->SetSrcRect(RectI(0, 0, 10, 10));
    rsSurfaceRenderNode->SetStaticCached(false);
    rsSurfaceRenderNode->SetSecurityLayer(true);
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);

    rsUniRenderPackVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderPackVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderPackVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);

    rsUniRenderVisitor->SetAnimateState(true);
    rsUniRenderPackVisitor = std::make_shared<RSParallelPackVisitor>(*rsUniRenderVisitor);
    rsUniRenderPackVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderPackVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderPackVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);

    auto& propertyD = rsDisplayRenderNode->GetMutableRenderProperties();
    propertyD.SetVisible(false);
    auto& propertyS = rsSurfaceRenderNode->GetMutableRenderProperties();
    propertyS.SetVisible(false);
    rsUniRenderPackVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderPackVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderPackVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);

    // SetSecurityDisplay for rsDisplayRenderNode
    rsDisplayRenderNode->SetSecurityDisplay(true);
    rsUniRenderPackVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderPackVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderPackVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);
}

/**
 * @tc.name: CalcSurfaceRenderNodeCostTest
 * @tc.desc: Test RSParallelPackVisitorTest.CalcSurfaceRenderNodeCostTest
 * @tc.type: FUNC
 * @tc.require:issueI6FZHQ
 */
HWTEST_F(RSParallelPackVisitorTest, CalcSurfaceRenderNodeCostTest1, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    auto rsParallelPackVisitor = std::make_shared<RSParallelPackVisitor>();
    rsParallelPackVisitor->isSecurityDisplay_ = true;
    rsSurfaceRenderNode->SetSecurityLayer(true);
    rsParallelPackVisitor->CalcSurfaceRenderNodeCost(*rsSurfaceRenderNode);
    auto result = rsParallelPackVisitor->isOpDropped_;
    ASSERT_FALSE(result);
}

/**
 * @tc.name: CalcSurfaceRenderNodeCostTest
 * @tc.desc: Test RSParallelPackVisitorTest.CalcSurfaceRenderNodeCostTest
 * @tc.type: FUNC
 * @tc.require:issueI6FZHQ
 */
HWTEST_F(RSParallelPackVisitorTest, CalcSurfaceRenderNodeCostTest2, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    auto rsParallelPackVisitor = std::make_shared<RSParallelPackVisitor>();
    rsSurfaceRenderNode->SetSecurityLayer(false);
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(2.0f);
    rsSurfaceRenderNode->GetMutableRenderProperties().SetVisible(true);
    rsSurfaceRenderNode->SetOcclusionVisible(true);
    rsParallelPackVisitor->CalcSurfaceRenderNodeCost(*rsSurfaceRenderNode);
    ASSERT_TRUE(rsSurfaceRenderNode->ShouldPaint());
    ASSERT_TRUE(rsSurfaceRenderNode->GetOcclusionVisible());
    auto result = rsParallelPackVisitor->isOpDropped_;
    ASSERT_FALSE(result);
}

/**
 * @tc.name: CalcBaseRenderNodeCostTest
 * @tc.desc: Test RSParallelPackVisitorTest.CalcDisplayRenderNodeCost
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSParallelPackVisitorTest, CalcDisplayRenderNodeCost1, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    auto rsParallelPackVisitor = std::make_shared<RSParallelPackVisitor>();
    rsParallelPackVisitor->CalcDisplayRenderNodeCost(*rsDisplayRenderNode);
    auto result = rsParallelPackVisitor->isOpDropped_;
    ASSERT_FALSE(result);
}

/**
 * @tc.name: CalcBaseRenderNodeCostTest
 * @tc.desc: Test RSParallelPackVisitorTest.CalcDisplayRenderNodeCost
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RSParallelPackVisitorTest, CalcDisplayRenderNodeCost2, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    auto rsParallelPackVisitor = std::make_shared<RSParallelPackVisitor>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode);
    rsParallelPackVisitor->CalcDisplayRenderNodeCost(*rsDisplayRenderNode);
    auto result = rsParallelPackVisitor->isOpDropped_;
    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RSParallelPackVisitorTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSParallelPackVisitorTest, IsSkipProcessingTest1, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    auto rsParallelPackVisitor = std::make_shared<RSParallelPackVisitor>();
    rsParallelPackVisitor->isSecurityDisplay_ = true;
    rsSurfaceRenderNode.SetSecurityLayer(1);
    ASSERT_TRUE(rsParallelPackVisitor->isSecurityDisplay_ && rsSurfaceRenderNode.GetSecurityLayer());
    auto result = rsParallelPackVisitor->IsSkipProcessing(rsSurfaceRenderNode);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RSParallelPackVisitorTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RSParallelPackVisitorTest, IsSkipProcessingTest2, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    auto rsParallelPackVisitor = std::make_shared<RSParallelPackVisitor>();
    ASSERT_FALSE(rsParallelPackVisitor->isSecurityDisplay_ && rsSurfaceRenderNode.GetSecurityLayer());
    rsSurfaceRenderNode.GetMutableRenderProperties().SetAlpha(0.0f);
    rsSurfaceRenderNode.shouldPaint_ = false;
    auto result = rsParallelPackVisitor->IsSkipProcessing(rsSurfaceRenderNode);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RSParallelPackVisitorTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RSParallelPackVisitorTest, IsSkipProcessingTest3, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    auto rsParallelPackVisitor = std::make_shared<RSParallelPackVisitor>();
    ASSERT_FALSE(rsParallelPackVisitor->isSecurityDisplay_ && rsSurfaceRenderNode.GetSecurityLayer());
    ASSERT_FALSE(!rsSurfaceRenderNode.ShouldPaint());
    rsSurfaceRenderNode.SetOcclusionVisible(false);
    rsParallelPackVisitor->doAnimate_ = false;
    rsParallelPackVisitor->isSecurityDisplay_ = false;
    ASSERT_TRUE(!rsSurfaceRenderNode.GetOcclusionVisible() && !rsParallelPackVisitor->doAnimate_ &&
                RSSystemProperties::GetOcclusionEnabled() && !rsParallelPackVisitor->isSecurityDisplay_);
    auto result = rsParallelPackVisitor->IsSkipProcessing(rsSurfaceRenderNode);
    if (RSSystemProperties::GetOcclusionEnabled()) {
        ASSERT_TRUE(result);
    }
}
#ifdef RS_ENABLE_EGLQUERYSURFACE
/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RSParallelPackVisitorTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RSParallelPackVisitorTest, IsSkipProcessingTest4, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    config.nodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    auto rsParallelPackVisitor = std::make_shared<RSParallelPackVisitor>();
    rsParallelPackVisitor->isOpDropped_ = true;
    rsSurfaceRenderNode.oldDirtyInSurface_ = RectI(0, 0, 100, 100);
    rsSurfaceRenderNode.visibleRegion_.rects_.emplace_back(Occlusion::Rect(200, 200, 200, 200));
    rsParallelPackVisitor->partialRenderType_ = PartialRenderType::SET_DAMAGE_AND_DROP_OP_OCCLUSION;
    rsSurfaceRenderNode.dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    rsSurfaceRenderNode.globalDirtyRegionIsEmpty_ = true;
    ASSERT_FALSE(rsParallelPackVisitor->isSecurityDisplay_ && rsSurfaceRenderNode.GetSecurityLayer());
    ASSERT_FALSE(!rsSurfaceRenderNode.ShouldPaint());
    ASSERT_FALSE(!rsSurfaceRenderNode.GetOcclusionVisible() && !rsParallelPackVisitor->doAnimate_ &&
                 RSSystemProperties::GetOcclusionEnabled() && !rsParallelPackVisitor->isSecurityDisplay_);
    auto result = rsParallelPackVisitor->IsSkipProcessing(rsSurfaceRenderNode);
    ASSERT_TRUE(result);
}
#endif

/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RSParallelPackVisitorTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RSParallelPackVisitorTest, IsSkipProcessingTest5, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    config.nodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    auto rsParallelPackVisitor = std::make_shared<RSParallelPackVisitor>();
    ASSERT_FALSE(rsParallelPackVisitor->isSecurityDisplay_ && rsSurfaceRenderNode.GetSecurityLayer());
    ASSERT_FALSE(!rsSurfaceRenderNode.ShouldPaint());
    ASSERT_FALSE(!rsSurfaceRenderNode.GetOcclusionVisible() && !rsParallelPackVisitor->doAnimate_ &&
                 RSSystemProperties::GetOcclusionEnabled() && !rsParallelPackVisitor->isSecurityDisplay_);
    auto result = rsParallelPackVisitor->IsSkipProcessing(rsSurfaceRenderNode);
    ASSERT_FALSE(result);
}
} // namespace OHOS::Rosen