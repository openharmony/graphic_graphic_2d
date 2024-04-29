/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "surface_buffer_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr Rect RECT_ONE = {0, 0, 100, 100};
constexpr Rect RECT_TWO = {100, 100, 100, 100};
constexpr Rect RECT_RESULT = {0, 0, 200, 200};
class RSBaseRenderUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void CompareMatrix(float mat1[], float mat2[]);

private:
    static inline sptr<Surface> psurf = nullptr;
    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_YCRCB_420_SP,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    static inline BufferFlushConfig flushConfig = {
        .damage = { .w = 0x100, .h = 0x100, },
    };
    static inline Drawing::Matrix matrix  = Drawing::Matrix();
    static const uint32_t MATRIX_SIZE = 20;
    static inline float InvertColorMat[MATRIX_SIZE] = {
        0.402,  -1.174, -0.228, 1.0, 0.0,
        -0.598, -0.174, -0.228, 1.0, 0.0,
        -0.599, -1.175, 0.772,  1.0, 0.0,
        0.0,    0.0,    0.0,    1.0, 0.0
    };
    static inline float ProtanomalyMat[MATRIX_SIZE] = {
        0.622,  0.377,  0.0, 0.0, 0.0,
        0.264,  0.736,  0.0, 0.0, 0.0,
        0.217,  -0.217, 1.0, 0.0, 0.0,
        0.0,    0.0,    0.0, 1.0, 0.0
    };
    static inline float DeuteranomalyMat[MATRIX_SIZE] = {
        0.288,  0.712, 0.0, 0.0, 0.0,
        0.053,  0.947, 0.0, 0.0, 0.0,
        -0.258, 0.258, 1.0, 0.0, 0.0,
        0.0,    0.0,   0.0, 1.0, 0.0
    };
    static inline float TritanomalyMat[MATRIX_SIZE] = {
        1.0, -0.806, 0.806, 0.0, 0.0,
        0.0, 0.379,  0.621, 0.0, 0.0,
        0.0, 0.105,  0.895, 0.0, 0.0,
        0.0, 0.0,    0.0,   1.0, 0.0
    };
    static inline float InvertProtanomalyMat[MATRIX_SIZE] = {
        -0.109, -0.663, -0.228, 1.0, 0.0,
        -0.468, -0.304, -0.228, 1.0, 0.0,
        -0.516, -1.258, 0.772,  1.0, 0.0,
        0.0,    0.0,    0.0,    1.0, 0.0
    };
    static inline float InvertDeuteranomalyMat[MATRIX_SIZE] = {
        0.113,  -0.885, -0.228, 1.0, 0.0,
        -0.123, -0.649, -0.228, 1.0, 0.0,
        -0.434, -1.341, 0.772,  1.0, 0.0,
        0.0,    0.0,    0.0,    1.0, 0.0
    };
    static inline float InvertTritanomalyMat[MATRIX_SIZE] = {
        0.402,  -0.792, -0.609, 1.0, 0.0,
        -0.598, 0.392,  -0.794, 1.0, 0.0,
        -0.598, 0.118,  -0.521, 1.0, 0.0,
        0.0,    0.0,    0.0,    1.0, 0.0
    };
};
std::shared_ptr<RSSurfaceRenderNode> node_ = nullptr;

const uint32_t STUB_PIXEL_FMT_RGBA_16161616 = 0X7fff0001;

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

void RSBaseRenderUtilTest::CompareMatrix(float mat1[], float mat2[])
{
    for (uint32_t i = 0; i < MATRIX_SIZE; i++) {
        ASSERT_EQ(mat1[i], mat2[i]);
    }
}

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
 * @tc.name: IsBufferValid_002
 * @tc.desc: Test IsBufferValid
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, IsBufferValid_002, TestSize.Level2)
{
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    ASSERT_EQ(buffer->GetBufferHandle(), nullptr);
    ASSERT_EQ(false, RSBaseRenderUtil::IsBufferValid(buffer));
    BufferRequestConfig requestConfig = {
        .width = 0x000,
        .height = 0x000,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
    };
    GSError ret = buffer->Alloc(requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
    ASSERT_EQ(false, RSBaseRenderUtil::IsBufferValid(buffer));
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
    GraphicColorGamut colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_1010102;
    BufferRequestConfig config = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo, true,
        false, colorGamut, pixelFormat);
    ASSERT_EQ(static_cast<int32_t>(screenInfo.width), config.width);
    ASSERT_EQ(colorGamut, config.colorGamut);
    ASSERT_EQ(pixelFormat, config.format);
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
    ASSERT_EQ(OHOS::GSERROR_NO_CONSUMER, RSBaseRenderUtil::DropFrameProcess(surfaceHandler));
}

