/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "pixel_map.h"
#include "skia_bitmap.h"

#include "render/rs_image_base.h"
#include "render/rs_pixel_map_util.h"
#include "render/rs_image_cache.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSImageBaseUintTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSImageBaseUintTest::SetUpTestCase() {}
void RSImageBaseUintTest::TearDownTestCase() {}
void RSImageBaseUintTest::SetUp() {}
void RSImageBaseUintTest::TearDown() {}
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
Drawing::ColorType GetColorTypeWithVKFormat(VkFormat vkFormat);
#endif

/**
 * @tc.name: DrawImageTest
 * @tc.desc: Verify function DrawImage
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageBaseUintTest, DrawImageTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    Drawing::SamplingOptions samplingOptions;
    imageBase->DrawImage(canvas, samplingOptions);
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    imageBase->DrawImage(canvas, samplingOptions);
    EXPECT_NE(imageBase->image_, nullptr);
}

/**
 * @tc.name: SetImageTest
 * @tc.desc: Verify function SetImage
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageBaseUintTest, SetImageTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    EXPECT_EQ(imageBase->image_, image);
}

/**
 * @tc.name: SetPixelMapTest
 * @tc.desc: Verify function SetPixelMap
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageBaseUintTest, SetPixelMapTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelmap = std::make_shared<Media::PixelMap>();
    imageBase->SetPixelMap(pixelmap);
    EXPECT_EQ(imageBase->pixelMap_, pixelmap);
}

/**
 * @tc.name: DumpPictureTest
 * @tc.desc: Verify function DumpPicture
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageBaseUintTest, DumpPictureTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    DfxString info;
    imageBase->DumpPicture(info);
    auto pixelmap = std::make_shared<Media::PixelMap>();
    imageBase->SetPixelMap(pixelmap);
    imageBase->DumpPicture(info);
    EXPECT_NE(imageBase->pixelMap_, nullptr);
}

/**
 * @tc.name: SetSrcRectTest
 * @tc.desc: Verify function SetSrcRect
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageBaseUintTest, SetSrcRectTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    RectF srcRect;
    srcRect.Clear();
    imageBase->SetSrcRect(srcRect);
    EXPECT_EQ(imageBase->srcRect_.GetLeft(), 0);
}

/**
 * @tc.name: SetDstRectTest
 * @tc.desc: Verify function SetDstRect
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageBaseUintTest, SetDstRectTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    RectF dstRect;
    imageBase->SetDstRect(dstRect);
    imageBase->SetDstRect(imageBase->dstRect_);
    EXPECT_FALSE(imageBase->isDrawn_);
}

/**
 * @tc.name: SetImagePixelAddrTest
 * @tc.desc: Verify function SetImagePixelAddr
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageBaseUintTest, SetImagePixelAddrTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    void* addr = nullptr;
    imageBase->SetImagePixelAddr(addr);
    EXPECT_EQ(imageBase->imagePixelAddr_, nullptr);
}

/**
 * @tc.name: UpdateNodeIdToPictureTest
 * @tc.desc: Verify function UpdateNodeIdToPicture
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageBaseUintTest, UpdateNodeIdToPictureTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    imageBase->UpdateNodeIdToPicture(0);
    auto pixelmap = std::make_shared<Media::PixelMap>();
    imageBase->SetPixelMap(pixelmap);
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    imageBase->UpdateNodeIdToPicture(1);
    EXPECT_NE(imageBase->pixelMap_, nullptr);
}

/**
 * @tc.name: MarkRenderServiceImageTest
 * @tc.desc: Verify function MarkRenderServiceImage
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageBaseUintTest, MarkRenderServiceImageTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    imageBase->MarkRenderServiceImage();
    EXPECT_TRUE(imageBase->renderServiceImage_);
}

/**
 * @tc.name: ConvertPixelMapToDrawingImageTest
 * @tc.desc: Verify function ConvertPixelMapToDrawingImage
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageBaseUintTest, ConvertPixelMapToDrawingImageTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelmap = std::make_shared<Media::PixelMap>();
    imageBase->SetPixelMap(pixelmap);
    imageBase->ConvertPixelMapToDrawingImage(true);
    EXPECT_EQ(imageBase->image_, nullptr);
}

/**
 * @tc.name: GenUniqueIdTest
 * @tc.desc: Verify function GenUniqueId
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageBaseUintTest, GenUniqueIdTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    imageBase->GenUniqueId(1);
    EXPECT_NE(imageBase->uniqueId_, 0);
}

/**
 * @tc.name: GetPixelMapTest
 * @tc.desc: Verify function GetPixelMap
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageBaseUintTest, GetPixelMapTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    EXPECT_EQ(imageBase->GetPixelMap(), nullptr);
}

/**
 * @tc.name: PurgeTest
 * @tc.desc: Verify function Purge
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageBaseUintTest, PurgeTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    ASSERT_NE(imageBase, nullptr);
    imageBase->Purge();
}

/**
 * @tc.name: SetDmaImageTest
 * @tc.desc: Verify function SetDmaImage
 * @tc.type:FUNC
 * @tc.require: issuesI9MO9U
 */
