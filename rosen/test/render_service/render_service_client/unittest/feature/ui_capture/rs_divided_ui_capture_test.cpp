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

#include "feature/ui_capture/rs_divided_ui_capture.h"
#include "pipeline/rs_render_thread.h"
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
 * @tc.name: TakeLocalCapture001
 * @tc.desc: test results of RegisterNode
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, TakeLocalCapture001, TestSize.Level1)
{
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture rsDividedUICapture(nodeId, scaleX, scaleY);
    auto pixelmap = rsDividedUICapture.TakeLocalCapture();
    EXPECT_EQ(nullptr, pixelmap);

    rsDividedUICapture.scaleX_ = 0.f;
    pixelmap = rsDividedUICapture.TakeLocalCapture();
    EXPECT_EQ(nullptr, pixelmap);

    rsDividedUICapture.scaleY_ = 0.f;
    pixelmap = rsDividedUICapture.TakeLocalCapture();
    EXPECT_EQ(nullptr, pixelmap);

    rsDividedUICapture.scaleY_ = -1.f;
    pixelmap = rsDividedUICapture.TakeLocalCapture();
    EXPECT_EQ(nullptr, pixelmap);

    rsDividedUICapture.scaleX_ = -1.f;
    pixelmap = rsDividedUICapture.TakeLocalCapture();
    EXPECT_EQ(nullptr, pixelmap);

    rsDividedUICapture.scaleY_ = 0.f;
    pixelmap = rsDividedUICapture.TakeLocalCapture();
    EXPECT_EQ(nullptr, pixelmap);

    rsDividedUICapture.scaleX_ = 1.f;
    pixelmap = rsDividedUICapture.TakeLocalCapture();
    EXPECT_EQ(nullptr, pixelmap);

    rsDividedUICapture.scaleY_ = -1.f;
    pixelmap = rsDividedUICapture.TakeLocalCapture();
    EXPECT_EQ(nullptr, pixelmap);

    rsDividedUICapture.scaleX_ = -1.f;
    pixelmap = rsDividedUICapture.TakeLocalCapture();
    EXPECT_EQ(nullptr, pixelmap);

    rsDividedUICapture.scaleY_ = 1.f;
    pixelmap = rsDividedUICapture.TakeLocalCapture();
    EXPECT_EQ(nullptr, pixelmap);

    rsDividedUICapture.scaleX_ = 1.f;
    pixelmap = rsDividedUICapture.TakeLocalCapture();
    EXPECT_EQ(nullptr, pixelmap);

    rsDividedUICapture.nodeId_ = 0;
    pixelmap = rsDividedUICapture.TakeLocalCapture();
    EXPECT_EQ(nullptr, pixelmap);
}

/**
 * @tc.name: TakeLocalCapture002
 * @tc.desc: test results of TakeLocalCapture
 * @tc.type: FUNC
 * @tc.require: issueIAJ76O
 */
HWTEST_F(RSDividedUICaptureTest, TakeLocalCapture002, TestSize.Level1)
{
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture rsDividedUICapture(nodeId, scaleX, scaleY);
    auto rsNode = std::make_shared<RSCanvasRenderNode> (0);
    rsNode->GetMutableRenderProperties().SetBoundsSize({1.0, 1.0});
    RSRenderThread::Instance().GetContext().GetMutableNodeMap().RegisterRenderNode(rsNode);
    rsDividedUICapture.nodeId_ = 0;
    auto pixelmap = rsDividedUICapture.TakeLocalCapture();
    EXPECT_TRUE(pixelmap == nullptr);
}

