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
#include "message_parcel.h"

#include "common/rs_vector4.h"
#include "modifier_ng/appearance/rs_compositing_filter_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSCompositingFilterModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.name: RSBorderModifierTest
 * @tc.desc: Test Set/Get functions of RSBorderModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSCompositingFilterModifierNGTypeTest, RSCompositingFilterModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSCompositingFilterModifier> modifier =
        std::make_shared<ModifierNG::RSCompositingFilterModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::COMPOSITING_FILTER);

    Vector4f aiInvertValue(0.1f, 0.2f, 0.3f, 0.4f);
    modifier->SetAiInvert(aiInvertValue);
    EXPECT_EQ(modifier->GetAiInvert(), std::optional<Vector4f>(aiInvertValue));

    float grayScaleValue = 0.5f;
    modifier->SetGrayScale(grayScaleValue);
    EXPECT_EQ(modifier->GetGrayScale(), grayScaleValue);

    float brigntnessValue = 0.6f;
    modifier->SetBrightness(brigntnessValue);
    EXPECT_EQ(modifier->GetBrightness(), brigntnessValue);

    float contrastValue = 0.7f;
    modifier->SetContrast(contrastValue);
    EXPECT_EQ(modifier->GetContrast(), contrastValue);

    float saturateValue = 0.8f;
    modifier->SetSaturate(saturateValue);
    EXPECT_EQ(modifier->GetSaturate(), saturateValue);

    float sepiaValue = 0.9f;
    modifier->SetSepia(sepiaValue);
    EXPECT_EQ(modifier->GetSepia(), sepiaValue);

    float invertValue = 1.0f;
    modifier->SetInvert(invertValue);
    EXPECT_EQ(modifier->GetInvert(), invertValue);

    float hueRotateValue = 1.1f;
    modifier->SetHueRotate(hueRotateValue);
    EXPECT_EQ(modifier->GetHueRotate(), std::optional<float>(hueRotateValue));

    Color colorBlendValue(16, 15, 14, 16);
    modifier->SetColorBlend(colorBlendValue);
    EXPECT_EQ(modifier->GetColorBlend(), std::optional<Color>(colorBlendValue));

    float lightUpEffectDegreeValue = 1.2f;
    modifier->SetLightUpEffectDegree(lightUpEffectDegreeValue);
    EXPECT_EQ(modifier->GetLightUpEffectDegree(), lightUpEffectDegreeValue);

    float dynamicDimDegreeValue = 1.3f;
    modifier->SetDynamicDimDegree(dynamicDimDegreeValue);
    EXPECT_EQ(modifier->GetDynamicDimDegree(), std::optional<float>(dynamicDimDegreeValue));

    std::vector<std::pair<float, float>> fractionStops = {{0.0f, 0.0f}, {1.0f, 1.0f}};
    float blurRadius = 1.0f;
    GradientDirection direction = GradientDirection::LEFT;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurParaValue =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    modifier->SetLinearGradientBlurPara(linearGradientBlurParaValue);
    EXPECT_EQ(modifier->GetLinearGradientBlurPara(), linearGradientBlurParaValue);

    float foregroundBlurRadiusValue = 1.4f;
    modifier->SetForegroundBlurRadius(foregroundBlurRadiusValue);
    EXPECT_EQ(modifier->GetForegroundBlurRadius(), foregroundBlurRadiusValue);

    float foregroundBlurSaturationValue = 1.5f;
    modifier->SetForegroundBlurSaturation(foregroundBlurSaturationValue);
    EXPECT_EQ(modifier->GetForegroundBlurSaturation(), foregroundBlurSaturationValue);

    float foregroundBlurBrightnessValue = 1.6f;
    modifier->SetForegroundBlurBrightness(foregroundBlurBrightnessValue);
    EXPECT_EQ(modifier->GetForegroundBlurBrightness(), foregroundBlurBrightnessValue);

    Color foregroundBlurMaskColorValue(15, 11, 15, 16);
    modifier->SetForegroundBlurMaskColor(foregroundBlurMaskColorValue);
    EXPECT_EQ(modifier->GetForegroundBlurMaskColor(), foregroundBlurMaskColorValue);

    int foregroundBlurColorModeValue = 1;
    modifier->SetForegroundBlurColorMode(foregroundBlurColorModeValue);
    EXPECT_EQ(modifier->GetForegroundBlurColorMode(), foregroundBlurColorModeValue);

    float foregroundBlurRadiusXValue = 1.7f;
    modifier->SetForegroundBlurRadiusX(foregroundBlurRadiusXValue);
    EXPECT_EQ(modifier->GetForegroundBlurRadiusX(), foregroundBlurRadiusXValue);

    float foregroundBlurRadiusYValue = 1.8f;
    modifier->SetForegroundBlurRadiusY(foregroundBlurRadiusYValue);
    EXPECT_EQ(modifier->GetForegroundBlurRadiusY(), foregroundBlurRadiusYValue);

    bool fgBlurDisableSystemAdaptationValue = true;
    modifier->SetFgBlurDisableSystemAdaptation(fgBlurDisableSystemAdaptationValue);
    EXPECT_EQ(modifier->GetFgBlurDisableSystemAdaptation(), fgBlurDisableSystemAdaptationValue);
}
} // namespace OHOS::Rosen