HWTEST_F(RSImageBaseUintTest, SetDmaImageTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    ASSERT_NE(imageBase, nullptr);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    imageBase->SetDmaImage(image);
    EXPECT_EQ(imageBase->image_, image);
}

/**
 * @tc.name: MarkYUVImageTest
 * @tc.desc: Verify function MarkYUVImage
 * @tc.type:FUNC
 * @tc.require: issuesI9SX8Q
 */
HWTEST_F(RSImageBaseUintTest, MarkYUVImageTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    ASSERT_NE(imageBase, nullptr);
    imageBase->MarkYUVImage();
    EXPECT_TRUE(imageBase->isYUVImage_);
}

/**
 * @tc.name: UnmarshallingTest
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9MO9U
 */
HWTEST_F(RSImageBaseUintTest, UnmarshallingTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    ASSERT_NE(imageBase, nullptr);
    Parcel parcel;
    EXPECT_EQ(imageBase->Unmarshalling(parcel), nullptr);
}

/**
 * @tc.name: ProcessYUVImageTest
 * @tc.desc: Verify function ProcessYUVImage
 * @tc.type:FUNC
 * @tc.require: issuesI9TOXM
 */
HWTEST_F(RSImageBaseUintTest, ProcessYUVImageTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    ASSERT_NE(imageBase, nullptr);
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    imageBase->ProcessYUVImage(gpuContext);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    imageBase->ProcessYUVImage(gpuContext);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::NV12;
    imageBase->SetPixelMap(pixelMap);
    imageBase->ProcessYUVImage(gpuContext);
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelMap), nullptr);
}

