/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_log.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline RSPaintFilterCanvas* canvas_;
    static inline SkCanvas skCanvas_;
};

void RSCanvasRenderNodeTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&skCanvas_);
}
void RSCanvasRenderNodeTest::TearDownTestCase()
{
    delete canvas_;
}
void RSCanvasRenderNodeTest::SetUp() {}
void RSCanvasRenderNodeTest::TearDown() {}

/**
 * @tc.name: UpdateRecording001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, UpdateRecording001, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
    canvasRenderNode->UpdateRecording(nullptr, RSModifierType::INVALID);
}

/**
 * @tc.name: UpdateRecording002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, UpdateRecording002, TestSize.Level1)
{
    int w;
    int h;
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id + 1);
    auto drawCmds = std::make_shared<DrawCmdList>(w, h);
    canvasRenderNode->UpdateRecording(drawCmds, RSModifierType::INVALID);
}

/**
 * @tc.name: PrepareTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, PrepareTest, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.Prepare(visitor);
}

/**
 * @tc.name: ProcessTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ProcessTest, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.Process(visitor);
}

/**
 * @tc.name: ProcessAnimatePropertyAfterChildrenTest001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ProcessAnimatePropertyAfterChildrenTest001, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
    canvasRenderNode->ProcessRenderAfterChildren(*canvas_);
}

/**
 * @tc.name: ProcessDrivenBackgroundRenderTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ProcessDrivenBackgroundRenderTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.ProcessDrivenBackgroundRender(*canvas_);
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    EXPECT_EQ(rsCanvasRenderNode.GetChildrenCount(), 0);
#endif
}

} // namespace OHOS::Rosen