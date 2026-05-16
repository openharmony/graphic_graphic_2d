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
#include "parcel.h" // Assumed definition of Parcel
#include "ge_visual_effect_container.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
using namespace testing;
using namespace testing::ext;
namespace OHOS::Rosen {

class RSNGRenderShaderBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
void RSNGRenderShaderBaseTest::SetUpTestCase() {}
void RSNGRenderShaderBaseTest::TearDownTestCase() {}
void RSNGRenderShaderBaseTest::SetUp() {}
void RSNGRenderShaderBaseTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc: Verify that the create method returns a non-empty instance for the registered shader type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, Create001, TestSize.Level1)
{
    // Test three known filter types
    auto contour = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    EXPECT_NE(contour, nullptr);

    auto dotMatrix = RSNGRenderShaderBase::Create(RSNGEffectType::DOT_MATRIX_SHADER);
    EXPECT_NE(dotMatrix, nullptr);

    auto ripple = RSNGRenderShaderBase::Create(RSNGEffectType::WAVY_RIPPLE_LIGHT);
    EXPECT_NE(ripple, nullptr);

    auto aurora = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    EXPECT_NE(aurora, nullptr);

    auto lightCave = RSNGRenderShaderBase::Create(RSNGEffectType::LIGHT_CAVE);
    EXPECT_NE(lightCave, nullptr);

    auto harmoniumEffect = RSNGRenderShaderBase::Create(RSNGEffectType::HARMONIUM_EFFECT);
    EXPECT_NE(harmoniumEffect, nullptr);
}

