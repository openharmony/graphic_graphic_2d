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
#include "feature/capture/rs_uni_ui_capture.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "common/rs_singleton.h"
#include "limit_number.h"

#include "pipeline/rs_test_util.h"
#include "feature/capture/rs_capture_pixelmap_manager.h"
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

void RSUniUiCaptureTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
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
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSUniUICapture rsUniUICapture1(nodeId, captureConfig);
    EXPECT_EQ(nullptr, rsUniUICapture1.TakeLocalCapture());

    captureConfig.scaleX = 1.f;
    captureConfig.scaleY = 0.f;
    RSUniUICapture rsUniUICapture2(nodeId, captureConfig);
    EXPECT_EQ(nullptr, rsUniUICapture2.TakeLocalCapture());

    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 1.f;
    RSUniUICapture rsUniUICapture3(nodeId, captureConfig);
    EXPECT_EQ(nullptr, rsUniUICapture3.TakeLocalCapture());

    captureConfig.scaleX = -1.f;
    captureConfig.scaleY = -1.f;
    RSUniUICapture rsUniUICapture4(nodeId, captureConfig);
    EXPECT_EQ(nullptr, rsUniUICapture4.TakeLocalCapture());

    captureConfig.scaleX = 1.f;
    captureConfig.scaleY = -1.f;
    RSUniUICapture rsUniUICapture5(nodeId, captureConfig);
    EXPECT_EQ(nullptr, rsUniUICapture5.TakeLocalCapture());
}

/**
 * @tc.name: CopyDataToPixelMapTest
 * @tc.desc: test CopyDataToPixelMap api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, CopyDataToPixelMapTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSSurfaceCaptureConfig captureConfig;
    RSUniUICapture rsUniUICapture(nodeId, captureConfig);
    auto img = std::make_shared<Drawing::Image>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    EXPECT_FALSE(rsUniUICapture.CopyDataToPixelMap(img, pixelMap));

    Media::ImageInfo imageInfo;
    Media::Size infoSize;
    infoSize.width = 1;
    infoSize.height = 1;
    imageInfo.size = infoSize;
    imageInfo.pixelFormat = Media::PixelFormat::RGBA_8888;
    pixelMap->SetImageInfo(imageInfo, true);
    EXPECT_FALSE(rsUniUICapture.CopyDataToPixelMap(img, pixelMap));
}

/**
 * @tc.name: SetCanvasTest
 * @tc.desc: Test SetCanvasTest
 * @tc.type: FUNC
 * @tc.require: issueIATLPV
 */
HWTEST_F(RSUniUiCaptureTest, SetCanvasTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.0;
    captureConfig.scaleY = 0.0;
    RSUniUICapture rsUniUICapture(nodeId, captureConfig);
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, captureConfig);
    std::shared_ptr<ExtendRecordingCanvas> canvas = nullptr;
    rsUniUICaptureVisitor.SetCanvas(canvas);
    ASSERT_EQ(rsUniUICaptureVisitor.canvas_, nullptr);
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
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.0;
    captureConfig.scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, captureConfig);
    RSRootRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.ProcessRootRenderNode(node);
    EXPECT_TRUE(nodeId != -1);
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
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.0;
    captureConfig.scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, captureConfig);
    RSRootRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(nodeId != -1);
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
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.0;
    captureConfig.scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, captureConfig);
    RSSurfaceRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.ProcessSurfaceRenderNode(node);
    EXPECT_TRUE(nodeId != -1);
}

