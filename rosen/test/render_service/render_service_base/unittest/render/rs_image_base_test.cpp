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

#include "render/rs_image_base.h"

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
} // namespace OHOS::Rosen