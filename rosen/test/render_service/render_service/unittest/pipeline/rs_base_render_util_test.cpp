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
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBaseRenderUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    static inline sptr<Surface> psurf = nullptr;
    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_YCRCB_420_SP,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    static inline BufferFlushConfig flushConfig = {
        .damage = { .w = 0x100, .h = 0x100, },
    };
};
std::shared_ptr<RSSurfaceRenderNode> node_ = nullptr;

void RSBaseRenderUtilTest::SetUpTestCase()
{
    RSSurfaceRenderNodeConfig config;
    node_ = std::make_shared<RSSurfaceRenderNode>(config);
}

void RSBaseRenderUtilTest::TearDownTestCase()
{
    node_ = nullptr;
}

void RSBaseRenderUtilTest::SetUp() {}
void RSBaseRenderUtilTest::TearDown() {}

/*
 * @tc.name: IsBufferValid_001
 * @tc.desc: Test IsBufferValid
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, IsBufferValid_001, TestSize.Level2)
{
    ASSERT_EQ(false, RSBaseRenderUtil::IsBufferValid(nullptr));
}

/*
 * @tc.name: GetFrameBufferRequestConfig_001
 * @tc.desc: Test GetFrameBufferRequestConfig
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, GetFrameBufferRequestConfig_001, TestSize.Level2)
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
HWTEST_F(RSBaseRenderUtilTest, DropFrameProcess_001, TestSize.Level2)
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
HWTEST_F(RSBaseRenderUtilTest, ConsumeAndUpdateBuffer_001, TestSize.Level2)
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
HWTEST_F(RSBaseRenderUtilTest, ReleaseBuffer_001, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceHandler surfaceHandler(id);
    ASSERT_EQ(false, RSBaseRenderUtil::ReleaseBuffer(surfaceHandler));
}

/*
 * @tc.name: SetColorFilterModeToPaint_001
 * @tc.desc: Test SetColorFilterModeToPaint
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, SetColorFilterModeToPaint_001, TestSize.Level2)
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
HWTEST_F(RSBaseRenderUtilTest, IsColorFilterModeValid_001, TestSize.Level2)
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
HWTEST_F(RSBaseRenderUtilTest, WriteSurfaceRenderNodeToPng_001, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig config;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(config);
    bool result = RSBaseRenderUtil::WriteSurfaceRenderNodeToPng(*node);
    ASSERT_EQ(false, result);
}

/*
 * @tc.name: ConvertBufferToBitmap_001
 * @tc.desc: Test ConvertBufferToBitmap IsBufferValid
 * @tc.type: FUNC
 * @tc.require: issueI614RU
 */
HWTEST_F(RSBaseRenderUtilTest, ConvertBufferToBitmap_001, TestSize.Level2)
{
    sptr<SurfaceBuffer> cbuffer;
    std::vector<uint8_t> newBuffer;
    ColorGamut dstGamut = ColorGamut::COLOR_GAMUT_SRGB;
    SkBitmap bitmap;
    ASSERT_EQ(false, RSBaseRenderUtil::ConvertBufferToBitmap(cbuffer, newBuffer, dstGamut, bitmap));
}

/*
 * @tc.name: ConvertBufferToBitmap_002
 * @tc.desc: Test ConvertBufferToBitmap by CreateYuvToRGBABitMap
 * @tc.type: FUNC
 * @tc.require: issueI614RU
 */
HWTEST_F(RSBaseRenderUtilTest, ConvertBufferToBitmap_002, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    const auto& surfaceConsumer = rsSurfaceRenderNode->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    psurf = Surface::CreateSurfaceAsProducer(producer);
    psurf->SetQueueSize(1);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    GSError ret = psurf->RequestBuffer(buffer, requestFence, requestConfig);
    sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
    ret = psurf->FlushBuffer(buffer, flushFence, flushConfig);
    OHOS::sptr<SurfaceBuffer> cbuffer;
    Rect damage;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    ret = surfaceConsumer->AcquireBuffer(cbuffer, acquireFence, timestamp, damage);

    std::vector<uint8_t> newBuffer;
    ColorGamut dstGamut = ColorGamut::COLOR_GAMUT_SRGB;
    SkBitmap bitmap;
    (void)RSBaseRenderUtil::ConvertBufferToBitmap(cbuffer, newBuffer, dstGamut, bitmap);
}

/*
 * @tc.name: ConvertBufferToBitmap_003
 * @tc.desc: Test ConvertBufferToBitmap by CreateNewColorGamutBitmap
 * @tc.type: FUNC
 * @tc.require: issueI614RU
 */
