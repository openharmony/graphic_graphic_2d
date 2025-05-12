/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "foundation/graphic/graphic_surface/surface/include/surface_buffer_impl.h"
#include "gtest/gtest.h"
#include "image_source.h"
#include "pixel_map.h"
#include "surface_buffer_impl.h"

#include "pipeline/rs_draw_cmd.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDrawCmdTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<OHOS::Media::PixelMap> pixelMap;
    static inline Drawing::AdaptiveImageInfo imageInfo;
    static inline NodeId id = 1;
};

void RSDrawCmdTest::SetUpTestCase() {}
void RSDrawCmdTest::TearDownTestCase() {}
void RSDrawCmdTest::SetUp() {}
void RSDrawCmdTest::TearDown() {}

/**
 * @tc.name: SetNodeId001
 * @tc.desc: test results of SetNodeId
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, SetNodeId001, TestSize.Level1)
{
    RSExtendImageObject extendImageObject(pixelMap, imageInfo);
    NodeId id = 1;
    extendImageObject.SetNodeId(id);

    RSExtendImageObject extendImageObjectTwo;
    extendImageObjectTwo.rsImage_ = std::make_shared<RSImage>();
    extendImageObjectTwo.SetNodeId(id);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetPaintTest
 * @tc.desc: test results of SetPaint
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, SetPaintTest, TestSize.Level1)
{
    RSExtendImageObject extendImageObject;
    Drawing::Paint paint;
    extendImageObject.SetPaint(paint);
    extendImageObject.rsImage_ = std::make_shared<RSImage>();
    extendImageObject.SetPaint(paint);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetDrawingImageFromSurfaceBuffer
 * @tc.desc: test results of GetDrawingImageFromSurfaceBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, GetDrawingImageFromSurfaceBuffer, TestSize.Level1)
{
    RSExtendImageObject extendImageObject(pixelMap, imageInfo);
    Drawing::Canvas canvas(1, 1);
    OHOS::SurfaceBuffer* surfaceBuffer = SurfaceBuffer::Create().GetRefPtr();
    ASSERT_FALSE(extendImageObject.GetDrawingImageFromSurfaceBuffer(canvas, surfaceBuffer));
    ASSERT_FALSE(extendImageObject.GetDrawingImageFromSurfaceBuffer(canvas, nullptr));
    extendImageObject.eglImage_ = EGL_NO_IMAGE_KHR;
    ASSERT_FALSE(extendImageObject.GetDrawingImageFromSurfaceBuffer(canvas, surfaceBuffer));
    extendImageObject.texId_ = 0U;
    ASSERT_FALSE(extendImageObject.GetDrawingImageFromSurfaceBuffer(canvas, surfaceBuffer));
    extendImageObject.nativeWindowBuffer_ = nullptr;
    ASSERT_FALSE(extendImageObject.GetDrawingImageFromSurfaceBuffer(canvas, surfaceBuffer));
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: test results of Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Unmarshalling001, TestSize.Level1)
{
    RSExtendImageObject extendImageObject(pixelMap, imageInfo);
    Parcel parcel;
    ASSERT_EQ(extendImageObject.Unmarshalling(parcel), nullptr);
}

/**
 * @tc.name: Playback001
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    Drawing::SamplingOptions sampling;
    bool isBackground = true;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    std::shared_ptr<Drawing::Data> data = std::make_shared<Drawing::Data>();
    Drawing::AdaptiveImageInfo imageInfo;
    RSExtendImageObject extendImageObject(image, data, imageInfo);
    extendImageObject.Playback(canvas, rect, sampling, isBackground);
    ASSERT_TRUE(true);
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::shared_ptr<Media::PixelMap> pixelMapT = Media::PixelMap::Create(opts);
    RSExtendImageObject extendImageObjectT(pixelMapT, imageInfo);
    extendImageObjectT.Playback(canvas, rect, sampling, isBackground);
    pixelMapT->isAstc_ = true;
    extendImageObjectT.Playback(canvas, rect, sampling, isBackground);
}

/**
 * @tc.name: PreProcessPixelMap
 * @tc.desc: test results of PreProcessPixelMap
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, PreProcessPixelMap, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::shared_ptr<Media::PixelMap> pixelMapT = Media::PixelMap::Create(opts);
    Drawing::SamplingOptions sampling;
    RSExtendImageObject extendImageObject(pixelMap, imageInfo);
    extendImageObject.PreProcessPixelMap(canvas, pixelMapT, sampling);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Playback002
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback002, TestSize.Level1)
{
    Drawing::Rect src;
    Drawing::Rect dst;
    RSExtendImageBaseObj extendImageBaseObj(pixelMap, src, dst);
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    Drawing::SamplingOptions sampling;
    extendImageBaseObj.Playback(canvas, rect, sampling);

    RSExtendImageBaseObj obj;
    obj.Playback(canvas, rect, sampling);
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::shared_ptr<Media::PixelMap> pixelMapT = Media::PixelMap::Create(opts);
    RSExtendImageBaseObj extendImageBaseObjT(pixelMapT, src, dst);
    extendImageBaseObjT.rsImage_ = std::make_shared<RSImage>();
    extendImageBaseObjT.Playback(canvas, rect, sampling);
    extendImageBaseObjT.SetNodeId(id);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetNodeId002
 * @tc.desc: test results of SetNodeId
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, SetNodeId002, TestSize.Level1)
{
    Drawing::Rect src;
    Drawing::Rect dst;
    RSExtendImageBaseObj extendImageBaseObj(pixelMap, src, dst);
    extendImageBaseObj.SetNodeId(id);

    RSExtendImageBaseObj obj;
    obj.rsImage_ = std::make_shared<RSImage>();
    obj.SetNodeId(id);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc: test results of Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Unmarshalling002, TestSize.Level1)
{
    Drawing::Rect src;
    Drawing::Rect dst;
    RSExtendImageBaseObj extendImageBaseObj(pixelMap, src, dst);
    Parcel parcel;
    ASSERT_EQ(extendImageBaseObj.Unmarshalling(parcel), nullptr);
}

/**
 * @tc.name: Unmarshalling003
 * @tc.desc: test results of Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Unmarshalling003, TestSize.Level1)
{
    Drawing::RecordingCanvas::DrawFunc drawFunc;
    RSExtendDrawFuncObj extendDrawFuncObj(std::move(drawFunc));
    Parcel parcel;
    extendDrawFuncObj.Marshalling(parcel);
    ASSERT_EQ(extendDrawFuncObj.Unmarshalling(parcel), nullptr);
}

/**
 * @tc.name: Unmarshalling004
 * @tc.desc: test results of Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Unmarshalling004, TestSize.Level1)
{
    std::shared_ptr<Drawing::Image> image;
    std::shared_ptr<Drawing::Data> data;
    Drawing::SamplingOptions sampling;
    Drawing::OpDataHandle objectHandle;
    Drawing::PaintHandle paintHandle;
    Drawing::Paint paint;
    Drawing::DrawImageWithParmOpItem drawImageWithParmOpItem(image, data, imageInfo, sampling, paint);
    Drawing::DrawCmdList cmdList;
    drawImageWithParmOpItem.Marshalling(cmdList);
    Drawing::DrawPixelMapWithParmOpItem::ConstructorHandle handle(objectHandle, sampling, paintHandle);
    ASSERT_NE(drawImageWithParmOpItem.Unmarshalling(cmdList, (void*)(&handle)), nullptr);
}

/**
 * @tc.name: Playback003
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback003, TestSize.Level1)
{
    Drawing::RecordingCanvas::DrawFunc drawFunc;
    RSExtendDrawFuncObj extendDrawFuncObj(std::move(drawFunc));
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    extendDrawFuncObj.Playback(&canvas, &rect);

    drawFunc = [](Drawing::Canvas* canvas, const Drawing::Rect* rect) {};
    RSExtendDrawFuncObj obj(std::move(drawFunc));
    obj.Playback(&canvas, &rect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Playback004
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback004, TestSize.Level1)
{
    std::shared_ptr<Drawing::Image> image;
    std::shared_ptr<Drawing::Data> data;
    Drawing::SamplingOptions sampling;
    Drawing::Paint paint;
    Drawing::DrawImageWithParmOpItem drawImageWithParmOpItem(image, data, imageInfo, sampling, paint);
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    drawImageWithParmOpItem.Playback(&canvas, &rect);
    drawImageWithParmOpItem.objectHandle_ = nullptr;
    drawImageWithParmOpItem.Playback(&canvas, &rect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetNodeId003
 * @tc.desc: test results of SetNodeId
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, SetNodeId003, TestSize.Level1)
{
    std::shared_ptr<Drawing::Image> image;
    std::shared_ptr<Drawing::Data> data;
    Drawing::SamplingOptions sampling;
    Drawing::Paint paint;
    Drawing::DrawImageWithParmOpItem drawImageWithParmOpItem(image, data, imageInfo, sampling, paint);
    drawImageWithParmOpItem.SetNodeId(id);

    drawImageWithParmOpItem.objectHandle_ = nullptr;
    drawImageWithParmOpItem.SetNodeId(id);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Playback005
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback005, TestSize.Level1)
{
    Drawing::DrawCmdList cmdList;
    Drawing::OpDataHandle objectHandle;
    Drawing::SamplingOptions sampling;
    Drawing::PaintHandle paintHandle;
    Drawing::DrawPixelMapWithParmOpItem::ConstructorHandle handle(objectHandle, sampling, paintHandle);
    Drawing::DrawPixelMapWithParmOpItem drawPixelMapWithParmOpItem(cmdList, &handle);
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    drawPixelMapWithParmOpItem.Playback(&canvas, &rect);
    drawPixelMapWithParmOpItem.objectHandle_ = nullptr;
    drawPixelMapWithParmOpItem.Playback(&canvas, &rect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetNodeId004
 * @tc.desc: test results of SetNodeId
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, SetNodeId004, TestSize.Level1)
{
    Drawing::SamplingOptions sampling;
    Drawing::Paint paint;
    Drawing::DrawPixelMapWithParmOpItem drawPixelMapWithParmOpItem(pixelMap, imageInfo, sampling, paint);
    drawPixelMapWithParmOpItem.SetNodeId(id);

    drawPixelMapWithParmOpItem.objectHandle_ = nullptr;
    drawPixelMapWithParmOpItem.SetNodeId(id);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Playback006
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback006, TestSize.Level1)
{
    Drawing::DrawCmdList list;
    Drawing::OpDataHandle objectHandle;
    Drawing::SamplingOptions sampling;
    Drawing::PaintHandle paintHandle;
    Drawing::DrawPixelMapRectOpItem::ConstructorHandle constructorHandle(objectHandle, sampling,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT, paintHandle);
    Drawing::DrawPixelMapRectOpItem drawPixelMapRectOpItem(list, &constructorHandle);
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    drawPixelMapRectOpItem.Playback(&canvas, &rect);

    drawPixelMapRectOpItem.objectHandle_ = nullptr;
    drawPixelMapRectOpItem.Playback(&canvas, &rect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetNodeId005
 * @tc.desc: test results of SetNodeId
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, SetNodeId005, TestSize.Level1)
{
    Drawing::DrawCmdList list;
    Drawing::OpDataHandle objectHandle;
    Drawing::SamplingOptions sampling;
    Drawing::PaintHandle paintHandle;
    Drawing::DrawPixelMapRectOpItem::ConstructorHandle constructorHandle(objectHandle, sampling,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT, paintHandle);
    Drawing::DrawPixelMapRectOpItem drawPixelMapRectOpItem(list, &constructorHandle);
    drawPixelMapRectOpItem.SetNodeId(id);

    drawPixelMapRectOpItem.objectHandle_ = nullptr;
    drawPixelMapRectOpItem.SetNodeId(id);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Playback007
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback007, TestSize.Level1)
{
    Drawing::DrawCmdList list;
    uint32_t funcObjectId = 1;
    Drawing::DrawFuncOpItem::ConstructorHandle constructorHandle(funcObjectId);
    Drawing::RecordingCanvas::DrawFunc drawFunc = [](Drawing::Canvas* canvas, const Drawing::Rect* rect) {};
    Drawing::DrawFuncOpItem drawFuncOpItem(std::move(drawFunc));
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    drawFuncOpItem.Playback(&canvas, &rect);
    ASSERT_NE(drawFuncOpItem.objectHandle_, nullptr);

    drawFuncOpItem.objectHandle_ = nullptr;
    drawFuncOpItem.Playback(&canvas, &rect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Playback008
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback008, TestSize.Level1)
{
    Drawing::DrawCmdList list;
    uint32_t surfaceBufferId = 1;
    int offSetX = 1;
    int offSetY = 1;
    int width = 1;
    int height = 1;
    pid_t pid = {};
    uint64_t uid = {};
    GraphicTransformType transform =
        GraphicTransformType::GRAPHIC_ROTATE_NONE;
    Drawing::Rect srcRect { 0, 0, 1, 1 };
    Drawing::PaintHandle paintHandle;
    Drawing::DrawSurfaceBufferOpItem::ConstructorHandle constructorHandle(
        surfaceBufferId, offSetX, offSetY, width, height, pid, uid, transform, srcRect, paintHandle);
    Drawing::DrawSurfaceBufferOpItem drawSurfaceBufferOpItem(list, &constructorHandle);
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    drawSurfaceBufferOpItem.Playback(&canvas, &rect);
    ASSERT_EQ(drawSurfaceBufferOpItem.nativeWindowBuffer_, nullptr);
  

    if (drawSurfaceBufferOpItem.nativeWindowBuffer_) {
        std::cerr << "1" << std::endl;
    } else {
        std::cerr << "0" << std::endl;
    }
}

/**
 * @tc.name: Playback009
 * @tc.desc: test results of CreateBitmapFormat
 * @tc.type:FUNC
 * @tc.require: issueIATYWQ
 */