/**
 * @tc.name: Create002
 * @tc.desc: Test that the create method returns a nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, Create002, TestSize.Level1)
{
    // Use an invalid enum value
    RSNGEffectType invalidType = static_cast<RSNGEffectType>(-1);
    auto shader = RSNGRenderShaderBase::Create(invalidType);
    EXPECT_EQ(shader, nullptr);
}

/**
 * @tc.name: GetShaderTypeString001
 * @tc.desc: Test GetEffectTypeString by valid type and unknown type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, GetShaderTypeString001, TestSize.Level1)
{
    // Valid types
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::INVALID), "Invalid");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT),
        "ContourDiagonalFlowLight");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::WAVY_RIPPLE_LIGHT), "WavyRippleLight");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::AURORA_NOISE), "AuroraNoise");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::LIGHT_CAVE), "LightCave");
    // Unknown type
    RSNGEffectType unknownType = static_cast<RSNGEffectType>(999); // 999 is random value
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(unknownType), "UNKNOWN");
}

/**
 * @tc.name: Dump001
 * @tc.desc: Test Dump with one effect type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, Dump001, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    ASSERT_NE(shader, nullptr);

    std::string out;
    shader->Dump(out);

    // Should contain the type name followed by ": "
    std::string typeName = RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    EXPECT_NE(out.find(typeName + ": "), std::string::npos);
}

/**
 * @tc.name: Dump002
 * @tc.desc: Test Dump with two effect type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, Dump002, TestSize.Level1)
{
    auto head = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    auto next = RSNGRenderShaderBase::Create(RSNGEffectType::WAVY_RIPPLE_LIGHT);
    ASSERT_NE(head, nullptr);
    ASSERT_NE(next, nullptr);

    head->nextEffect_ = next;

    std::string out;
    head->Dump(out);

    // Chained output should contain ", " separator
    EXPECT_NE(out.find(", "), std::string::npos);
    // And include the next effect's type name
    std::string nextName = RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::WAVY_RIPPLE_LIGHT);
    EXPECT_NE(out.find(nextName), std::string::npos);
}

/**
 * @tc.name: AppendToGEContainerTest
 * @tc.desc: Test AppendToGEContainerTest
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, AppendToGEContainerTest, TestSize.Level1)
{
    std::shared_ptr<Drawing::GEVisualEffectContainer> nullContainer = nullptr;
    std::string name = "ge1";
    auto shader = std::make_shared<Drawing::GEVisualEffect>(name);
    RSNGRenderEffectHelper::AppendToGEContainer(nullContainer, shader);
    SUCCEED();

    auto container = std::make_shared<Drawing::GEVisualEffectContainer>();
    RSNGRenderEffectHelper::AppendToGEContainer(container, shader);

    EXPECT_EQ(container->filterVec_.size(), 1u);
    EXPECT_EQ(container->filterVec_[0], shader);
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Test Unmarshalling with valid parcel
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, Unmarshalling001, TestSize.Level1)
{
    auto original = RSNGRenderShaderBase::Create(RSNGEffectType::WAVY_RIPPLE_LIGHT);
    EXPECT_NE(original, nullptr);
    Parcel parcel;
    EXPECT_TRUE(original->Marshalling(parcel));
    std::shared_ptr<RSNGRenderShaderBase> val;
    bool result = RSNGRenderShaderBase::Unmarshalling(parcel, val);
    EXPECT_TRUE(result);
    EXPECT_NE(val, nullptr);
    EXPECT_EQ(val->GetType(), RSNGEffectType::WAVY_RIPPLE_LIGHT);
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc: Test Unmarshalling with two valid parcel
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, Unmarshalling002, TestSize.Level1)
{
    auto head = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    auto next = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    EXPECT_NE(head, nullptr);
    EXPECT_NE(next, nullptr);
    head->nextEffect_ = next;
    Parcel parcel;
    EXPECT_TRUE(head->Marshalling(parcel));
    std::shared_ptr<RSNGRenderShaderBase> val;
    bool result = RSNGRenderShaderBase::Unmarshalling(parcel, val);
    EXPECT_TRUE(result);
    // Verify head and next are correctly linked
    EXPECT_NE(val, nullptr);
    EXPECT_EQ(val->GetType(), RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    auto chained = val->nextEffect_;
    EXPECT_NE(chained, nullptr);
    EXPECT_EQ(chained->GetType(), RSNGEffectType::AURORA_NOISE);
}

/**
 * @tc.name: CheckEnableEDR001
 * @tc.desc: 验证 Unmarshalling 方法在链式Parcel（两节点）时返回true并正确链接两个实例
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, CheckEnableEDR001, TestSize.Level1)
{
    auto head = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    auto next = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    EXPECT_NE(head, nullptr);
    EXPECT_NE(next, nullptr);
    head->nextEffect_ = next;
    Parcel parcel;
    EXPECT_TRUE(head->Marshalling(parcel));
    std::shared_ptr<RSNGRenderShaderBase> val;
    bool result = RSNGRenderShaderBase::Unmarshalling(parcel, val);
    EXPECT_TRUE(result);
    // Verify head and next are correctly linked
    EXPECT_NE(val, nullptr);
    EXPECT_EQ(val->GetType(), RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    auto chained = val->nextEffect_;
    EXPECT_NE(chained, nullptr);
    EXPECT_EQ(chained->GetType(), RSNGEffectType::AURORA_NOISE);
}

/**
 * @tc.name: SetRotationAngle
 * @tc.desc: test SetRotationAngle
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, SetRotationAngle001, TestSize.Level1)
{
    auto head = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_LIGHT);
    std::shared_ptr<RSNGRenderShaderBase> emptyHead = nullptr;
    const Vector3f rotationAngle = {1.0f, 0.0f, 0.0f};
    {
        RSNGRenderShaderHelper::SetRotationAngle(emptyHead, rotationAngle);
    }
    {
        head = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
        RSNGRenderShaderHelper::SetRotationAngle(head, rotationAngle);
    }
    {
        RSNGRenderShaderHelper::SetRotationAngle(emptyHead, rotationAngle);
    }
    {
        head = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_LIGHT);
        RSNGRenderShaderHelper::SetRotationAngle(head, rotationAngle);
        while (head) {
            if (head->GetType() == RSNGEffectType::BORDER_LIGHT) {
                auto borderLightShader = std::static_pointer_cast<RSNGRenderBorderLight>(head);
                auto val = borderLightShader->Getter<BorderLightRotationAngleRenderTag>();
                EXPECT_NE(val, nullptr);
                EXPECT_EQ(val->Get(), rotationAngle);
            }
            head = head->nextEffect_;
        }
    }
}

/**
 * @tc.name: SetCornerRadius
 * @tc.desc: test SetCornerRadius
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, SetCornerRadius001, TestSize.Level1)
{
    auto head = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_LIGHT);
    std::shared_ptr<RSNGRenderShaderBase> emptyHead = nullptr;
    const float cornerRadius = 1.0f;
    {
        RSNGRenderShaderHelper::SetCornerRadius(emptyHead, cornerRadius);
    }
    {
        head = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
        RSNGRenderShaderHelper::SetCornerRadius(head, cornerRadius);
    }
    {
        RSNGRenderShaderHelper::SetCornerRadius(emptyHead, cornerRadius);
    }
    {
        head = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_LIGHT);
        RSNGRenderShaderHelper::SetCornerRadius(head, cornerRadius);
        while (head) {
            if (head->GetType() == RSNGEffectType::BORDER_LIGHT) {
                auto borderLightShader = std::static_pointer_cast<RSNGRenderBorderLight>(head);
                auto val = borderLightShader->Getter<BorderLightCornerRadiusRenderTag>();
                EXPECT_NE(val, nullptr);
                EXPECT_EQ(val->Get(), cornerRadius);
            }
        head = head->nextEffect_;
        }
    }
    {
        head = RSNGRenderShaderBase::Create(RSNGEffectType::HARMONIUM_EFFECT);
        RSNGRenderShaderHelper::SetCornerRadius(head, cornerRadius);
        while (head) {
            if (head->GetType() == RSNGEffectType::HARMONIUM_EFFECT) {
                auto borderLightShader = std::static_pointer_cast<RSNGRenderHarmoniumEffect>(head);
                auto val = borderLightShader->Getter<HarmoniumEffectCornerRadiusRenderTag>();
                EXPECT_NE(val, nullptr);
                EXPECT_EQ(val->Get(), cornerRadius);
            }
            head = head->nextEffect_;
        }
    }
}

/**
 * @tc.name: CalcRect001
 * @tc.desc: test CalcRect with null shader
 * @tc.type: FUNC
 * @tc.require: issue23101
 */
