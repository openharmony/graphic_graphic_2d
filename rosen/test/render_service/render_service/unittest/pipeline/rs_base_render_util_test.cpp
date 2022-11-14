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
#include "limit_number.h"
#include "pipeline/rs_base_render_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBaseRenderUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBaseRenderUtilTest::SetUpTestCase() {}
void RSBaseRenderUtilTest::TearDownTestCase() {}
void RSBaseRenderUtilTest::SetUp() {}
void RSBaseRenderUtilTest::TearDown() {}

/*
 * @tc.name: IsBufferValid_001
 * @tc.desc: Test IsBufferValid
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, IsBufferValid_001, TestSize.Level1)
{
    ASSERT_EQ(false, RSBaseRenderUtil::IsBufferValid(nullptr));
}

/*
 * @tc.name: GetFrameBufferRequestConfig_001
 * @tc.desc: Test GetFrameBufferRequestConfig
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, GetFrameBufferRequestConfig_001, TestSize.Level1)
{
    ScreenInfo screenInfo;
    screenInfo.width = 480;
    BufferRequestConfig config = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo, true);
    ASSERT_EQ(static_cast<int32_t>(screenInfo.width), config.width);
}

/*
 * @tc.name: DropFrameProcess_001
 * @tc.desc: Test DropFrameProcess
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, DropFrameProcess_001, TestSize.Level1)
{
    NodeId id = 0;
    RSSurfaceHandler surfaceHandler(id);
    RSBaseRenderUtil::DropFrameProcess(surfaceHandler);
}

/*
 * @tc.name: ConsumeAndUpdateBuffer_001
 * @tc.desc: Test ConsumeAndUpdateBuffer
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, ConsumeAndUpdateBuffer_001, TestSize.Level1)
{
    NodeId id = 0;
    RSSurfaceHandler surfaceHandler(id);
    ASSERT_EQ(true, RSBaseRenderUtil::ConsumeAndUpdateBuffer(surfaceHandler));
}

/*
 * @tc.name: ReleaseBuffer_001
 * @tc.desc: Test ReleaseBuffer
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, ReleaseBuffer_001, TestSize.Level1)
{
    NodeId id = 0;
    RSSurfaceHandler surfaceHandler(id);
    ASSERT_EQ(false, RSBaseRenderUtil::ReleaseBuffer(surfaceHandler));
}

/*
 * @tc.name: ConvertBufferToBitmap_001
 * @tc.desc: Test ConvertBufferToBitmap
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, ConvertBufferToBitmap_001, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer;
    std::vector<uint8_t> newBuffer;
    ColorGamut dstGamut = ColorGamut::COLOR_GAMUT_SRGB;
    SkBitmap bitmap;
    ASSERT_EQ(false, RSBaseRenderUtil::ConvertBufferToBitmap(buffer, newBuffer, dstGamut, bitmap));
}

/*
 * @tc.name: SetColorFilterModeToPaint_001
 * @tc.desc: Test SetColorFilterModeToPaint
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, SetColorFilterModeToPaint_001, TestSize.Level1)
{
    SkPaint paint;
    ColorFilterMode colorFilterMode = ColorFilterMode::INVERT_COLOR_ENABLE_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    colorFilterMode = ColorFilterMode::DALTONIZATION_PROTANOMALY_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    colorFilterMode = ColorFilterMode::DALTONIZATION_DEUTERANOMALY_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    colorFilterMode = ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    colorFilterMode = ColorFilterMode::INVERT_DALTONIZATION_PROTANOMALY_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    colorFilterMode = ColorFilterMode::INVERT_DALTONIZATION_DEUTERANOMALY_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    colorFilterMode = ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    colorFilterMode = static_cast<ColorFilterMode>(40); // use invalid number to test default mode
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
}

/*
 * @tc.name: IsColorFilterModeValid_001
 * @tc.desc: Test IsColorFilterModeValid
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, IsColorFilterModeValid_001, TestSize.Level1)
{
    ColorFilterMode colorFilterMode = ColorFilterMode::INVERT_COLOR_ENABLE_MODE;
    ASSERT_EQ(true, RSBaseRenderUtil::IsColorFilterModeValid(colorFilterMode));
}

/*
 * @tc.name: WriteSurfaceRenderNodeToPng_001
 * @tc.desc: Test WriteSurfaceRenderNodeToPng
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, WriteSurfaceRenderNodeToPng_001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(config);
    bool result = RSBaseRenderUtil::WriteSurfaceRenderNodeToPng(*node);
    ASSERT_EQ(false, result);
}

/*
 * @tc.name: WritePixelMapToPng_001
 * @tc.desc: Test WritePixelMapToPng
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, WritePixelMapToPng_001, TestSize.Level1)
{
    Media::PixelMap pixelMap;
    bool result = RSBaseRenderUtil::WritePixelMapToPng(pixelMap);
    ASSERT_EQ(false, result);
}

/*
 * @tc.name: DealWithSurfaceRotationAndGravity_001
 * @tc.desc: Test DealWithSurfaceRotationAndGravity
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, DealWithSurfaceRotationAndGravity_001, TestSize.Level1)
{
    RectF localBounds;
    BufferDrawParam params;
    RSSurfaceRenderNodeConfig config;
    std::shared_ptr<RSSurfaceRenderNode> rsNode = std::make_shared<RSSurfaceRenderNode>(config);

    std::shared_ptr<RSSurfaceRenderNode> rsParentNode = std::make_shared<RSSurfaceRenderNode>(config);
    rsParentNode->AddChild(rsNode);
    rsNode->SetIsOnTheTree(true);

    sptr<Surface> csurf = Surface::CreateSurfaceAsConsumer(config.name);
    rsNode->SetConsumer(csurf);
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(*rsNode, localBounds, params);
}

/*
 * @tc.name: SetPropertiesForCanvas_001
 * @tc.desc: Test SetPropertiesForCanvas
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, SetPropertiesForCanvas_001, TestSize.Level1)
{
    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(10, 10); // width height
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(skCanvas.get());;
    BufferDrawParam params;
    RSBaseRenderUtil::SetPropertiesForCanvas(*canvas, params);
}
} // namespace OHOS::Rosen