/*
 * @tc.name: MergeBufferDamages_001
 * @tc.desc: check MergeBufferDamages result
 * @tc.type: FUNC
 * @tc.require: #I9E60C
 */
HWTEST_F(RSBaseRenderUtilTest, MergeBufferDamages_001, TestSize.Level2)
{
    std::vector<Rect> damages;
    damages.push_back(RECT_ONE);
    damages.push_back(RECT_TWO);
    Rect damageAfterMerge = RSBaseRenderUtil::MergeBufferDamages(damages);
    bool compareResult = (damageAfterMerge.x == RECT_RESULT.x) && (damageAfterMerge.y == RECT_RESULT.y) &&
        (damageAfterMerge.w == RECT_RESULT.w) && (damageAfterMerge.h == RECT_RESULT.h);
    ASSERT_EQ(true, compareResult);
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
    ASSERT_EQ(true, RSBaseRenderUtil::ConsumeAndUpdateBuffer(surfaceHandler, true));
}

/*
 * @tc.name: ConsumeAndUpdateBuffer_002
 * @tc.desc: Test ConsumeAndUpdateBuffer while buffer not satisfy consume time
 * @tc.type: FUNC
 * @tc.require: issueI9J3IQ
 */
HWTEST_F(RSBaseRenderUtilTest, ConsumeAndUpdateBuffer_002, TestSize.Level2)
{
    // create producer and consumer
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    const auto& surfaceConsumer = rsSurfaceRenderNode->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    psurf = Surface::CreateSurfaceAsProducer(producer);
    psurf->SetQueueSize(1);

    // request buffer
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    [[maybe_unused]] GSError ret = psurf->RequestBuffer(buffer, requestFence, requestConfig);

    // flush buffer
    sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
    flushConfig.timestamp = 100; // this timestamp can be any nunmber
    ret = psurf->FlushBuffer(buffer, flushFence, flushConfig);

    auto& surfaceHandler = static_cast<RSSurfaceHandler&>(*(rsSurfaceRenderNode.get()));
    uint64_t vsyncTimestamp = 50; // let vync's timestamp smaller than buffer timestamp
    RSBaseRenderUtil::ConsumeAndUpdateBuffer(surfaceHandler, false, vsyncTimestamp);
    ASSERT_NE(surfaceHandler.bufferCache_.size(), 0);
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
 * @tc.name: ReleaseBuffer_002
 * @tc.desc: Test ReleaseBuffer
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, ReleaseBuffer_002, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceHandler surfaceHandler(id);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create();
    surfaceHandler.SetConsumer(consumer);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    Rect damage;
    surfaceHandler.SetBuffer(buffer, acquireFence, damage, timestamp);
    surfaceHandler.SetBuffer(buffer, acquireFence, damage, timestamp);
    ASSERT_EQ(true, RSBaseRenderUtil::ReleaseBuffer(surfaceHandler));
}

/*
 * @tc.name: SetColorFilterModeToPaint_001
 * @tc.desc: Test SetColorFilterModeToPaint
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, SetColorFilterModeToPaint_001, TestSize.Level2)
{
    Drawing::Brush paint;
    float matrix[MATRIX_SIZE];
    Drawing::Filter filter;
    ColorFilterMode colorFilterMode = ColorFilterMode::INVERT_COLOR_ENABLE_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    filter = paint.GetFilter();
    filter.GetColorFilter()->AsAColorMatrix(matrix);
    CompareMatrix(matrix, InvertColorMat);

    colorFilterMode = ColorFilterMode::DALTONIZATION_PROTANOMALY_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    filter = paint.GetFilter();
    filter.GetColorFilter()->AsAColorMatrix(matrix);
    CompareMatrix(matrix, ProtanomalyMat);

    colorFilterMode = ColorFilterMode::DALTONIZATION_DEUTERANOMALY_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    filter = paint.GetFilter();
    filter.GetColorFilter()->AsAColorMatrix(matrix);
    CompareMatrix(matrix, DeuteranomalyMat);

    colorFilterMode = ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    filter = paint.GetFilter();
    filter.GetColorFilter()->AsAColorMatrix(matrix);
    CompareMatrix(matrix, TritanomalyMat);

    colorFilterMode = ColorFilterMode::INVERT_DALTONIZATION_PROTANOMALY_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    filter = paint.GetFilter();
    filter.GetColorFilter()->AsAColorMatrix(matrix);
    CompareMatrix(matrix, InvertProtanomalyMat);

    colorFilterMode = ColorFilterMode::INVERT_DALTONIZATION_DEUTERANOMALY_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    filter = paint.GetFilter();
    filter.GetColorFilter()->AsAColorMatrix(matrix);
    CompareMatrix(matrix, InvertDeuteranomalyMat);

    colorFilterMode = ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    filter = paint.GetFilter();
    filter.GetColorFilter()->AsAColorMatrix(matrix);
    CompareMatrix(matrix, InvertTritanomalyMat);

    colorFilterMode = ColorFilterMode::INVERT_COLOR_DISABLE_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    filter = paint.GetFilter();
    ASSERT_EQ(filter.GetColorFilter(), nullptr);

    colorFilterMode = ColorFilterMode::DALTONIZATION_NORMAL_MODE;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    filter = paint.GetFilter();
    ASSERT_EQ(filter.GetColorFilter(), nullptr);

    colorFilterMode = ColorFilterMode::COLOR_FILTER_END;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    filter = paint.GetFilter();
    ASSERT_EQ(filter.GetColorFilter(), nullptr);

    colorFilterMode = static_cast<ColorFilterMode>(40); // use invalid number to test default mode
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
    filter = paint.GetFilter();
    ASSERT_EQ(filter.GetColorFilter(), nullptr);

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
 * @tc.name: IsColorFilterModeValid_002
 * @tc.desc: Test IsColorFilterModeValid
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, IsColorFilterModeValid_002, TestSize.Level2)
{
    ColorFilterMode colorFilterMode = static_cast<ColorFilterMode>(50);
    ASSERT_EQ(false, RSBaseRenderUtil::IsColorFilterModeValid(colorFilterMode));
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
 * @tc.name: WriteCacheRenderNodeToPng_001
 * @tc.desc: Test WriteCacheRenderNodeToPng
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, WriteCacheRenderNodeToPng_001, TestSize.Level2)
{
    bool result = RSBaseRenderUtil::WriteCacheRenderNodeToPng(*node_);
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
    GraphicColorGamut dstGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    Drawing::Bitmap bitmap;
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
    [[maybe_unused]] GSError ret = psurf->RequestBuffer(buffer, requestFence, requestConfig);
    sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
    ret = psurf->FlushBuffer(buffer, flushFence, flushConfig);
    OHOS::sptr<SurfaceBuffer> cbuffer;
    Rect damage;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    ret = surfaceConsumer->AcquireBuffer(cbuffer, acquireFence, timestamp, damage);

    std::vector<uint8_t> newBuffer;
    GraphicColorGamut dstGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    Drawing::Bitmap bitmap;
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
    requestConfig.format = GRAPHIC_PIXEL_FMT_RGBA_8888;
    [[maybe_unused]] GSError ret = psurf->RequestBuffer(buffer, requestFence, requestConfig);
    sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
    ret = psurf->FlushBuffer(buffer, flushFence, flushConfig);
    OHOS::sptr<SurfaceBuffer> cbuffer;
    Rect damage;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    ret = surfaceConsumer->AcquireBuffer(cbuffer, acquireFence, timestamp, damage);

    std::vector<uint8_t> newBuffer;
    GraphicColorGamut dstGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_INVALID;
    Drawing::Bitmap bitmap;
    ASSERT_EQ(true, RSBaseRenderUtil::ConvertBufferToBitmap(cbuffer, newBuffer, dstGamut, bitmap));
}

/*
 * @tc.name: ConvertBufferToBitmap_004
 * @tc.desc: Test ConvertBufferToBitmap by CreateNewColorGamutBitmap
 * @tc.type: FUNC
 * @tc.require: issueI614RU
 */
HWTEST_F(RSBaseRenderUtilTest, ConvertBufferToBitmap_004, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    const auto& surfaceConsumer = rsSurfaceRenderNode->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    psurf = Surface::CreateSurfaceAsProducer(producer);
    psurf->SetQueueSize(1);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    requestConfig.format = STUB_PIXEL_FMT_RGBA_16161616;
    [[maybe_unused]] GSError ret = psurf->RequestBuffer(buffer, requestFence, requestConfig);
    sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
    ret = psurf->FlushBuffer(buffer, flushFence, flushConfig);
    OHOS::sptr<SurfaceBuffer> cbuffer;
    Rect damage;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    ret = surfaceConsumer->AcquireBuffer(cbuffer, acquireFence, timestamp, damage);

    std::vector<uint8_t> newBuffer;
    GraphicColorGamut dstGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    Drawing::Bitmap bitmap;
    ASSERT_EQ(false, RSBaseRenderUtil::ConvertBufferToBitmap(cbuffer, newBuffer, dstGamut, bitmap));
}

