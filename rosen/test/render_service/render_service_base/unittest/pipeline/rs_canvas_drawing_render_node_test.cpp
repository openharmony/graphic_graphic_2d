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

#include "pipeline/rs_canvas_drawing_render_node.h"
#include "draw/canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasDrawingRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline RSPaintFilterCanvas* canvas_;
    static inline Drawing::Canvas drawingCanvas_;
};

void RSCanvasDrawingRenderNodeTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSCanvasDrawingRenderNodeTest::TearDownTestCase()
{
    delete canvas_;
}
void RSCanvasDrawingRenderNodeTest::SetUp() {}
void RSCanvasDrawingRenderNodeTest::TearDown() {}

/**
 * @tc.name: ProcessRenderContentsTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ProcessRenderContentsTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    rsCanvasDrawingRenderNode.ProcessRenderContents(*canvas_);
}

/**
 * @tc.name: ProcessRenderContentsTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ResetSurfaceTest, TestSize.Level1)
{
    int width = 0;
    int height = 0;
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    auto res = rsCanvasDrawingRenderNode.ResetSurface(width, height, *canvas_);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: GetSizeFromDrawCmdModifiersTest001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetSizeFromDrawCmdModifiersTest001, TestSize.Level1)
{
    int width = 1;
    int height = 1;
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    rsCanvasDrawingRenderNode.GetSizeFromDrawCmdModifiers(width, height);
}

/**
 * @tc.name: GetSizeFromDrawCmdModifiersTest002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetSizeFromDrawCmdModifiersTest002, TestSize.Level1)
{
    int width = -1;
    int height = -1;
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    auto res = rsCanvasDrawingRenderNode.GetSizeFromDrawCmdModifiers(width, height);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: IsNeedResetSurfaceTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, IsNeedResetSurfaceTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    rsCanvasDrawingRenderNode.IsNeedResetSurface();
}
} // namespace OHOS::Rosen