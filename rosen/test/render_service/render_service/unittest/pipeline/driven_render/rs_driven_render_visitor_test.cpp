/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <memory>

#include "gtest/gtest.h"
#include "limit_number.h"

#include "pipeline/driven_render/rs_driven_render_visitor.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDrivenRenderVisitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDrivenRenderVisitorTest::SetUpTestCase() {}
void RSDrivenRenderVisitorTest::TearDownTestCase() {}
void RSDrivenRenderVisitorTest::SetUp() {}
void RSDrivenRenderVisitorTest::TearDown() {}

/**
 * @tc.name: ProcessBaseRenderNode
 * @tc.desc: Test RSDrivenRenderVisitorTest.ProcessBaseRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI6J4IL
 */
HWTEST_F(RSDrivenRenderVisitorTest, ProcessBaseRenderNode, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsDrivenRenderVisitor = std::make_shared<RSDrivenRenderVisitor>();
    // content node id = 1
    auto drivenContentNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    // background node id = 2
    auto drivenBackgroundNode = std::make_shared<RSCanvasRenderNode>(2, rsContext->weak_from_this());
    std::shared_ptr<RSDrivenSurfaceRenderNode> contentSurfaceNode =
        std::make_shared<RSDrivenSurfaceRenderNode>(0, DrivenSurfaceType::CONTENT);
    std::shared_ptr<RSDrivenSurfaceRenderNode> backgroundSurfaceNode =
        std::make_shared<RSDrivenSurfaceRenderNode>(0, DrivenSurfaceType::BACKGROUND);

    drivenContentNode->SetIsMarkDrivenRender(true);
    drivenContentNode->SetIsMarkDriven(true);
    contentSurfaceNode->SetDrivenCanvasNode(drivenContentNode);
    rsDrivenRenderVisitor->PrepareDrivenSurfaceRenderNode(*contentSurfaceNode);

    // firstItemNode id = 3
    auto firstItemNode = std::make_shared<RSCanvasRenderNode>(3, rsContext->weak_from_this());
    firstItemNode->SetItemIndex(0);
    drivenContentNode->AddChild(firstItemNode, -1);
    // secondItemNode id = 4
    auto secondItemNode = std::make_shared<RSCanvasRenderNode>(4, rsContext->weak_from_this());
    secondItemNode->SetItemIndex(1);
    drivenContentNode->AddChild(secondItemNode, -1);
    drivenBackgroundNode->AddChild(drivenContentNode, -1);
    rsDrivenRenderVisitor->ProcessBaseRenderNode(*drivenBackgroundNode);
}

} // namespace OHOS::Rosen