/*
 * @tc.name: ConvertBufferToBitmap_005
 * @tc.desc: Test ConvertBufferToBitmap by CreateBitmap
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSBaseRenderUtilTest, ConvertBufferToBitmap_005, TestSize.Level2)
{
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    ASSERT_EQ(buffer->GetBufferHandle(), nullptr);
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
    };
    GSError ret = buffer->Alloc(requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    std::vector<uint8_t> newBuffer;
    GraphicColorGamut dstGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    Drawing::Bitmap bitmap;
    (void)RSBaseRenderUtil::ConvertBufferToBitmap(buffer, newBuffer, dstGamut, bitmap);
}

/*
 * @tc.name: WritePixelMapToPng_001
 * @tc.desc: Test WritePixelMapToPng
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, WritePixelMapToPng_001, TestSize.Level2)
{
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    bool result = RSBaseRenderUtil::WritePixelMapToPng(*pixelMap);
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

    sptr<IConsumerSurface> csurf = IConsumerSurface::Create(config.name);
    rsNode->SetConsumer(csurf);
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(csurf->GetTransform(),
        rsNode->GetRenderProperties().GetFrameGravity(), localBounds, params);
}

/*
 * @tc.name: SetPropertiesForCanvas_001
 * @tc.desc: Test SetPropertiesForCanvas
 * @tc.type: FUNC
 * @tc.require: issueI605F4
 */
HWTEST_F(RSBaseRenderUtilTest, SetPropertiesForCanvas_001, TestSize.Level2)
{
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());

    BufferDrawParam params;
    params.clipRect = Drawing::Rect(0, 0, 5, 5);
    RSBaseRenderUtil::SetPropertiesForCanvas(*canvas, params);
    auto rect = drawingCanvas->GetDeviceClipBounds();
    ASSERT_EQ(params.clipRect.GetWidth(), rect.GetWidth());
    ASSERT_EQ(params.clipRect.GetHeight(), rect.GetHeight());
}

/*
 * @tc.name: SetPropertiesForCanvas_002
 * @tc.desc: Test SetPropertiesForCanvas
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSBaseRenderUtilTest, SetPropertiesForCanvas_002, TestSize.Level2)
{
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    BufferDrawParam params;
    params.isNeedClip = false;
    RSBaseRenderUtil::SetPropertiesForCanvas(*canvas, params);
    auto rect = drawingCanvas->GetDeviceClipBounds();
    ASSERT_EQ(10, rect.GetWidth());
}

/*
 * @tc.name: SetPropertiesForCanvas_003
 * @tc.desc: Test SetPropertiesForCanvas
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSBaseRenderUtilTest, SetPropertiesForCanvas_003, TestSize.Level2)
{
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    BufferDrawParam params;
    params.cornerRadius = Vector4f(1.0f);
    RSBaseRenderUtil::SetPropertiesForCanvas(*canvas, params);
    auto rect = drawingCanvas->GetDeviceClipBounds();
    ASSERT_EQ(0, rect.GetWidth());
}

/*
 * @tc.name: SetPropertiesForCanvas_004
 * @tc.desc: Test SetPropertiesForCanvas
 * @tc.type: FUNC
 * @tc.require: issuesI6Q871
 */
HWTEST_F(RSBaseRenderUtilTest, SetPropertiesForCanvas_004, TestSize.Level2)
{
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    BufferDrawParam params;
    params.clipRect = Drawing::Rect(0, 0, 5, 5);
    params.backgroundColor = Drawing::Color::COLOR_RED;
    RSBaseRenderUtil::SetPropertiesForCanvas(*canvas, params);
    auto rect = drawingCanvas->GetDeviceClipBounds();
    ASSERT_EQ(params.clipRect.GetWidth(), rect.GetWidth());
    ASSERT_EQ(params.clipRect.GetHeight(), rect.GetHeight());
}