HWTEST_F(RSDrawCmdTest, Playback009, TestSize.Level1)
{
    Drawing::DrawCmdList list;
    uint32_t surfaceBufferId = 1;
    int offSetX = 1;
    int offSetY = 1;
    int width = 1;
    int height = 1;
    pid_t pid = {};
    uint64_t uid = {};
    GraphicTransformType transform =
        GraphicTransformType::GRAPHIC_ROTATE_NONE;
    Drawing::Rect srcRect { 0, 0, 1, 1 };
    Drawing::PaintHandle paintHandle;
    Drawing::DrawSurfaceBufferOpItem::ConstructorHandle constructorHandle(
        surfaceBufferId, offSetX, offSetY, width, height, pid, uid, transform, srcRect, paintHandle);
    Drawing::DrawSurfaceBufferOpItem drawSurfaceBufferOpItem(list, &constructorHandle);
    Drawing::BitmapFormat bitmapFormat =
        drawSurfaceBufferOpItem.CreateBitmapFormat(OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_RGBA_8888);
    ASSERT_EQ(bitmapFormat.colorType, Drawing::ColorType::COLORTYPE_RGBA_8888);
    ASSERT_EQ(bitmapFormat.alphaType, Drawing::AlphaType::ALPHATYPE_PREMUL);
    bitmapFormat =
        drawSurfaceBufferOpItem.CreateBitmapFormat(OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_RGBX_8888);
    ASSERT_EQ(bitmapFormat.colorType, Drawing::ColorType::COLORTYPE_RGB_888X);
    ASSERT_EQ(bitmapFormat.alphaType, Drawing::AlphaType::ALPHATYPE_OPAQUE);
}