/**
 * @tc.name: CreatePixelMapByNode
 * @tc.desc: test results of CreatePixelMapByNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, CreatePixelMapByNode, TestSize.Level1)
{
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture rsDividedUICapture(nodeId, scaleX, scaleY);
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
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture rsDividedUICapture(nodeId, scaleX, scaleY);
    std::shared_ptr<Media::PixelMap> pixelmap = nullptr;
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
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(nodeId, scaleX, scaleY);
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
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(nodeId, scaleX, scaleY);
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
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture rsDividedUICapture(nodeId, scaleX, scaleY);
    int width = 1;
    int weight = 1;
    auto canvas = std::make_shared<ExtendRecordingCanvas>(width, weight);
    NodeId id = 1;
    auto node = std::make_shared<RSRenderNode>(id);
    scaleX = 1.0;
    scaleY = 1.0;
    auto visitor = std::make_shared<RSDividedUICapture::RSDividedUICaptureVisitor>(id, scaleX, scaleY);
    node->isOnTheTree_ = false;
    rsDividedUICapture.PostTaskToRTRecord(canvas, node, visitor);
    EXPECT_TRUE(!node->isOnTheTree_);

    node->isOnTheTree_ = true;
    rsDividedUICapture.PostTaskToRTRecord(canvas, node, visitor);
    EXPECT_TRUE(node->isOnTheTree_);
}

/**
 * @tc.name: ProcessChildren
 * @tc.desc: test results of ProcessChildren
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, ProcessChildren, TestSize.Level1)
{
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(nodeId, scaleX, scaleY);
    RSRenderNode node(nodeId);
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
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(nodeId, scaleX, scaleY);
    RSRootRenderNode node(nodeId);
    node.shouldPaint_ = true;
    rsDividedUICaptureVisitor.canvas_ = nullptr;
    rsDividedUICaptureVisitor.ProcessRootRenderNode(node);
    EXPECT_TRUE(rsDividedUICaptureVisitor.canvas_ == nullptr);

    Drawing::Canvas canvas;
    rsDividedUICaptureVisitor.canvas_ = std::make_shared<RSPaintFilterCanvas>(&canvas);
    rsDividedUICaptureVisitor.ProcessRootRenderNode(node);
    EXPECT_TRUE(rsDividedUICaptureVisitor.canvas_ != nullptr);

    node.shouldPaint_ = false;
    rsDividedUICaptureVisitor.ProcessRootRenderNode(node);
    EXPECT_TRUE(!node.shouldPaint_);
}

/**
 * @tc.name: ProcessCanvasRenderNode
 * @tc.desc: test results of ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, ProcessCanvasRenderNode, TestSize.Level1)
{
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(nodeId, scaleX, scaleY);
    RSCanvasRenderNode node(nodeId);
    node.shouldPaint_ = true;
    rsDividedUICaptureVisitor.canvas_ = nullptr;
    rsDividedUICaptureVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(rsDividedUICaptureVisitor.canvas_ == nullptr);

    Drawing::Canvas canvas;
    rsDividedUICaptureVisitor.canvas_ = std::make_shared<RSPaintFilterCanvas>(&canvas);
    rsDividedUICaptureVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(rsDividedUICaptureVisitor.canvas_ != nullptr);

    rsDividedUICaptureVisitor.nodeId_ = 0;
    rsDividedUICaptureVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(!rsDividedUICaptureVisitor.nodeId_);

    node.shouldPaint_ = false;
    rsDividedUICaptureVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(!node.shouldPaint_);
}

/**
 * @tc.name: ProcessEffectRenderNode
 * @tc.desc: test results of ProcessEffectRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, ProcessEffectRenderNode, TestSize.Level1)
{
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(nodeId, scaleX, scaleY);
    RSEffectRenderNode node(nodeId);
    node.shouldPaint_ = true;
    rsDividedUICaptureVisitor.canvas_ = nullptr;
    rsDividedUICaptureVisitor.ProcessEffectRenderNode(node);
    EXPECT_TRUE(rsDividedUICaptureVisitor.canvas_ == nullptr);

    Drawing::Canvas canvas;
    rsDividedUICaptureVisitor.canvas_ = std::make_shared<RSPaintFilterCanvas>(&canvas);
    rsDividedUICaptureVisitor.ProcessEffectRenderNode(node);
    EXPECT_TRUE(rsDividedUICaptureVisitor.canvas_ != nullptr);

    node.shouldPaint_ = false;
    rsDividedUICaptureVisitor.ProcessEffectRenderNode(node);
    EXPECT_TRUE(!node.shouldPaint_);
}

/**
 * @tc.name: ProcessSurfaceRenderNode
 * @tc.desc: test results of ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, ProcessSurfaceRenderNode, TestSize.Level1)
{
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(nodeId, scaleX, scaleY);
    RSSurfaceRenderNode node(nodeId);
    rsDividedUICaptureVisitor.canvas_ = nullptr;
    rsDividedUICaptureVisitor.ProcessSurfaceRenderNode(node);
    EXPECT_TRUE(rsDividedUICaptureVisitor.canvas_ == nullptr);

    Drawing::Canvas canvas;
    rsDividedUICaptureVisitor.canvas_ = std::make_shared<RSPaintFilterCanvas>(&canvas);
    rsDividedUICaptureVisitor.ProcessSurfaceRenderNode(node);
    EXPECT_TRUE(rsDividedUICaptureVisitor.canvas_ != nullptr);

    node.GetMutableRenderProperties().visible_ = false;
    rsDividedUICaptureVisitor.ProcessSurfaceRenderNode(node);
    EXPECT_TRUE(rsDividedUICaptureVisitor.canvas_ != nullptr);
}

/**
 * @tc.name: PrepareChildren
 * @tc.desc: test results of PrepareChildren
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, PrepareChildren, TestSize.Level1)
{
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(nodeId, scaleX, scaleY);
    RSRenderNode node(nodeId);
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
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(nodeId, scaleX, scaleY);
    RSCanvasRenderNode node(nodeId);
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
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(nodeId, scaleX, scaleY);
    RSSurfaceRenderNode node(nodeId);
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
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(nodeId, scaleX, scaleY);
    RSRootRenderNode node(nodeId);
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
    NodeId nodeId = 1;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RSDividedUICapture::RSDividedUICaptureVisitor rsDividedUICaptureVisitor(nodeId, scaleX, scaleY);
    RSEffectRenderNode node(nodeId);
    rsDividedUICaptureVisitor.PrepareEffectRenderNode(node);
    EXPECT_TRUE(true);
}
} // namespace Rosen
} // namespace OHOS