/*
 * @tc.name: IsNeedClient_001
 * @tc.desc: default value
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient_001, Function | SmallTest | Level2)
{
    ComposeInfo info;
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, false);
}

/*
 * @tc.name: IsNeedClient_002
 * @tc.desc: need client composition when SetCornerRadius
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient_002, Function | SmallTest | Level2)
{
    ComposeInfo info;
    Rosen::Vector4f cornerRadius(5.0f, 5.0f, 5.0f, 5.0f);
    node_->GetMutableRenderProperties().SetCornerRadius(cornerRadius);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient_003
 * @tc.desc: need client composition when Shadow is vaild
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient_003, Function | SmallTest | Level2)
{
    ComposeInfo info;
    node_->GetMutableRenderProperties().SetShadowAlpha(1.0f);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient_004
 * @tc.desc: need client composition when SetRotation
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient_004, Function | SmallTest | Level2)
{
    ComposeInfo info;
    node_->GetMutableRenderProperties().SetRotation(90.0f);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient_005
 * @tc.desc: need client composition when SetRotationX
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient_005, Function | SmallTest | Level2)
{
    ComposeInfo info;
    node_->GetMutableRenderProperties().SetRotationX(90.0f);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient_006
 * @tc.desc: need client composition when SetRotationY
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient_006, Function | SmallTest | Level2)
{
    ComposeInfo info;
    node_->GetMutableRenderProperties().SetRotationY(90.0f);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient_007
 * @tc.desc: need client composition when SetQuaternion
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient_007, Function | SmallTest | Level2)
{
    ComposeInfo info;
    Quaternion quaternion(90.0f, 90.0f, 90.0f, 90.0f);
    node_->GetMutableRenderProperties().SetQuaternion(quaternion);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient_008
 * @tc.desc: need client composition when Gravity != RESIZE &&
 *           BackgroundColor != Alpha && (srcRect.w != dstRect.w || srcRect.h != dstRect.h)
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient_008, Function | SmallTest | Level2)
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
 * @tc.name: IsNeedClient_009
 * @tc.desc: need client composition when SetBackgroundFilter
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient_009, Function | SmallTest | Level2)
{
    ComposeInfo info;
    std::shared_ptr<RSFilter> bgFilter = RSFilter::CreateBlurFilter(5.0f, 5.0f);
    node_->GetMutableRenderProperties().SetBackgroundFilter(bgFilter);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient_010
 * @tc.desc: need client composition when SetFilter
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient_010, Function | SmallTest | Level2)
{
    ComposeInfo info;
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(5.0f, 5.0f);
    node_->GetMutableRenderProperties().SetFilter(filter);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: IsNeedClient_011
 * @tc.desc: need client composition when SetNeedClient is True
 * @tc.type: FUNC
 * @tc.require: issueI5VTW0
*/
HWTEST_F(RSBaseRenderUtilTest, IsNeedClient_011, Function | SmallTest | Level2)
{
    ComposeInfo info;
    RSBaseRenderUtil::SetNeedClient(true);
    bool needClient = RSBaseRenderUtil::IsNeedClient(*node_, info);
    ASSERT_EQ(needClient, true);
}

/*
 * @tc.name: FlipMatrix_001
 * @tc.desc: Test FlipMatrix By Setting GRAPHIC_FLIP_H
 * @tc.type: FUNC
 * @tc.require: issueI6AOQZ
*/
HWTEST_F(RSBaseRenderUtilTest, FlipMatrix_001, Function | SmallTest | Level2)
{
    BufferDrawParam params;
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    params.matrix = Drawing::Matrix();
    params.matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    RSSurfaceRenderNodeConfig config;
    std::shared_ptr<RSSurfaceRenderNode> rsNode = std::make_shared<RSSurfaceRenderNode>(config);

    sptr<IConsumerSurface> surface = IConsumerSurface::Create(config.name);
    surface->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H);
    rsNode->SetConsumer(surface);
    RSBaseRenderUtil::FlipMatrix(surface->GetTransform(), params);

    ASSERT_EQ(params.matrix.Get(0), -matrix.Get(0));
    ASSERT_EQ(params.matrix.Get(3), -matrix.Get(3));
    ASSERT_EQ(params.matrix.Get(6), -matrix.Get(6));
}

/*
 * @tc.name: FlipMatrix_002
 * @tc.desc: Test FlipMatrix By Setting GRAPHIC_FLIP_V
 * @tc.type: FUNC
 * @tc.require: issueI6AOQZ
*/
HWTEST_F(RSBaseRenderUtilTest, FlipMatrix_002, Function | SmallTest | Level2)
{
    BufferDrawParam params;
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    params.matrix = Drawing::Matrix();
    params.matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);

    RSSurfaceRenderNodeConfig config;
    std::shared_ptr<RSSurfaceRenderNode> rsNode = std::make_shared<RSSurfaceRenderNode>(config);

    sptr<IConsumerSurface> surface = IConsumerSurface::Create(config.name);
    surface->SetTransform(GraphicTransformType::GRAPHIC_FLIP_V);
    rsNode->SetConsumer(surface);
    RSBaseRenderUtil::FlipMatrix(surface->GetTransform(), params);

    ASSERT_EQ(params.matrix.Get(1), -matrix.Get(1));
    ASSERT_EQ(params.matrix.Get(4), -matrix.Get(4));
    ASSERT_EQ(params.matrix.Get(7), -matrix.Get(7));
}

/*
 * @tc.name: GetRotateTransform_001
 * @tc.desc: Test GetRotateTransform Default
 * @tc.type: FUNC
 * @tc.require: issueI6AOQZ
*/
HWTEST_F(RSBaseRenderUtilTest, GetRotateTransform_001, Function | SmallTest | Level2)
{
    RSSurfaceRenderNodeConfig config;
    std::shared_ptr<RSSurfaceRenderNode> rsNode = std::make_shared<RSSurfaceRenderNode>(config);
    sptr<IConsumerSurface> surface = IConsumerSurface::Create(config.name);
    auto transform = RSBaseRenderUtil::GetRotateTransform(surface->GetTransform());
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_ROTATE_NONE);
}

/*
 * @tc.name: GetRotateTransform_002
 * @tc.desc: Test GetRotateTransform GRAPHIC_FLIP_H_ROT90
 * @tc.type: FUNC
 * @tc.require: issueI6AOQZ
*/
HWTEST_F(RSBaseRenderUtilTest, GetRotateTransform_002, Function | SmallTest | Level2)
{
    RSSurfaceRenderNodeConfig config;
    std::shared_ptr<RSSurfaceRenderNode> rsNode = std::make_shared<RSSurfaceRenderNode>(config);
    sptr<IConsumerSurface> surface = IConsumerSurface::Create(config.name);
    surface->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT90);
    auto transform = RSBaseRenderUtil::GetRotateTransform(surface->GetTransform());
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_ROTATE_90);
}

/*
 * @tc.name: GetRotateTransform_003
 * @tc.desc: Test GetRotateTransform GRAPHIC_FLIP_V_ROT180
 * @tc.type: FUNC
 * @tc.require: issueI6AOQZ
*/
HWTEST_F(RSBaseRenderUtilTest, GetRotateTransform_003, Function | SmallTest | Level2)
{
    RSSurfaceRenderNodeConfig config;
    std::shared_ptr<RSSurfaceRenderNode> rsNode = std::make_shared<RSSurfaceRenderNode>(config);
    sptr<IConsumerSurface> surface = IConsumerSurface::Create(config.name);
    surface->SetTransform(GraphicTransformType::GRAPHIC_FLIP_V_ROT180);
    auto transform = RSBaseRenderUtil::GetRotateTransform(surface->GetTransform());
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_ROTATE_180);
}

/*
 * @tc.name: GetFlipTransform_001
 * @tc.desc: Test GetFlipTransform Default
 * @tc.type: FUNC
 * @tc.require: issueI6AOQZ
*/
HWTEST_F(RSBaseRenderUtilTest, GetFlipTransform_001, Function | SmallTest | Level2)
{
    RSSurfaceRenderNodeConfig config;
    std::shared_ptr<RSSurfaceRenderNode> rsNode = std::make_shared<RSSurfaceRenderNode>(config);
    sptr<IConsumerSurface> surface = IConsumerSurface::Create(config.name);
    auto transform = RSBaseRenderUtil::GetFlipTransform(surface->GetTransform());
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_ROTATE_NONE);
}

/*
 * @tc.name: GetFlipTransform_002
 * @tc.desc: Test GetFlipTransform GRAPHIC_FLIP_H_ROT90
 * @tc.type: FUNC
 * @tc.require: issueI6AOQZ
*/
HWTEST_F(RSBaseRenderUtilTest, GetFlipTransform_002, Function | SmallTest | Level2)
{
    RSSurfaceRenderNodeConfig config;
    std::shared_ptr<RSSurfaceRenderNode> rsNode = std::make_shared<RSSurfaceRenderNode>(config);
    sptr<IConsumerSurface> surface = IConsumerSurface::Create(config.name);
    surface->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT90);
    auto transform = RSBaseRenderUtil::GetFlipTransform(surface->GetTransform());
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_FLIP_H);
}

/*
 * @tc.name: GetFlipTransform_003
 * @tc.desc: Test GetFlipTransform GRAPHIC_FLIP_H_ROT180
 * @tc.type: FUNC
 * @tc.require: I6HE0M
*/
HWTEST_F(RSBaseRenderUtilTest, GetFlipTransform_003, Function | SmallTest | Level2)
{
    auto transform = RSBaseRenderUtil::GetFlipTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_FLIP_H);
}

/*
 * @tc.name: GetFlipTransform_004
 * @tc.desc: Test GetFlipTransform GRAPHIC_FLIP_H_ROT270
 * @tc.type: FUNC
 * @tc.require: I6HE0M
*/
HWTEST_F(RSBaseRenderUtilTest, GetFlipTransform_004, Function | SmallTest | Level2)
{
    auto transform = RSBaseRenderUtil::GetFlipTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT270);
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_FLIP_H);
}

