/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include <memory>
#include <ostream>
#include "gtest/gtest.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPropertyModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    const Vector2f INITIAL_VALUE_SIZE_2F = Vector2f(60.f, 30.f);
    const Vector2f INITIAL_VALUE_POSITION_2F = Vector2f(50.f, 50.f);
    const Vector2f INITIAL_VALUE_PIVOT_2F = Vector2f(70.f, 80.f);
    const Vector2f INITIAL_VALUE_SCALE_2F = Vector2f(2.f, 2.f);
    const Vector2f INITIAL_VALUE_SKEW_2F = Vector2f(30.f, 30.f);
    const Vector3f INITIAL_VALUE_SKEW_3F = Vector3f(30.f, 30.f, 30.f);
    const Vector4f INITIAL_VALUE_RECT_4F = Vector4f(10.f, 10.f, 10.f, 10.f);
};

void RSPropertyModifierTest::SetUpTestCase() {}
void RSPropertyModifierTest::TearDownTestCase() {}
void RSPropertyModifierTest::SetUp() {}
void RSPropertyModifierTest::TearDown() {}

/**
 * @tc.name: GetModifierType
 * @tc.desc: RSEnvForegroundColorModifier Test
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyModifierTest, GetModifierType, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSEnvForegroundColorModifier> colorModifier =
        std::make_shared<RSEnvForegroundColorModifier>(property);
    ASSERT_NE(colorModifier, nullptr);
    RSModifierType ModifierType = colorModifier->GetModifierType();
    EXPECT_EQ(ModifierType, RSModifierType::ENV_FOREGROUND_COLOR);
}

/**
 * @tc.name: CreateRenderModifier
 * @tc.desc: RSEnvForegroundColorModifier Test
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyModifierTest, CreateRenderModifier, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSEnvForegroundColorModifier> colorModifier =
        std::make_shared<RSEnvForegroundColorModifier>(property);
    ASSERT_NE(colorModifier, nullptr);
    auto res = colorModifier->CreateRenderModifier();
    EXPECT_NE(res, nullptr);
}

/**
 * @tc.name: RSHDRBrightnessModifier
 * @tc.desc: RSHDRBrightnessModifier CreateRenderModifier Test
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyModifierTest, RSHDRBrightnessModifier001, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSHDRBrightnessModifier> hdrModifier =
        std::make_shared<RSHDRBrightnessModifier>(property);
    ASSERT_NE(hdrModifier, nullptr);
    auto res = hdrModifier->CreateRenderModifier();
    EXPECT_NE(res, nullptr);
}

/**
 * @tc.name: RSHDRBrightnessModifier
 * @tc.desc: RSHDRBrightnessModifier GetModifierType Test
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyModifierTest, RSHDRBrightnessModifier002, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSHDRBrightnessModifier> hdrModifier =
        std::make_shared<RSHDRBrightnessModifier>(property);
    ASSERT_NE(hdrModifier, nullptr);
    RSModifierType res = hdrModifier->GetModifierType();
    EXPECT_EQ(res, RSModifierType::HDR_BRIGHTNESS);
}

/**
 * @tc.name: GetModifierType02
 * @tc.desc: RSEnvForegroundColorStrategyModifier Test
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyModifierTest, GetModifierType02, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSEnvForegroundColorStrategyModifier> colorStrategyModifier =
        std::make_shared<RSEnvForegroundColorStrategyModifier>(property);
    ASSERT_NE(colorStrategyModifier, nullptr);
    RSModifierType res = colorStrategyModifier->GetModifierType();
    EXPECT_EQ(res, RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY);
}

/**
 * @tc.name: CreateRenderModifier02
 * @tc.desc: RSEnvForegroundColorStrategyModifier Test
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyModifierTest, CreateRenderModifier02, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSEnvForegroundColorStrategyModifier> colorStrategyModifier =
        std::make_shared<RSEnvForegroundColorStrategyModifier>(property);
    ASSERT_NE(colorStrategyModifier, nullptr);
    auto renderModifier = colorStrategyModifier->CreateRenderModifier();
    EXPECT_NE(renderModifier, nullptr);
}

/**
 * @tc.name: GetModifierType03
 * @tc.desc: RSCustomClipToFrameModifier Test
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyModifierTest, GetModifierType03, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSCustomClipToFrameModifier> clipModifier =
        std::make_shared<RSCustomClipToFrameModifier>(property);
    ASSERT_NE(clipModifier, nullptr);
    RSModifierType ModifierType = clipModifier->GetModifierType();
    EXPECT_EQ(ModifierType, RSModifierType::CUSTOM_CLIP_TO_FRAME);
}

/**
 * @tc.name: CreateRenderModifier03
 * @tc.desc: RSCustomClipToFrameModifier Test
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyModifierTest, CreateRenderModifier03, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSCustomClipToFrameModifier> clipModifier =
        std::make_shared<RSCustomClipToFrameModifier>(property);
    ASSERT_NE(clipModifier, nullptr);
    auto res = clipModifier->CreateRenderModifier();
    EXPECT_NE(res, nullptr);
}

/**
 * @tc.name: Apply01
 * @tc.desc: RSBoundsModifier/RSBoundsSizeModifier/RSBoundsPositionModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyModifierTest, Apply01, TestSize.Level1)
{
    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    ASSERT_NE(geometry, nullptr);

    auto property = std::make_shared<RSProperty<Vector4f>>(INITIAL_VALUE_RECT_4F);
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSModifier> boundsModifier = std::make_shared<RSBoundsModifier>(property);
    ASSERT_NE(boundsModifier, nullptr);
    boundsModifier->Apply(geometry);
    EXPECT_EQ(geometry->x_, 10.f);
    EXPECT_EQ(geometry->y_, 10.f);
    EXPECT_EQ(geometry->width_, 10.f);
    EXPECT_EQ(geometry->height_, 10.f);

    auto property02 = std::make_shared<RSProperty<Vector2f>>(INITIAL_VALUE_SIZE_2F);
    ASSERT_NE(property02, nullptr);
    std::shared_ptr<RSModifier> boundsSizeModifier = std::make_shared<RSBoundsSizeModifier>(property02);
    ASSERT_NE(boundsSizeModifier, nullptr);
    boundsSizeModifier->Apply(geometry);
    EXPECT_EQ(geometry->width_, 60.f);
    EXPECT_EQ(geometry->height_, 30.f);

    auto property03 = std::make_shared<RSProperty<Vector2f>>(INITIAL_VALUE_POSITION_2F);
    ASSERT_NE(property03, nullptr);
    std::shared_ptr<RSModifier> boundsPositionModifier =
        std::make_shared<RSBoundsPositionModifier>(property03);
    ASSERT_NE(boundsPositionModifier, nullptr);
    boundsPositionModifier->Apply(geometry);
    EXPECT_EQ(geometry->x_, 50.f);
    EXPECT_EQ(geometry->y_, 50.f);
}

/**
 * @tc.name: Apply02
 * @tc.desc: RSPivotModifier/RSPivotZModifier/RSQuaternionModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyModifierTest, Apply02, TestSize.Level1)
{
    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    ASSERT_NE(geometry, nullptr);

    auto property = std::make_shared<RSProperty<Vector2f>>(INITIAL_VALUE_PIVOT_2F);
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSPivotModifier> pivotModifier = std::make_shared<RSPivotModifier>(property);
    ASSERT_NE(pivotModifier, nullptr);
    pivotModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->pivotX_, 70.f);
    EXPECT_EQ(geometry->trans_->pivotY_, 80.f);

    auto property02 = std::make_shared<RSProperty<float>>(90.f);
    ASSERT_NE(property02, nullptr);
    std::shared_ptr<RSPivotZModifier> pivotZModifier = std::make_shared<RSPivotZModifier>(property02);
    ASSERT_NE(pivotZModifier, nullptr);
    pivotZModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->pivotZ_, 90.f);

    Quaternion value = Quaternion(10, 20, 30, 40);
    auto property03 = std::make_shared<RSProperty<Quaternion>>(value);
    ASSERT_NE(property03, nullptr);
    std::shared_ptr<RSQuaternionModifier> quaternionModifier = std::make_shared<RSQuaternionModifier>(property03);
    ASSERT_NE(quaternionModifier, nullptr);
    quaternionModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->quaternion_, value);
}

/**
 * @tc.name: Apply03
 * @tc.desc: RSRotationModifier/RSRotationXModifier/RSRotationYModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyModifierTest, Apply03, TestSize.Level1)
{
    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    ASSERT_NE(geometry, nullptr);

    auto property = std::make_shared<RSProperty<float>>(30.f);
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSRotationModifier> rotationModifier = std::make_shared<RSRotationModifier>(property);
    ASSERT_NE(rotationModifier, nullptr);
    rotationModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->rotation_, 30);

    auto property02 = std::make_shared<RSProperty<float>>(60.f);
    ASSERT_NE(property02, nullptr);
    rotationModifier = std::make_shared<RSRotationModifier>(property02);
    rotationModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->rotation_, 90);

    auto propertyX = std::make_shared<RSProperty<float>>(60.f);
    ASSERT_NE(propertyX, nullptr);
    std::shared_ptr<RSRotationXModifier> rotationXModifier = std::make_shared<RSRotationXModifier>(propertyX);
    ASSERT_NE(rotationXModifier, nullptr);
    rotationXModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->rotationX_, 60);

    auto propertyX02 = std::make_shared<RSProperty<float>>(30.f);
    ASSERT_NE(propertyX02, nullptr);
    rotationXModifier = std::make_shared<RSRotationXModifier>(propertyX02);
    rotationXModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->rotationX_, 90);

    auto propertyY = std::make_shared<RSProperty<float>>(40.f);
    ASSERT_NE(propertyY, nullptr);
    std::shared_ptr<RSRotationYModifier> rotationYModifier = std::make_shared<RSRotationYModifier>(propertyY);
    ASSERT_NE(rotationYModifier, nullptr);
    rotationYModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->rotationY_, 40);

    auto propertyY02 = std::make_shared<RSProperty<float>>(50.f);
    ASSERT_NE(propertyY02, nullptr);
    rotationYModifier = std::make_shared<RSRotationYModifier>(propertyY02);
    rotationYModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->rotationY_, 90);
}

/**
 * @tc.name: Apply04
 * @tc.desc: RSCameraDistanceModifier/RSScaleModifier/RSSkewModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyModifierTest, Apply04, TestSize.Level1)
{
    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    ASSERT_NE(geometry, nullptr);

    auto property = std::make_shared<RSProperty<float>>(10);
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSCameraDistanceModifier> cameraDistanceModifier =
        std::make_shared<RSCameraDistanceModifier>(property);
    ASSERT_NE(cameraDistanceModifier, nullptr);
    cameraDistanceModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->cameraDistance_, 10);

    auto property02 = std::make_shared<RSProperty<Vector2f>>(INITIAL_VALUE_SCALE_2F);
    ASSERT_NE(property02, nullptr);
    std::shared_ptr<RSScaleModifier> scaleModifier = std::make_shared<RSScaleModifier>(property02);
    ASSERT_NE(scaleModifier, nullptr);
    scaleModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->scaleX_, 2);
    EXPECT_EQ(geometry->trans_->scaleY_, 2);

    auto property03 = std::make_shared<RSProperty<Vector3f>>(INITIAL_VALUE_SKEW_3F);
    ASSERT_NE(property03, nullptr);
    std::shared_ptr<RSSkewModifier> skewModifier = std::make_shared<RSSkewModifier>(property03);
    ASSERT_NE(skewModifier, nullptr);
    skewModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->skewX_, 30);
    EXPECT_EQ(geometry->trans_->skewY_, 30);
    EXPECT_EQ(geometry->trans_->skewZ_, 30);

    auto property04 = std::make_shared<RSProperty<float>>(1.f);
    ASSERT_NE(property04, nullptr);
    std::shared_ptr<RSScaleZModifier> scalezModifier = std::make_shared<RSScaleZModifier>(property04);
    ASSERT_NE(scalezModifier, nullptr);
    scalezModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->scaleZ_, 1.f);
}

/**
 * @tc.name: Apply05
 * @tc.desc: RSPerspModifier/RSTranslateModifier/RSTranslateZModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyModifierTest, Apply05, TestSize.Level1)
{
    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    ASSERT_NE(geometry, nullptr);

    auto property = std::make_shared<RSProperty<Vector4f>>(INITIAL_VALUE_RECT_4F);
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSPerspModifier> perspModifier = std::make_shared<RSPerspModifier>(property);
    ASSERT_NE(perspModifier, nullptr);
    perspModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->perspX_, 0);
    EXPECT_EQ(geometry->trans_->perspY_, 0);
    EXPECT_EQ(geometry->trans_->perspZ_, 0);
    EXPECT_EQ(geometry->trans_->perspW_, 1);

    auto property02 = std::make_shared<RSProperty<Vector4f>>(INITIAL_VALUE_RECT_4F);
    ASSERT_NE(property02, nullptr);
    perspModifier = std::make_shared<RSPerspModifier>(property02);
    perspModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->perspX_, 0);
    EXPECT_EQ(geometry->trans_->perspY_, 0);
    EXPECT_EQ(geometry->trans_->perspZ_, 0);
    EXPECT_EQ(geometry->trans_->perspW_, 1);

    auto property03 = std::make_shared<RSProperty<Vector2f>>(INITIAL_VALUE_SIZE_2F);
    ASSERT_NE(property03, nullptr);
    std::shared_ptr<RSTranslateModifier> translateModifier = std::make_shared<RSTranslateModifier>(property03);
    ASSERT_NE(translateModifier, nullptr);
    translateModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->translateX_, 60);
    EXPECT_EQ(geometry->trans_->translateY_, 30);

    auto property04 = std::make_shared<RSProperty<Vector2f>>(INITIAL_VALUE_POSITION_2F);
    ASSERT_NE(property04, nullptr);
    translateModifier = std::make_shared<RSTranslateModifier>(property04);
    translateModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->translateX_, 110);
    EXPECT_EQ(geometry->trans_->translateY_, 80);

    auto property05 = std::make_shared<RSProperty<float>>(20.f);
    ASSERT_NE(property05, nullptr);
    std::shared_ptr<RSTranslateZModifier> translateZModifier = std::make_shared<RSTranslateZModifier>(property05);
    ASSERT_NE(translateZModifier, nullptr);
    translateZModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->translateZ_, 20);

    auto property06 = std::make_shared<RSProperty<float>>(30.f);
    ASSERT_NE(property06, nullptr);
    translateZModifier = std::make_shared<RSTranslateZModifier>(property06);
    translateZModifier->Apply(geometry);
    EXPECT_EQ(geometry->trans_->translateZ_, 50);
}

/**
 * @tc.name: RSBehindWindowFilterRadiusModifierTest
 * @tc.desc: RSBehindWindowFilterRadiusModifierTest
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyModifierTest, RSBehindWindowFilterRadiusModifierTest, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSBehindWindowFilterRadiusModifier> modifier =
        std::make_shared<RSBehindWindowFilterRadiusModifier>(property);
    ASSERT_NE(modifier, nullptr);
    RSModifierType ModifierType = modifier->GetModifierType();
    EXPECT_EQ(ModifierType, RSModifierType::BEHIND_WINDOW_FILTER_RADIUS);
    ASSERT_NE(modifier->CreateRenderModifier(), nullptr);
}

/**
 * @tc.name: RSBehindWindowFilterSaturationModifierTest
 * @tc.desc: RSBehindWindowFilterSaturationModifierTest
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyModifierTest, RSBehindWindowFilterSaturationModifierTest, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSBehindWindowFilterSaturationModifier> modifier =
        std::make_shared<RSBehindWindowFilterSaturationModifier>(property);
    ASSERT_NE(modifier, nullptr);
    RSModifierType ModifierType = modifier->GetModifierType();
    EXPECT_EQ(ModifierType, RSModifierType::BEHIND_WINDOW_FILTER_SATURATION);
    ASSERT_NE(modifier->CreateRenderModifier(), nullptr);
}

/**
 * @tc.name: RSBehindWindowFilterBrightnessModifierTest
 * @tc.desc: RSBehindWindowFilterBrightnessModifierTest
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyModifierTest, RSBehindWindowFilterBrightnessModifierTest, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSBehindWindowFilterBrightnessModifier> modifier =
        std::make_shared<RSBehindWindowFilterBrightnessModifier>(property);
    ASSERT_NE(modifier, nullptr);
    RSModifierType ModifierType = modifier->GetModifierType();
    EXPECT_EQ(ModifierType, RSModifierType::BEHIND_WINDOW_FILTER_BRIGHTNESS);
    ASSERT_NE(modifier->CreateRenderModifier(), nullptr);
}

/**
 * @tc.name: RSBehindWindowFilterMaskColorModifierTest
 * @tc.desc: RSBehindWindowFilterMaskColorModifierTest
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyModifierTest, RSBehindWindowFilterMaskColorModifierTest, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSBehindWindowFilterMaskColorModifier> modifier =
        std::make_shared<RSBehindWindowFilterMaskColorModifier>(property);
    ASSERT_NE(modifier, nullptr);
    RSModifierType ModifierType = modifier->GetModifierType();
    EXPECT_EQ(ModifierType, RSModifierType::BEHIND_WINDOW_FILTER_MASK_COLOR);
    ASSERT_NE(modifier->CreateRenderModifier(), nullptr);
}

/**
 * @tc.name: RSBackgroundUIFilterModifierTest
 * @tc.desc: RSBackgroundUIFilterModifierTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyModifierTest, RSBackgroundUIFilterModifierTest, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSBackgroundUIFilterModifier> modifier =
        std::make_shared<RSBackgroundUIFilterModifier>(property);
    ASSERT_NE(modifier, nullptr);
    RSModifierType modifierType = modifier->GetModifierType();
    EXPECT_EQ(modifierType, RSModifierType::BACKGROUND_UI_FILTER);
}

/**
 * @tc.name: RSComplexShaderParamModifierTest
 * @tc.desc: RSComplexShaderParamModifierTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyModifierTest, RSComplexShaderParamModifierTest, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    ASSERT_NE(property, nullptr);
    std::shared_ptr<RSComplexShaderParamModifier> modifier =
        std::make_shared<RSComplexShaderParamModifier>(property);
    ASSERT_NE(modifier, nullptr);
    RSModifierType modifierType = modifier->GetModifierType();
    EXPECT_EQ(modifierType, RSModifierType::COMPLEX_SHADER_PARAM);
    ASSERT_NE(modifier->CreateRenderModifier(), nullptr);
}
} // namespace OHOS::Rosen