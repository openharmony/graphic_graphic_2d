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

#include <gtest/gtest.h>

#include "ge_magnifier_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEMagnifierShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void GEMagnifierShaderFilterTest::SetUpTestCase() {}
void GEMagnifierShaderFilterTest::TearDownTestCase() {}

void GEMagnifierShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEMagnifierShaderFilterTest::TearDown() {}

/**
 * @tc.name: GetDescription001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, GetDescription001, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000,
        0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::string expectStr = "GEMagnifierShaderFilter";
    EXPECT_EQ(filter->GetDescription(), expectStr);
}

/**
 * @tc.name: ProcessImage001
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, ProcessImage001, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000,
        0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::shared_ptr<Drawing::Image> image = nullptr;
    EXPECT_EQ(filter->ProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: ProcessImage002
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, ProcessImage002, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000,
        0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    EXPECT_EQ(filter->ProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: ProcessImage003
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, ProcessImage003, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000,
        0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    // 1.0f, 1.0f, 200.0f, 200.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 200.0f, 200.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    EXPECT_EQ(filter->ProcessImage(canvas_, image_, src, dst), image_);
}

/**
 * @tc.name: ProcessImage004
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, ProcessImage004, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000,
        0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    EXPECT_EQ(filter->ProcessImage(canvas_, image_, src_, dst_), image_);

    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_EQ(filter->ProcessImage(canvas_, image, src_, dst_), image);
    EXPECT_TRUE(filter->InitMagnifierEffect());
}

/**
 * @tc.name: ConvertToRgba001
 * @tc.desc: Verify function ConvertToRgba
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, ConvertToRgba001, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000,
        0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    uint32_t color1 = uint32_t(0x80808000);
    float maskColor1[4] = { 0.0f }; // 4 len of tuple
    filter->ConvertToRgba(color1, maskColor1, 4);
}

/**
 * @tc.name: ConvertToRgba002
 * @tc.desc: Verify function ConvertToRgba
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, ConvertToRgba002, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000,
        0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    uint32_t color1 = uint32_t(0x80808000);
    float maskColor1[4] = { 0.0f }; // 4 len of tuple
    filter->ConvertToRgba(color1, maskColor1, 3);
    filter->ConvertToRgba(color1, nullptr, 3);
}

/**
 * @tc.name: MakeMagnifierShader001
 * @tc.desc: Verify function MakeMagnifierShader
 * @tc.type:FUNC
 */
HWTEST_F(GEMagnifierShaderFilterTest, MakeMagnifierShader001, TestSize.Level1)
{
    Drawing::GEMagnifierShaderFilterParams params{1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0x00000000, 0x00000000,
        0x00000000, 0x00000000};
    auto filter = std::make_shared<GEMagnifierShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    filter->magnifierPara_ = nullptr;
    Drawing::Matrix matrix;
    EXPECT_NE(image_, nullptr);
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image_, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    float imageWidth = image_->GetWidth();
    float imageHeight = image_->GetHeight();
    auto builder = filter->MakeMagnifierShader(imageShader, imageWidth, imageHeight);
    EXPECT_EQ(builder, nullptr);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS