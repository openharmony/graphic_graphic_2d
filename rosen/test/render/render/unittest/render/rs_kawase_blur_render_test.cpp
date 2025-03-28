/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

class KawaseBlurFilterRenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void KawaseBlurFilterRenderTest::SetUpTestCase() {}
void KawaseBlurFilterRenderTest::TearDownTestCase() {}
void KawaseBlurFilterRenderTest::SetUp() {}
void KawaseBlurFilterRenderTest::TearDown() {}

/**
 * @tc.name: TestInterface
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterRenderTest, TestInterface, TestSize.Level1)
{
    KawaseBlurFilter* kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();

    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    KawaseParameter param = KawaseParameter(src, dst, 1);
    std::shared_ptr<Drawing::Image> image;

    EXPECT_FALSE(kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param));
}

/**
 * @tc.name: TestSetupBlurEffectAdvancedFilter001
 * @tc.desc: Verify function setupBlurEffectAdvancedFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterRenderTest, TestSetupBlurEffectAdvancedFilter001, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    kawaseBlurFilter->setupBlurEffectAdvancedFilter();
    EXPECT_NE(kawaseBlurFilter->blurEffectAF_, nullptr);
}

/**
 * @tc.name: TestGetShaderTransform001
 * @tc.desc: Verify function GetShaderTransform
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterRenderTest, TestGetShaderTransform001, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    Drawing::Canvas canvas(1, 1);
    Drawing::Rect blurRect;
    kawaseBlurFilter->GetShaderTransform(&canvas, blurRect, 1.0f, 1.0f);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: TestOutputOriginalImage001
 * @tc.desc: Verify function OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterRenderTest, TestOutputOriginalImage001, TestSize.Level1)
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
 * @tc.name: TestApplyBlur001
 * @tc.desc: Verify function ApplyBlur
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterRenderTest, TestApplyBlur001, TestSize.Level1)
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
 * @tc.name: TestApplyBlur002
 * @tc.desc: Verify function ApplyBlur
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterRenderTest, TestApplyBlur002, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    Drawing::Canvas canvas(1, 1);
    auto blurImage = std::make_shared<Drawing::Image>();
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>();
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    EXPECT_TRUE(!kawaseBlurFilter->ApplyBlur(canvas, nullptr, blurImage, param));
}

/**
 * @tc.name: TestAdjustRadiusAndScale001
 * @tc.desc: Verify function AdjustRadiusAndScale
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterRenderTest, TestAdjustRadiusAndScale001, TestSize.Level1)
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
 * @tc.name: TestGetDescription001
 * @tc.desc: Verify function GetDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterRenderTest, TestGetDescription001, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    EXPECT_EQ(kawaseBlurFilter->GetDescription(), "blur radius is 0.000000");
}

/**
 * @tc.name: TestCheckInputImage
 * @tc.desc: Verify function CheckInputImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterRenderTest, TestCheckInputImage, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    auto checkedImage = std::make_shared<Drawing::Image>();
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>();
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->CheckInputImage(canvas, image, param, checkedImage);
    EXPECT_EQ(param.src.GetBottom(), 0);
}

/**
 * @tc.name: TestExecutePingPongBlur
 * @tc.desc: Verify function ExecutePingPongBlur
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KawaseBlurFilterRenderTest, TestExecutePingPongBlur, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    Drawing::Canvas canvas;
    auto input = std::make_shared<Drawing::Image>();
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>();
    KawaseParameter inParam(srcRect, dstRect, 1, colorFilter, 0.f);
    KawaseBlurFilter::BlurParams blur;
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    kawaseBlurFilter->SetupSimpleFilter();
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    EXPECT_EQ(kawaseBlurFilter->blurEffectAF_, nullptr);
}

} // namespace Rosen
} // namespace OHOS