HWTEST_F(RSBaseRenderUtilTest, ConvertBufferToBitmap_003, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    const auto& surfaceConsumer = rsSurfaceRenderNode->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    psurf = Surface::CreateSurfaceAsProducer(producer);
    psurf->SetQueueSize(1);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    requestConfig.format = PIXEL_FMT_RGBA_8888;
    GSError ret = psurf->RequestBuffer(buffer, requestFence, requestConfig);
    sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
    ret = psurf->FlushBuffer(buffer, flushFence, flushConfig);
    OHOS::sptr<SurfaceBuffer> cbuffer;
    Rect damage;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    ret = surfaceConsumer->AcquireBuffer(cbuffer, acquireFence, timestamp, damage);

    std::vector<uint8_t> newBuffer;
    ColorGamut dstGamut = ColorGamut::COLOR_GAMUT_INVALID;
    SkBitmap bitmap;
    ASSERT_EQ(true, RSBaseRenderUtil::ConvertBufferToBitmap(cbuffer, newBuffer, dstGamut, bitmap));
}

/*
 * @tc.name: WritePixelMapToPng_001
 * @tc.desc: Test WritePixelMapToPng
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, WritePixelMapToPng_001, TestSize.Level2)
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
HWTEST_F(RSBaseRenderUtilTest, DealWithSurfaceRotationAndGravity_001, TestSize.Level2)
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
HWTEST_F(RSBaseRenderUtilTest, SetPropertiesForCanvas_001, TestSize.Level2)
{
    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(10, 10); // width height
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(skCanvas.get());;
    BufferDrawParam params;
    RSBaseRenderUtil::SetPropertiesForCanvas(*canvas, params);
}

/*
 * @tc.name: IsNeedClient01
 * @tc.desc: default value
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient01, Function | SmallTest | Level2)
{
    ComposeInfo info;
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, false);
}

/*
 * @tc.name: IsNeedClient02
 * @tc.desc: need client composition when SetCornerRadius
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient02, Function | SmallTest | Level2)
{
    ComposeInfo info;
    Rosen::Vector4f cornerRadius(5.0f, 5.0f, 5.0f, 5.0f);
    node_->GetMutableRenderProperties().SetCornerRadius(cornerRadius);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient03
 * @tc.desc: need client composition when Shadow is vaild
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient03, Function | SmallTest | Level2)
{
    ComposeInfo info;
    node_->GetMutableRenderProperties().SetShadowAlpha(1.0f);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient04
 * @tc.desc: need client composition when SetRotation
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient04, Function | SmallTest | Level2)
{
    ComposeInfo info;
    node_->GetMutableRenderProperties().SetRotation(90.0f);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient05
 * @tc.desc: need client composition when SetRotationX
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient05, Function | SmallTest | Level2)
{
    ComposeInfo info;
    node_->GetMutableRenderProperties().SetRotationX(90.0f);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient06
 * @tc.desc: need client composition when SetRotationY
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient06, Function | SmallTest | Level2)
{
    ComposeInfo info;
    node_->GetMutableRenderProperties().SetRotationY(90.0f);;
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient07
 * @tc.desc: need client composition when SetQuaternion
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient07, Function | SmallTest | Level2)
{
    ComposeInfo info;
    Quaternion quaternion(90.0f, 90.0f, 90.0f, 90.0f);
    node_->GetMutableRenderProperties().SetQuaternion(quaternion);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient08
 * @tc.desc: need client composition when Gravity != RESIZE &&
 *           BackgroundColor != Alpha && (srcRect.w != dstRect.w || srcRect.h != dstRect.h)
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient08, Function | SmallTest | Level2)
{
    ComposeInfo info;
    info.srcRect = GraphicIRect {0, 0, 100, 100};
    info.dstRect = GraphicIRect {0, 0, 200, 200};
    node_->GetMutableRenderProperties().SetBackgroundColor(RgbPalette::White());
    node_->GetMutableRenderProperties().SetFrameGravity(Gravity::TOP_LEFT);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient09
 * @tc.desc: need client composition when SetBackgroundFilter
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient09, Function | SmallTest | Level2)
{
    ComposeInfo info;
    std::shared_ptr<RSFilter> bgFilter = RSFilter::CreateBlurFilter(5.0f, 5.0f);
    node_->GetMutableRenderProperties().SetBackgroundFilter(bgFilter);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient10
 * @tc.desc: need client composition when SetFilter
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient10, Function | SmallTest | Level2)
{
    ComposeInfo info;
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(5.0f, 5.0f);
    node_->GetMutableRenderProperties().SetFilter(filter);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient11
 * @tc.desc: need client composition when SetNeedClient is True
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient11, Function | SmallTest | Level2)
{
    ComposeInfo info;
    RSBaseRenderUtil::SetNeedClient(true);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}
} // namespace OHOS::Rosen
