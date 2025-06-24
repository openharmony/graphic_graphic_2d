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

#include <cstdlib>
#include <cstring>
#include <memory>
#include <securec.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "ui_effect/property/include/rs_ui_blur_filter.h"

#include "modifier_ng/appearance/rs_background_filter_modifier.h"
#include "ui/rs_canvas_node.h"

using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen::ModifierNG {
class RSBackgroundFilterModifierNGTypeTest : public testing::Test {
public:
    std::shared_ptr<ModifierNG::RSBackgroundFilterModifier> modifier =
        std::make_shared<ModifierNG::RSBackgroundFilterModifier>();
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

HWTEST_F(RSBackgroundFilterModifierNGTypeTest, SystemBarEffectTest, TestSize.Level1)
{
    modifier->SetSystemBarEffect(true);
    EXPECT_TRUE(modifier->GetSystemBarEffect());

    modifier->SetSystemBarEffect(false);
    EXPECT_FALSE(modifier->GetSystemBarEffect());
}

HWTEST_F(RSBackgroundFilterModifierNGTypeTest, WaterRippleProgressTest, TestSize.Level1)
{
    modifier->SetWaterRippleProgress(0.75f);
    EXPECT_FLOAT_EQ(modifier->GetWaterRippleProgress(), 0.75f);
}

HWTEST_F(RSBackgroundFilterModifierNGTypeTest, WaterRippleParamsTest, TestSize.Level1)
{
    RSWaterRipplePara para{};
    modifier->SetWaterRippleParams(para);
    auto result = modifier->GetWaterRippleParams();
    EXPECT_TRUE(result.has_value());
}

HWTEST_F(RSBackgroundFilterModifierNGTypeTest, MagnifierParamsTest, TestSize.Level1)
{
    auto magnifierParams = std::make_shared<RSMagnifierParams>();
    modifier->SetMagnifierParams(magnifierParams);
    EXPECT_EQ(modifier->GetMagnifierParams(), magnifierParams);
}

HWTEST_F(RSBackgroundFilterModifierNGTypeTest, BackgroundBlurRadiusTest, TestSize.Level1)
{
    modifier->SetBackgroundBlurRadius(5.5f);
    EXPECT_FLOAT_EQ(modifier->GetBackgroundBlurRadius(), 5.5f);
}

HWTEST_F(RSBackgroundFilterModifierNGTypeTest, BackgroundBlurSaturationTest, TestSize.Level1)
{
    modifier->SetBackgroundBlurSaturation(0.8f);
    EXPECT_FLOAT_EQ(modifier->GetBackgroundBlurSaturation(), 0.8f);
}

HWTEST_F(RSBackgroundFilterModifierNGTypeTest, BackgroundBlurBrightnessTest, TestSize.Level1)
{
    modifier->SetBackgroundBlurBrightness(1.2f);
    EXPECT_FLOAT_EQ(modifier->GetBackgroundBlurBrightness(), 1.2f);
}

HWTEST_F(RSBackgroundFilterModifierNGTypeTest, BackgroundBlurMaskColorTest, TestSize.Level1)
{
    Color maskColor = Color(255, 0, 0, 255);
    modifier->SetBackgroundBlurMaskColor(maskColor);
    EXPECT_EQ(modifier->GetBackgroundBlurMaskColor(), maskColor);
}

HWTEST_F(RSBackgroundFilterModifierNGTypeTest, BackgroundBlurColorModeTest, TestSize.Level1)
{
    modifier->SetBackgroundBlurColorMode(2);
    EXPECT_EQ(modifier->GetBackgroundBlurColorMode(), 2);
}

HWTEST_F(RSBackgroundFilterModifierNGTypeTest, BackgroundBlurRadiusXYTest, TestSize.Level1)
{
    modifier->SetBackgroundBlurRadiusX(3.3f);
    modifier->SetBackgroundBlurRadiusY(4.4f);
    EXPECT_FLOAT_EQ(modifier->GetBackgroundBlurRadiusX(), 3.3f);
    EXPECT_FLOAT_EQ(modifier->GetBackgroundBlurRadiusY(), 4.4f);
}

HWTEST_F(RSBackgroundFilterModifierNGTypeTest, BgBlurDisableSystemAdaptationTest, TestSize.Level1)
{
    modifier->SetBgBlurDisableSystemAdaptation(true);
    EXPECT_TRUE(modifier->GetBgBlurDisableSystemAdaptation());

    modifier->SetBgBlurDisableSystemAdaptation(false);
    EXPECT_FALSE(modifier->GetBgBlurDisableSystemAdaptation());
}

HWTEST_F(RSBackgroundFilterModifierNGTypeTest, GreyCoefTest, TestSize.Level1)
{
    Vector2f coef{0.6f, 0.4f};
    modifier->SetGreyCoef(coef);
    auto result = modifier->GetGreyCoef();
    EXPECT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result->data_[0], 0.6f);
    EXPECT_FLOAT_EQ(result->data_[1], 0.4f);
}

HWTEST_F(RSBackgroundFilterModifierNGTypeTest, GetTypeTest, TestSize.Level1)
{
    EXPECT_EQ(modifier->GetType(), RSModifierType::BACKGROUND_FILTER);
}

HWTEST_F(RSBackgroundFilterModifierNGTypeTest, SetUIFilterTest, TestSize.Level1)
{
    modifier->SetUIFilter(nullptr);

    auto rsUIFilter = std::make_shared<RSUIFilter>();
    EXPECT_NE(rsUIFilter, nullptr);

    std::shared_ptr<ModifierNG::RSBackgroundFilterModifier> modifier01 =
    std::make_shared<ModifierNG::RSBackgroundFilterModifier>();

    rsUIFilter->Insert(nullptr);
    float radius = 1.0f;
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    filterBlurPara->SetRadius(radius);
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara->SetBlurPara(filterBlurPara);
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);
    rsUIFilter->Insert(rsUIFilterParaBase);

    modifier01->SetUIFilter(rsUIFilter);
    auto node = RSCanvasNode::Create();
    modifier01->node_ = node->weak_from_this();
    modifier01->SetUIFilter(rsUIFilter);
    modifier01->SetUIFilter(rsUIFilter);
    auto rsUIFilter01 = std::make_shared<RSUIFilter>();
    modifier01->SetUIFilter(rsUIFilter01);
}

HWTEST_F(RSBackgroundFilterModifierNGTypeTest, AttachUIFilterProperty, TestSize.Level1)
{
    modifier->AttachUIFilterProperty(nullptr);
    auto node = RSCanvasNode::Create();
    modifier->node_ = node->weak_from_this();

    auto rsUIFilter = std::make_shared<RSUIFilter>();
    EXPECT_NE(rsUIFilter, nullptr);

    modifier->AttachUIFilterProperty(rsUIFilter);
    rsUIFilter->Insert(nullptr);
    float radius = 1.0f;
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    filterBlurPara->SetRadius(radius);
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara->SetBlurPara(filterBlurPara);
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);
    rsUIFilter->Insert(rsUIFilterParaBase);
    modifier->AttachUIFilterProperty(rsUIFilter);
}

} // namespace OHOS::Rosen