/**
 * @tc.name: CreateSamplingOptions
 * @tc.desc: test results of CreateSamplingOptions
 * @tc.type:FUNC
 * @tc.require: issueIATYWQ
 */
HWTEST_F(RSDrawCmdTest, CreateSamplingOptions, TestSize.Level1)
{
    Drawing::Matrix matrix;

    Drawing::scalar tx = 100;
    Drawing::scalar ty = 100;
    matrix.Translate(tx, ty);
    Drawing::SamplingOptions samplingOptions;
    samplingOptions = Drawing::DrawSurfaceBufferOpItem::CreateSamplingOptions(matrix);
    ASSERT_EQ(samplingOptions.GetFilterMode(), Drawing::FilterMode::NEAREST);
    ASSERT_EQ(samplingOptions.GetMipmapMode(), Drawing::MipmapMode::NONE);

    Drawing::scalar sx = 10;
    Drawing::scalar sy = 10;
    matrix.SetScale(sx, sy);
    samplingOptions = Drawing::DrawSurfaceBufferOpItem::CreateSamplingOptions(matrix);
    ASSERT_EQ(samplingOptions.GetFilterMode(), Drawing::FilterMode::LINEAR);
    ASSERT_EQ(samplingOptions.GetMipmapMode(), Drawing::MipmapMode::NONE);

    Drawing::scalar rd = 30;
    matrix.Rotate(rd, 0, 0);
    samplingOptions = Drawing::DrawSurfaceBufferOpItem::CreateSamplingOptions(matrix);
    ASSERT_EQ(samplingOptions.GetFilterMode(), Drawing::FilterMode::LINEAR);
    ASSERT_EQ(samplingOptions.GetMipmapMode(), Drawing::MipmapMode::NONE);
}

