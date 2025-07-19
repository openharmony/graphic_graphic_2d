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
class RSImageBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSImageBaseTest::SetUpTestCase() {}
void RSImageBaseTest::TearDownTestCase() {}
void RSImageBaseTest::SetUp() {}
void RSImageBaseTest::TearDown() {}
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
Drawing::ColorType GetColorTypeWithVKFormat(VkFormat vkFormat);
#endif

/**
 * @tc.name: DrawImageTest
 * @tc.desc: Verify function DrawImage
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageBaseTest, DrawImageTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, SetImageTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, SetPixelMapTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, DumpPictureTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, SetSrcRectTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, SetDstRectTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, SetImagePixelAddrTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, UpdateNodeIdToPictureTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, MarkRenderServiceImageTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, ConvertPixelMapToDrawingImageTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, GenUniqueIdTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, GetPixelMapTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, PurgeTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, SetDmaImageTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, MarkYUVImageTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, UnmarshallingTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, ProcessYUVImageTest, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    ASSERT_NE(imageBase, nullptr);
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    imageBase->ProcessYUVImage(gpuContext, canvas);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    imageBase->ProcessYUVImage(gpuContext, canvas);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::NV12;
    imageBase->SetPixelMap(pixelMap);
    imageBase->ProcessYUVImage(gpuContext, canvas);
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelMap), nullptr);
}


/**
 * @tc.name: ProcessYUVImageTest001
 * @tc.desc: pixelFormat is ARGB_8888
 * @tc.type:FUNC
 * @tc.require: issuesI9TOXM
 */
HWTEST_F(RSImageBaseTest, ProcessYUVImageTest013, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    ASSERT_NE(imageBase, nullptr);
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    imageBase->ProcessYUVImage(gpuContext, canvas);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    imageBase->ProcessYUVImage(gpuContext, canvas);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::ARGB_8888;
    imageBase->SetPixelMap(pixelMap);
    imageBase->ProcessYUVImage(gpuContext, canvas);
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelMap), nullptr);
}

/**
 * @tc.name: ProcessYUVImageTest001
 * @tc.desc: pixelFormat is RGB_565
 * @tc.type:FUNC
 * @tc.require: issuesI9TOXM
 */
HWTEST_F(RSImageBaseTest, ProcessYUVImageTest012, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    ASSERT_NE(imageBase, nullptr);
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    imageBase->ProcessYUVImage(gpuContext, canvas);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    imageBase->ProcessYUVImage(gpuContext, canvas);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::RGB_565;
    imageBase->SetPixelMap(pixelMap);
    imageBase->ProcessYUVImage(gpuContext, canvas);
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelMap), nullptr);
}
/**
 * @tc.name: ProcessYUVImageTest001
 * @tc.desc: pixelFormat is RGBA_8888
 * @tc.type:FUNC
 * @tc.require: issuesI9TOXM
 */
HWTEST_F(RSImageBaseTest, ProcessYUVImageTest011, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    ASSERT_NE(imageBase, nullptr);
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    imageBase->ProcessYUVImage(gpuContext, canvas);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    imageBase->ProcessYUVImage(gpuContext, canvas);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::RGBA_8888;
    imageBase->SetPixelMap(pixelMap);
    imageBase->ProcessYUVImage(gpuContext, canvas);
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelMap), nullptr);
}
/**
 * @tc.name: ProcessYUVImageTest001
 * @tc.desc: pixelFormat is BGRA_8888
 * @tc.type:FUNC
 * @tc.require: issuesI9TOXM
 */
HWTEST_F(RSImageBaseTest, ProcessYUVImageTest010, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    ASSERT_NE(imageBase, nullptr);
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    imageBase->ProcessYUVImage(gpuContext, canvas);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    imageBase->ProcessYUVImage(gpuContext, canvas);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::BGRA_8888;
    imageBase->SetPixelMap(pixelMap);
    imageBase->ProcessYUVImage(gpuContext, canvas);
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelMap), nullptr);
}
/**
 * @tc.name: ProcessYUVImageTest001
 * @tc.desc: pixelFormat is RGB_888
 * @tc.type:FUNC
 * @tc.require: issuesI9TOXM
 */
HWTEST_F(RSImageBaseTest, ProcessYUVImageTest009, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    ASSERT_NE(imageBase, nullptr);
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    imageBase->ProcessYUVImage(gpuContext, canvas);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    imageBase->ProcessYUVImage(gpuContext, canvas);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::RGB_888;
    imageBase->SetPixelMap(pixelMap);
    imageBase->ProcessYUVImage(gpuContext, canvas);
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelMap), nullptr);
}
/**
 * @tc.name: ProcessYUVImageTest001
 * @tc.desc: pixelFormat is ALPHA_8
 * @tc.type:FUNC
 * @tc.require: issuesI9TOXM
 */