/**
 * @tc.name: MarkRenderServiceImageTest001
 * @tc.desc: Verify function MarkRenderServiceImage
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, MarkRenderServiceImageTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->allocatorType_ = Media::AllocatorType::SHARE_MEM_ALLOC;
    pixelMap->editable_ = true;
    pixelMap->isAstc_ = true;
    imageBase->SetPixelMap(pixelMap);
    imageBase->MarkRenderServiceImage();
    EXPECT_TRUE(imageBase->renderServiceImage_);
}

/**
 * @tc.name: MarkRenderServiceImageTest002
 * @tc.desc: Verify function MarkRenderServiceImage
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, MarkRenderServiceImageTest002, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->allocatorType_ = Media::AllocatorType::HEAP_ALLOC;
    pixelMap->editable_ = true;
    pixelMap->isAstc_ = true;
    imageBase->SetPixelMap(pixelMap);
    imageBase->MarkRenderServiceImage();
    EXPECT_TRUE(imageBase->renderServiceImage_);
}

/**
 * @tc.name: MarkRenderServiceImageTest003
 * @tc.desc: Verify function MarkRenderServiceImage
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, MarkRenderServiceImageTest003, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    auto ImageInfo = std::make_shared<Media::ImageInfo>();
    ImageInfo->pixelFormat = Media::PixelFormat::RGBA_F16;
    imageBase->canPurgeShareMemFlag_ = RSImageBase::CanPurgeFlag::UNINITED;
    pixelMap->allocatorType_ = Media::AllocatorType::SHARE_MEM_ALLOC;
    pixelMap->editable_ = true;
    pixelMap->isAstc_ = true;
    imageBase->SetPixelMap(pixelMap);
    imageBase->MarkRenderServiceImage();
    EXPECT_TRUE(imageBase->renderServiceImage_);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
/**
 * @tc.name: GetColorTypeWithVKFormatTest
 * @tc.desc: Verify function GetColorTypeWithVKFormat
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, GetColorTypeWithVKFormatTest, TestSize.Level1)
{
    VkFormat vkFormat = VK_FORMAT_R8G8B8A8_UNORM;
    EXPECT_EQ(GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGBA_8888);
}

/**
 * @tc.name: GetColorTypeWithVKFormatTest001
 * @tc.desc: Verify function GetColorTypeWithVKFormat
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, GetColorTypeWithVKFormatTest001, TestSize.Level1)
{
    VkFormat vkFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    EXPECT_EQ(GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGBA_F16);
}

/**
 * @tc.name: GetColorTypeWithVKFormatTest002
 * @tc.desc: Verify function GetColorTypeWithVKFormat
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, GetColorTypeWithVKFormatTest002, TestSize.Level1)
{
    VkFormat vkFormat = VK_FORMAT_R5G6B5_UNORM_PACK16;
    EXPECT_EQ(GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGB_565);
}

/**
 * @tc.name: GetColorTypeWithVKFormatTest003
 * @tc.desc: Verify function GetColorTypeWithVKFormat
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, GetColorTypeWithVKFormatTest003, TestSize.Level1)
{
    VkFormat vkFormat = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
    EXPECT_EQ(GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGBA_1010102);
}

/**
 * @tc.name: GetColorTypeWithVKFormatTest004
 * @tc.desc: Verify function GetColorTypeWithVKFormat
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, GetColorTypeWithVKFormatTest004, TestSize.Level1)
{
    VkFormat vkFormat = VK_FORMAT_R32_SFLOAT;
    EXPECT_EQ(GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGBA_8888);
}
#endif

/**
 * @tc.name: PurgeTest001
 * @tc.desc: Verify function Purge
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, PurgeTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageBase->SetPixelMap(pixelMap);
    ASSERT_NE(imageBase, nullptr);
    imageBase->Purge();
}

/**
 * @tc.name: PurgeTest002
 * @tc.desc: Verify function Purge
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, PurgeTest002, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    imageBase->uniqueId_ = 1;
    imageBase->canPurgeShareMemFlag_ = RSImageBase::CanPurgeFlag::ENABLED;
    ASSERT_NE(imageBase, nullptr);
    imageBase->Purge();
}

/**
 * @tc.name: PurgeTest003
 * @tc.desc: Verify function Purge
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, PurgeTest003, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    imageBase->uniqueId_ = -1;
    imageBase->canPurgeShareMemFlag_ = RSImageBase::CanPurgeFlag::ENABLED;
    ASSERT_NE(imageBase, nullptr);
    imageBase->Purge();
}

/**
 * @tc.name: PurgeTest004
 * @tc.desc: Verify function Purge
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, PurgeTest004, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    imageBase->uniqueId_ = 1;
    imageBase->canPurgeShareMemFlag_ = RSImageBase::CanPurgeFlag::DISABLED;
    ASSERT_NE(imageBase, nullptr);
    imageBase->Purge();
}

/**
 * @tc.name: PurgeTest005
 * @tc.desc: Verify function Purge
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, PurgeTest005, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    imageBase->uniqueId_ = -1;
    imageBase->canPurgeShareMemFlag_ = RSImageBase::CanPurgeFlag::DISABLED;
    ASSERT_NE(imageBase, nullptr);
    imageBase->Purge();
}

/**
 * @tc.name: PurgeTest006
 * @tc.desc: Verify function Purge
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, PurgeTest006, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    imageBase->uniqueId_ = 1;
    imageBase->canPurgeShareMemFlag_ = RSImageBase::CanPurgeFlag::ENABLED;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->allocatorType_ = Media::AllocatorType::SHARE_MEM_ALLOC;
    pixelMap->isUnMap_ = true;
    imageBase->SetPixelMap(pixelMap);
    ASSERT_NE(imageBase, nullptr);
    imageBase->Purge();
}

/**
 * @tc.name: PurgeTest007
 * @tc.desc: Verify function Purge
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, PurgeTest007, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    imageBase->uniqueId_ = 1;
    imageBase->canPurgeShareMemFlag_ = RSImageBase::CanPurgeFlag::ENABLED;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->allocatorType_ = Media::AllocatorType::HEAP_ALLOC;
    pixelMap->isUnMap_ = true;
    imageBase->SetPixelMap(pixelMap);
    ASSERT_NE(imageBase, nullptr);
    imageBase->Purge();
}

/**
 * @tc.name: PurgeTest008
 * @tc.desc: Verify function Purge
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, PurgeTest008, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    imageBase->uniqueId_ = 1;
    imageBase->canPurgeShareMemFlag_ = RSImageBase::CanPurgeFlag::ENABLED;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->allocatorType_ = Media::AllocatorType::HEAP_ALLOC;
    pixelMap->isUnMap_ = false;
    imageBase->SetPixelMap(pixelMap);
    ASSERT_NE(imageBase, nullptr);
    imageBase->Purge();
}

/**
 * @tc.name: UnmarshallingDrawingImageAndPixelMapTest
 * @tc.desc: Verify function UnmarshallingDrawingImageAndPixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, UnmarshallingDrawingImageAndPixelMapTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Parcel parcel;
    parcel.WriteInt32(-1);
    uint64_t uniqueId = 1;
    bool useSKImage = false;
    auto image = std::make_shared<Drawing::Image>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->allocatorType_ = Media::AllocatorType::HEAP_ALLOC;
    void* addr = nullptr;
    bool ret = imageBase->UnmarshallingDrawingImageAndPixelMap(parcel, uniqueId, useSKImage, image, pixelMap, addr);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: UnmarshallingDrawingImageAndPixelMapTest001
 * @tc.desc: Verify function UnmarshallingDrawingImageAndPixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, UnmarshallingDrawingImageAndPixelMapTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Parcel parcel;
    parcel.WriteInt32(1);
    uint64_t uniqueId = 1;
    bool useSKImage = false;
    auto image = std::make_shared<Drawing::Image>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    void* addr = nullptr;
    bool ret = imageBase->UnmarshallingDrawingImageAndPixelMap(parcel, uniqueId, useSKImage, image, pixelMap, addr);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: UnmarshallingDrawingImageAndPixelMapTest002
 * @tc.desc: Verify function UnmarshallingDrawingImageAndPixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, UnmarshallingDrawingImageAndPixelMapTest002, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Parcel parcel;
    parcel.WriteInt32(1);
    uint64_t uniqueId = 1;
    bool useSKImage = true;
    auto image = std::make_shared<Drawing::Image>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    void* addr = nullptr;
    bool ret = imageBase->UnmarshallingDrawingImageAndPixelMap(parcel, uniqueId, useSKImage, image, pixelMap, addr);
    EXPECT_EQ(ret, false);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
/**
 * @tc.name: MakeFromTextureForVKTest
 * @tc.desc: Verify function MakeFromTextureForVK
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, MakeFromTextureForVKTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    EXPECT_EQ(imageBase->MakeFromTextureForVK(canvas, nullptr), nullptr);
}

/**
 * @tc.name: MakeFromTextureForVKTest001
 * @tc.desc: Verify function MakeFromTextureForVK
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, MakeFromTextureForVKTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create().GetRefPtr();
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    EXPECT_EQ(imageBase->MakeFromTextureForVK(canvas, surfaceBuffer), nullptr);
}

/**
 * @tc.name: MakeFromTextureForVKTest002
 * @tc.desc: Verify function MakeFromTextureForVK
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, MakeFromTextureForVKTest002, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create().GetRefPtr();
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    imageBase->nativeWindowBuffer_ = nullptr;
    EXPECT_EQ(imageBase->MakeFromTextureForVK(canvas, surfaceBuffer), nullptr);
}

/**
 * @tc.name: MakeFromTextureForVKTest003
 * @tc.desc: Verify function MakeFromTextureForVK
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, MakeFromTextureForVKTest003, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create().GetRefPtr();
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    imageBase->nativeWindowBuffer_ = nullptr;
    imageBase->backendTexture_.isValid_ = true;
    EXPECT_EQ(imageBase->MakeFromTextureForVK(canvas, surfaceBuffer), nullptr);
}

/**
 * @tc.name: MakeFromTextureForVKTest004
 * @tc.desc: Verify function MakeFromTextureForVK
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, MakeFromTextureForVKTest004, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create().GetRefPtr();
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    imageBase->nativeWindowBuffer_ = nullptr;
    imageBase->backendTexture_.isValid_ = false;
    EXPECT_EQ(imageBase->MakeFromTextureForVK(canvas, surfaceBuffer), nullptr);
}
#endif

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
/**
 * @tc.name: BindPixelMapToDrawingImageTest
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, BindPixelMapToDrawingImageTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    ASSERT_NE(imageBase, nullptr);
    imageBase->BindPixelMapToDrawingImage(canvas);
}

/**
 * @tc.name: BindPixelMapToDrawingImageTest001
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, BindPixelMapToDrawingImageTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    ASSERT_NE(imageBase, nullptr);
    imageBase->BindPixelMapToDrawingImage(canvas);
}

/**
 * @tc.name: BindPixelMapToDrawingImageTest002
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, BindPixelMapToDrawingImageTest002, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->isAstc_ = true;
    imageBase->SetPixelMap(pixelMap);
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    ASSERT_NE(imageBase, nullptr);
    imageBase->BindPixelMapToDrawingImage(canvas);
}

/**
 * @tc.name: BindPixelMapToDrawingImageTest003
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, BindPixelMapToDrawingImageTest003, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->isAstc_ = false;
    imageBase->SetPixelMap(pixelMap);
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    ASSERT_NE(imageBase, nullptr);
    imageBase->BindPixelMapToDrawingImage(canvas);
}

/**
 * @tc.name: BindPixelMapToDrawingImageTest004
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, BindPixelMapToDrawingImageTest004, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->isAstc_ = true;
    imageBase->SetPixelMap(pixelMap);
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    ASSERT_NE(imageBase, nullptr);
    imageBase->BindPixelMapToDrawingImage(canvas);
}

/**
 * @tc.name: BindPixelMapToDrawingImageTest005
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, BindPixelMapToDrawingImageTest005, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->isAstc_ = false;
    imageBase->SetPixelMap(pixelMap);
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    ASSERT_NE(imageBase, nullptr);
    imageBase->BindPixelMapToDrawingImage(canvas);
}

/**
 * @tc.name: BindPixelMapToDrawingImageTest006
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, BindPixelMapToDrawingImageTest006, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->isAstc_ = false;
    pixelMap->editable_ = true;
    imageBase->SetPixelMap(pixelMap);
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    ASSERT_NE(imageBase, nullptr);
    imageBase->BindPixelMapToDrawingImage(canvas);
}

/**
 * @tc.name: BindPixelMapToDrawingImageTest007
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseUintTest, BindPixelMapToDrawingImageTest007, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->isAstc_ = false;
    pixelMap->editable_ = false;
    imageBase->SetPixelMap(pixelMap);
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    ASSERT_NE(imageBase, nullptr);
    imageBase->BindPixelMapToDrawingImage(canvas);
}
#endif

/**
 * @tc.name: UnmarshallingDrawingImageAndPixelMapTest003
 * @tc.desc: Verify UnmarshallingDrawingImageAndPixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2LQP
 */
