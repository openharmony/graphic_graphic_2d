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

#include <memory>

#include "gtest/gtest.h"
#include "ui_effect/effect/include/brightness_blender.h"
#include "ui_effect/effect/include/rs_ui_mask_base.h"
#include "ui_effect/effect/include/rs_ui_mask_shape.h"

#include "animation/rs_animation.h"
#include "animation/rs_animation_callback.h"
#include "animation/rs_implicit_animation_param.h"
#include "animation/rs_implicit_animator.h"
#include "animation/rs_implicit_animator_map.h"
#include "animation/rs_transition.h"
#include "common/rs_vector4.h"
#include "modifier_ng/appearance/rs_alpha_modifier.h"
#include "modifier_ng/appearance/rs_background_filter_modifier.h"
#include "modifier_ng/appearance/rs_blend_modifier.h"
#include "modifier_ng/appearance/rs_border_modifier.h"
#include "modifier_ng/appearance/rs_color_picker_modifier.h"
#include "modifier_ng/appearance/rs_compositing_filter_modifier.h"
#include "modifier_ng/appearance/rs_dynamic_light_up_modifier.h"
#include "modifier_ng/appearance/rs_foreground_filter_modifier.h"
#include "modifier_ng/appearance/rs_hdr_brightness_modifier.h"
#include "modifier_ng/appearance/rs_mask_modifier.h"
#include "modifier_ng/appearance/rs_outline_modifier.h"
#include "modifier_ng/appearance/rs_particle_effect_modifier.h"
#include "modifier_ng/appearance/rs_pixel_stretch_modifier.h"
#include "modifier_ng/appearance/rs_point_light_modifier.h"
#include "modifier_ng/appearance/rs_shadow_modifier.h"
#include "modifier_ng/appearance/rs_use_effect_modifier.h"
#include "modifier_ng/appearance/rs_visibility_modifier.h"
#include "modifier_ng/background/rs_background_color_modifier.h"
#include "modifier_ng/background/rs_background_image_modifier.h"
#include "modifier_ng/background/rs_background_shader_modifier.h"
#include "modifier_ng/custom/rs_content_style_modifier.h"
#include "modifier_ng/custom/rs_custom_modifier.h"
#include "modifier_ng/custom/rs_node_modifier.h"
#include "modifier_ng/foreground/rs_env_foreground_color_modifier.h"
#include "modifier_ng/foreground/rs_foreground_color_modifier.h"
#include "modifier_ng/geometry/rs_bounds_clip_modifier.h"
#include "modifier_ng/geometry/rs_bounds_modifier.h"
#include "modifier_ng/geometry/rs_frame_clip_modifier.h"
#include "modifier_ng/geometry/rs_frame_modifier.h"
#include "modifier_ng/geometry/rs_transform_modifier.h"
#include "modifier_ng/rs_modifier_ng.h"
#include "modifier_render_thread/rs_modifiers_draw_thread.h"
#include "render/rs_filter.h"
#include "render/rs_material_filter.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"
#include "ui_effect/property/include/rs_ui_dispersion_filter.h"
#include "ui_effect/property/include/rs_ui_displacement_distort_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

namespace {
constexpr static float FLOAT_DATA_ZERO = 0.0f;
constexpr static float FLOAT_DATA_POSITIVE = 485.44f;
constexpr static float FLOAT_DATA_NEGATIVE = -34.4f;
constexpr static float FLOAT_DATA_MAX = std::numeric_limits<float>::max();
constexpr static float FLOAT_DATA_MIN = std::numeric_limits<float>::min();
constexpr static float FLOAT_DATA_INIT = 0.5f;
constexpr static float FLOAT_DATA_UPDATE = 1.0f;
constexpr static float FLOAT_DATA[] = {
    FLOAT_DATA_ZERO, FLOAT_DATA_POSITIVE, FLOAT_DATA_NEGATIVE,
    FLOAT_DATA_MAX, FLOAT_DATA_MIN, FLOAT_DATA_INIT, FLOAT_DATA_UPDATE
};
}

class RSNodeTest2 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeTest2::SetUpTestCase() {}

void RSNodeTest2::TearDownTestCase()
{
    RSModifiersDrawThread::Instance().runner_ = nullptr;
    RSModifiersDrawThread::Instance().handler_ = nullptr;
}

void RSNodeTest2::SetUp() {}
void RSNodeTest2::TearDown() {}

