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
#include "skia_adapter/skia_image_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaImageInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaImageInfoTest::SetUpTestCase() {}
void SkiaImageInfoTest::TearDownTestCase() {}
void SkiaImageInfoTest::SetUp() {}
void SkiaImageInfoTest::TearDown() {}

/**
 * @tc.name: ConvertToSkColorType001
 * @tc.desc: Test ConvertToSkColorType
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageInfoTest, ConvertToSkColorType001, TestSize.Level1)
{
    SkiaImageInfo::ConvertToSkColorType(ColorType::COLORTYPE_UNKNOWN);
    SkiaImageInfo::ConvertToSkColorType(ColorType::COLORTYPE_ALPHA_8);
    SkiaImageInfo::ConvertToSkColorType(ColorType::COLORTYPE_RGB_565);
    SkiaImageInfo::ConvertToSkColorType(ColorType::COLORTYPE_ARGB_4444);
    SkiaImageInfo::ConvertToSkColorType(ColorType::COLORTYPE_RGBA_8888);
    SkiaImageInfo::ConvertToSkColorType(ColorType::COLORTYPE_BGRA_8888);
    SkiaImageInfo::ConvertToSkColorType(ColorType::COLORTYPE_RGBA_F16);
    SkiaImageInfo::ConvertToSkColorType(ColorType::COLORTYPE_N32);
    SkiaImageInfo::ConvertToSkColorType(ColorType::COLORTYPE_RGBA_1010102);
}

/**
 * @tc.name: ConvertToSkAlphaType001
 * @tc.desc: Test ConvertToSkAlphaType
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageInfoTest, ConvertToSkAlphaType001, TestSize.Level1)
{
    SkiaImageInfo::ConvertToSkAlphaType(AlphaType::ALPHATYPE_UNKNOWN);
    SkiaImageInfo::ConvertToSkAlphaType(AlphaType::ALPHATYPE_OPAQUE);
    SkiaImageInfo::ConvertToSkAlphaType(AlphaType::ALPHATYPE_PREMUL);
    SkiaImageInfo::ConvertToSkAlphaType(AlphaType::ALPHATYPE_UNPREMUL);
}

/**
 * @tc.name: ConvertToSkImageInfo001
 * @tc.desc: Test ConvertToSkImageInfo
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageInfoTest, ConvertToSkImageInfo001, TestSize.Level1)
{
    ImageInfo info;
    SkiaImageInfo::ConvertToSkImageInfo(info);
}

/**
 * @tc.name: ConvertToColorType001
 * @tc.desc: Test ConvertToColorType
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageInfoTest, ConvertToColorType001, TestSize.Level1)
{
    SkiaImageInfo::ConvertToColorType(SkColorType::kUnknown_SkColorType);
    SkiaImageInfo::ConvertToColorType(SkColorType::kAlpha_8_SkColorType);
    SkiaImageInfo::ConvertToColorType(SkColorType::kRGB_565_SkColorType);
    SkiaImageInfo::ConvertToColorType(SkColorType::kARGB_4444_SkColorType);
    SkiaImageInfo::ConvertToColorType(SkColorType::kRGBA_8888_SkColorType);
    SkiaImageInfo::ConvertToColorType(SkColorType::kBGRA_8888_SkColorType);
    SkiaImageInfo::ConvertToColorType(SkColorType::kRGBA_F16_SkColorType);
    SkiaImageInfo::ConvertToColorType(SkColorType::kRGBA_1010102_SkColorType);
}

/**
 * @tc.name: ConvertToAlphaType001
 * @tc.desc: Test ConvertToAlphaType
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageInfoTest, ConvertToAlphaType001, TestSize.Level1)
{
    SkiaImageInfo::ConvertToAlphaType(SkAlphaType::kUnknown_SkAlphaType);
    SkiaImageInfo::ConvertToAlphaType(SkAlphaType::kOpaque_SkAlphaType);
    SkiaImageInfo::ConvertToAlphaType(SkAlphaType::kPremul_SkAlphaType);
    SkiaImageInfo::ConvertToAlphaType(SkAlphaType::kUnpremul_SkAlphaType);
}

/**
 * @tc.name: ConvertToRSImageInfo001
 * @tc.desc: Test ConvertToRSImageInfo
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageInfoTest, ConvertToRSImageInfo001, TestSize.Level1)
{
    SkImageInfo info;
    SkiaImageInfo::ConvertToRSImageInfo(info);
}

/**
 * @tc.name: ConvertToSkEncodedImageFormat001
 * @tc.desc: Test ConvertToSkEncodedImageFormat
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageInfoTest, ConvertToSkEncodedImageFormat001, TestSize.Level1)
{
    SkiaImageInfo::ConvertToSkEncodedImageFormat(EncodedImageFormat::JPEG);
    SkiaImageInfo::ConvertToSkEncodedImageFormat(EncodedImageFormat::PNG);
    SkiaImageInfo::ConvertToSkEncodedImageFormat(EncodedImageFormat::WEBP);
    SkiaImageInfo::ConvertToSkEncodedImageFormat(EncodedImageFormat::UNKNOWN);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS