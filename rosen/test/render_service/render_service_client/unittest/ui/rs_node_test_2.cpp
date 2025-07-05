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
#include "ui_effect/property/include/rs_ui_bezier_warp_filter.h"
#include "ui_effect/property/include/rs_ui_blur_filter.h"
#include "ui_effect/property/include/rs_ui_content_light_filter.h"
#include "ui_effect/property/include/rs_ui_filter.h"

#include "animation/rs_animation.h"
#include "animation/rs_animation_callback.h"
#include "animation/rs_implicit_animation_param.h"
#include "animation/rs_implicit_animator.h"
#include "animation/rs_implicit_animator_map.h"
#include "animation/rs_transition.h"
#include "common/rs_vector4.h"
#include "modifier/rs_modifier.h"
#include "modifier/rs_property_modifier.h"
#include "modifier/rs_extended_modifier.h"
#include "modifier_ng/appearance/rs_alpha_modifier.h"
#include "modifier_ng/appearance/rs_background_filter_modifier.h"
#include "modifier_ng/appearance/rs_blend_modifier.h"
#include "modifier_ng/appearance/rs_border_modifier.h"
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
#include "modifier_ng/custom/rs_custom_modifier.h"
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
#include "ui_effect/property/include/rs_ui_edge_light_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

#if defined(MODIFIER_NG)
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
#endif // MODIFIER_NG

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

#if defined(MODIFIER_NG)
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

    auto para2 = std::make_shared<DisplacementDistortPara>();
    filterObj->AddPara(para2);
    auto para3 = std::make_shared<EdgeLightPara>();
    filterObj->AddPara(para3);
    rsNode->SetUIBackgroundFilter(filterObj.get());
    EXPECT_TRUE(rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetBackgroundUIFilter
 * @tc.desc: test results of SetBackgroundUIFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetBackgroundUIFilter, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto backgroundFilter = std::make_shared<RSUIFilter>();

    rsNode->SetBackgroundUIFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->propertyModifiers_.empty());
    rsNode->SetBackgroundUIFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->propertyModifiers_.empty());
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

    auto para2 = std::make_shared<DisplacementDistortPara>();
    filterObj->AddPara(para2);
    auto para3 = std::make_shared<EdgeLightPara>();
    filterObj->AddPara(para3);
    rsNode->SetUIForegroundFilter(filterObj.get());
    EXPECT_TRUE(rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetForegroundUIFilter
 * @tc.desc: test results of SetForegroundUIFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, SetForegroundUIFilter, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto rsUIFilter = std::make_shared<RSUIFilter>();
    std::shared_ptr<RSUIFilter> nullUIFilter = nullptr;
    rsNode->SetForegroundUIFilter(nullUIFilter);
    rsNode->SetForegroundUIFilter(rsUIFilter);
    EXPECT_TRUE(rsNode->propertyModifiers_.empty());

    auto filterObj = std::make_unique<Filter>();
    std::shared_ptr<FilterBlurPara> para = std::make_shared<FilterBlurPara>();
    para->SetRadius(FLOAT_DATA[1]);
    filterObj->AddPara(para);
    auto bz = std::make_shared<BezierWarpPara>();
    filterObj->AddPara(bz);
    rsNode->SetUIForegroundFilter(filterObj.get());
    EXPECT_TRUE(rsNode->propertyModifiers_.empty());
    rsNode->SetUIForegroundFilter(filterObj.get());
    EXPECT_TRUE(rsNode->propertyModifiers_.empty());
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
 * @tc.name: RemoveModifier
 * @tc.desc: test results of RemoveModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, RemoveModifier, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<ModifierNG::RSModifier> modifierNull = nullptr;
    rsNode->RemoveModifier(modifierNull);
    EXPECT_TRUE(rsNode->propertyModifiers_.empty());

    auto para = std::make_shared<RSMagnifierParams>();
    rsNode->SetMagnifierParams(para);
    auto modifier = rsNode->GetModifierByType(ModifierNG::RSBackgroundFilterModifier::Type);
    rsNode->RemoveModifier(modifier);
    EXPECT_TRUE(rsNode->propertyModifiers_.empty());
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
 * @tc.name: DetachUIFilterProperties
 * @tc.desc: test results of DetachUIFilterProperties
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTest2, DetachUIFilterProperties, TestSize.Level1)
{
    auto backgroundFilterModifier = std::make_shared<ModifierNG::RSBackgroundFilterModifier>();
    auto rsUIFilter = std::make_shared<RSUIFilter>();
    float radius = 1.0f;
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    filterBlurPara->SetRadius(radius);
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara->SetBlurPara(filterBlurPara);
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);
    rsUIFilter->Insert(rsUIFilterParaBase);
    backgroundFilterModifier->SetUIFilter(rsUIFilter);
    auto rsNode = RSCanvasNode::Create();
    auto property1 = std::static_pointer_cast<RSProperty<std::shared_ptr<RSUIFilter>>>(
        backgroundFilterModifier->GetProperty(ModifierNG::RSPropertyType::BACKGROUND_UI_FILTER));
    rsNode->DetachUIFilterProperties(backgroundFilterModifier);
    EXPECT_TRUE(property1 == nullptr);

    auto foregroundFilterModifier = std::make_shared<ModifierNG::RSForegroundFilterModifier>();
    auto property2 = std::static_pointer_cast<RSProperty<std::shared_ptr<RSUIFilter>>>(
        foregroundFilterModifier->GetProperty(ModifierNG::RSPropertyType::FOREGROUND_UI_FILTER));
    rsNode->DetachUIFilterProperties(foregroundFilterModifier);
    EXPECT_TRUE(property2 == nullptr);
}
#endif // MODIFIER_NG
} // namespace OHOS::Rosen
