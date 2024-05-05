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

#include "pipeline/rs_divided_ui_capture.h"
#include "pipeline/rs_root_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSDividedUICaptureTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDividedUICaptureTest::SetUpTestCase() {}
void RSDividedUICaptureTest::TearDownTestCase() {}
void RSDividedUICaptureTest::SetUp() {}
void RSDividedUICaptureTest::TearDown() {}

/**
 * @tc.name: TakeLocalCapture
 * @tc.desc: test results of RegisterNode
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, TakeLocalCapture, TestSize.Level1)
{
    RSDividedUICapture rsDividedUICapture(1, 1.0, 1.0);
    auto pixelmap = rsDividedUICapture.TakeLocalCapture();

    RSDividedUICapture rsDividedUICapture2(1, 0, 1.0);
    pixelmap = rsDividedUICapture2.TakeLocalCapture();
    ASSERT_EQ(nullptr, pixelmap);

    RSDividedUICapture rsDividedUICapture3(1, 1.0, 0);
    pixelmap = rsDividedUICapture3.TakeLocalCapture();
    ASSERT_EQ(nullptr, pixelmap);

    RSDividedUICapture rsDividedUICapture4(1, -1, 1);
    pixelmap = rsDividedUICapture4.TakeLocalCapture();
    ASSERT_EQ(nullptr, pixelmap);

    RSDividedUICapture rsDividedUICapture5(1, 1.0, -1);
    pixelmap = rsDividedUICapture5.TakeLocalCapture();
    ASSERT_EQ(nullptr, pixelmap);
}

/**
 * @tc.name: CreatePixelMapByNode
 * @tc.desc: test results of CreatePixelMapByNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, CreatePixelMapByNode, TestSize.Level1)
{
    RSDividedUICapture rsDividedUICapture(1, 1.0, 1.0);
    NodeId id = 1;
    auto node = std::make_shared<RSRenderNode>(id);
    auto pixelmap = rsDividedUICapture.CreatePixelMapByNode(node);
    EXPECT_EQ(pixelmap, nullptr);
}

/**
 * @tc.name: CreateSurface
 * @tc.desc: test results of CreateSurface
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, CreateSurface, TestSize.Level1)
{
    RSDividedUICapture rsDividedUICapture(1, 1.0, 1.0);
    std::shared_ptr<Media::PixelMap> pixelmap;
    auto surface = rsDividedUICapture.CreateSurface(pixelmap);
    EXPECT_EQ(surface, nullptr);

    pixelmap = std::make_shared<Media::PixelMap>();
    surface = rsDividedUICapture.CreateSurface(pixelmap);
    EXPECT_EQ(surface, nullptr);
}

/**
 * @tc.name: SetCanvas
 * @tc.desc: test results of SetCanvas
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, SetCanvas, TestSize.Level1)
{
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(1, 1.0, 1.0);
    std::shared_ptr<ExtendRecordingCanvas> canvas;
    rsDividedUICaptureVisitor.SetCanvas(canvas);
    EXPECT_TRUE(true);

    int width = 1;
    int weight = 1;
    canvas = std::make_shared<ExtendRecordingCanvas>(width, weight);
    rsDividedUICaptureVisitor.SetCanvas(canvas);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetPaintFilterCanvas
 * @tc.desc: test results of SetPaintFilterCanvas
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, SetPaintFilterCanvas, TestSize.Level1)
{
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(1, 1.0, 1.0);
    std::shared_ptr<RSPaintFilterCanvas> filterCanvas;
    Drawing::Canvas canvas;
    rsDividedUICaptureVisitor.SetPaintFilterCanvas(filterCanvas);
    EXPECT_TRUE(true);

    filterCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    rsDividedUICaptureVisitor.SetPaintFilterCanvas(filterCanvas);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: PostTaskToRTRecord
 * @tc.desc: test results of PostTaskToRTRecord
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, PostTaskToRTRecord, TestSize.Level1)
{
    RSDividedUICapture rsDividedUICapture(1, 1.0, 1.0);
    int width = 1;
    int weight = 1;
    auto canvas = std::make_shared<ExtendRecordingCanvas>(width, weight);
    NodeId id = 1;
    auto node = std::make_shared<RSRenderNode>(id);
    auto visitor = std::make_shared<RSDividedUICapture::RSDividedUICaptureVisitor>(1, 1.0, 1.0);
    rsDividedUICapture.PostTaskToRTRecord(canvas, node, visitor);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: ProcessChildren
 * @tc.desc: test results of ProcessChildren
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, ProcessChildren, TestSize.Level1)
{
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(1, 1.0, 1.0);
    RSRenderNode node(1);
    rsDividedUICaptureVisitor.ProcessChildren(node);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: ProcessRootRenderNode
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, ProcessRootRenderNode, TestSize.Level1)
{
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(1, 1.0, 1.0);
    RSRenderNode rsRenderNode(1);
    RSRootRenderNode node(1);
    RSProperties rsProperties;
    Drawing::Canvas canvas;
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    rsDividedUICaptureVisitor.ProcessRootRenderNode(node);
    EXPECT_TRUE(true);

    rsProperties.SetAlpha(0.0f);
    rsRenderNode.UpdateShouldPaint();
    rsDividedUICaptureVisitor.ProcessRootRenderNode(node);
    EXPECT_TRUE(true);

    rsDividedUICaptureVisitor.SetPaintFilterCanvas(filterCanvas);
    rsDividedUICaptureVisitor.ProcessRootRenderNode(node);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: ProcessCanvasRenderNode
 * @tc.desc: test results of ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, ProcessCanvasRenderNode, TestSize.Level1)
{
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(1, 1.0, 1.0);
    RSRenderNode rsRenderNode(1);
    RSCanvasRenderNode node(1);
    RSProperties rsProperties;
    Drawing::Canvas canvas;
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    rsDividedUICaptureVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(true);

    rsProperties.SetAlpha(0.0f);
    rsRenderNode.UpdateShouldPaint();
    rsDividedUICaptureVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(true);

    rsDividedUICaptureVisitor.SetPaintFilterCanvas(filterCanvas);
    rsDividedUICaptureVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: ProcessEffectRenderNode
 * @tc.desc: test results of ProcessEffectRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, ProcessEffectRenderNode, TestSize.Level1)
{
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(1, 1.0, 1.0);
    RSRenderNode rsRenderNode(1);
    RSEffectRenderNode node(1);
    RSProperties rsProperties;
    Drawing::Canvas canvas;
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    rsDividedUICaptureVisitor.ProcessEffectRenderNode(node);
    EXPECT_TRUE(true);

    rsProperties.SetAlpha(0.0f);
    rsRenderNode.UpdateShouldPaint();
    rsDividedUICaptureVisitor.ProcessEffectRenderNode(node);
    EXPECT_TRUE(true);

    rsDividedUICaptureVisitor.SetPaintFilterCanvas(filterCanvas);
    rsDividedUICaptureVisitor.ProcessEffectRenderNode(node);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: ProcessSurfaceRenderNode
 * @tc.desc: test results of ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, ProcessSurfaceRenderNode, TestSize.Level1)
{
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(1, 1.0, 1.0);
    RSSurfaceRenderNode node(1);
    RSProperties rsProperties;
    Drawing::Canvas canvas;
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    rsDividedUICaptureVisitor.ProcessSurfaceRenderNode(node);
    EXPECT_TRUE(true);

    rsDividedUICaptureVisitor.SetPaintFilterCanvas(filterCanvas);
    rsProperties.SetVisible(false);
    rsDividedUICaptureVisitor.ProcessSurfaceRenderNode(node);
    EXPECT_TRUE(true);

    rsProperties.SetVisible(true);
    rsDividedUICaptureVisitor.ProcessSurfaceRenderNode(node);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: PrepareChildren
 * @tc.desc: test results of PrepareChildren
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, PrepareChildren, TestSize.Level1)
{
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(1, 1.0, 1.0);
    RSRenderNode node(1);
    rsDividedUICaptureVisitor.PrepareChildren(node);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: PrepareCanvasRenderNode
 * @tc.desc: test results of PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, PrepareCanvasRenderNode, TestSize.Level1)
{
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(1, 1.0, 1.0);
    RSCanvasRenderNode node(1);
    rsDividedUICaptureVisitor.PrepareCanvasRenderNode(node);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: PrepareSurfaceRenderNode
 * @tc.desc: test results of PrepareSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, PrepareSurfaceRenderNode, TestSize.Level1)
{
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(1, 1.0, 1.0);
    RSSurfaceRenderNode node(1);
    rsDividedUICaptureVisitor.PrepareSurfaceRenderNode(node);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: PrepareRootRenderNode
 * @tc.desc: test results of PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, PrepareRootRenderNode, TestSize.Level1)
{
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(1, 1.0, 1.0);
    RSRootRenderNode node(1);
    rsDividedUICaptureVisitor.PrepareRootRenderNode(node);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: PrepareEffectRenderNode
 * @tc.desc: test results of PrepareEffectRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, PrepareEffectRenderNode, TestSize.Level1)
{
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(1, 1.0, 1.0);
    RSEffectRenderNode node(1);
    rsDividedUICaptureVisitor.PrepareEffectRenderNode(node);
    EXPECT_TRUE(true);
}
} // namespace Rosen
} // namespace OHOS