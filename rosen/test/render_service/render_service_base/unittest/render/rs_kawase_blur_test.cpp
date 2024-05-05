/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "render/rs_kawase_blur.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class KawaseBlurFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void KawaseBlurFilterTest::SetUpTestCase() {}
void KawaseBlurFilterTest::TearDownTestCase() {}
void KawaseBlurFilterTest::SetUp() {}
void KawaseBlurFilterTest::TearDown() {}

/**
 * @tc.name: testInterface
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, testInterface, TestSize.Level1)
{
    KawaseBlurFilter* kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();

    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    KawaseParameter param = KawaseParameter(src, dst, 1);
    std::shared_ptr<Drawing::Image> image;

    kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param);
}

/**
 * @tc.name: setupBlurEffectAdvancedFilterTest001
 * @tc.desc: Verify function setupBlurEffectAdvancedFilter
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, setupBlurEffectAdvancedFilterTest001, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    kawaseBlurFilter->setupBlurEffectAdvancedFilter();
    EXPECT_NE(kawaseBlurFilter->blurEffectAF_, nullptr);
}

/**
 * @tc.name: GetShaderTransformTest001
 * @tc.desc: Verify function GetShaderTransform
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, GetShaderTransformTest001, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    Drawing::Canvas canvas(1, 1);
    Drawing::Rect blurRect;
    kawaseBlurFilter->GetShaderTransform(&canvas, blurRect, 1.0f, 1.0f);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: OutputOriginalImageTest001
 * @tc.desc: Verify function OutputOriginalImage
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, OutputOriginalImageTest001, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>();
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->OutputOriginalImage(canvas, image, param);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: ApplyBlurTest001
 * @tc.desc: Verify function ApplyBlur
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, ApplyBlurTest001, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    auto blurImage = std::make_shared<Drawing::Image>();
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>();
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    EXPECT_TRUE(kawaseBlurFilter->ApplyBlur(canvas, image, blurImage, param));
}

/**
 * @tc.name: AdjustRadiusAndScaleTest001
 * @tc.desc: Verify function AdjustRadiusAndScale
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, AdjustRadiusAndScaleTest001, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    kawaseBlurFilter->ComputeRadiusAndScale(101);
    EXPECT_EQ(kawaseBlurFilter->blurScale_, 0.0625f);
    kawaseBlurFilter->ComputeRadiusAndScale(40);
    EXPECT_EQ(kawaseBlurFilter->blurScale_, 0.125f);
    kawaseBlurFilter->ComputeRadiusAndScale(20);
    EXPECT_EQ(kawaseBlurFilter->blurScale_, 0.25f);
    kawaseBlurFilter->ComputeRadiusAndScale(1);
    EXPECT_EQ(kawaseBlurFilter->blurScale_, 0.5F);
}

/**
 * @tc.name: GetDescriptionTest001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, GetDescriptionTest001, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    EXPECT_EQ(kawaseBlurFilter->GetDescription(), "blur radius is 0.000000");
}
} // namespace Rosen
} // namespace OHOS