/*
 * @tc.name: GetFlipTransform_005
 * @tc.desc: Test GetFlipTransform GRAPHIC_FLIP_V_ROT180
 * @tc.type: FUNC
 * @tc.require: I6HE0M
*/
HWTEST_F(RSBaseRenderUtilTest, GetFlipTransform_005, Function | SmallTest | Level2)
{
    auto transform = RSBaseRenderUtil::GetFlipTransform(GraphicTransformType::GRAPHIC_FLIP_V_ROT180);
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_FLIP_V);
}

/*
 * @tc.name: GetFlipTransform_006
 * @tc.desc: Test GetFlipTransform GRAPHIC_FLIP_V_ROT270
 * @tc.type: FUNC
 * @tc.require: I6HE0M
*/
HWTEST_F(RSBaseRenderUtilTest, GetFlipTransform_006, Function | SmallTest | Level2)
{
    auto transform = RSBaseRenderUtil::GetFlipTransform(GraphicTransformType::GRAPHIC_FLIP_V_ROT270);
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_FLIP_V);
}

/*
 * @tc.name: ClockwiseToAntiClockwiseTransform
 * @tc.desc: Test ClockwiseToAntiClockwiseTransform GRAPHIC_ROTATE_90
 * @tc.type: FUNC
 * @tc.require: I6HE0M
*/
HWTEST_F(RSBaseRenderUtilTest, ClockwiseToAntiClockwiseTransform_001, Function | SmallTest | Level2)
{
    auto transform = RSBaseRenderUtil::ClockwiseToAntiClockwiseTransform(GraphicTransformType::GRAPHIC_ROTATE_90);
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_ROTATE_270);
}

/*
 * @tc.name: ClockwiseToAntiClockwiseTransform
 * @tc.desc: Test ClockwiseToAntiClockwiseTransform GRAPHIC_ROTATE_270
 * @tc.type: FUNC
 * @tc.require: I6HE0M
*/
HWTEST_F(RSBaseRenderUtilTest, ClockwiseToAntiClockwiseTransform_002, Function | SmallTest | Level2)
{
    auto transform = RSBaseRenderUtil::ClockwiseToAntiClockwiseTransform(GraphicTransformType::GRAPHIC_ROTATE_270);
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_ROTATE_90);
}

/*
 * @tc.name: ClockwiseToAntiClockwiseTransform
 * @tc.desc: Test ClockwiseToAntiClockwiseTransform GRAPHIC_FLIP_H_ROT90
 * @tc.type: FUNC
 * @tc.require: I6HE0M
*/
HWTEST_F(RSBaseRenderUtilTest, ClockwiseToAntiClockwiseTransform_003, Function | SmallTest | Level2)
{
    auto transform = RSBaseRenderUtil::ClockwiseToAntiClockwiseTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT90);
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_FLIP_V_ROT90);
}

/*
 * @tc.name: ClockwiseToAntiClockwiseTransform
 * @tc.desc: Test ClockwiseToAntiClockwiseTransform GRAPHIC_FLIP_H_ROT270
 * @tc.type: FUNC
 * @tc.require: I6HE0M
*/
HWTEST_F(RSBaseRenderUtilTest, ClockwiseToAntiClockwiseTransform_004, Function | SmallTest | Level2)
{
    auto transform = RSBaseRenderUtil::ClockwiseToAntiClockwiseTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT270);
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_FLIP_V_ROT270);
}

/*
 * @tc.name: ClockwiseToAntiClockwiseTransform
 * @tc.desc: Test ClockwiseToAntiClockwiseTransform GRAPHIC_FLIP_V_ROT90
 * @tc.type: FUNC
 * @tc.require: I6HE0M
*/
HWTEST_F(RSBaseRenderUtilTest, ClockwiseToAntiClockwiseTransform_005, Function | SmallTest | Level2)
{
    auto transform = RSBaseRenderUtil::ClockwiseToAntiClockwiseTransform(GraphicTransformType::GRAPHIC_FLIP_V_ROT90);
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_FLIP_H_ROT90);
}

/*
 * @tc.name: ClockwiseToAntiClockwiseTransform
 * @tc.desc: Test ClockwiseToAntiClockwiseTransform GRAPHIC_FLIP_V_ROT270
 * @tc.type: FUNC
 * @tc.require: I6HE0M
*/
HWTEST_F(RSBaseRenderUtilTest, ClockwiseToAntiClockwiseTransform_006, Function | SmallTest | Level2)
{
    auto transform = RSBaseRenderUtil::ClockwiseToAntiClockwiseTransform(GraphicTransformType::GRAPHIC_FLIP_V_ROT270);
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_FLIP_H_ROT270);
}

