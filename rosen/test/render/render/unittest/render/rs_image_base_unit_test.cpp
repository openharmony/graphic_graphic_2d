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
 * @tc.name: TestDrawImage
 * @tc.desc: Verify function DrawImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestDrawImage, TestSize.Level1)
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
 * @tc.name: TestSetImage
 * @tc.desc: Verify function SetImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestSetImage, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    EXPECT_EQ(imageBase->image_, image);
}

/**
 * @tc.name: TestSetPixelMap
 * @tc.desc: Verify function SetPixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestSetPixelMap, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelmap = std::make_shared<Media::PixelMap>();
    imageBase->SetPixelMap(pixelmap);
    EXPECT_EQ(imageBase->pixelMap_, pixelmap);
}

/**
 * @tc.name: TestDumpPicture
 * @tc.desc: Verify function DumpPicture
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestDumpPicture, TestSize.Level1)
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
 * @tc.name: TestSetSrcRect
 * @tc.desc: Verify function SetSrcRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestSetSrcRect, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    RectF srcRect;
    srcRect.Clear();
    imageBase->SetSrcRect(srcRect);
    EXPECT_EQ(imageBase->srcRect_.GetLeft(), 0);
}

/**
 * @tc.name: TestSetDstRect
 * @tc.desc: Verify function SetDstRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestSetDstRect, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    RectF dstRect;
    imageBase->SetDstRect(dstRect);
    imageBase->SetDstRect(imageBase->dstRect_);
    EXPECT_FALSE(imageBase->isDrawn_);
}

/**
 * @tc.name: TestSetImagePixelAddr
 * @tc.desc: Verify function SetImagePixelAddr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestSetImagePixelAddr, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    void* addr = nullptr;
    imageBase->SetImagePixelAddr(addr);
    EXPECT_EQ(imageBase->imagePixelAddr_, nullptr);
}

/**
 * @tc.name: TestUpdateNodeIdToPicture
 * @tc.desc: Verify function UpdateNodeIdToPicture
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestUpdateNodeIdToPicture, TestSize.Level1)
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
 * @tc.name: TestMarkRenderServiceImage
 * @tc.desc: Verify function MarkRenderServiceImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestMarkRenderServiceImage, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    imageBase->MarkRenderServiceImage();
    EXPECT_TRUE(imageBase->renderServiceImage_);
}

/**
 * @tc.name: TestConvertPixelMapToDrawingImage
 * @tc.desc: Verify function ConvertPixelMapToDrawingImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestConvertPixelMapToDrawingImage, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelmap = std::make_shared<Media::PixelMap>();
    imageBase->SetPixelMap(pixelmap);
    imageBase->ConvertPixelMapToDrawingImage(true);
    EXPECT_EQ(imageBase->image_, nullptr);
}

/**
 * @tc.name: TestGenUniqueId
 * @tc.desc: Verify function GenUniqueId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestGenUniqueId, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    imageBase->GenUniqueId(1);
    EXPECT_NE(imageBase->uniqueId_, 0);
}

/**
 * @tc.name: TestGetPixelMap
 * @tc.desc: Verify function GetPixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestGetPixelMap, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    EXPECT_EQ(imageBase->GetPixelMap(), nullptr);
}

/**
 * @tc.name: TestPurge
 * @tc.desc: Verify function Purge
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestPurge, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    ASSERT_NE(imageBase, nullptr);
    imageBase->Purge();
}

/**
 * @tc.name: TestSetDmaImage
 * @tc.desc: Verify function SetDmaImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestSetDmaImage, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    ASSERT_NE(imageBase, nullptr);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    imageBase->SetDmaImage(image);
    EXPECT_EQ(imageBase->image_, image);
}

/**
 * @tc.name: TestMarkYUVImage
 * @tc.desc: Verify function MarkYUVImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestMarkYUVImage, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    ASSERT_NE(imageBase, nullptr);
    imageBase->MarkYUVImage();
    EXPECT_TRUE(imageBase->isYUVImage_);
}

/**
 * @tc.name: TestUnmarshalling
 * @tc.desc: Verify function Unmarshalling
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestUnmarshalling, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    ASSERT_NE(imageBase, nullptr);
    Parcel parcel;
    EXPECT_EQ(imageBase->Unmarshalling(parcel), nullptr);
}

/**
 * @tc.name: TestProcessYUVImage
 * @tc.desc: Verify function ProcessYUVImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestProcessYUVImage, TestSize.Level1)
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
 * @tc.name: TestMarkRenderServiceImage001
 * @tc.desc: Verify function MarkRenderServiceImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestMarkRenderServiceImage001, TestSize.Level1)
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
 * @tc.name: TestMarkRenderServiceImage002
 * @tc.desc: Verify function MarkRenderServiceImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestMarkRenderServiceImage002, TestSize.Level1)
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
 * @tc.name: TestMarkRenderServiceImage003
 * @tc.desc: Verify function MarkRenderServiceImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestMarkRenderServiceImage003, TestSize.Level1)
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
 * @tc.name: TestGetColorTypeWithVKFormat
 * @tc.desc: Verify function GetColorTypeWithVKFormat
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestGetColorTypeWithVKFormat, TestSize.Level1)
{
    VkFormat vkFormat = VK_FORMAT_R8G8B8A8_UNORM;
    EXPECT_EQ(GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGBA_8888);
}

/**
 * @tc.name: TestGetColorTypeWithVKFormat001
 * @tc.desc: Verify function GetColorTypeWithVKFormat
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestGetColorTypeWithVKFormat001, TestSize.Level1)
{
    VkFormat vkFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    EXPECT_EQ(GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGBA_F16);
}

/**
 * @tc.name: TestGetColorTypeWithVKFormat002
 * @tc.desc: Verify function GetColorTypeWithVKFormat
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestGetColorTypeWithVKFormat002, TestSize.Level1)
{
    VkFormat vkFormat = VK_FORMAT_R5G6B5_UNORM_PACK16;
    EXPECT_EQ(GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGB_565);
}

/**
 * @tc.name: TestGetColorTypeWithVKFormat003
 * @tc.desc: Verify function GetColorTypeWithVKFormat
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestGetColorTypeWithVKFormat003, TestSize.Level1)
{
    VkFormat vkFormat = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
    EXPECT_EQ(GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGBA_1010102);
}

/**
 * @tc.name: TestGetColorTypeWithVKFormat004
 * @tc.desc: Verify function GetColorTypeWithVKFormat
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestGetColorTypeWithVKFormat004, TestSize.Level1)
{
    VkFormat vkFormat = VK_FORMAT_R32_SFLOAT;
    EXPECT_EQ(GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGBA_8888);
}
#endif

/**
 * @tc.name: TestPurge001
 * @tc.desc: Verify function Purge
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestPurge001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageBase->SetPixelMap(pixelMap);
    ASSERT_NE(imageBase, nullptr);
    imageBase->Purge();
}

/**
 * @tc.name: TestPurge002
 * @tc.desc: Verify function Purge
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestPurge002, TestSize.Level1)
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
 * @tc.name: TestPurge003
 * @tc.desc: Verify function Purge
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestPurge003, TestSize.Level1)
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
 * @tc.name: TestPurge004
 * @tc.desc: Verify function Purge
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestPurge004, TestSize.Level1)
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
 * @tc.name: TestPurge005
 * @tc.desc: Verify function Purge
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestPurge005, TestSize.Level1)
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
 * @tc.name: TestPurge006
 * @tc.desc: Verify function Purge
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestPurge006, TestSize.Level1)
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
 * @tc.name: TestPurge007
 * @tc.desc: Verify function Purge
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestPurge007, TestSize.Level1)
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
 * @tc.name: TestPurge008
 * @tc.desc: Verify function Purge
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestPurge008, TestSize.Level1)
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
 * @tc.name: TestUnmarshallingDrawingImageAndPixelMap
 * @tc.desc: Verify function UnmarshallingDrawingImageAndPixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestUnmarshallingDrawingImageAndPixelMap, TestSize.Level1)
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
 * @tc.name: TestUnmarshallingDrawingImageAndPixelMap001
 * @tc.desc: Verify function UnmarshallingDrawingImageAndPixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestUnmarshallingDrawingImageAndPixelMap001, TestSize.Level1)
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
 * @tc.name: TestUnmarshallingDrawingImageAndPixelMap002
 * @tc.desc: Verify function UnmarshallingDrawingImageAndPixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestUnmarshallingDrawingImageAndPixelMap002, TestSize.Level1)
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
 * @tc.name: TestMakeFromTextureForVK
 * @tc.desc: Verify function MakeFromTextureForVK
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestMakeFromTextureForVK, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    EXPECT_EQ(imageBase->MakeFromTextureForVK(canvas, nullptr), nullptr);
}

/**
 * @tc.name: TestMakeFromTextureForVK001
 * @tc.desc: Verify function MakeFromTextureForVK
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestMakeFromTextureForVK001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create().GetRefPtr();
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    EXPECT_EQ(imageBase->MakeFromTextureForVK(canvas, surfaceBuffer), nullptr);
}

/**
 * @tc.name: TestMakeFromTextureForVK002
 * @tc.desc: Verify function MakeFromTextureForVK
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestMakeFromTextureForVK002, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create().GetRefPtr();
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    imageBase->nativeWindowBuffer_ = nullptr;
    EXPECT_EQ(imageBase->MakeFromTextureForVK(canvas, surfaceBuffer), nullptr);
}

/**
 * @tc.name: TestMakeFromTextureForVK003
 * @tc.desc: Verify function MakeFromTextureForVK
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestMakeFromTextureForVK003, TestSize.Level1)
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
 * @tc.name: TestMakeFromTextureForVK004
 * @tc.desc: Verify function MakeFromTextureForVK
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestMakeFromTextureForVK004, TestSize.Level1)
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
 * @tc.name: TestBindPixelMapToDrawingImage
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestBindPixelMapToDrawingImage, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    ASSERT_NE(imageBase, nullptr);
    imageBase->BindPixelMapToDrawingImage(canvas);
}

/**
 * @tc.name: TestBindPixelMapToDrawingImage001
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestBindPixelMapToDrawingImage001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    ASSERT_NE(imageBase, nullptr);
    imageBase->BindPixelMapToDrawingImage(canvas);
}

/**
 * @tc.name: TestBindPixelMapToDrawingImage002
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestBindPixelMapToDrawingImage002, TestSize.Level1)
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
 * @tc.name: TestBindPixelMapToDrawingImage003
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestBindPixelMapToDrawingImage003, TestSize.Level1)
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
 * @tc.name: TestBindPixelMapToDrawingImage004
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestBindPixelMapToDrawingImage004, TestSize.Level1)
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
 * @tc.name: TestBindPixelMapToDrawingImage005
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestBindPixelMapToDrawingImage005, TestSize.Level1)
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
 * @tc.name: TestBindPixelMapToDrawingImage006
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestBindPixelMapToDrawingImage006, TestSize.Level1)
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
 * @tc.name: TestBindPixelMapToDrawingImage007
 * @tc.desc: Verify function BindPixelMapToDrawingImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestBindPixelMapToDrawingImage007, TestSize.Level1)
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
 * @tc.name: TestUnmarshallingDrawingImageAndPixelMap003
 * @tc.desc: Verify UnmarshallingDrawingImageAndPixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestUnmarshallingDrawingImageAndPixelMap003, TestSize.Level1)
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
 * @tc.name: TestUnmarshallingDrawingImageAndPixelMap004
 * @tc.desc: Verify UnmarshallingDrawingImageAndPixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestUnmarshallingDrawingImageAndPixelMap004, TestSize.Level1)
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
 * @tc.name: TestUnmarshallingDrawingImageAndPixelMap005
 * @tc.desc: Verify UnmarshallingDrawingImageAndPixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestUnmarshallingDrawingImageAndPixelMap005, TestSize.Level1)
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
 * @tc.name: TestUnmarshallingDrawingImageAndPixelMap006
 * @tc.desc: Verify UnmarshallingDrawingImageAndPixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestUnmarshallingDrawingImageAndPixelMap006, TestSize.Level1)
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
 * @tc.name: TestUnmarshallingDrawingImageAndPixelMap007
 * @tc.desc: Verify UnmarshallingDrawingImageAndPixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestUnmarshallingDrawingImageAndPixelMap007, TestSize.Level1)
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
 * @tc.name: TestUnmarshallingDrawingImageAndPixelMap008
 * @tc.desc: Verify UnmarshallingDrawingImageAndPixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestUnmarshallingDrawingImageAndPixelMap008, TestSize.Level1)
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
 * @tc.name: TestIncreaseCacheRefCount
 * @tc.desc: Verify IncreaseCacheRefCount
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestIncreaseCacheRefCount, TestSize.Level1)
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
 * @tc.name: TestIncreaseCacheRefCount001
 * @tc.desc: Verify IncreaseCacheRefCount
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestIncreaseCacheRefCount001, TestSize.Level1)
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
 * @tc.name: TestIncreaseCacheRefCount002
 * @tc.desc: Verify IncreaseCacheRefCount
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageBaseUintTest, TestIncreaseCacheRefCount002, TestSize.Level1)
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