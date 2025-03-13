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

#include <cstddef>
#include "gtest/gtest.h"
#include "draw/surface.h"
#include "effect/color_space.h"
#include "image/bitmap.h"
#include "image/image_info.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkImageInfo.h"
#include "skia_adapter/skia_bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaBitmapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaBitmapTest::SetUpTestCase() {}
void SkiaBitmapTest::TearDownTestCase() {}
void SkiaBitmapTest::SetUp() {}
void SkiaBitmapTest::TearDown() {}

/**
 * @tc.name: Build001
 * @tc.desc: Test Build
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, Build001, TestSize.Level1)
{
    int width = 100;
    int height = 50;
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_BGRA_8888,
        AlphaType::ALPHATYPE_PREMUL};
    SkiaBitmap skiaBitmap;
    ASSERT_TRUE(skiaBitmap.Build(width, height, bitmapFormat, 0));
}

/**
 * @tc.name: Build002
 * @tc.desc: Test Build
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, Build002, TestSize.Level1)
{
    int width = 100;
    int height = 50;
    ImageInfo offscreenInfo = { width, height, COLORTYPE_RGBA_8888,
        ALPHATYPE_PREMUL, nullptr};
    SkiaBitmap skiaBitmap;
    ASSERT_TRUE(skiaBitmap.Build(offscreenInfo, 0));
}

/**
 * @tc.name: GetWidth001
 * @tc.desc: Test GetWidth
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, GetWidth001, TestSize.Level1)
{
    SkiaBitmap skiaBitmap;
    ASSERT_TRUE(skiaBitmap.GetWidth() >= 0);
}

/**
 * @tc.name: GetHeight001
 * @tc.desc: Test GetHeight
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, GetHeight001, TestSize.Level1)
{
    SkiaBitmap skiaBitmap;
    ASSERT_TRUE(skiaBitmap.GetHeight() >= 0);
}

/**
 * @tc.name: GetRowBytes001
 * @tc.desc: Test GetRowBytes
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, GetRowBytes001, TestSize.Level1)
{
    SkiaBitmap skiaBitmap;
    ASSERT_TRUE(skiaBitmap.GetRowBytes() >= 0);
}

/**
 * @tc.name: GetColorType001
 * @tc.desc: Test GetColorType
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, GetColorType001, TestSize.Level1)
{
    SkiaBitmap skiaBitmap;
    ASSERT_TRUE(skiaBitmap.GetColorType() == ColorType::COLORTYPE_UNKNOWN);
}

/**
 * @tc.name: GetAlphaType001
 * @tc.desc: Test GetAlphaType
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, GetAlphaType001, TestSize.Level1)
{
    SkiaBitmap skiaBitmap;
    ASSERT_TRUE(skiaBitmap.GetAlphaType() == AlphaType::ALPHATYPE_UNKNOWN);
}

/**
 * @tc.name: ExtractSubset001
 * @tc.desc: Test ExtractSubset
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, ExtractSubset001, TestSize.Level1)
{
    Bitmap left;
    Rect subset = Rect(0, 0, 50, 50); // 50: right, bottom
    SkiaBitmap skiaBitmap;
    ASSERT_TRUE(!skiaBitmap.ExtractSubset(left, subset));
}

/**
 * @tc.name: GetPixels001
 * @tc.desc: Test GetPixels
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, GetPixels001, TestSize.Level1)
{
    SkiaBitmap skiaBitmap;
    ASSERT_TRUE(skiaBitmap.GetPixels() == nullptr);
}

/**
 * @tc.name: PeekPixels001
 * @tc.desc: Test PeekPixels
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, PeekPixels001, TestSize.Level1)
{
    Pixmap pixmap;
    SkiaBitmap skiaBitmap;
    ASSERT_TRUE(!skiaBitmap.PeekPixels(pixmap));
}

/**
 * @tc.name: ExportSkiaBitmap001
 * @tc.desc: Test ExportSkiaBitmap
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, ExportSkiaBitmap001, TestSize.Level1)
{
    SkiaBitmap skiaBitmap;
    ASSERT_TRUE(skiaBitmap.ExportSkiaBitmap().empty());
}

/**
 * @tc.name: IsImmutable001
 * @tc.desc: Test IsImmutable
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, IsImmutable001, TestSize.Level1)
{
    SkiaBitmap skiaBitmap;
    ASSERT_TRUE(!skiaBitmap.IsImmutable());
}

/**
 * @tc.name: SetImmutable001
 * @tc.desc: Test SetImmutable
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, SetImmutable001, TestSize.Level1)
{
    SkiaBitmap skiaBitmap;
    skiaBitmap.SetImmutable();
    ASSERT_TRUE(!skiaBitmap.IsImmutable());
}

/**
 * @tc.name: GetColor001
 * @tc.desc: Test GetColor
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, GetColor001, TestSize.Level1)
{
    int width = 100;
    int height = 50;
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_BGRA_8888,
        AlphaType::ALPHATYPE_PREMUL};
    SkiaBitmap skiaBitmap;
    skiaBitmap.Build(width, height, bitmapFormat, 0);
    skiaBitmap.ClearWithColor(Color::COLOR_BLACK);
    ASSERT_TRUE(skiaBitmap.GetColor(0, 0) == Color::COLOR_BLACK);
}

/**
 * @tc.name: Free001
 * @tc.desc: Test Free
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, Free001, TestSize.Level1)
{
    SkiaBitmap skiaBitmap;
    skiaBitmap.Free();
    ASSERT_TRUE(skiaBitmap.IsValid());
}

/**
 * @tc.name: IsValid001
 * @tc.desc: Test IsValid
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, IsValid001, TestSize.Level1)
{
    SkiaBitmap skiaBitmap;
    ASSERT_TRUE(skiaBitmap.IsValid());
}

/**
 * @tc.name: IsEmpty001
 * @tc.desc: Test IsEmpty
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, IsEmpty001, TestSize.Level1)
{
    SkiaBitmap skiaBitmap;
    ASSERT_TRUE(skiaBitmap.IsEmpty());
}

/**
 * @tc.name: GetPixmap001
 * @tc.desc: Test GetPixmap
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, GetPixmap001, TestSize.Level1)
{
    SkiaBitmap skiaBitmap;
    ASSERT_TRUE(skiaBitmap.GetPixmap().GetWidth() == 0);
}

/**
 * @tc.name: TryAllocPixels001
 * @tc.desc: Test TryAllocPixels
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, TryAllocPixels001, TestSize.Level1)
{
    int width = 100;
    int height = 50;
    ImageInfo offscreenInfo = { width, height, COLORTYPE_RGBA_8888,
        ALPHATYPE_PREMUL, nullptr};
    SkiaBitmap skiaBitmap;
    skiaBitmap.TryAllocPixels(offscreenInfo);
    ASSERT_TRUE(!skiaBitmap.IsValid());
}

/**
 * @tc.name: Serialize001
 * @tc.desc: Test Serialize
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, Serialize001, TestSize.Level1)
{
    SkiaBitmap skiaBitmap;
    skiaBitmap.Serialize();
    skiaBitmap.Deserialize(nullptr);
    skiaBitmap.Deserialize(std::make_shared<Data>());

    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_BGRA_8888,
        AlphaType::ALPHATYPE_PREMUL};
    skiaBitmap.Build(100, 50, bitmapFormat, 0); // 100:width, 50:height
    skiaBitmap.ClearWithColor(0xFF000000);
    ASSERT_TRUE(skiaBitmap.Serialize() != nullptr);
    skiaBitmap.Deserialize(std::make_shared<Data>());
}

/**
 * @tc.name: Build003
 * @tc.desc: Test Build
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, Build003, TestSize.Level1)
{
    const int width = -100;
    const int height = -50;
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_BGRA_8888, AlphaType::ALPHATYPE_PREMUL};
    SkiaBitmap skiaBitmap;
    bool build = skiaBitmap.Build(width, height, bitmapFormat, 0);
    ASSERT_FALSE(build);
}

/**
 * @tc.name: Build004
 * @tc.desc: Test Build
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, Build004, TestSize.Level1)
{
    const int width = -100;
    const int height = -50;
    ImageInfo offscreenInfo = { width, height, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, nullptr};
    SkiaBitmap skiaBitmap;
    bool build = skiaBitmap.Build(offscreenInfo, 0);
    ASSERT_FALSE(build);
}

/**
 * @tc.name: SetInfo
 * @tc.desc: Test SetInfo
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, SetInfo, TestSize.Level1)
{
    const int width = 100;
    const int height = 50;
    SkiaBitmap skiaBitmap;
    ImageInfo offscreenInfo = { width, height, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, nullptr};
    skiaBitmap.SetInfo(offscreenInfo);
    ASSERT_EQ(ColorType::COLORTYPE_RGBA_8888, skiaBitmap.GetColorType());
    ASSERT_EQ(AlphaType::ALPHATYPE_PREMUL, skiaBitmap.GetAlphaType());
}

/**
 * @tc.name: ComputeByteSize
 * @tc.desc: Test ComputeByteSize
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, ComputeByteSize, TestSize.Level1)
{
    const int width = 100;
    const int height = 50;
    SkiaBitmap skiaBitmap;
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_BGRA_8888, AlphaType::ALPHATYPE_PREMUL };
    bool build = skiaBitmap.Build(width, height, bitmapFormat, 0);
    ASSERT_TRUE(build);
    size_t byteSize = skiaBitmap.ComputeByteSize();
    ASSERT_TRUE(byteSize >= 0);
}

/**
 * @tc.name: CopyPixels
 * @tc.desc: Test CopyPixels
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, CopyPixels, TestSize.Level1)
{
    const int srcWidth = 100;
    const int srcHight = 50;
    std::unique_ptr<Bitmap> dstBitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(dstBitmap != nullptr);
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_BGRA_8888, AlphaType::ALPHATYPE_PREMUL };
    SkiaBitmap skiaBitmap;
    skiaBitmap.Build(srcWidth, srcHight, bitmapFormat, 0);
    skiaBitmap.CopyPixels(*dstBitmap, 0, 0);
    void* pixels = dstBitmap->GetPixels();
    ASSERT_TRUE(pixels != skiaBitmap.GetPixels());
}

/**
 * @tc.name: MakeImage
 * @tc.desc: Test MakeImage
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, MakeImage, TestSize.Level1)
{
    SkBitmap skBitmap;
    SkiaBitmap skiaBitmap;
    skiaBitmap.SetSkBitmap(skBitmap);
    std::shared_ptr<Image> result = skiaBitmap.MakeImage();
    ASSERT_TRUE(result == nullptr);
}

/**
 * @tc.name: Serialize002
 * @tc.desc: Test Serialize
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, Serialize002, TestSize.Level1)
{
    const int width = 100;
    const int height = 50;
    SkiaBitmap skiaBitmap;
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    ImageInfo offscreenInfo = { width, height, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorSpace};
    bool build = skiaBitmap.Build(offscreenInfo, 0);
    ASSERT_TRUE(build);
    std::shared_ptr<Data> result = skiaBitmap.Serialize();
    ASSERT_TRUE(result != nullptr);
}

/**
 * @tc.name: Serialize003
 * @tc.desc: Test Serialize
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, Serialize003, TestSize.Level1)
{
    const int width = 100;
    const int height = 50;
    SkiaBitmap skiaBitmap;
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    ImageInfo offscreenInfo = { width, height, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorSpace};
    bool build = skiaBitmap.Build(offscreenInfo, 0);
    ASSERT_TRUE(build);
    std::shared_ptr<Data> result = skiaBitmap.Serialize();
    ASSERT_TRUE(result->GetSize() == 20104);
}

/**
 * @tc.name: Deserialize
 * @tc.desc: Test Deserialize
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaBitmapTest, Deserialize, TestSize.Level1)
{
    const int width = 100;
    const int height = 50;
    SkiaBitmap skiaBitmap;
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_BGRA_8888, AlphaType::ALPHATYPE_PREMUL};
    skiaBitmap.Build(width, height, bitmapFormat, 0);
    std::shared_ptr<Data> data = skiaBitmap.Serialize();
    ASSERT_TRUE(data != nullptr);
    bool ret = skiaBitmap.Deserialize(data);
    ASSERT_TRUE(ret);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS