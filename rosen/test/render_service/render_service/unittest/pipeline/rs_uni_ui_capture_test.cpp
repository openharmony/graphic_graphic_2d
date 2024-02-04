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

#include "gtest/gtest.h"
#include "pipeline/rs_uni_ui_capture.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/round_corner_display/rs_round_corner_display.h"
#include "common/rs_singleton.h"
#include "limit_number.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUniUiCaptureTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniUiCaptureTest::SetUpTestCase() {}
void RSUniUiCaptureTest::TearDownTestCase() {}
void RSUniUiCaptureTest::SetUp() {}
void RSUniUiCaptureTest::TearDown() {}

/**
 * @tc.name: TakeLocalCapture001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, TakeLocalCapture001, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    RSUniUICapture rsUniUICapture(nodeId, scaleX, scaleY);
    EXPECT_EQ(nullptr, rsUniUICapture.TakeLocalCapture());
}

/**
 * @tc.name: SetCanvasTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, SetCanvasTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);
#ifndef USE_ROSEN_DRAWING
    std::shared_ptr<RSRecordingCanvas> canvas = nullptr;
#else
    std::shared_ptr<ExtendRecordingCanvas> canvas = nullptr;
#endif
    rsUniUICaptureVisitor.SetCanvas(canvas);
}

/**
 * @tc.name: ProcessRootRenderNodeTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessRootRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);
    RSRootRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNodeTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessCanvasRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);
    RSRootRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessSurfaceRenderNodeTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessSurfaceRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);
    RSSurfaceRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.ProcessSurfaceRenderNode(node);
}

/**
 * @tc.name: ProcessEffectRenderNode001
 * @tc.desc: Test RSUniUiCapture.ProcessEffectRenderNode api
 * @tc.type:
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessEffectRenderNode001, TestSize.Level1)
{
    NodeId id = 0;
    RSEffectRenderNode node(id);
    float scaleX = 0.0;
    float scaleY = 0.0;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(id, scaleX, scaleY);
    rsUniUICaptureVisitor.ProcessEffectRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNodeTest
 * @tc.desc: Test RSUniUiCapture.CreateSurface api
 * @tc.type:
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, PrepareCanvasRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);
    RSRootRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareSurfaceRenderNodeTest
 * @tc.desc: Test RSUniUiCapture.CreateSurface api
 * @tc.type:
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, PrepareSurfaceRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);
    RSSurfaceRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.PrepareSurfaceRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNodeTest
 * @tc.desc: Test RSUniUiCapture.CreateSurface api
 * @tc.type:
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, PrepareRootRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);
    RSRootRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareEffectRenderNodeTest
 * @tc.desc: Test RSUniUiCapture.CreateSurface api
 * @tc.type:
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, PrepareEffectRenderNodeTest, TestSize.Level1)
{
    NodeId id = 0;
    RSEffectRenderNode node(id);
    float scaleX = 0.0;
    float scaleY = 0.0;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(id, scaleX, scaleY);
    rsUniUICaptureVisitor.PrepareEffectRenderNode(node);
}

/**
 * @tc.name: CreateSurface
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, CreateSurface, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    RSUniUICapture rsUniUICapture(nodeId, scaleX, scaleY);

    std::shared_ptr<Media::PixelMap> pixelmap = nullptr;
    EXPECT_EQ(nullptr, rsUniUICapture.CreateSurface(pixelmap));

    const uint32_t color[8] = { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 };
    uint32_t colorLength = sizeof(color) / sizeof(color[0]);
    const int32_t offset = 0;
    Media::InitializationOptions opts;
    int32_t stride = 3;
    std::shared_ptr<Media::PixelMap> pixelMap1 = Media::PixelMap::Create(color, colorLength, offset, stride, opts);
    EXPECT_EQ(nullptr, rsUniUICapture.CreateSurface(pixelMap1));
}

/**
 * @tc.name: PostTaskToRSRecord
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, PostTaskToRSRecord, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    std::shared_ptr<RSRenderNode> node = nullptr;
    RSUniUICapture rsUniUICapture(nodeId, scaleX, scaleY);
    std::shared_ptr<RSUniUICapture::RSUniUICaptureVisitor> rsUniUICaptureVisitor =
        std::make_shared<RSUniUICapture::RSUniUICaptureVisitor>(nodeId, scaleX, scaleY);
    EXPECT_NE(rsUniUICaptureVisitor, nullptr);
    
#ifndef USE_ROSEN_DRAWING
    std::shared_ptr<RSRecordingCanvas> canvas = nullptr;
#else
    std::shared_ptr<ExtendRecordingCanvas> canvas = nullptr;
#endif
    rsUniUICapture.PostTaskToRSRecord(canvas, node, rsUniUICaptureVisitor);
}

/**
 * @tc.name: SetPaintFilterCanvas
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, SetPaintFilterCanvas, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;

    RSUniUICapture rsUniUICapture(nodeId, scaleX, scaleY);
    std::shared_ptr<RSUniUICapture::RSUniUICaptureVisitor> rsUniUICaptureVisitor =
        std::make_shared<RSUniUICapture::RSUniUICaptureVisitor>(nodeId, scaleX, scaleY);

#ifndef USE_ROSEN_DRAWING
    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> recordingCanvas = std::make_shared<RSPaintFilterCanvas>(skCanvas.get());
#else
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> recordingCanvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
#endif
    EXPECT_NE(recordingCanvas, nullptr);
    rsUniUICaptureVisitor->SetPaintFilterCanvas(recordingCanvas);

    std::shared_ptr<RSPaintFilterCanvas> recordingCanvas1 = nullptr;
    rsUniUICaptureVisitor->SetPaintFilterCanvas(recordingCanvas1);
}

/**
 * @tc.name: ProcessRootRenderNode
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessRootRenderNode, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);

    constexpr NodeId nodeId1 = TestSrc::limitNumber::Uint64[1];
    constexpr NodeId nodeId2 = TestSrc::limitNumber::Uint64[2];
    RSRootRenderNode node1(nodeId1);
    RSCanvasRenderNode node2(nodeId2);
    rsUniUICaptureVisitor.ProcessRootRenderNode(node1);
    rsUniUICaptureVisitor.ProcessCanvasRenderNode(node2);
}

/**
 * @tc.name: ProcessSurfaceRenderNodeWithUni
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessSurfaceRenderNodeWithUni, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;

    RSUniUICapture rsUniUICapture(nodeId, scaleX, scaleY);
    std::shared_ptr<RSUniUICapture::RSUniUICaptureVisitor> rsUniUICaptureVisitor =
        std::make_shared<RSUniUICapture::RSUniUICaptureVisitor>(nodeId, scaleX, scaleY);

#ifndef USE_ROSEN_DRAWING
    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> recordingCanvas = std::make_shared<RSPaintFilterCanvas>(skCanvas.get());
#else
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> recordingCanvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
#endif
    rsUniUICaptureVisitor->SetPaintFilterCanvas(recordingCanvas);
    
    std::weak_ptr<RSContext> context;
    RSSurfaceRenderNode node(nodeId, context);
    rsUniUICaptureVisitor->ProcessSurfaceRenderNodeWithUni(node);
}
} // namespace OHOS::Rosen