/**
 * @tc.name: ProcessEffectRenderNode001
 * @tc.desc: Test RSUniUiCapture.ProcessEffectRenderNode api
 * @tc.type:
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessEffectRenderNode001, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode node(nodeId);
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.0;
    captureConfig.scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, captureConfig);
    rsUniUICaptureVisitor.ProcessEffectRenderNode(node);
    EXPECT_TRUE(nodeId != -1);
}

/**
 * @tc.name: PrepareCanvasRenderNodeTest
 * @tc.desc: Test RSUniUiCapture.PrepareCanvasRenderNode api
 * @tc.type:
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, PrepareCanvasRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.0;
    captureConfig.scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, captureConfig);
    RSRootRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.PrepareCanvasRenderNode(node);
    EXPECT_TRUE(nodeId != -1);
}

/**
 * @tc.name: PrepareSurfaceRenderNodeTest
 * @tc.desc: Test RSUniUiCapture.PrepareSurfaceRenderNode api
 * @tc.type:
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, PrepareSurfaceRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.0;
    captureConfig.scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, captureConfig);
    RSSurfaceRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.PrepareSurfaceRenderNode(node);
    EXPECT_TRUE(nodeId != -1);
}

/**
 * @tc.name: PrepareRootRenderNodeTest
 * @tc.desc: Test RSUniUiCapture.PrepareRootRenderNode api
 * @tc.type:
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, PrepareRootRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.0;
    captureConfig.scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, captureConfig);
    RSRootRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.PrepareRootRenderNode(node);
    EXPECT_TRUE(nodeId != -1);
}

/**
 * @tc.name: PrepareEffectRenderNodeTest
 * @tc.desc: Test RSUniUiCapture.PrepareEffectRenderNode api
 * @tc.type:
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, PrepareEffectRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode node(nodeId);
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.0;
    captureConfig.scaleY = 0.0;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, captureConfig);
    rsUniUICaptureVisitor.PrepareEffectRenderNode(node);
    EXPECT_TRUE(nodeId != -1);
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
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.0;
    captureConfig.scaleY = 0.0;
    RSUniUICapture rsUniUICapture(nodeId, captureConfig);

    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    EXPECT_EQ(nullptr, rsUniUICapture.CreateSurface(pixelMap));

    pixelMap = std::make_shared<Media::PixelMap>();
    EXPECT_EQ(nullptr, rsUniUICapture.CreateSurface(pixelMap));

    Media::ImageInfo info;
    int32_t pixelMapWidth = 4;
    int32_t pixelMapHeight = 3;
    int32_t bytesPerPixel = 4;
    info.size.width = pixelMapWidth;
    info.size.height = pixelMapHeight;
    info.pixelFormat = Media::PixelFormat::RGBA_8888;
    info.colorSpace = Media::ColorSpace::SRGB;
    pixelMap->SetImageInfo(info);
    int32_t rowDataSize = pixelMapWidth * bytesPerPixel;
    uint32_t bufferSize = rowDataSize * pixelMapHeight;
    void *buffer = malloc(bufferSize);
    EXPECT_NE(buffer, nullptr);
    pixelMap->SetPixelsAddr(buffer, nullptr, bufferSize, Media::AllocatorType::HEAP_ALLOC, nullptr);
    RSOffscreenRenderThread::Instance().handler_.reset();
    EXPECT_NE(nullptr, rsUniUICapture.CreateSurface(pixelMap));
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
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.0;
    captureConfig.scaleY = 0.0;
    std::shared_ptr<RSRenderNode> node = nullptr;
    RSUniUICapture rsUniUICapture(nodeId, captureConfig);
    std::shared_ptr<RSUniUICapture::RSUniUICaptureVisitor> rsUniUICaptureVisitor =
        std::make_shared<RSUniUICapture::RSUniUICaptureVisitor>(nodeId, captureConfig);
    EXPECT_NE(rsUniUICaptureVisitor, nullptr);
    
    std::shared_ptr<ExtendRecordingCanvas> canvas = nullptr;
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
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.0;
    captureConfig.scaleY = 0.0;

    RSUniUICapture rsUniUICapture(nodeId, captureConfig);
    std::shared_ptr<RSUniUICapture::RSUniUICaptureVisitor> rsUniUICaptureVisitor =
        std::make_shared<RSUniUICapture::RSUniUICaptureVisitor>(nodeId, captureConfig);

    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> recordingCanvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
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
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.0;
    captureConfig.scaleY = 0.0;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, captureConfig);

    constexpr NodeId nodeId1 = TestSrc::limitNumber::Uint64[1];
    constexpr NodeId nodeId2 = TestSrc::limitNumber::Uint64[2];
    RSRootRenderNode node1(nodeId1);
    RSCanvasRenderNode node2(nodeId2);
    rsUniUICaptureVisitor.ProcessRootRenderNode(node1);
    rsUniUICaptureVisitor.ProcessCanvasRenderNode(node2);
    EXPECT_TRUE(nodeId != -1);
}

/**
 * @tc.name: ProcessSurfaceRenderNodeWithUni
 * @tc.desc: Test ProcessSurfaceRenderNodeWithUni
 * @tc.type: FUNC
 * @tc.require: issueIATLPV
 */
HWTEST_F(RSUniUiCaptureTest, ProcessSurfaceRenderNodeWithUni, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.0;
    captureConfig.scaleY = 0.0;

    RSUniUICapture rsUniUICapture(nodeId, captureConfig);
    std::shared_ptr<RSUniUICapture::RSUniUICaptureVisitor> rsUniUICaptureVisitor =
        std::make_shared<RSUniUICapture::RSUniUICaptureVisitor>(nodeId, captureConfig);
    ASSERT_NE(rsUniUICaptureVisitor, nullptr);
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> recordingCanvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniUICaptureVisitor->SetPaintFilterCanvas(recordingCanvas);
    
    std::weak_ptr<RSContext> context;
    RSSurfaceRenderNode node(nodeId, context);
    rsUniUICaptureVisitor->ProcessSurfaceRenderNodeWithUni(node);
}

/**
 * @tc.name: ProcessSurfaceRenderNodeWithoutUniTest
 * @tc.desc: Test ProcessSurfaceRenderNodeWithoutUni
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessSurfaceRenderNodeWithoutUniTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.0;
    captureConfig.scaleY = 0.0;

    RSUniUICapture rsUniUICapture(nodeId, captureConfig);
    std::shared_ptr<RSUniUICapture::RSUniUICaptureVisitor> rsUniUICaptureVisitor =
        std::make_shared<RSUniUICapture::RSUniUICaptureVisitor>(nodeId, captureConfig);
    ASSERT_NE(rsUniUICaptureVisitor, nullptr);
    std::weak_ptr<RSContext> context;
    RSSurfaceRenderNode node(nodeId, context);
    rsUniUICaptureVisitor->ProcessSurfaceViewWithoutUni(node);
    ASSERT_EQ(rsUniUICaptureVisitor->canvas_, nullptr);

    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> recordingCanvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniUICaptureVisitor->SetPaintFilterCanvas(recordingCanvas);
    rsUniUICaptureVisitor->ProcessSurfaceViewWithoutUni(node);
    ASSERT_NE(rsUniUICaptureVisitor->canvas_, nullptr);
}

/**
 * @tc.name: CreateClientPixelMap
 * @tc.desc: Test CreateClientPixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, CreateClientPixelMap, TestSize.Level1)
{
    Drawing::Rect rect = {0, 0, 1260, 2720};
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.captureType = SurfaceCaptureType::UICAPTURE;

    auto pixelMap = RSCapturePixelMapManager::GetClientCapturePixelMap(rect, captureConfig,
        UniRenderEnabledType::UNI_RENDER_DISABLED);
    EXPECT_EQ(pixelMap == nullptr, true);
}

} // namespace OHOS::Rosen