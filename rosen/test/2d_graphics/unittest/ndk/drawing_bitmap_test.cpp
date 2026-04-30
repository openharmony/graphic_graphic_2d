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

#include "gtest/gtest.h"

#include "drawing_bitmap.h"
#include "drawing_error_code.h"
#include "drawing_types.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingBitmapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
protected:
    OH_Drawing_Bitmap* bitmap_ = nullptr;
};

void NativeDrawingBitmapTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NativeDrawingBitmapTest::TearDownTestCase() {}
void NativeDrawingBitmapTest::SetUp()
{
    bitmap_ = OH_Drawing_BitmapCreate();
    ASSERT_NE(bitmap_, nullptr);
}

void NativeDrawingBitmapTest::TearDown()
{
    if (bitmap_ != nullptr) {
        OH_Drawing_BitmapDestroy(bitmap_);
        bitmap_ = nullptr;
    }
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapBuild001
 * @tc.desc: test for drawing_bitmap build.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapBuild001, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_ALPHA_8, ALPHA_FORMAT_PREMUL };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    EXPECT_EQ(width, OH_Drawing_BitmapGetWidth(bitmap_));
    EXPECT_EQ(height, OH_Drawing_BitmapGetHeight(bitmap_));
    OH_Drawing_BitmapBuild(bitmap_, width, height, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BitmapBuild(nullptr, width, height, &bitmapFormat);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BitmapGetWidth(nullptr), 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BitmapGetHeight(nullptr), 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapBuild002
 * @tc.desc: test for drawing_bitmap build.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapBuild002, TestSize.Level1)
{
    const unsigned int width = 0;
    const unsigned int height = 0;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGB_565, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    EXPECT_EQ(width, OH_Drawing_BitmapGetWidth(bitmap_));
    EXPECT_EQ(height, OH_Drawing_BitmapGetHeight(bitmap_));
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapBuild003
 * @tc.desc: test for drawing_bitmap build.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapBuild003, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_ARGB_4444, ALPHA_FORMAT_UNPREMUL };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    EXPECT_EQ(width, OH_Drawing_BitmapGetWidth(bitmap_));
    EXPECT_EQ(height, OH_Drawing_BitmapGetHeight(bitmap_));
    EXPECT_EQ(OH_Drawing_BitmapGetPixels(bitmap_) == nullptr, false);
    EXPECT_EQ(OH_Drawing_BitmapGetPixels(nullptr) == nullptr, true);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapBuild004
 * @tc.desc: test for drawing_bitmap build.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapBuild004, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    EXPECT_EQ(width, OH_Drawing_BitmapGetWidth(bitmap_));
    EXPECT_EQ(height, OH_Drawing_BitmapGetHeight(bitmap_));
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapCreateFromPixels005
 * @tc.desc: test for OH_Drawing_BitmapCreateFromPixels.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapCreateFromPixels005, TestSize.Level1)
{
    OH_Drawing_Image_Info imageInfo;
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200;
    constexpr uint32_t height = 200;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    void* pixels = OH_Drawing_BitmapGetPixels(bitmap);
    EXPECT_NE(pixels, nullptr);
    uint32_t rowBytes = width * height * 4;
    bitmap_ = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    EXPECT_NE(bitmap_, nullptr);
    bitmap_ = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, 0);
    EXPECT_EQ(bitmap_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    bitmap_ = OH_Drawing_BitmapCreateFromPixels(&imageInfo, nullptr, 0);
    EXPECT_EQ(bitmap_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    bitmap_ = OH_Drawing_BitmapCreateFromPixels(nullptr, nullptr, 0);
    EXPECT_EQ(bitmap_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetImageInfo006
 * @tc.desc: test for drawing_bitmapGetImageInfo.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetImageInfo006, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    OH_Drawing_Image_Info* imageInfo = new OH_Drawing_Image_Info();
    OH_Drawing_BitmapGetImageInfo(bitmap_, imageInfo);
    EXPECT_EQ(width, imageInfo->width);
    EXPECT_EQ(height, imageInfo->height);
    OH_Drawing_BitmapGetImageInfo(nullptr, imageInfo);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BitmapGetImageInfo(bitmap_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingBitmapTest_BitmapReadPixels007
 * @tc.desc: test for drawing_BitmapReadPixels.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_BitmapReadPixels007, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat {COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL};
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    OH_Drawing_Image_Info imageInfo {width, height, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL};
    void* pixels = new uint32_t[width * height];
    bool res = OH_Drawing_BitmapReadPixels(nullptr, nullptr, nullptr, width * 4, 0, 0);
    EXPECT_EQ(res, false);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    res = OH_Drawing_BitmapReadPixels(nullptr, &imageInfo, pixels, width * 4, 0, 0);
    EXPECT_EQ(res, false);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    res = OH_Drawing_BitmapReadPixels(bitmap_, nullptr, pixels, width * 4, 0, 0);
    EXPECT_EQ(res, false);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    res = OH_Drawing_BitmapReadPixels(bitmap_, &imageInfo, nullptr, width * 4, 0, 0);
    EXPECT_EQ(res, false);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    res = OH_Drawing_BitmapReadPixels(bitmap_, &imageInfo, pixels, width * 4, 0, 0);
    EXPECT_EQ(res, true);
    if (pixels != nullptr) {
        delete[] reinterpret_cast<uint32_t*>(pixels);
        pixels = nullptr;
    }
}

/*
 * @tc.name: NativeDrawingBitmapTest_GetColorFormat008
 * @tc.desc: test for drawing_BitmapGetColorFormat.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_GetColorFormat008, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_ColorFormat formats[] = {
        COLOR_FORMAT_UNKNOWN,
        COLOR_FORMAT_ALPHA_8,
        COLOR_FORMAT_RGB_565,
        COLOR_FORMAT_ARGB_4444,
        COLOR_FORMAT_RGBA_8888,
        COLOR_FORMAT_BGRA_8888
    };

    OH_Drawing_AlphaFormat alphaFormats[] = {
        ALPHA_FORMAT_UNKNOWN,
        ALPHA_FORMAT_OPAQUE,
        ALPHA_FORMAT_PREMUL,
        ALPHA_FORMAT_UNPREMUL
    };
    OH_Drawing_ColorFormat colorFormat_;
    for (int i = 1; i < 6; i++) {
        OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();
        OH_Drawing_BitmapFormat bitmapFormat = {formats[i], alphaFormats[2]};
        OH_Drawing_BitmapBuild(bitmap, width, height, &bitmapFormat);
        if (bitmap == nullptr) {
            colorFormat_ = OH_Drawing_BitmapGetColorFormat(bitmap);
            EXPECT_EQ(colorFormat_, formats[0]);
            EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
        }
        colorFormat_ = OH_Drawing_BitmapGetColorFormat(bitmap);
        EXPECT_EQ(colorFormat_, formats[i]);
    }
}

/*
 * @tc.name: NativeDrawingBitmapTest_GetAlphaFormat009
 * @tc.desc: test for drawing_BitmapGetAlphaFormat.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_GetAlphaFormat009, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_ColorFormat formats[] = {
        COLOR_FORMAT_UNKNOWN,
        COLOR_FORMAT_ALPHA_8,
        COLOR_FORMAT_RGB_565,
        COLOR_FORMAT_ARGB_4444,
        COLOR_FORMAT_RGBA_8888,
        COLOR_FORMAT_BGRA_8888
    };

    OH_Drawing_AlphaFormat alphaFormats[] = {
        ALPHA_FORMAT_UNKNOWN,
        ALPHA_FORMAT_OPAQUE,
        ALPHA_FORMAT_PREMUL,
        ALPHA_FORMAT_UNPREMUL
    };
    OH_Drawing_AlphaFormat alphaFormat_;
    for (int i = 1; i < 4; i++) {
        OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();
        OH_Drawing_BitmapFormat bitmapFormat = {formats[3], alphaFormats[i]};
        OH_Drawing_BitmapBuild(bitmap, width, height, &bitmapFormat);
        if (bitmap == nullptr) {
            alphaFormat_ = OH_Drawing_BitmapGetAlphaFormat(bitmap);
            EXPECT_EQ(alphaFormat_, alphaFormats[0]);
            EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
        }
        alphaFormat_ = OH_Drawing_BitmapGetAlphaFormat(bitmap);
        EXPECT_EQ(alphaFormat_, alphaFormats[i]);
    }
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes001
 * @tc.desc: test for BitmapGetRowBytes func with valid bitmap.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes001, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    const uint32_t rowBytes = 2000;
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGBA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes002
 * @tc.desc: test for BitmapGetRowBytes func with null bitmap.
 * @tc.type: {FUNC}
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes002, TestSize.Level1)
{
    uint32_t bytes = 0;
    EXPECT_EQ(OH_Drawing_BitmapGetRowBytes(nullptr, &bytes), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_BitmapGetRowBytes(nullptr, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes003
 * @tc.desc: test for BitmapGetRowBytes func with null bytes parameter.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes003, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    const uint32_t rowBytes = 2000;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGBA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    EXPECT_EQ(OH_Drawing_BitmapGetRowBytes(bitmap, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes004
 * @tc.desc: test for BitmapGetRowBytes func with ALPHA_8 format.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes004, TestSize.Level1)
{
    const unsigned int width = 100;
    const unsigned int height = 100;
    const uint32_t rowBytes = 100;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_ALPHA_8;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes005
 * @tc.desc: test for BitmapGetRowBytes func with RGB_565 format.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes005, TestSize.Level1)
{
    const unsigned int width = 100;
    const unsigned int height = 100;
    const uint32_t rowBytes = 200;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGB_565;
    imageInfo.alphaType = ALPHA_FORMAT_OPAQUE;
    
    uint8_t* pixels = new uint8_t[width * height * 2];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes006
 * @tc.desc: test for BitmapGetRowBytes func with ARGB_4444 format.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes006, TestSize.Level1)
{
    const unsigned int width = 100;
    const unsigned int height = 100;
    const uint32_t rowBytes = 200;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_ARGB_4444;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 2];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes007
 * @tc.desc: test for BitmapGetRowBytes func with RGBA_8888 format.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes007, TestSize.Level1)
{
    const unsigned int width = 100;
    const unsigned int height = 100;
    const uint32_t rowBytes = 400;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGBA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes008
 * @tc.desc: test for BitmapGetRowBytes func with BGRA_8888 format.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes008, TestSize.Level1)
{
    const unsigned int width = 100;
    const unsigned int height = 100;
    const uint32_t rowBytes = 400;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_BGRA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes009
 * @tc.desc: test for BitmapGetRowBytes func with OPAQUE alpha format.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes009, TestSize.Level1)
{
    const unsigned int width = 100;
    const unsigned int height = 100;
    const uint32_t rowBytes = 400;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGBA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_OPAQUE;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes010
 * @tc.desc: test for BitmapGetRowBytes func with UNPREMUL alpha format.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes010, TestSize.Level1)
{
    const unsigned int width = 100;
    const unsigned int height = 100;
    const uint32_t rowBytes = 400;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGBA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_UNPREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes011
 * @tc.desc: test for BitmapGetRowBytes func with small dimensions.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes011, TestSize.Level1)
{
    const unsigned int width = 1;
    const unsigned int height = 1;
    const uint32_t rowBytes = 4;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGBA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes012
 * @tc.desc: test for BitmapGetRowBytes func with large dimensions.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes012, TestSize.Level1)
{
    const unsigned int width = 1920;
    const unsigned int height = 1080;
    const uint32_t rowBytes = 7680;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGBA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes013
 * @tc.desc: test for BitmapGetRowBytes func with odd width.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes013, TestSize.Level1)
{
    const unsigned int width = 123;
    const unsigned int height = 456;
    const uint32_t rowBytes = 492;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGBA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes014
 * @tc.desc: test for BitmapGetRowBytes func with minimum width.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes014, TestSize.Level1)
{
    const unsigned int width = 1;
    const unsigned int height = 100;
    const uint32_t rowBytes = 4;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGBA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes015
 * @tc.desc: test for BitmapGetRowBytes func with minimum height.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes015, TestSize.Level1)
{
    const unsigned int width = 100;
    const unsigned int height = 1;
    const uint32_t rowBytes = 400;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGBA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes016
 * @tc.desc: test for BitmapGetRowBytes func with square bitmap.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes016, TestSize.Level1)
{
    const unsigned int width = 1024;
    const unsigned int height = 1024;
    const uint32_t rowBytes = 4096;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGBA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes017
 * @tc.desc: test for BitmapGetRowBytes func with wide bitmap.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes017, TestSize.Level1)
{
    const unsigned int width = 2048;
    const unsigned int height = 100;
    const uint32_t rowBytes = 8192;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGBA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes018
 * @tc.desc: test for BitmapGetRowBytes func with tall bitmap.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes018, TestSize.Level1)
{
    const unsigned int width = 100;
    const unsigned int height = 2048;
    const uint32_t rowBytes = 400;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGBA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes019
 * @tc.desc: test for BitmapGetRowBytes func with power of 2 dimension.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes019, TestSize.Level1)
{
    const unsigned int width = 256;
    const unsigned int height = 256;
    const uint32_t rowBytes = 1024;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGBA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapGetRowBytes020
 * @tc)desc: test for BitmapGetRowBytes func with non-power of 2 dimension.
 * @tc.type: FUNC
 * @tc.require: 22932
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetRowBytes020, TestSize.Level1)
{
    const unsigned int width = 123;
    const unsigned int height = 456;
    const uint32_t rowBytes = 492;
    
    OH_Drawing_Image_Info imageInfo;
    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.colorType = COLOR_FORMAT_RGBA_8888;
    imageInfo.alphaType = ALPHA_FORMAT_PREMUL;
    
    uint8_t* pixels = new uint8_t[width * height * 4];
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    ASSERT_NE(bitmap, nullptr);
    
    uint32_t bytes = 0;
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &bytes));
    EXPECT_EQ(rowBytes, bytes);
    
    delete[] pixels;
    OH_Drawing_BitmapDestroy(bitmap);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