#ifdef RS_ENABLE_VK
/**
 * @tc.name: DrawHybridPixelMapOpItem_Unmarshalling
 * @tc.desc: test results of Unmarshalling
 * @tc.type:FUNC
 * @tc.require:issueIC3UZH
 */
HWTEST_F(RSDrawCmdTest, Unmarshalling005, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelMap;
    Drawing::AdaptiveImageInfo rsImageInfo;
    Drawing::SamplingOptions sampling;
    Drawing::OpDataHandle objectHandle;
    Drawing::PaintHandle paintHandle;
    Drawing::Paint paint;
    Drawing::DrawHybridPixelMapOpItem drawHybridPixelMapOpItem(pixelMap, rsImageInfo, sampling, paint);
    Drawing::DrawCmdList cmdList;
    drawHybridPixelMapOpItem.Marshalling(cmdList);
    Drawing::DrawHybridPixelMapOpItem::ConstructorHandle handle(objectHandle, sampling, paintHandle, -1, false);
    ASSERT_NE(drawHybridPixelMapOpItem.Unmarshalling(cmdList, (void*)(&handle)), nullptr);
}
 
/**
 * @tc.name: DrawHybridPixelMapOpItem_SetNodeId
 * @tc.desc: test results of SetNodeId
 * @tc.type:FUNC
 * @tc.require:issueIC3UZH
 */
HWTEST_F(RSDrawCmdTest, SetNodeId006, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelMap;
    Drawing::AdaptiveImageInfo rsImageInfo;
    Drawing::SamplingOptions sampling;
    Drawing::Paint paint;
    Drawing::DrawHybridPixelMapOpItem drawHybridPixelMapOpItem(pixelMap, rsImageInfo, sampling, paint);
    ASSERT_NE(drawHybridPixelMapOpItem.objectHandle_, nullptr);
    drawHybridPixelMapOpItem.SetNodeId(id);
    drawHybridPixelMapOpItem.objectHandle_ = nullptr;
    drawHybridPixelMapOpItem.SetNodeId(id);
}
 
/**
 * @tc.name: DrawHybridPixelMapOpItem_Playback
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require:issueIC3UZH
 */
HWTEST_F(RSDrawCmdTest, Playback010, TestSize.Level1)
{
    Drawing::DrawCmdList cmdList;
    Drawing::OpDataHandle objectHandle;
    Drawing::SamplingOptions sampling;
    Drawing::PaintHandle paintHandle;
    Drawing::DrawHybridPixelMapOpItem::ConstructorHandle handle(objectHandle, sampling, paintHandle, -1, false);
    Drawing::DrawHybridPixelMapOpItem drawHybridPixelMapOpItem(cmdList, &handle);
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    ASSERT_EQ(drawHybridPixelMapOpItem.objectHandle_, nullptr);
    drawHybridPixelMapOpItem.Playback(&canvas, &rect);
    drawHybridPixelMapOpItem.objectHandle_ = nullptr;
    drawHybridPixelMapOpItem.Playback(&canvas, &rect);
}
#endif
} // namespace OHOS::Rosen