/**
 * @tc.name: SetUIBackgroundFilter
 * @tc.desc: test results of SetUIBackgroundFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetUIBackgroundFilter, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto filterObj = std::make_unique<Filter>();
    filterObj->AddPara(nullptr);

    auto para = std::make_shared<FilterBlurPara>();
    para->SetRadius(FLOAT_DATA[1]);
    filterObj->AddPara(para);
    rsNode->SetUIBackgroundFilter(filterObj.get());
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundBlurRadiusX() == FLOAT_DATA[1]);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundBlurRadiusY() == FLOAT_DATA[1]);
}

/**
 * @tc.name: SetUICompositingFilter002
 * @tc.desc: SetUICompositingFilter: FilterPara::PIXEL_STRETCH
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetUICompositingFilter002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto filterObj = std::make_unique<Filter>();
    std::shared_ptr<PixelStretchPara> para = std::make_shared<PixelStretchPara>();

    Vector4f tmpPercent{ 0.0f, 0.25f, 0.75f, 1.0f };
    para->SetStretchPercent(tmpPercent);
    auto tileMode = Drawing::TileMode::CLAMP;
    para->SetTileMode(tileMode);

    filterObj->AddPara(para);
    rsNode->SetUICompositingFilter(filterObj.get());

    auto modifier = rsNode->GetModifierByType(ModifierNG::RSPixelStretchModifier::Type);
    ASSERT_TRUE(modifier != nullptr);

    auto property1 = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(
        modifier->GetProperty(ModifierNG::RSPropertyType::PIXEL_STRETCH_PERCENT));
    ASSERT_TRUE(property1 != nullptr);
    EXPECT_EQ(property1->Get(), tmpPercent);

    auto property2 = std::static_pointer_cast<RSAnimatableProperty<int>>(
        modifier->GetProperty(ModifierNG::RSPropertyType::PIXEL_STRETCH_TILE_MODE));
    ASSERT_TRUE(property2 != nullptr);
    EXPECT_EQ(property2->Get(), static_cast<int>(tileMode));
}

/**
 * @tc.name: SetUICompositingFilter003
 * @tc.desc: SetUICompositingFilter: FilterPara::RADIUS_GRADIENT_BLUR
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetUICompositingFilter003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto filterObj = std::make_unique<Filter>();
    auto radiusGradientBlurPara = std::make_shared<RadiusGradientBlurPara>();
    const std::vector<std::pair<float, float>> fractionStops = {
        {0.0f, 0.0f},
        {0.5f, 0.5f},
        {1.0f, 1.0f}
    };
    GradientDirection direction = GradientDirection::LEFT;

    radiusGradientBlurPara->SetBlurRadius(FLOAT_DATA[1]);
    radiusGradientBlurPara->SetFractionStops(fractionStops);
    radiusGradientBlurPara->SetDirection(direction);

    filterObj->AddPara(radiusGradientBlurPara);
    rsNode->SetUICompositingFilter(filterObj.get());

    auto modifier = rsNode->GetModifierByType(ModifierNG::RSCompositingFilterModifier::Type);
    ASSERT_TRUE(modifier != nullptr);

    auto property = std::static_pointer_cast<RSProperty<std::shared_ptr<RSLinearGradientBlurPara>>>(
        modifier->GetProperty(ModifierNG::RSPropertyType::LINEAR_GRADIENT_BLUR_PARA));
    ASSERT_TRUE(property != nullptr);

    auto linearGradientBlurPara = property->Get();
    EXPECT_TRUE(linearGradientBlurPara->isRadiusGradient_);
    EXPECT_EQ(linearGradientBlurPara->blurRadius_, FLOAT_DATA[1]);
    EXPECT_EQ(linearGradientBlurPara->fractionStops_, fractionStops);
    EXPECT_EQ(linearGradientBlurPara->direction_, direction);
}

/**
 * @tc.name: SetUIForegroundFilter
 * @tc.desc: test results of SetUIForegroundFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetUIForegroundFilter, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto filterObj = std::make_unique<Filter>();
    filterObj->AddPara(nullptr);

    std::shared_ptr<FilterBlurPara> para = std::make_shared<FilterBlurPara>();
    para->SetRadius(FLOAT_DATA[1]);
    filterObj->AddPara(para);
    rsNode->SetUIForegroundFilter(filterObj.get());
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundEffectRadius() == FLOAT_DATA[1]);
}

/**
 * @tc.name: RegisterColorPickerCallbackTest001
 * @tc.desc: Test RegisterColorPickerCallback with null callback returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, RegisterColorPickerCallbackTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ASSERT_NE(node, nullptr);

    bool result = node->RegisterColorPickerCallback(100, nullptr, 50);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: RegisterColorPickerCallbackTest002
 * @tc.desc: Test RegisterColorPickerCallback with valid callback returns true
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, RegisterColorPickerCallbackTest002, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ASSERT_NE(node, nullptr);

    bool callbackInvoked = false;
    auto callback = [&callbackInvoked](uint32_t color) { callbackInvoked = true; };

    bool result = node->RegisterColorPickerCallback(100, callback, 50);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: RegisterColorPickerCallbackTest003
 * @tc.desc: Test RegisterColorPickerCallback sets correct parameters
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest, RegisterColorPickerCallbackTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ASSERT_NE(node, nullptr);

    uint32_t receivedColor = 0;
    auto callback = [&](uint32_t color) { receivedColor = color; };

    uint64_t interval = 500;
    uint32_t notifyThreshold = 50;

    bool result = node->RegisterColorPickerCallback(interval, callback, notifyThreshold);
    EXPECT_TRUE(result);

    // Verify color picker params are set
    auto modifier = node->GetModifierByType(ModifierNG::RSModifierType::COLOR_PICKER);
    ASSERT_NE(modifier, nullptr);
    auto colorPickerModifier = std::static_pointer_cast<ModifierNG::RSColorPickerModifier>(modifier);

    EXPECT_EQ(colorPickerModifier->GetColorPickerStrategy(), ColorPickStrategyType::CLIENT_CALLBACK);
    EXPECT_EQ(colorPickerModifier->GetColorPickerInterval(), interval);
    // Check packed threshold value: default is dark=150, light=220, packed as (220 << 16) | 150
    constexpr uint32_t expectedPacked = (220u << 16) | 150u;
    EXPECT_EQ(colorPickerModifier->GetColorPickerNotifyThreshold(), expectedPacked);
}

/**
 * @tc.name: UnregisterColorPickerCallbackTest001
 * @tc.desc: Test UnregisterColorPickerCallback clears callback
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest, UnregisterColorPickerCallbackTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ASSERT_NE(node, nullptr);

    auto callback = [](uint32_t color) {};
    node->RegisterColorPickerCallback(100, callback, 30);

    bool result = node->UnregisterColorPickerCallback();
    EXPECT_TRUE(result);
    auto modifier = node->GetModifierByType(ModifierNG::RSModifierType::COLOR_PICKER);
    ASSERT_NE(modifier, nullptr);
    auto colorPickerModifier = std::static_pointer_cast<ModifierNG::RSColorPickerModifier>(modifier);
    ASSERT_NE(colorPickerModifier, nullptr);
    EXPECT_EQ(colorPickerModifier->GetColorPickerStrategy(), ColorPickStrategyType::NONE);
}

/**
 * @tc.name: UnregisterColorPickerCallbackTest002
 * @tc.desc: Test UnregisterColorPickerCallback without prior registration
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, UnregisterColorPickerCallbackTest002, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ASSERT_NE(node, nullptr);

    bool result = node->UnregisterColorPickerCallback();
    EXPECT_TRUE(result);
}

/**
 * @tc.name: FireColorPickerCallbackTest001
 * @tc.desc: Test FireColorPickerCallback with no registered callback returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, FireColorPickerCallbackTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ASSERT_NE(node, nullptr);

    bool result = node->FireColorPickerCallback(0xFF0000FF);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: FireColorPickerCallbackTest002
 * @tc.desc: Test FireColorPickerCallback invokes registered callback
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, FireColorPickerCallbackTest002, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ASSERT_NE(node, nullptr);

    bool callbackInvoked = false;
    uint32_t receivedColor = 0;
    auto callback = [&](uint32_t color) {
        callbackInvoked = true;
        receivedColor = color;
    };

    node->RegisterColorPickerCallback(100, callback, 50);

    uint32_t testColor = 0xFF00FF00;
    bool result = node->FireColorPickerCallback(testColor);

    EXPECT_TRUE(result);
    EXPECT_TRUE(callbackInvoked);
    EXPECT_EQ(receivedColor, testColor);
}

/**
 * @tc.name: FireColorPickerCallbackTest003
 * @tc.desc: Test FireColorPickerCallback after unregister returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, FireColorPickerCallbackTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ASSERT_NE(node, nullptr);

    auto callback = [](uint32_t color) {};
    node->RegisterColorPickerCallback(100, callback, 30);
    node->UnregisterColorPickerCallback();

    bool result = node->FireColorPickerCallback(0xFF0000FF);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: ColorPickerCallbackMultipleRegistrationTest
 * @tc.desc: Test multiple registrations overwrite previous callback
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, ColorPickerCallbackMultipleRegistrationTest, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ASSERT_NE(node, nullptr);

    bool firstCallbackInvoked = false;
    bool secondCallbackInvoked = false;

    auto firstCallback = [&](uint32_t color) { firstCallbackInvoked = true; };
    auto secondCallback = [&](uint32_t color) { secondCallbackInvoked = true; };

    node->RegisterColorPickerCallback(100, firstCallback, 50);
    node->RegisterColorPickerCallback(200, secondCallback, 75);

    node->FireColorPickerCallback(0xFF0000FF);

    // Only the second callback should be invoked
    EXPECT_FALSE(firstCallbackInvoked);
    EXPECT_TRUE(secondCallbackInvoked);
}

/**
 * @tc.name: SetColorPickerOptionsTest001
 * @tc.desc: Test SetColorPickerOptions sets interval and thresholds correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetColorPickerOptionsTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ASSERT_NE(node, nullptr);

    uint64_t interval = 500;
    uint32_t darkThreshold = 50;
    uint32_t lightThreshold = 200;

    node->SetColorPickerOptions(interval, {darkThreshold, lightThreshold});

    auto modifier = node->GetModifierByType(ModifierNG::RSModifierType::COLOR_PICKER);
    ASSERT_NE(modifier, nullptr);
    auto colorPickerModifier = std::static_pointer_cast<ModifierNG::RSColorPickerModifier>(modifier);

    EXPECT_EQ(colorPickerModifier->GetColorPickerStrategy(), ColorPickStrategyType::CLIENT_CALLBACK);
    EXPECT_EQ(colorPickerModifier->GetColorPickerInterval(), interval);
    // Check packed threshold value: (lightThreshold << 16) | darkThreshold
    constexpr uint32_t expectedPacked = (200u << 16) | 50u;
    EXPECT_EQ(colorPickerModifier->GetColorPickerNotifyThreshold(), expectedPacked);
}

/**
 * @tc.name: SetColorPickerOptionsTest002
 * @tc.desc: Test SetColorPickerOptions with valid rect
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetColorPickerOptionsTest002, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ASSERT_NE(node, nullptr);

    uint64_t interval = 500;
    uint32_t darkThreshold = 50;
    uint32_t lightThreshold = 200;
    Vector4f rect(0.f, 0.f, 100.f, 100.f);

    node->SetColorPickerOptions(interval, {darkThreshold, lightThreshold}, rect);

    auto modifier = node->GetModifierByType(ModifierNG::RSModifierType::COLOR_PICKER);
    ASSERT_NE(modifier, nullptr);
    auto colorPickerModifier = std::static_pointer_cast<ModifierNG::RSColorPickerModifier>(modifier);

    EXPECT_EQ(colorPickerModifier->GetColorPickerStrategy(), ColorPickStrategyType::CLIENT_CALLBACK);
    EXPECT_EQ(colorPickerModifier->GetColorPickerInterval(), interval);
    constexpr uint32_t expectedPacked = (200u << 16) | 50u;
    EXPECT_EQ(colorPickerModifier->GetColorPickerNotifyThreshold(), expectedPacked);
    EXPECT_EQ(colorPickerModifier->GetColorPickerRect(), rect);
}

/**
 * @tc.name: SetColorPickerCallbackTest001
 * @tc.desc: Test SetColorPickerCallback stores and invokes callback correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetColorPickerCallbackTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ASSERT_NE(node, nullptr);

    bool callbackInvoked = false;
    uint32_t receivedColor = 0;
    auto callback = [&](uint32_t color) {
        callbackInvoked = true;
        receivedColor = color;
    };

    node->SetColorPickerCallback(callback);

    uint32_t testColor = 0xFF00FF00;
    bool result = node->FireColorPickerCallback(testColor);

    EXPECT_TRUE(result);
    EXPECT_TRUE(callbackInvoked);
    EXPECT_EQ(receivedColor, testColor);
}

/**
 * @tc.name: SetVisualEffect002
 * @tc.desc: test results of SetVisualEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetVisualEffect002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto effectObj = std::make_unique<VisualEffect>();
    std::shared_ptr<BorderLightEffectPara> para = std::make_shared<BorderLightEffectPara>();
    para->SetLightIntensity(1.0f);
    effectObj->AddPara(para);
    rsNode->SetVisualEffect(effectObj.get());
    EXPECT_EQ(para->GetLightIntensity(), 1.0f);
}

template<typename ModifierType, auto Setter, typename T>
void SetPropertyNGTest(T value1, T value2)
{
    auto node = RSCanvasNode::Create();
    node->SetPropertyNG<ModifierType, Setter>(value1); // modifier should be nullptr
    node->SetPropertyNG<ModifierType, Setter>(value2); // modifier should not be nullptr

    auto modifier = node->GetModifierByType(ModifierType::Type);
    ASSERT_TRUE(modifier != nullptr);
};

/**
 * @tc.name: SetVisualEffect003
 * @tc.desc: test results of SetVisualEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest, SetVisualEffect003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    VisualEffect* effectObj = new(std::nothrow) VisualEffect();
    std::shared_ptr<BrightnessBlender> blender = std::make_shared<BrightnessBlender>();
    blender->SetFraction(floatData[1]);
    blender->SetHdr(true);
    std::shared_ptr<BackgroundColorEffectPara> para = std::make_shared<BackgroundColorEffectPara>();
    para->SetBlender(blender);
    effectObj->AddPara(para);
    rsNode->SetVisualEffect(effectObj);
    EXPECT_EQ(rsNode->hdrEffectType_, 2);
    if (effectObj != nullptr) {
        delete effectObj;
        effectObj = nullptr;
    }
}

/**
 * @tc.name: SetVisualEffect004
 * @tc.desc: test results of SetVisualEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest, SetVisualEffect004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    VisualEffect* effectObj = new(std::nothrow) VisualEffect();
    std::shared_ptr<BrightnessBlender> blender = std::make_shared<BrightnessBlender>();
    blender->SetFraction(floatData[0]);
    blender->SetHdr(true);
    std::shared_ptr<BackgroundColorEffectPara> para = std::make_shared<BackgroundColorEffectPara>();
    para->SetBlender(blender);
    effectObj->AddPara(para);
    rsNode->SetVisualEffect(effectObj);
    EXPECT_EQ(rsNode->hdrEffectType_, 0);
    if (effectObj != nullptr) {
        delete effectObj;
        effectObj = nullptr;
    }
}

template<typename ModifierType, auto Setter, typename T>
void SetAnimatablePropertyNGTest(T value1, T value2, bool animatable)
{
    auto node = RSCanvasNode::Create();
    node->SetPropertyNG<ModifierType, Setter>(value1, animatable); // modifier should be nullptr
    node->SetPropertyNG<ModifierType, Setter>(value2, animatable); // modifier should not be nullptr

    auto modifier = node->GetModifierByType(ModifierType::Type);
    ASSERT_TRUE(modifier != nullptr);
};

/**
 * @tc.name: SetProperty001
 * @tc.desc: RSAlphaModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetProperty001, TestSize.Level1)
{
    const auto vect2f0 = Vector2f(FLOAT_DATA[0], FLOAT_DATA[0]);
    const auto vect2f1 = Vector2f(FLOAT_DATA_UPDATE, FLOAT_DATA_UPDATE);
    const auto vect4f0 = Vector4f(FLOAT_DATA[0]);
    const auto vect4f1 = Vector4f(FLOAT_DATA_UPDATE);

    SetPropertyNGTest<ModifierNG::RSAlphaModifier, &ModifierNG::RSAlphaModifier::SetAlpha>(
        FLOAT_DATA[0], FLOAT_DATA_INIT);
    SetPropertyNGTest<ModifierNG::RSAlphaModifier, &ModifierNG::RSAlphaModifier::SetAlphaOffscreen>(false, true);
    SetPropertyNGTest<ModifierNG::RSBoundsModifier, &ModifierNG::RSBoundsModifier::SetBounds>(vect4f0, vect4f1);
    SetPropertyNGTest<ModifierNG::RSFrameModifier, &ModifierNG::RSFrameModifier::SetFrame>(vect4f0, vect4f1);
    SetPropertyNGTest<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetPositionZ>(
        FLOAT_DATA[0], FLOAT_DATA_UPDATE);
    SetAnimatablePropertyNGTest<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetPivot>(
        vect2f0, vect2f1, true);

    SetPropertyNGTest<ModifierNG::RSForegroundColorModifier,
        &ModifierNG::RSForegroundColorModifier::SetForegroundColor>(Color(), Color(0xFF00FF00));
    SetPropertyNGTest<ModifierNG::RSBackgroundColorModifier,
        &ModifierNG::RSBackgroundColorModifier::SetBackgroundColor>(Color(), Color(0xFF00FF00));

    SetPropertyNGTest<ModifierNG::RSFrameClipModifier, &ModifierNG::RSFrameClipModifier::SetFrameGravity>(
        Gravity::TOP_RIGHT, Gravity::RESIZE);

    SetPropertyNGTest<ModifierNG::RSFrameClipModifier, &ModifierNG::RSFrameClipModifier::SetClipToFrame>(false, true);
    SetPropertyNGTest<ModifierNG::RSVisibilityModifier, &ModifierNG::RSVisibilityModifier::SetVisible>(false, true);
    SetPropertyNGTest<ModifierNG::RSMaskModifier, &ModifierNG::RSMaskModifier::SetMask>(
        std::make_shared<RSMask>(), (std::shared_ptr<OHOS::Rosen::RSMask>)nullptr);

    auto node = RSCanvasNode::Create();
    auto modifier = node->GetModifierByType(ModifierNG::RSMaskModifier::Type);
    EXPECT_TRUE(modifier == nullptr);
}

/**
 * @tc.name: SetPropertyNG_RSTransformModifierTest
 * @tc.desc: SetPropertyNG: RSTransformModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetPropertyNG_RSTransformModifierTest, TestSize.Level1)
{
    const auto vect2f0 = Vector2f(FLOAT_DATA[0], FLOAT_DATA[0]);
    const auto vect2f1 = Vector2f(FLOAT_DATA_UPDATE, FLOAT_DATA_UPDATE);
    const auto vect4f0 = Vector4f(FLOAT_DATA[0]);
    const auto vect4f1 = Vector4f(FLOAT_DATA_UPDATE);
    SetPropertyNGTest<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetQuaternion>(
        Quaternion(), Quaternion(0.f, 1.f, 0.f, 0.f));
    // angel: 45.0f 90.0f
    SetPropertyNGTest<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetRotation>(45.0f, 90.0f);
    // angel: 45.0f 90.0f
    SetPropertyNGTest<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetRotationX>(45.0f, 90.0f);
    // angel: 45.0f 90.0f
    SetPropertyNGTest<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetRotationY>(45.0f, 90.0f);
    SetPropertyNGTest<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetTranslate>(
        vect2f0, vect2f1);
    // angel: 45.0f 90.0f
    SetPropertyNGTest<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetTranslateZ>(45.f, 90.f);
    SetPropertyNGTest<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetScale>(vect2f0, vect2f1);
    SetPropertyNGTest<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetScaleZ>(
        FLOAT_DATA_INIT, FLOAT_DATA_UPDATE);
    SetPropertyNGTest<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetSkew>(
        Vector3f(1.0f, 1.0f, 1.0f), Vector3f(-1.0f, -1.0f, -1.0f)); // 1.0f -1.0f
    SetPropertyNGTest<ModifierNG::RSTransformModifier, &ModifierNG::RSTransformModifier::SetPersp>(vect4f0, vect4f1);

    auto node = RSCanvasNode::Create();
    auto modifier = node->GetModifierByType(ModifierNG::RSTransformModifier::Type);
    EXPECT_TRUE(modifier == nullptr);
}

/**
 * @tc.name: SetPropertyNG_RSBackgroundImageModifierTest
 * @tc.desc: SetPropertyNG: RSBackgroundImageModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetPropertyNG_RSBackgroundImageModifierTest, TestSize.Level1)
{
    SetPropertyNGTest<ModifierNG::RSBackgroundImageModifier, &ModifierNG::RSBackgroundImageModifier::SetBgImage>(
        std::make_shared<RSImage>(), (std::shared_ptr<OHOS::Rosen::RSImage>)nullptr);
    // angel: 45.0f 90.0f
    SetPropertyNGTest<ModifierNG::RSBackgroundImageModifier, &ModifierNG::RSBackgroundImageModifier::SetBgImageWidth>(
        45.f, 90.f);
    // angel: 45.0f 90.0f
    SetPropertyNGTest<ModifierNG::RSBackgroundImageModifier, &ModifierNG::RSBackgroundImageModifier::SetBgImageHeight>(
        45.f, 90.f);

    auto node = RSCanvasNode::Create();
    auto modifier = node->GetModifierByType(ModifierNG::RSBackgroundImageModifier::Type);
    EXPECT_TRUE(modifier == nullptr);
}

/**
 * @tc.name: SetPropertyNG_RSBorderModifierTest
 * @tc.desc: SetPropertyNG: RSBorderModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetPropertyNG_RSBorderModifierTest, TestSize.Level1)
{
    const auto vect4f0 = Vector4f(FLOAT_DATA[0]);
    const auto vect4f1 = Vector4f(FLOAT_DATA_UPDATE);
    SetPropertyNGTest<ModifierNG::RSBorderModifier, &ModifierNG::RSBorderModifier::SetBorderColor>(
        Vector4<Color>(), Vector4<Color>(Color(0xFF00FF00)));
    SetPropertyNGTest<ModifierNG::RSBorderModifier, &ModifierNG::RSBorderModifier::SetBorderWidth>(vect4f0, vect4f1);
    SetPropertyNGTest<ModifierNG::RSBorderModifier, &ModifierNG::RSBorderModifier::SetBorderStyle>(
        Vector4<uint32_t>(), Vector4<uint32_t>(1));
    auto node = RSCanvasNode::Create();
    auto modifier = node->GetModifierByType(ModifierNG::RSBorderModifier::Type);
    EXPECT_TRUE(modifier == nullptr);
}

/**
 * @tc.name: SetPropertyNG_RSShadowModifierTest
 * @tc.desc: SetPropertyNG: RSShadowModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetPropertyNG_RSShadowModifierTest, TestSize.Level1)
{
    SetPropertyNGTest<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowColor>(
        Color(), Color(0xFF00FF00));
    SetPropertyNGTest<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowOffsetX>(1.f, 2.f); // 2.f
    SetPropertyNGTest<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowOffsetY>(1.f, 2.f); // 2.f
    // 0.2f, 0.5f
    SetPropertyNGTest<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowAlpha>(0.2f, 0.5f);
    SetPropertyNGTest<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowElevation>(1.f, 2.f); // 2.f
    SetPropertyNGTest<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowRadius>(1.f, 2.f); // 2.f
    SetPropertyNGTest<ModifierNG::RSShadowModifier, &ModifierNG::RSShadowModifier::SetShadowPath>(
        RSPath::CreateRSPath(), (std::shared_ptr<RSPath>)nullptr);
    auto node = RSCanvasNode::Create();
    auto modifier = node->GetModifierByType(ModifierNG::RSShadowModifier::Type);
    EXPECT_TRUE(modifier == nullptr);
}

/**
 * @tc.name: SetPropertyNG_RSBoundsClipModifierTest
 * @tc.desc: SetPropertyNG: RSBoundsClipModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetPropertyNG_RSBoundsClipModifierTest, TestSize.Level1)
{
    const auto vect4f0 = Vector4f(FLOAT_DATA[0]);
    const auto vect4f1 = Vector4f(FLOAT_DATA_UPDATE);
    SetPropertyNGTest<ModifierNG::RSBoundsClipModifier, &ModifierNG::RSBoundsClipModifier::SetCornerRadius>(
        vect4f0, vect4f1);
    SetPropertyNGTest<ModifierNG::RSBoundsClipModifier, &ModifierNG::RSBoundsClipModifier::SetClipBounds>(
        RSPath::CreateRSPath(), (std::shared_ptr<RSPath>)nullptr);
    SetPropertyNGTest<ModifierNG::RSBoundsClipModifier, &ModifierNG::RSBoundsClipModifier::SetClipToBounds>(
        false, true);

    auto node = RSCanvasNode::Create();
    auto modifier = node->GetModifierByType(ModifierNG::RSBoundsClipModifier::Type);
    EXPECT_TRUE(modifier == nullptr);
}

/**
 * @tc.name: SetMagnifierParamsTest
 * @tc.desc: SetMagnifierParams
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetMagnifierParamsTest, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto para = std::make_shared<RSMagnifierParams>();
    rsNode->SetMagnifierParams(para);

    auto modifier = rsNode->GetModifierByType(ModifierNG::RSBackgroundFilterModifier::Type);
    ASSERT_TRUE(modifier != nullptr);
    auto property = std::static_pointer_cast<RSProperty<std::shared_ptr<RSMagnifierParams>>>(
        modifier->GetProperty(ModifierNG::RSPropertyType::MAGNIFIER_PARA));
    ASSERT_TRUE(property != nullptr);

    EXPECT_EQ(property->Get(), para);
}

/**
 * @tc.name: AddModifier
 * @tc.desc: test results of AddModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, AddModifier, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    EXPECT_TRUE(node->modifiersNG_.empty());
    node->AddModifier(nullptr);
    EXPECT_TRUE(node->modifiersNG_.empty());
    auto alphaModifier = std::make_shared<ModifierNG::RSAlphaModifier>();
    node->AddModifier(alphaModifier);
    EXPECT_EQ(node->modifiersNG_.size(), 1);
    node->AddModifier(alphaModifier);
    EXPECT_EQ(node->modifiersNG_.size(), 1);
    auto nodeModifier = std::make_shared<ModifierNG::RSNodeModifier>();
    node->AddModifier(nodeModifier);
    EXPECT_EQ(node->modifiersNG_.size(), 1);
}

/**
 * @tc.name: RemoveModifier
 * @tc.desc: test results of RemoveModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, RemoveModifier, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<ModifierNG::RSModifier> modifierNull = nullptr;
    EXPECT_TRUE(rsNode->modifiersNG_.empty());
    rsNode->RemoveModifier(modifierNull);
    EXPECT_TRUE(rsNode->modifiersNG_.empty());

    auto para = std::make_shared<RSMagnifierParams>();
    rsNode->SetMagnifierParams(para);
    EXPECT_FALSE(rsNode->modifiersNG_.empty());
    auto modifier = rsNode->GetModifierByType(ModifierNG::RSBackgroundFilterModifier::Type);
    rsNode->RemoveModifier(modifier);
    EXPECT_TRUE(rsNode->modifiersNG_.empty());

    auto alphaModifier = std::make_shared<ModifierNG::RSAlphaModifier>();
    EXPECT_TRUE(rsNode->modifiersNG_.empty());
    rsNode->RemoveModifier(alphaModifier);
    EXPECT_TRUE(rsNode->modifiersNG_.empty());
}

/**
 * @tc.name: AttachProperty
 * @tc.desc: test results of AttachProperty
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, AttachProperty, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto property1 = std::make_shared<RSProperty<float>>(1.0f);
    property1->SetPropertyTypeNG(ModifierNG::RSPropertyType::BOUNDS);
    std::string  ANIMATION_PATH = "M0 0 L300 200 L300 500 Z";
    rsNode->motionPathOption_ = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    rsNode->AttachProperty(property1);
    EXPECT_EQ(property1->GetPropertyTypeNG(), ModifierNG::RSPropertyType::BOUNDS);

    auto property2 = std::make_shared<RSProperty<float>>(1.0f);
    property2->SetPropertyTypeNG(ModifierNG::RSPropertyType::FRAME);
    rsNode->AttachProperty(property2);
    EXPECT_EQ(property2->GetPropertyTypeNG(), ModifierNG::RSPropertyType::FRAME);

    auto property3 = std::make_shared<RSProperty<float>>(1.0f);
    property3->SetPropertyTypeNG(ModifierNG::RSPropertyType::TRANSLATE);
    rsNode->AttachProperty(property3);
    EXPECT_EQ(property3->GetPropertyTypeNG(), ModifierNG::RSPropertyType::TRANSLATE);

    auto property4 = std::make_shared<RSProperty<float>>(1.0f);
    property4->SetPropertyTypeNG(ModifierNG::RSPropertyType::INVALID);
    rsNode->AttachProperty(property4);
    EXPECT_EQ(property4->GetPropertyTypeNG(), ModifierNG::RSPropertyType::INVALID);
}

/**
 * @tc.name: GetModifierCreatedBySetter
 * @tc.desc: test results of GetModifierCreatedBySetter
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, GetModifierCreatedBySetter, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto alphaModifier = rsNode->GetModifierCreatedBySetter(ModifierNG::RSModifierType::ALPHA);
    EXPECT_EQ(alphaModifier, nullptr);
    rsNode->SetAlpha(0.5f);
    alphaModifier = rsNode->GetModifierCreatedBySetter(ModifierNG::RSModifierType::ALPHA);
    EXPECT_NE(alphaModifier, nullptr);
}

/**
 * @tc.name: DoFlushModifier
 * @tc.desc: test results of DoFlushModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, DoFlushModifier, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlpha(0.5f);
    EXPECT_EQ(rsNode->modifiersNG_.size(), 1);
    auto contentStyleModifier = std::make_shared<ModifierNG::RSContentStyleModifier>();
    rsNode->AddModifier(contentStyleModifier);
    rsNode->DoFlushModifier();
    EXPECT_EQ(rsNode->modifiersNG_.size(), 2);
}

/**
 * @tc.name: GetLocalGeometry
 * @tc.desc: test results of GetLocalGeometry
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, GetLocalGeometry, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto boundsModifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    boundsModifier->SetBounds({ 10.f, 10.f, 100.f, 100.f });
    rsNode->AddModifier(boundsModifier);
    rsNode->UpdateLocalGeometry();
    auto localGeometry = rsNode->GetLocalGeometry();
    rsNode->MarkDirty(NodeDirtyType::GEOMETRY, false);
    EXPECT_NE(localGeometry, nullptr);
    EXPECT_EQ(localGeometry->x_, 10.f);
    EXPECT_EQ(localGeometry->y_, 10.f);
    EXPECT_EQ(localGeometry->width_, 100.f);
    EXPECT_EQ(localGeometry->height_, 100.f);
    auto property = boundsModifier->GetProperty(ModifierNG::RSPropertyType::BOUNDS);
    EXPECT_NE(property, nullptr);
    rsNode->UnregisterProperty(property->GetId());
    rsNode->RemoveModifier(boundsModifier);
    rsNode->UpdateLocalGeometry();
    localGeometry = rsNode->GetLocalGeometry();
    EXPECT_NE(localGeometry, nullptr);
    EXPECT_EQ(localGeometry->x_, -INFINITY);
    EXPECT_EQ(localGeometry->y_, -INFINITY);
    EXPECT_EQ(localGeometry->width_, -INFINITY);
    EXPECT_EQ(localGeometry->height_, -INFINITY);
}

/**
 * @tc.name: SetSDFUnionOPShape
 * @tc.desc: test results of SDF_UNION_OP_SHAPE
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetSDFUnionOPShape, TestSize.Level1)
{
    auto modifierType = ModifierNG::RSModifierType::BOUNDS;
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetSDFShape(nullptr);
    EXPECT_EQ(rsNode->GetModifierCreatedBySetter(modifierType), nullptr);

    // create SDF Shape
    auto shape0 = RSNGShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    auto shape = std::static_pointer_cast<RSNGSDFUnionOpshape>(shape0);
    auto shapeX = RSNGShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    auto shapeY = RSNGShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);

    shapeX = std::static_pointer_cast<RSNGShapeBase>(shapeX);
    shapeY = std::static_pointer_cast<RSNGShapeBase>(shapeY);
    shape->Setter<SDFUnionOpShapeShapeXTag>(shapeX);
    shape->Setter<SDFUnionOpShapeShapeYTag>(shapeY);

    // set shape
    rsNode->SetSDFShape(shape);
    EXPECT_NE(rsNode->GetModifierCreatedBySetter(modifierType), nullptr);

    auto& properties = rsNode->GetModifierCreatedBySetter(modifierType)->properties_;
    EXPECT_NE(properties.find(ModifierNG::RSPropertyType::SDF_SHAPE), properties.end());

    rsNode->SetSDFShape(nullptr);
    EXPECT_EQ(properties.find(ModifierNG::RSPropertyType::SDF_SHAPE), properties.end());
    rsNode->SetSDFShape(nullptr);
    EXPECT_EQ(properties.find(ModifierNG::RSPropertyType::SDF_SHAPE), properties.end());
}

/**
 * @tc.name: SetSDFSmoothUnionOPShape
 * @tc.desc: test results of SDF_SMOOTH_UNION_OP_SHAPE
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetSDFSmoothUnionOPShape, TestSize.Level1)
{
    auto modifierType = ModifierNG::RSModifierType::BOUNDS;
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetSDFShape(nullptr);
    EXPECT_EQ(rsNode->GetModifierCreatedBySetter(modifierType), nullptr);

    // create SDF Shape
    auto shape0 = RSNGShapeBase::Create(RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE);
    auto shape = std::static_pointer_cast<RSNGSDFSmoothUnionOpshape>(shape0);
    auto shapeX = RSNGShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    auto shapeY = RSNGShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);

    shapeX = std::static_pointer_cast<RSNGShapeBase>(shapeX);
    shapeY = std::static_pointer_cast<RSNGShapeBase>(shapeY);

    shape->Setter<SDFSmoothUnionOpShapeSpacingTag>(shapeX);
    shape->Setter<SDFSmoothUnionOpShapeShapeXTag>(shapeX);
    shape->Setter<SDFSmoothUnionOpShapeShapeYTag>(shapeY);

    // set shape
    rsNode->SetSDFShape(shape);
    EXPECT_NE(rsNode->GetModifierCreatedBySetter(modifierType), nullptr);

    auto& properties = rsNode->GetModifierCreatedBySetter(modifierType)->properties_;
    EXPECT_NE(properties.find(ModifierNG::RSPropertyType::SDF_SHAPE), properties.end());

    rsNode->SetSDFShape(nullptr);
    EXPECT_EQ(properties.find(ModifierNG::RSPropertyType::SDF_SHAPE), properties.end());
    rsNode->SetSDFShape(nullptr);
    EXPECT_EQ(properties.find(ModifierNG::RSPropertyType::SDF_SHAPE), properties.end());
}

/**
 * @tc.name: SetSDFRRectShape
 * @tc.desc: test results of SDF_RRECT_SHAPE and modifier path
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetSDFRRectShape, TestSize.Level1)
{
    auto modifierType = ModifierNG::RSModifierType::BOUNDS;
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetSDFShape(nullptr);
    EXPECT_EQ(rsNode->GetModifierCreatedBySetter(modifierType), nullptr);

    // create SDF Shape
    auto shape0 = RSNGShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    auto shape = std::static_pointer_cast<RSNGSDFRRectShape>(shape0);

    auto boundsModifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    rsNode->AddModifier(boundsModifier);

    EXPECT_EQ(boundsModifier->GetSDFShape(), nullptr);
    EXPECT_EQ(boundsModifier->GetUnionSpacing(), 0.0f);
    EXPECT_EQ(boundsModifier->GetUseUnion(), false);

    // test the path of using modifier to set these properties
    boundsModifier->SetSDFShape(shape);
    boundsModifier->SetUnionSpacing(0.5f);
    boundsModifier->SetUseUnion(true);

    EXPECT_NE(boundsModifier->GetSDFShape(), nullptr);
    EXPECT_EQ(boundsModifier->GetUnionSpacing(), 0.5f);
    EXPECT_EQ(boundsModifier->GetUseUnion(), true);
}

/**
 * @tc.name: SetUseUnion
 * @tc.desc: test results of RSNode::SetUseUnion
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetUseUnion, TestSize.Level1)
{
    auto modifierType = ModifierNG::RSModifierType::BOUNDS;
    auto rsNode = RSCanvasNode::Create();

    EXPECT_EQ(rsNode->GetModifierCreatedBySetter(modifierType), nullptr);

    rsNode->SetUseUnion(false);
    auto& properties = rsNode->GetModifierCreatedBySetter(modifierType)->properties_;

    EXPECT_NE(rsNode->GetModifierCreatedBySetter(modifierType), nullptr);
    EXPECT_NE(properties.find(ModifierNG::RSPropertyType::USE_UNION), properties.end());

    rsNode->SetUseUnion(true);
    EXPECT_NE(rsNode->GetModifierCreatedBySetter(modifierType), nullptr);
    EXPECT_NE(properties.find(ModifierNG::RSPropertyType::USE_UNION), properties.end());
}

/**
 * @tc.name: SetUIMaterialFilter001
 * @tc.desc: test results of SetUIMaterialFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetUIMaterialFilter001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetUIMaterialFilter(nullptr);
    ASSERT_EQ(rsNode->GetModifierByType(ModifierNG::RSModifierType::MATERIAL_FILTER), nullptr);
}

/**
 * @tc.name: SetUIMaterialFilter002
 * @tc.desc: test results of SetUIMaterialFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetUIMaterialFilter002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto filterObj = std::make_unique<Filter>();
    std::shared_ptr<ContentLightPara> para = std::make_shared<ContentLightPara>();
    float lightIntensity = 0.5f;
    para->SetLightIntensity(lightIntensity);
    filterObj->AddPara(para);

    rsNode->SetUIMaterialFilter(filterObj.get());
    ASSERT_NE(rsNode->GetModifierByType(ModifierNG::RSModifierType::MATERIAL_FILTER), nullptr);
}

/**
 * @tc.name: SetUIMaterialFilter003
 * @tc.desc: test results of SetUIMaterialFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetUIMaterialFilter003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto filterObj = std::make_unique<Filter>();
    filterObj->AddPara(nullptr);
    std::shared_ptr<ContentLightPara> para1 = std::make_shared<ContentLightPara>();
    float lightIntensity = 0.5f;
    para1->SetLightIntensity(lightIntensity);
    filterObj->AddPara(para1);
    td::shared_ptr<ContentLightPara> para2 = std::make_shared<ContentLightPara>();
    lightIntensity = 0.6f;
    para2->SetLightIntensity(lightIntensity);
    filterObj->AddPara(para2);

    rsNode->SetUIMaterialFilter(filterObj.get());
    ASSERT_NE(rsNode->GetModifierByType(ModifierNG::RSModifierType::MATERIAL_FILTER), nullptr);
}

/**
 * @tc.name: SetMaterialNGFilter001
 * @tc.desc: test results of SetMaterialNGFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetMaterialNGFilter001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetMaterialNGFilter(nullptr);
    ASSERT_EQ(rsNode->GetModifierByType(ModifierNG::RSModifierType::MATERIAL_FILTER), nullptr);

    auto filter = std::make_shared<RSNGBlurFilter>();
    rsNode->SetMaterialNGFilter(filter);
    ASSERT_NE(rsNode->GetModifierByType(ModifierNG::RSModifierType::MATERIAL_FILTER), nullptr);

    rsNode->SetMaterialNGFilter(nullptr);
    auto modifier = rsNode->GetModifierByType(ModifierNG::RSModifierType::MATERIAL_FILTER);
    ASSERT_NE(modifier, nullptr);
    ASSERT_EQ(modifier->properties_.find(ModifierNG::RSPropertyType::MATERIAL_NG_FILTER), modifier->properties_.end());
}

/**
 * @tc.name: SetMaterialWithQualityLevel_ClearColorPicker
 * @tc.desc: When materialFilter is nullptr, color picker params should be cleared
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetMaterialWithQualityLevel_ClearColorPicker, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();

    // First set color picker params to create the modifier and properties
    rsNode->SetColorPickerParams(ColorPlaceholder::SURFACE_CONTRAST, ColorPickStrategyType::CONTRAST, 200);
    auto modifierBefore = rsNode->GetModifierByType(ModifierNG::RSModifierType::COLOR_PICKER);
    ASSERT_NE(modifierBefore, nullptr);

    // Ensure properties exist
    EXPECT_NE(modifierBefore->properties_.find(ModifierNG::RSPropertyType::COLOR_PICKER_PLACEHOLDER),
        modifierBefore->properties_.end());
    EXPECT_NE(modifierBefore->properties_.find(ModifierNG::RSPropertyType::COLOR_PICKER_STRATEGY),
        modifierBefore->properties_.end());
    EXPECT_NE(modifierBefore->properties_.find(ModifierNG::RSPropertyType::COLOR_PICKER_INTERVAL),
        modifierBefore->properties_.end());

    // Call SetMaterialWithQualityLevel with nullptr should clear color picker properties
    rsNode->SetMaterialWithQualityLevel(nullptr, FilterQuality::DEFAULT);

    auto modifierAfter = rsNode->GetModifierByType(ModifierNG::RSModifierType::COLOR_PICKER);
    // Modifier and properties still exist to pick up previously extracted color
    ASSERT_TRUE(modifierAfter);
    EXPECT_NE(modifierAfter->properties_.find(ModifierNG::RSPropertyType::COLOR_PICKER_PLACEHOLDER),
        modifierAfter->properties_.end());
    EXPECT_NE(modifierAfter->properties_.find(ModifierNG::RSPropertyType::COLOR_PICKER_STRATEGY),
        modifierAfter->properties_.end());
    EXPECT_NE(modifierAfter->properties_.find(ModifierNG::RSPropertyType::COLOR_PICKER_INTERVAL),
        modifierAfter->properties_.end());
}

/**
 * @tc.name: SetMaterialWithQualityLevel_AdaptiveFrostedGlass
 * @tc.desc: Frosted glass material with ADAPTIVE quality should enable color picker with clamped interval
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetMaterialWithQualityLevel_AdaptiveFrostedGlass, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();

    // Create a frosted glass NG filter
    auto materialFilter = RSNGFilterBase::Create(RSNGEffectType::FROSTED_GLASS);
    ASSERT_NE(materialFilter, nullptr);

    // Call with ADAPTIVE quality should set color picker params
    rsNode->SetMaterialWithQualityLevel(materialFilter, FilterQuality::ADAPTIVE);

    auto modifier = rsNode->GetModifierByType(ModifierNG::RSModifierType::COLOR_PICKER);
    ASSERT_NE(modifier, nullptr);

    // Check properties exist and values (interval should be clamped by SetColorPickerParams MIN_INTERVAL = 180)
    auto propPlaceholder = std::static_pointer_cast<RSProperty<int>>(
        modifier->GetProperty(ModifierNG::RSPropertyType::COLOR_PICKER_PLACEHOLDER));
    ASSERT_NE(propPlaceholder, nullptr);
    EXPECT_EQ(propPlaceholder->Get(), static_cast<int>(ColorPlaceholder::SURFACE_CONTRAST));

    auto propStrategy = std::static_pointer_cast<RSProperty<int>>(
        modifier->GetProperty(ModifierNG::RSPropertyType::COLOR_PICKER_STRATEGY));
    ASSERT_NE(propStrategy, nullptr);
    EXPECT_EQ(propStrategy->Get(), static_cast<int>(ColorPickStrategyType::CONTRAST));

    auto propInterval = std::static_pointer_cast<RSProperty<int>>(
        modifier->GetProperty(ModifierNG::RSPropertyType::COLOR_PICKER_INTERVAL));
    ASSERT_NE(propInterval, nullptr);
}

} // namespace OHOS::Rosen
