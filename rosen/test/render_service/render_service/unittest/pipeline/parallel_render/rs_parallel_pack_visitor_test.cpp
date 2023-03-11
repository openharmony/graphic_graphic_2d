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
    rsSurfaceRenderNode->SetFreeze(false);
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
HWTEST_F(RSParallelPackVisitorTest, CalcSurfaceRenderNodeCostTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    auto rsParallelPackVisitor = std::make_shared<RSParallelPackVisitor>();
    rsParallelPackVisitor->CalcSurfaceRenderNodeCost(*rsSurfaceRenderNode);
}

/**
 * @tc.name: CalcBaseRenderNodeCostTest
 * @tc.desc: Test RSParallelPackVisitorTest.CalcDisplayRenderNodeCost
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSParallelPackVisitorTest, CalcDisplayRenderNodeCost, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    auto rsParallelPackVisitor = std::make_shared<RSParallelPackVisitor>();
    rsParallelPackVisitor->CalcDisplayRenderNodeCost(*rsDisplayRenderNode);
}

/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RSParallelPackVisitorTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSParallelPackVisitorTest, IsSkipProcessingTest, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    auto rsParallelPackVisitor = std::make_shared<RSParallelPackVisitor>();
    rsParallelPackVisitor->isSecurityDisplay_ = true;
    rsSurfaceRenderNode.SetSecurityLayer(1);
    auto result = rsParallelPackVisitor->IsSkipProcessing(rsSurfaceRenderNode);
    ASSERT_TRUE(result);
}

} // namespace OHOS::Rosen