HWTEST_F(RSImageBaseTest, ProcessYUVImageTest008, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    ASSERT_NE(imageBase, nullptr);
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    imageBase->ProcessYUVImage(gpuContext, canvas);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    imageBase->ProcessYUVImage(gpuContext, canvas);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::ALPHA_8;
    imageBase->SetPixelMap(pixelMap);
    imageBase->ProcessYUVImage(gpuContext, canvas);
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelMap), nullptr);
}
/**
 * @tc.name: ProcessYUVImageTest001
 * @tc.desc: pixelFormat is RGBA_F16
 * @tc.type:FUNC
 * @tc.require: issuesI9TOXM
 */
HWTEST_F(RSImageBaseTest, ProcessYUVImageTest007, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    ASSERT_NE(imageBase, nullptr);
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    imageBase->ProcessYUVImage(gpuContext, canvas);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    imageBase->ProcessYUVImage(gpuContext, canvas);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::RGBA_F16;
    imageBase->SetPixelMap(pixelMap);
    imageBase->ProcessYUVImage(gpuContext, canvas);
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelMap), nullptr);
}
/**
 * @tc.name: ProcessYUVImageTest001
 * @tc.desc: pixelFormat is NV21
 * @tc.type:FUNC
 * @tc.require: issuesI9TOXM
 */
HWTEST_F(RSImageBaseTest, ProcessYUVImageTest006, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    ASSERT_NE(imageBase, nullptr);
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    imageBase->ProcessYUVImage(gpuContext, canvas);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    imageBase->ProcessYUVImage(gpuContext, canvas);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::NV21;
    imageBase->SetPixelMap(pixelMap);
    imageBase->ProcessYUVImage(gpuContext, canvas);
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelMap), nullptr);
}
/**
 * @tc.name: ProcessYUVImageTest001
 * @tc.desc: pixelFormat is CMYK
 * @tc.type:FUNC
 * @tc.require: issuesI9TOXM
 */
HWTEST_F(RSImageBaseTest, ProcessYUVImageTest005, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    ASSERT_NE(imageBase, nullptr);
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    imageBase->ProcessYUVImage(gpuContext, canvas);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    imageBase->ProcessYUVImage(gpuContext, canvas);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::CMYK;
    imageBase->SetPixelMap(pixelMap);
    imageBase->ProcessYUVImage(gpuContext, canvas);
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelMap), nullptr);
}
/**
 * @tc.name: ProcessYUVImageTest001
 * @tc.desc: pixelFormat is YCBCR_P010
 * @tc.type:FUNC
 * @tc.require: issuesI9TOXM
 */
HWTEST_F(RSImageBaseTest, ProcessYUVImageTest004, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    ASSERT_NE(imageBase, nullptr);
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    imageBase->ProcessYUVImage(gpuContext, canvas);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    imageBase->ProcessYUVImage(gpuContext, canvas);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::YCBCR_P010;
    imageBase->SetPixelMap(pixelMap);
    imageBase->ProcessYUVImage(gpuContext, canvas);
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelMap), nullptr);
}
/**
 * @tc.name: ProcessYUVImageTest001
 * @tc.desc: pixelFormat is YCRCB_P010
 * @tc.type:FUNC
 * @tc.require: issuesI9TOXM
 */
HWTEST_F(RSImageBaseTest, ProcessYUVImageTest003, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    ASSERT_NE(imageBase, nullptr);
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    imageBase->ProcessYUVImage(gpuContext, canvas);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    imageBase->ProcessYUVImage(gpuContext, canvas);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::YCRCB_P010;
    imageBase->SetPixelMap(pixelMap);
    imageBase->ProcessYUVImage(gpuContext, canvas);
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelMap), nullptr);
}
/**
 * @tc.name: ProcessYUVImageTest001
 * @tc.desc: pixelFormat is RGBA_1010102
 * @tc.type:FUNC
 * @tc.require: issuesI9TOXM
 */
HWTEST_F(RSImageBaseTest, ProcessYUVImageTest002, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    ASSERT_NE(imageBase, nullptr);
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    imageBase->ProcessYUVImage(gpuContext, canvas);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    imageBase->ProcessYUVImage(gpuContext, canvas);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::RGBA_1010102;
    imageBase->SetPixelMap(pixelMap);
    imageBase->ProcessYUVImage(gpuContext, canvas);
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelMap), nullptr);
}