HWTEST_F(RSNGRenderShaderBaseTest, CalcRect001, TestSize.Level1)
{
    std::shared_ptr<RSNGRenderShaderBase> shader = nullptr;
    RectF bound(0.0f, 0.0f, 100.0f, 100.0f);
    RectF result = RSNGRenderShaderHelper::CalcRect(shader, bound);
    EXPECT_TRUE(result.IsEmpty());
}
/**
 * @tc.name: CalcRect002
 * @tc.desc: test CalcRect with non SDF_EDGE_LIGHT_EFFECT shader
 * @tc.type: FUNC
 * @tc.require: issue23101
 */
HWTEST_F(RSNGRenderShaderBaseTest, CalcRect002, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    ASSERT_NE(shader, nullptr);
    RectF bound(0.0f, 0.0f, 100.0f, 100.0f);
    RectF result = RSNGRenderShaderHelper::CalcRect(shader, bound);
    EXPECT_TRUE(result == bound);
}
/**
 * @tc.name: CalcRect003
 * @tc.desc: test CalcRect with SDF_EDGE_LIGHT_EFFECT, maxBorderWidth > outerBorderBloomWidth
 * @tc.type: FUNC
 * @tc.require: issue23101
 */
HWTEST_F(RSNGRenderShaderBaseTest, CalcRect003, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::SDF_EDGE_LIGHT_EFFECT);
    ASSERT_NE(shader, nullptr);
    auto sdfEdgeLightEffect = std::static_pointer_cast<RSNGRenderSDFEdgeLightEffect>(shader);
    constexpr float maxBorderWidth = 10.0f;
    constexpr float outerBorderBloomWidth = 5.0f;
    sdfEdgeLightEffect->Setter<SDFEdgeLightEffectMaxBorderWidthRenderTag>(maxBorderWidth,
        PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    sdfEdgeLightEffect->Setter<SDFEdgeLightEffectOuterBorderBloomWidthRenderTag>(outerBorderBloomWidth,
        PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    RectF bound(0.0f, 0.0f, 100.0f, 100.0f);
    RectF result = RSNGRenderShaderHelper::CalcRect(shader, bound);
    RectF expected(-maxBorderWidth, -maxBorderWidth, 100.0f + maxBorderWidth * 2.0f,
        100.0f + maxBorderWidth * 2.0f);
    EXPECT_TRUE(result == expected);
}
/**
 * @tc.name: CalcRect004
 * @tc.desc: test CalcRect with SDF_EDGE_LIGHT_EFFECT, maxBorderWidth < outerBorderBloomWidth
 * @tc.type: FUNC
 * @tc.require: issue23101
 */
HWTEST_F(RSNGRenderShaderBaseTest, CalcRect004, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::SDF_EDGE_LIGHT_EFFECT);
    ASSERT_NE(shader, nullptr);
    auto sdfEdgeLightEffect = std::static_pointer_cast<RSNGRenderSDFEdgeLightEffect>(shader);
    constexpr float maxBorderWidth = 5.0f;
    constexpr float outerBorderBloomWidth = 10.0f;
    sdfEdgeLightEffect->Setter<SDFEdgeLightEffectMaxBorderWidthRenderTag>(maxBorderWidth,
        PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    sdfEdgeLightEffect->Setter<SDFEdgeLightEffectOuterBorderBloomWidthRenderTag>(outerBorderBloomWidth,
        PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    RectF bound(0.0f, 0.0f, 100.0f, 100.0f);
    RectF result = RSNGRenderShaderHelper::CalcRect(shader, bound);
    RectF expected(-outerBorderBloomWidth, -outerBorderBloomWidth,
        100.0f + outerBorderBloomWidth * 2.0f, 100.0f + outerBorderBloomWidth * 2.0f);
    EXPECT_TRUE(result == expected);
}
/**
 * @tc.name: CalcRect005
 * @tc.desc: test CalcRect with chained shaders
 * @tc.type: FUNC
 * @tc.require: issue23101
 */
HWTEST_F(RSNGRenderShaderBaseTest, CalcRect005, TestSize.Level1)
{
    auto head = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    auto next = RSNGRenderShaderBase::Create(RSNGEffectType::SDF_EDGE_LIGHT_EFFECT);
    ASSERT_NE(head, nullptr);
    ASSERT_NE(next, nullptr);
    head->nextEffect_ = next;
    auto sdfEdgeLightEffect = std::static_pointer_cast<RSNGRenderSDFEdgeLightEffect>(next);
    constexpr float maxBorderWidth = 8.0f;
    constexpr float outerBorderBloomWidth = 12.0f;
    sdfEdgeLightEffect->Setter<SDFEdgeLightEffectMaxBorderWidthRenderTag>(maxBorderWidth,
        PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    sdfEdgeLightEffect->Setter<SDFEdgeLightEffectOuterBorderBloomWidthRenderTag>(outerBorderBloomWidth,
        PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    RectF bound(0.0f, 0.0f, 100.0f, 100.0f);
    RectF result = RSNGRenderShaderHelper::CalcRect(head, bound);
    RectF expected(-outerBorderBloomWidth, -outerBorderBloomWidth,
        100.0f + outerBorderBloomWidth * 2.0f, 100.0f + outerBorderBloomWidth * 2.0f);
    EXPECT_TRUE(result == expected);
}
/**
 * @tc.name: CalcRect006
 * @tc.desc: test CalcRect with zero border widths
 * @tc.type: FUNC
 * @tc.require: issue23101
 */
HWTEST_F(RSNGRenderShaderBaseTest, CalcRect006, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::SDF_EDGE_LIGHT_EFFECT);
    ASSERT_NE(shader, nullptr);
    auto sdfEdgeLightEffect = std::static_pointer_cast<RSNGRenderSDFEdgeLightEffect>(shader);
    constexpr float maxBorderWidth = 0.0f;
    constexpr float outerBorderBloomWidth = 0.0f;
    sdfEdgeLightEffect->Setter<SDFEdgeLightEffectMaxBorderWidthRenderTag>(maxBorderWidth,
        PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    sdfEdgeLightEffect->Setter<SDFEdgeLightEffectOuterBorderBloomWidthRenderTag>(outerBorderBloomWidth,
        PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    RectF bound(0.0f, 0.0f, 100.0f, 100.0f);
    RectF result = RSNGRenderShaderHelper::CalcRect(shader, bound);
    EXPECT_TRUE(result == bound);
}

/**
 * @tc.name: CalcRect007
 * @tc.desc: test CalcRect with sdfShape, transformDrawRect differs from bound
 * @tc.type: FUNC
 * @tc.require: issue23101
 */
HWTEST_F(RSNGRenderShaderBaseTest, CalcRect007, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::SDF_EDGE_LIGHT_EFFECT);
    ASSERT_NE(shader, nullptr);
    auto sdfEdgeLightEffect = std::static_pointer_cast<RSNGRenderSDFEdgeLightEffect>(shader);
    constexpr float maxBorderWidth = 8.0f;
    constexpr float outerBorderBloomWidth = 12.0f;
    sdfEdgeLightEffect->Setter<SDFEdgeLightEffectMaxBorderWidthRenderTag>(maxBorderWidth,
        PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    sdfEdgeLightEffect->Setter<SDFEdgeLightEffectOuterBorderBloomWidthRenderTag>(outerBorderBloomWidth,
        PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);

    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(sdfShape, nullptr);
    EXPECT_TRUE(sdfShape->GetTransformDrawRect().IsEmpty());
    sdfEdgeLightEffect->Setter<SDFEdgeLightEffectSDFShapeRenderTag>(sdfShape,
        PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    RectF bound(0.0f, 0.0f, 100.0f, 100.0f);
    RSNGRenderShaderHelper::CalcRect(shader, bound);

    sdfShape->transformDrawRect_ = RectF(10.0f, 10.0f, 80.0f, 80.0f);
    EXPECT_FALSE(sdfShape->GetTransformDrawRect().IsEmpty());
    constexpr float shapeLeft = 10.0f;
    constexpr float shapeTop = 15.0f;
    constexpr float shapeWidth = 80.0f;
    constexpr float shapeHeight = 70.0f;
    RectF shapeBound(shapeLeft, shapeTop, shapeWidth, shapeHeight);
    RSNGRenderShapeHelper::CalcRect(sdfShape, shapeBound);
    RectF result = RSNGRenderShaderHelper::CalcRect(shader, bound);
    RectF expected(-outerBorderBloomWidth + shapeLeft, -outerBorderBloomWidth + shapeTop,
        shapeWidth + outerBorderBloomWidth * 2.0f, shapeHeight + outerBorderBloomWidth * 2.0f);
    EXPECT_TRUE(result == expected);
}

/**
 * @tc.name: CreateBorderSDFShader001
 * @tc.desc: Verify that Create returns a non-null instance for BORDER_SDF_SHADER type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, CreateBorderSDFShader001, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_SDF_SHADER);
    EXPECT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::BORDER_SDF_SHADER);
}

/**
* @tc.name: DistortChromaShaderCreate
 * @tc.desc: Test creating DistortChroma shader and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, DistortChromaShaderCreate, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::DISTORT_CHROMA);
    ASSERT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::DISTORT_CHROMA);
}

/**
 * @tc.name: DistortChromaShaderSetterGetter
 * @tc.desc: Test DistortChroma shader Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, DistortChromaShaderSetterGetter, TestSize.Level1)
{
    auto shader = std::make_shared<RSNGRenderDistortChroma>();

    float progress = 0.5f;
    shader->Setter<DistortChromaProgressRenderTag>(progress);
    auto progressVal = shader->Getter<DistortChromaProgressRenderTag>();
    ASSERT_NE(progressVal, nullptr);
    EXPECT_FLOAT_EQ(progressVal->Get(), progress);

    Vector2f granularity { 1.0f, 1.0f };
    shader->Setter<DistortChromaGranularityRenderTag>(granularity);
    auto granularityVal = shader->Getter<DistortChromaGranularityRenderTag>();
    ASSERT_NE(granularityVal, nullptr);
    EXPECT_FLOAT_EQ(granularityVal->Get().x_, granularity.x_);

    float frequency = 2.0f;
    shader->Setter<DistortChromaFrequencyRenderTag>(frequency);
    auto frequencyVal = shader->Getter<DistortChromaFrequencyRenderTag>();
    ASSERT_NE(frequencyVal, nullptr);
    EXPECT_FLOAT_EQ(frequencyVal->Get(), frequency);

    float brightness = 1.5f;
    shader->Setter<DistortChromaBrightnessRenderTag>(brightness);
    auto brightnessVal = shader->Getter<DistortChromaBrightnessRenderTag>();
    ASSERT_NE(brightnessVal, nullptr);
    EXPECT_FLOAT_EQ(brightnessVal->Get(), brightness);
}

/**
 * @tc.name: DistortChromaShaderContains
 * @tc.desc: Test DistortChroma shader Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, DistortChromaShaderContains, TestSize.Level1)
{
    auto shader = std::make_shared<RSNGRenderDistortChroma>();
    EXPECT_TRUE(shader->Contains<DistortChromaProgressRenderTag>());
    EXPECT_TRUE(shader->Contains<DistortChromaGranularityRenderTag>());
    EXPECT_TRUE(shader->Contains<DistortChromaFrequencyRenderTag>());
    EXPECT_TRUE(shader->Contains<DistortChromaBrightnessRenderTag>());
}

/**
 * @tc.name: LightCaveShaderSetterGetter
 * @tc.desc: Test LightCave shader Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, LightCaveShaderSetterGetter, TestSize.Level1)
{
    auto shader = std::make_shared<RSNGRenderLightCave>();

    Vector4f colorA { 1.0f, 1.0f, 1.0f, 1.0f };
    shader->Setter<LightCaveColorARenderTag>(colorA);
    auto colorAVal = shader->Getter<LightCaveColorARenderTag>();
    ASSERT_NE(colorAVal, nullptr);
    EXPECT_FLOAT_EQ(colorAVal->Get().x_, colorA.x_);

    Vector4f colorB { 0.8f, 0.8f, 1.0f, 1.0f };
    shader->Setter<LightCaveColorBRenderTag>(colorB);
    auto colorBVal = shader->Getter<LightCaveColorBRenderTag>();
    ASSERT_NE(colorBVal, nullptr);
    EXPECT_FLOAT_EQ(colorBVal->Get().x_, colorB.x_);

    Vector4f colorC { 0.6f, 0.6f, 1.0f, 1.0f };
    shader->Setter<LightCaveColorCRenderTag>(colorC);
    auto colorCVal = shader->Getter<LightCaveColorCRenderTag>();
    ASSERT_NE(colorCVal, nullptr);
    EXPECT_FLOAT_EQ(colorCVal->Get().x_, colorC.x_);

    Vector2f position { 0.5f, 0.5f };
    shader->Setter<LightCavePositionRenderTag>(position);
    auto positionVal = shader->Getter<LightCavePositionRenderTag>();
    ASSERT_NE(positionVal, nullptr);
    EXPECT_FLOAT_EQ(positionVal->Get().x_, position.x_);

    Vector2f radiusXY { 50.0f, 50.0f };
    shader->Setter<LightCaveRadiusXYRenderTag>(radiusXY);
    auto radiusXYVal = shader->Getter<LightCaveRadiusXYRenderTag>();
    ASSERT_NE(radiusXYVal, nullptr);
    EXPECT_FLOAT_EQ(radiusXYVal->Get().x_, radiusXY.x_);

    float progress = 0.5f;
    shader->Setter<LightCaveProgressRenderTag>(progress);
    auto progressVal = shader->Getter<LightCaveProgressRenderTag>();
    ASSERT_NE(progressVal, nullptr);
    EXPECT_FLOAT_EQ(progressVal->Get(), progress);
}

/**
 * @tc.name: LightCaveShaderContains
 * @tc.desc: Test LightCave shader Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, LightCaveShaderContains, TestSize.Level1)
{
    auto shader = std::make_shared<RSNGRenderLightCave>();
    EXPECT_TRUE(shader->Contains<LightCaveColorARenderTag>());
    EXPECT_TRUE(shader->Contains<LightCaveColorBRenderTag>());
    EXPECT_TRUE(shader->Contains<LightCaveColorCRenderTag>());
    EXPECT_TRUE(shader->Contains<LightCavePositionRenderTag>());
    EXPECT_TRUE(shader->Contains<LightCaveRadiusXYRenderTag>());
    EXPECT_TRUE(shader->Contains<LightCaveProgressRenderTag>());
}

/**
 * @tc.name: AIBarGlowShaderCreate
 * @tc.desc: Test creating AIBarGlow shader and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, AIBarGlowShaderCreate, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AIBAR_GLOW);
    ASSERT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::AIBAR_GLOW);
}

/**
 * @tc.name: AIBarGlowShaderSetterGetter
 * @tc.desc: Test AIBarGlow shader Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, AIBarGlowShaderSetterGetter, TestSize.Level1)
{
    auto shader = std::make_shared<RSNGRenderAIBarGlow>();

    Vector4f ltwh { 0.0f, 0.0f, 100.0f, 100.0f };
    shader->Setter<AIBarGlowLTWHRenderTag>(ltwh);
    auto ltwhVal = shader->Getter<AIBarGlowLTWHRenderTag>();
    ASSERT_NE(ltwhVal, nullptr);
    EXPECT_FLOAT_EQ(ltwhVal->Get().z_, ltwh.z_);

    float stretchFactor = 1.0f;
    shader->Setter<AIBarGlowStretchFactorRenderTag>(stretchFactor);
    auto stretchFactorVal = shader->Getter<AIBarGlowStretchFactorRenderTag>();
    ASSERT_NE(stretchFactorVal, nullptr);
    EXPECT_FLOAT_EQ(stretchFactorVal->Get(), stretchFactor);

    Vector4f color0 { 1.0f, 1.0f, 1.0f, 1.0f };
    shader->Setter<AIBarGlowColor0RenderTag>(color0);
    auto color0Val = shader->Getter<AIBarGlowColor0RenderTag>();
    ASSERT_NE(color0Val, nullptr);
    EXPECT_FLOAT_EQ(color0Val->Get().x_, color0.x_);

    float progress = 0.5f;
    shader->Setter<AIBarGlowProgressRenderTag>(progress);
    auto progressVal = shader->Getter<AIBarGlowProgressRenderTag>();
    ASSERT_NE(progressVal, nullptr);
    EXPECT_FLOAT_EQ(progressVal->Get(), progress);

    float strength = 1.0f;
    shader->Setter<AIBarGlowStrengthRenderTag>(strength);
    auto strengthVal = shader->Getter<AIBarGlowStrengthRenderTag>();
    ASSERT_NE(strengthVal, nullptr);
}

/**
 * @tc.name: AIBarGlowShaderContains
 * @tc.desc: Test AIBarGlow shader Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, AIBarGlowShaderContains, TestSize.Level1)
{
    auto shader = std::make_shared<RSNGRenderAIBarGlow>();
    EXPECT_TRUE(shader->Contains<AIBarGlowLTWHRenderTag>());
    EXPECT_TRUE(shader->Contains<AIBarGlowStretchFactorRenderTag>());
    EXPECT_TRUE(shader->Contains<AIBarGlowColor0RenderTag>());
    EXPECT_TRUE(shader->Contains<AIBarGlowProgressRenderTag>());
    EXPECT_TRUE(shader->Contains<AIBarGlowStrengthRenderTag>());
}

/**
 * @tc.name: RoundedRectFlowlightShaderCreate
 * @tc.desc: Test creating RoundedRectFlowlight shader and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, RoundedRectFlowlightShaderCreate, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::ROUNDED_RECT_FLOWLIGHT);
    ASSERT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::ROUNDED_RECT_FLOWLIGHT);
}

/**
 * @tc.name: RoundedRectFlowlightShaderSetterGetter
 * @tc.desc: Test RoundedRectFlowlight shader Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, RoundedRectFlowlightShaderSetterGetter, TestSize.Level1)
{
    auto shader = std::make_shared<RSNGRenderRoundedRectFlowlight>();

    Vector2f startEndPosition { 0.0f, 1.0f };
    shader->Setter<RoundedRectFlowlightStartEndPositionRenderTag>(startEndPosition);
    auto startEndPositionVal = shader->Getter<RoundedRectFlowlightStartEndPositionRenderTag>();
    ASSERT_NE(startEndPositionVal, nullptr);
    EXPECT_FLOAT_EQ(startEndPositionVal->Get().x_, startEndPosition.x_);

    float waveLength = 0.2f;
    shader->Setter<RoundedRectFlowlightWaveLengthRenderTag>(waveLength);
    auto waveLengthVal = shader->Getter<RoundedRectFlowlightWaveLengthRenderTag>();
    ASSERT_NE(waveLengthVal, nullptr);
    EXPECT_FLOAT_EQ(waveLengthVal->Get(), waveLength);

    float cornerRadius = 10.0f;
    shader->Setter<RoundedRectFlowlightCornerRadiusRenderTag>(cornerRadius);
    auto cornerRadiusVal = shader->Getter<RoundedRectFlowlightCornerRadiusRenderTag>();
    ASSERT_NE(cornerRadiusVal, nullptr);
    EXPECT_FLOAT_EQ(cornerRadiusVal->Get(), cornerRadius);

    float brightness = 1.0f;
    shader->Setter<RoundedRectFlowlightBrightnessRenderTag>(brightness);
    auto brightnessVal = shader->Getter<RoundedRectFlowlightBrightnessRenderTag>();
    ASSERT_NE(brightnessVal, nullptr);
    EXPECT_FLOAT_EQ(brightnessVal->Get(), brightness);

    float progress = 0.5f;
    shader->Setter<RoundedRectFlowlightProgressRenderTag>(progress);
    auto progressVal = shader->Getter<RoundedRectFlowlightProgressRenderTag>();
    ASSERT_NE(progressVal, nullptr);
    EXPECT_FLOAT_EQ(progressVal->Get(), progress);
}

/**
 * @tc.name: RoundedRectFlowlightShaderContains
 * @tc.desc: Test RoundedRectFlowlight shader Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, RoundedRectFlowlightShaderContains, TestSize.Level1)
{
    auto shader = std::make_shared<RSNGRenderRoundedRectFlowlight>();
    EXPECT_TRUE(shader->Contains<RoundedRectFlowlightStartEndPositionRenderTag>());
    EXPECT_TRUE(shader->Contains<RoundedRectFlowlightWaveLengthRenderTag>());
    EXPECT_TRUE(shader->Contains<RoundedRectFlowlightCornerRadiusRenderTag>());
    EXPECT_TRUE(shader->Contains<RoundedRectFlowlightBrightnessRenderTag>());
    EXPECT_TRUE(shader->Contains<RoundedRectFlowlightProgressRenderTag>());
}

/**
 * @tc.name: GradientFlowColorsShaderCreate
 * @tc.desc: Test creating GradientFlowColors shader and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, GradientFlowColorsShaderCreate, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::GRADIENT_FLOW_COLORS);
    ASSERT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::GRADIENT_FLOW_COLORS);
}

/**
 * @tc.name: GradientFlowColorsShaderSetterGetter
 * @tc.desc: Test GradientFlowColors shader Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, GradientFlowColorsShaderSetterGetter, TestSize.Level1)
{
    auto shader = std::make_shared<RSNGRenderGradientFlowColors>();

    Vector4f color0 { 1.0f, 1.0f, 1.0f, 1.0f };
    shader->Setter<GradientFlowColorsColor0RenderTag>(color0);
    auto color0Val = shader->Getter<GradientFlowColorsColor0RenderTag>();
    ASSERT_NE(color0Val, nullptr);
    EXPECT_FLOAT_EQ(color0Val->Get().x_, color0.x_);

    Vector4f color1 { 0.8f, 0.8f, 1.0f, 1.0f };
    shader->Setter<GradientFlowColorsColor1RenderTag>(color1);
    auto color1Val = shader->Getter<GradientFlowColorsColor1RenderTag>();
    ASSERT_NE(color1Val, nullptr);
    EXPECT_FLOAT_EQ(color1Val->Get().x_, color1.x_);

    float gradientBegin = 0.0f;
    shader->Setter<GradientFlowColorsGradientBeginRenderTag>(gradientBegin);
    auto gradientBeginVal = shader->Getter<GradientFlowColorsGradientBeginRenderTag>();
    ASSERT_NE(gradientBeginVal, nullptr);
    EXPECT_FLOAT_EQ(gradientBeginVal->Get(), gradientBegin);

    float gradientEnd = 1.0f;
    shader->Setter<GradientFlowColorsGradientEndRenderTag>(gradientEnd);
    auto gradientEndVal = shader->Getter<GradientFlowColorsGradientEndRenderTag>();
    ASSERT_NE(gradientEndVal, nullptr);
    EXPECT_FLOAT_EQ(gradientEndVal->Get(), gradientEnd);

    float effectAlpha = 1.0f;
    shader->Setter<GradientFlowColorsEffectAlphaRenderTag>(effectAlpha);
    auto effectAlphaVal = shader->Getter<GradientFlowColorsEffectAlphaRenderTag>();
    ASSERT_NE(effectAlphaVal, nullptr);
    EXPECT_FLOAT_EQ(effectAlphaVal->Get(), effectAlpha);

    float progress = 0.5f;
    shader->Setter<GradientFlowColorsProgressRenderTag>(progress);
    auto progressVal = shader->Getter<GradientFlowColorsProgressRenderTag>();
    ASSERT_NE(progressVal, nullptr);
    EXPECT_FLOAT_EQ(progressVal->Get(), progress);
}

/**
 * @tc.name: GradientFlowColorsShaderContains
 * @tc.desc: Test GradientFlowColors shader Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, GradientFlowColorsShaderContains, TestSize.Level1)
{
    auto shader = std::make_shared<RSNGRenderGradientFlowColors>();
    EXPECT_TRUE(shader->Contains<GradientFlowColorsColor0RenderTag>());
    EXPECT_TRUE(shader->Contains<GradientFlowColorsColor1RenderTag>());
    EXPECT_TRUE(shader->Contains<GradientFlowColorsGradientBeginRenderTag>());
    EXPECT_TRUE(shader->Contains<GradientFlowColorsGradientEndRenderTag>());
    EXPECT_TRUE(shader->Contains<GradientFlowColorsEffectAlphaRenderTag>());
    EXPECT_TRUE(shader->Contains<GradientFlowColorsProgressRenderTag>());
}

/**
 * @tc.name: CircleFlowlightShaderCreate
 * @tc.desc: Test creating CircleFlowlight shader and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, CircleFlowlightShaderCreate, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::CIRCLE_FLOWLIGHT);
    ASSERT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::CIRCLE_FLOWLIGHT);
}

/**
 * @tc.name: CircleFlowlightShaderSetterGetter
 * @tc.desc: Test CircleFlowlight shader Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, CircleFlowlightShaderSetterGetter, TestSize.Level1)
{
    auto shader = std::make_shared<RSNGRenderCircleFlowlight>();

    Vector4f color0 { 1.0f, 1.0f, 1.0f, 1.0f };
    shader->Setter<CircleFlowlightColor0RenderTag>(color0);
    auto color0Val = shader->Getter<CircleFlowlightColor0RenderTag>();
    ASSERT_NE(color0Val, nullptr);
    EXPECT_FLOAT_EQ(color0Val->Get().x_, color0.x_);

    Vector4f color1 { 0.8f, 0.8f, 1.0f, 1.0f };
    shader->Setter<CircleFlowlightColor1RenderTag>(color1);
    auto color1Val = shader->Getter<CircleFlowlightColor1RenderTag>();
    ASSERT_NE(color1Val, nullptr);
    EXPECT_FLOAT_EQ(color1Val->Get().x_, color1.x_);

    Vector4f rotationFrequency { 1.0f, 1.0f, 1.0f, 1.0f };
    shader->Setter<CircleFlowlightRotationFrequencyRenderTag>(rotationFrequency);
    auto rotationFrequencyVal = shader->Getter<CircleFlowlightRotationFrequencyRenderTag>();
    ASSERT_NE(rotationFrequencyVal, nullptr);
    EXPECT_FLOAT_EQ(rotationFrequencyVal->Get().x_, rotationFrequency.x_);

    float progress = 0.5f;
    shader->Setter<CircleFlowlightProgressRenderTag>(progress);
    auto progressVal = shader->Getter<CircleFlowlightProgressRenderTag>();
    ASSERT_NE(progressVal, nullptr);
    EXPECT_FLOAT_EQ(progressVal->Get(), progress);

    float strength = 1.0f;
    shader->Setter<CircleFlowlightStrengthRenderTag>(strength);
    auto strengthVal = shader->Getter<CircleFlowlightStrengthRenderTag>();
    ASSERT_NE(strengthVal, nullptr);
}

/**
 * @tc.name: CircleFlowlightShaderContains
 * @tc.desc: Test CircleFlowlight shader Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, CircleFlowlightShaderContains, TestSize.Level1)
{
    auto shader = std::make_shared<RSNGRenderCircleFlowlight>();
    EXPECT_TRUE(shader->Contains<CircleFlowlightColor0RenderTag>());
    EXPECT_TRUE(shader->Contains<CircleFlowlightColor1RenderTag>());
    EXPECT_TRUE(shader->Contains<CircleFlowlightRotationFrequencyRenderTag>());
    EXPECT_TRUE(shader->Contains<CircleFlowlightProgressRenderTag>());
    EXPECT_TRUE(shader->Contains<CircleFlowlightStrengthRenderTag>());
}

/**
 * @tc.name: ColorGradientEffectShaderCreate
 * @tc.desc: Test creating ColorGradientEffect shader and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, ColorGradientEffectShaderCreate, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::COLOR_GRADIENT_EFFECT);
    ASSERT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::COLOR_GRADIENT_EFFECT);
}

/**
 * @tc.name: ColorGradientEffectShaderContains
 * @tc.desc: Test ColorGradientEffect shader Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, ColorGradientEffectShaderContains, TestSize.Level1)
{
    auto shader = std::make_shared<RSNGRenderColorGradientEffect>();
    EXPECT_TRUE(shader->Contains<ColorGradientEffectColor0RenderTag>());
    EXPECT_TRUE(shader->Contains<ColorGradientEffectColor1RenderTag>());
    EXPECT_TRUE(shader->Contains<ColorGradientEffectPosition0RenderTag>());
    EXPECT_TRUE(shader->Contains<ColorGradientEffectPosition1RenderTag>());
    EXPECT_TRUE(shader->Contains<ColorGradientEffectColorNumberRenderTag>());
    EXPECT_TRUE(shader->Contains<ColorGradientEffectBrightnessRenderTag>());
}
} // namespace OHOS::Rosen