/*
 * @tc.name: ClockwiseToAntiClockwiseTransform
 * @tc.desc: Test ClockwiseToAntiClockwiseTransform GRAPHIC_ROTATE_NONE
 * @tc.type: FUNC
 * @tc.require: I6HE0M
 */
HWTEST_F(RSBaseRenderUtilTest, ClockwiseToAntiClockwiseTransform_007, Function | SmallTest | Level2)
{
    auto transform = RSBaseRenderUtil::ClockwiseToAntiClockwiseTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    ASSERT_EQ(transform, GraphicTransformType::GRAPHIC_ROTATE_NONE);
}

/*
 * @tc.name: GetSurfaceTransformMatrix_001
 * @tc.desc: Test GetSurfaceTransformMatrix GRAPHIC_ROTATE_90
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderUtilTest, GetSurfaceTransformMatrix_001, TestSize.Level2)
{
    RectF bounds(1, 2, 3, 4);
    Drawing::Matrix matrix;
    const float boundsHeight = bounds.GetHeight();
    matrix.PreTranslate(0, boundsHeight);
    matrix.PreRotate(-90);
    GraphicTransformType rotationTransform = GraphicTransformType::GRAPHIC_ROTATE_90;
    ASSERT_EQ(matrix, RSBaseRenderUtil::GetSurfaceTransformMatrix(rotationTransform, bounds));
}

/*
 * @tc.name: GetSurfaceTransformMatrix_002
 * @tc.desc: Test GetSurfaceTransformMatrix GRAPHIC_ROTATE_180
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderUtilTest, GetSurfaceTransformMatrix_002, TestSize.Level2)
{
    RectF bounds(1, 2, 3, 4);
    Drawing::Matrix matrix;
    const float boundsWidth = bounds.GetWidth();
    const float boundsHeight = bounds.GetHeight();
    matrix.PreTranslate(boundsWidth, boundsHeight);
    matrix.PreRotate(-180);
    GraphicTransformType rotationTransform = GraphicTransformType::GRAPHIC_ROTATE_180;
    ASSERT_EQ(matrix, RSBaseRenderUtil::GetSurfaceTransformMatrix(rotationTransform, bounds));
}

/*
 * @tc.name: GetSurfaceTransformMatrix_003
 * @tc.desc: Test GetSurfaceTransformMatrix GRAPHIC_ROTATE_270
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderUtilTest, GetSurfaceTransformMatrix_003, TestSize.Level2)
{
    RectF bounds(1, 2, 3, 4);
    Drawing::Matrix matrix;
    const float boundsWidth = bounds.GetWidth();
    matrix.PreTranslate(boundsWidth, 0);
    matrix.PreRotate(-270);
    GraphicTransformType rotationTransform = GraphicTransformType::GRAPHIC_ROTATE_270;
    ASSERT_EQ(matrix, RSBaseRenderUtil::GetSurfaceTransformMatrix(rotationTransform, bounds));
}

/*
 * @tc.name: WriteToPng_001
 * @tc.desc: Test WriteToPng
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderUtilTest, WriteToPng_001, TestSize.Level2)
{
    std::string filename = "";
    WriteToPngParam param;
    ASSERT_EQ(false, RSBaseRenderUtil::WriteToPng(filename, param));
}

/*
 * @tc.name: WriteToPng_002
 * @tc.desc: Test WriteToPng
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderUtilTest, WriteToPng_002, TestSize.Level2)
{
    std::string filename = "/test.png";
    WriteToPngParam param;
    ASSERT_EQ(false, RSBaseRenderUtil::WriteToPng(filename, param));
}

/*
 * @tc.name: RotateEnumToInt_001
 * @tc.desc: Test RotateEnumToInt GRAPHIC_FLIP_H
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderUtilTest, RotateEnumToInt_001, TestSize.Level2)
{
    int angle = 0;
    GraphicTransformType flip = GraphicTransformType::GRAPHIC_FLIP_H;
    ASSERT_EQ(flip, RSBaseRenderUtil::RotateEnumToInt(angle, flip));
}

/*
 * @tc.name: ConvertBufferToBitmapTest
 * @tc.desc: Test ConvertBufferToBitmap when dstGamut is different from srcGamut
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSBaseRenderUtilTest, ConvertBufferToBitmapTest, TestSize.Level2)
{
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    ASSERT_EQ(buffer->GetBufferHandle(), nullptr);
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
    };
    GSError ret = buffer->Alloc(requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    std::vector<uint8_t> newBuffer;
    GraphicColorGamut dstGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3;
    Drawing::Bitmap bitmap;
    ASSERT_EQ(true, RSBaseRenderUtil::ConvertBufferToBitmap(buffer, newBuffer, dstGamut, bitmap));
}
} // namespace OHOS::Rosen