HWTEST_F(RSImageBaseUintTest, UnmarshallingDrawingImageAndPixelMapTest003, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Parcel parcel;
    parcel.WriteInt32(-1);
    uint64_t uniqueId = -1;
    bool useSKImage = false;
    auto image = std::make_shared<Drawing::Image>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    void* addr = nullptr;
    bool ret = imageBase->UnmarshallingDrawingImageAndPixelMap(parcel, uniqueId, useSKImage, image, pixelMap, addr);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: UnmarshallingDrawingImageAndPixelMapTest004
 * @tc.desc: Verify UnmarshallingDrawingImageAndPixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2LQP
 */
HWTEST_F(RSImageBaseUintTest, UnmarshallingDrawingImageAndPixelMapTest004, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Parcel parcel;
    parcel.WriteInt32(-1);
    uint64_t uniqueId = -1;
    bool useSKImage = false;
    auto image = std::make_shared<Drawing::Image>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    void* addr = nullptr;
    bool ret = imageBase->UnmarshallingDrawingImageAndPixelMap(parcel, uniqueId, useSKImage, image, pixelMap, addr);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: UnmarshallingDrawingImageAndPixelMapTest005
 * @tc.desc: Verify UnmarshallingDrawingImageAndPixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2LQP
 */
HWTEST_F(RSImageBaseUintTest, UnmarshallingDrawingImageAndPixelMapTest005, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Parcel parcel;
    parcel.WriteInt32(-1);
    uint64_t uniqueId = -1;
    bool useSKImage = false;
    auto image = std::make_shared<Drawing::Image>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->editable_ = false;
    void* addr = nullptr;
    bool ret = imageBase->UnmarshallingDrawingImageAndPixelMap(parcel, uniqueId, useSKImage, image, pixelMap, addr);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: UnmarshallingDrawingImageAndPixelMapTest006
 * @tc.desc: Verify UnmarshallingDrawingImageAndPixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2LQP
 */
HWTEST_F(RSImageBaseUintTest, UnmarshallingDrawingImageAndPixelMapTest006, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Parcel parcel;
    parcel.WriteInt32(-1);
    uint64_t uniqueId = -1;
    bool useSKImage = false;
    auto image = std::make_shared<Drawing::Image>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->editable_ = true;
    void* addr = nullptr;
    bool ret = imageBase->UnmarshallingDrawingImageAndPixelMap(parcel, uniqueId, useSKImage, image, pixelMap, addr);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: UnmarshallingDrawingImageAndPixelMapTest007
 * @tc.desc: Verify UnmarshallingDrawingImageAndPixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2LQP
 */
HWTEST_F(RSImageBaseUintTest, UnmarshallingDrawingImageAndPixelMapTest007, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Parcel parcel;
    parcel.WriteInt32(-1);
    uint64_t uniqueId = -1;
    bool useSKImage = true;
    auto image = std::make_shared<Drawing::Image>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->editable_ = true;
    void* addr = nullptr;
    image = nullptr;
    bool ret = imageBase->UnmarshallingDrawingImageAndPixelMap(parcel, uniqueId, useSKImage, image, pixelMap, addr);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: UnmarshallingDrawingImageAndPixelMapTest008
 * @tc.desc: Verify UnmarshallingDrawingImageAndPixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2LQP
 */
HWTEST_F(RSImageBaseUintTest, UnmarshallingDrawingImageAndPixelMapTest008, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Parcel parcel;
    parcel.WriteInt32(-1);
    uint64_t uniqueId = -1;
    bool useSKImage = true;
    auto image = std::make_shared<Drawing::Image>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    void* addr = nullptr;
    bool ret = imageBase->UnmarshallingDrawingImageAndPixelMap(parcel, uniqueId, useSKImage, image, pixelMap, addr);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: IncreaseCacheRefCountTest
 * @tc.desc: Verify IncreaseCacheRefCount
 * @tc.type:FUNC
 * @tc.require: issue#IB2LQP
 */
HWTEST_F(RSImageBaseUintTest, IncreaseCacheRefCountTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    uint64_t uniqueId = 1;
    bool useSKImage = true;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->editable_ = false;
    ASSERT_NE(imageBase, nullptr);
    imageBase->IncreaseCacheRefCount(uniqueId, useSKImage, pixelMap);
}

/**
 * @tc.name: IncreaseCacheRefCountTest001
 * @tc.desc: Verify IncreaseCacheRefCount
 * @tc.type:FUNC
 * @tc.require: issue#IB2LQP
 */
HWTEST_F(RSImageBaseUintTest, IncreaseCacheRefCountTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    uint64_t uniqueId = 1;
    bool useSKImage = false;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->editable_ = false;
    ASSERT_NE(imageBase, nullptr);
    imageBase->IncreaseCacheRefCount(uniqueId, useSKImage, pixelMap);
}

/**
 * @tc.name: IncreaseCacheRefCountTest002
 * @tc.desc: Verify IncreaseCacheRefCount
 * @tc.type:FUNC
 * @tc.require: issue#IB2LQP
 */
HWTEST_F(RSImageBaseUintTest, IncreaseCacheRefCountTest002, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    uint64_t uniqueId = 1;
    bool useSKImage = false;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->editable_ = true;
    ASSERT_NE(imageBase, nullptr);
    imageBase->IncreaseCacheRefCount(uniqueId, useSKImage, pixelMap);
}
} // namespace OHOS::Rosen