/**
 * @tc.name: MarkRenderServiceImageTest001
 * @tc.desc: Verify function MarkRenderServiceImage
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseTest, MarkRenderServiceImageTest001, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, MarkRenderServiceImageTest002, TestSize.Level1)
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
 * @tc.name: MarkRenderServiceImageTest103
 * @tc.desc: allocatorType_ is DEFAULT
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseTest, MarkRenderServiceImageTest103, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->allocatorType_ = Media::AllocatorType::DEFAULT;
    pixelMap->editable_ = true;
    pixelMap->isAstc_ = true;
    imageBase->SetPixelMap(pixelMap);
    imageBase->MarkRenderServiceImage();
    EXPECT_TRUE(imageBase->renderServiceImage_);
}

/**
 * @tc.name: MarkRenderServiceImageTest004
 * @tc.desc: allocatorType_ is CUSTOM_ALLOC
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseTest, MarkRenderServiceImageTest004, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->allocatorType_ = Media::AllocatorType::CUSTOM_ALLOC;
    pixelMap->editable_ = true;
    pixelMap->isAstc_ = true;
    imageBase->SetPixelMap(pixelMap);
    imageBase->MarkRenderServiceImage();
    EXPECT_TRUE(imageBase->renderServiceImage_);
}

/**
 * @tc.name: MarkRenderServiceImageTest005
 * @tc.desc: allocatorType_ is DMA_ALLOC
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageBaseTest, MarkRenderServiceImageTest005, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->allocatorType_ = Media::AllocatorType::DMA_ALLOC;
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
HWTEST_F(RSImageBaseTest, MarkRenderServiceImageTest003, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, GetColorTypeWithVKFormatTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, GetColorTypeWithVKFormatTest001, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, GetColorTypeWithVKFormatTest002, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, GetColorTypeWithVKFormatTest003, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, GetColorTypeWithVKFormatTest004, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, PurgeTest001, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, PurgeTest002, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, PurgeTest003, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, PurgeTest004, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, PurgeTest005, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, PurgeTest006, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, PurgeTest007, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, PurgeTest008, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, UnmarshallingDrawingImageAndPixelMapTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, UnmarshallingDrawingImageAndPixelMapTest001, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, UnmarshallingDrawingImageAndPixelMapTest002, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, MakeFromTextureForVKTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, MakeFromTextureForVKTest001, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, MakeFromTextureForVKTest002, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, MakeFromTextureForVKTest003, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, MakeFromTextureForVKTest004, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, BindPixelMapToDrawingImageTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, BindPixelMapToDrawingImageTest001, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, BindPixelMapToDrawingImageTest002, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, BindPixelMapToDrawingImageTest003, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, BindPixelMapToDrawingImageTest004, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, BindPixelMapToDrawingImageTest005, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, BindPixelMapToDrawingImageTest006, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, BindPixelMapToDrawingImageTest007, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, UnmarshallingDrawingImageAndPixelMapTest003, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, UnmarshallingDrawingImageAndPixelMapTest004, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, UnmarshallingDrawingImageAndPixelMapTest005, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, UnmarshallingDrawingImageAndPixelMapTest006, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, UnmarshallingDrawingImageAndPixelMapTest007, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, UnmarshallingDrawingImageAndPixelMapTest008, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, IncreaseCacheRefCountTest, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, IncreaseCacheRefCountTest001, TestSize.Level1)
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
HWTEST_F(RSImageBaseTest, IncreaseCacheRefCountTest002, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    uint64_t uniqueId = 1;
    bool useSKImage = false;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->editable_ = true;
    ASSERT_NE(imageBase, nullptr);
    imageBase->IncreaseCacheRefCount(uniqueId, useSKImage, pixelMap);
}

/**
 * @tc.name: DrawImageNineTest001
 * @tc.desc: Verify DrawImageNine
 * @tc.type:FUNC
 * @tc.require: issue#IB2LQP
 */
HWTEST_F(RSImageBaseTest, DrawImageNineTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    Drawing::RectI center(2, 2, 2, 2);
    Drawing::Rect dst;
    imageBase->DrawImageNine(canvas, center, dst);
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    imageBase->DrawImageNine(canvas, center, dst);
    ASSERT_NE(imageBase->image_, nullptr);
}

/**
 * @tc.name: DrawImageLatticeTest001
 * @tc.desc: Verify DrawImageLattice
 * @tc.type:FUNC
 * @tc.require: issue#IB2LQP
 */
HWTEST_F(RSImageBaseTest, DrawImageLatticeTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    Drawing::Lattice lat;
    Drawing::Rect dst;
    imageBase->DrawImageLattice(canvas, lat, dst);
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    imageBase->DrawImageLattice(canvas, lat, dst);
    ASSERT_NE(imageBase->image_, nullptr);
}
} // namespace OHOS::Rosen