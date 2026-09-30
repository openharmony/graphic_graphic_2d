/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "drawable/rs_effect_drawable_utils.h"
#include "draw/surface.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "ge_visual_effect_container.h"
#include "property/rs_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSEffectDrawableUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSEffectDrawableUtilsTest::SetUpTestCase() {}
void RSEffectDrawableUtilsTest::TearDownTestCase() {}
void RSEffectDrawableUtilsTest::SetUp() {}
void RSEffectDrawableUtilsTest::TearDown() {}

/**
 * @tc.name: DrawColorUsingSDFWithDRMTest
 * @tc.desc: DrawColorUsingSDFWithDRM test
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, DrawColorUsingSDFWithDRMTest, testing::ext::TestSize.Level1)
{
    Drawing::Canvas canvas;
    bool isDark = true;
    Drawing::Rect rect(0, 0, 0, 0);

    DrawableV2::RSEffectDrawableUtils::DrawColorUsingSDFWithDRM(&canvas, nullptr, isDark, nullptr, "Tag1", "Tag2");
    DrawableV2::RSEffectDrawableUtils::DrawColorUsingSDFWithDRM(&canvas, &rect, isDark, nullptr, "Tag1", "Tag2");
    EXPECT_TRUE(isDark);
    auto filterGEContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto geVisualEffect = std::make_shared<Drawing::GEVisualEffect>("test");
    filterGEContainer->AddToChainedFilter(geVisualEffect);
    DrawableV2::RSEffectDrawableUtils::DrawColorUsingSDFWithDRM(&canvas, &rect, isDark, filterGEContainer, "Tag1",
        "Tag2");
    DrawableV2::RSEffectDrawableUtils::DrawColorUsingSDFWithDRM(&canvas, &rect, isDark, filterGEContainer, "test",
        "Tag2");
    EXPECT_TRUE(isDark);
    isDark = false;
    DrawableV2::RSEffectDrawableUtils::DrawColorUsingSDFWithDRM(&canvas, &rect, isDark, filterGEContainer, "test",
        "Tag2");
    EXPECT_FALSE(isDark);
}

/**
 * @tc.name: DrawColorUsingSDFWithDRMNullCanvasTest
 * @tc.desc: DrawColorUsingSDFWithDRM returns safely when canvas is null
 * @tc.type: FUNC
 * @tc.require: issue25648
 */
HWTEST_F(RSEffectDrawableUtilsTest, DrawColorUsingSDFWithDRMNullCanvasTest, testing::ext::TestSize.Level1)
{
    Drawing::Rect rect(0, 0, 0, 0);
    bool isDark = true;

    DrawableV2::RSEffectDrawableUtils::DrawColorUsingSDFWithDRM(nullptr, &rect, isDark, nullptr, "Tag1", "Tag2");
    EXPECT_TRUE(isDark);
}

/**
 * @tc.name: ApplySDFShapeToEffect001
 * @tc.desc: Test ApplySDFShapeToEffect with null shader
 * @tc.type: FUNC
 * @tc.require: issue23101
 */
HWTEST_F(RSEffectDrawableUtilsTest, ApplySDFShapeToEffect001, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    NodeId nodeId = 1;
    std::shared_ptr<RSNGRenderShaderBase> shader = nullptr;
    DrawableV2::RSEffectDrawableUtils::ApplySDFShapeToEffect(properties, shader, nodeId);
    EXPECT_EQ(shader, nullptr);
}

/**
 * @tc.name: ApplySDFShapeToEffect002
 * @tc.desc: Test ApplySDFShapeToEffect with non SDF_EDGE_LIGHT_EFFECT shader
 * @tc.type: FUNC
 * @tc.require: issue23101
 */
HWTEST_F(RSEffectDrawableUtilsTest, ApplySDFShapeToEffect002, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    NodeId nodeId = 1;
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    DrawableV2::RSEffectDrawableUtils::ApplySDFShapeToEffect(properties, shader, nodeId);
    ASSERT_NE(shader, nullptr);
    EXPECT_NE(shader->GetType(), RSNGEffectType::SDF_EDGE_LIGHT_EFFECT);
}

/**
 * @tc.name: ApplySDFShapeToEffect003
 * @tc.desc: Test ApplySDFShapeToEffect with sdfShape
 * @tc.type: FUNC
 * @tc.require: issue23101
 */
HWTEST_F(RSEffectDrawableUtilsTest, ApplySDFShapeToEffect003, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    auto sdfRRectShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    properties.SetSDFShape(sdfRRectShape);
    NodeId nodeId = 1;
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::SDF_EDGE_LIGHT_EFFECT);
    const auto& effectShader = std::static_pointer_cast<RSNGRenderSDFEdgeLightEffect>(shader);
    EXPECT_EQ(effectShader->Getter<SDFEdgeLightEffectSDFShapeRenderTag>()->stagingValue_, nullptr);
    DrawableV2::RSEffectDrawableUtils::ApplySDFShapeToEffect(properties, shader, nodeId);
    ASSERT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::SDF_EDGE_LIGHT_EFFECT);
    const auto& shaderFromResult = std::static_pointer_cast<RSNGRenderSDFEdgeLightEffect>(shader);
    EXPECT_NE(shaderFromResult->Getter<SDFEdgeLightEffectSDFShapeRenderTag>()->stagingValue_, nullptr);
}

/**
 * @tc.name: ApplySDFShapeToEffect004
 * @tc.desc: Test ApplySDFShapeToEffect without sdfShape but with clipRRect
 * @tc.type: FUNC
 * @tc.require: issue23101
 */
HWTEST_F(RSEffectDrawableUtilsTest, ApplySDFShapeToEffect004, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    EXPECT_EQ(properties.GetSDFShape(), nullptr);
    properties.SetClipRRect(RRect(RectT<float>(0.0f, 0.0f, 100.0f, 100.0f), 10.0f, 10.0f));
    NodeId nodeId = 1;
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::SDF_EDGE_LIGHT_EFFECT);
    const auto& effectShader = std::static_pointer_cast<RSNGRenderSDFEdgeLightEffect>(shader);
    EXPECT_EQ(effectShader->Getter<SDFEdgeLightEffectSDFShapeRenderTag>()->stagingValue_, nullptr);
    DrawableV2::RSEffectDrawableUtils::ApplySDFShapeToEffect(properties, shader, nodeId);
    ASSERT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::SDF_EDGE_LIGHT_EFFECT);
    const auto& shaderFromResult = std::static_pointer_cast<RSNGRenderSDFEdgeLightEffect>(shader);
    const auto& rrectShape = std::static_pointer_cast<RSNGRenderSDFRRectShape>(
        shaderFromResult->Getter<SDFEdgeLightEffectSDFShapeRenderTag>()->stagingValue_);
    auto rrectFromShape = rrectShape->Getter<SDFRRectShapeRRectRenderTag>()->stagingValue_;
    EXPECT_TRUE(rrectFromShape.IsNearEqual(RRect(RectT<float>(0.0f, 0.0f, 100.0f, 100.0f), 10.0f, 10.0f)));
}

/**
 * @tc.name: ApplySDFShapeToEffect005
 * @tc.desc: Test ApplySDFShapeToEffect with cornerRadius
 * @tc.type: FUNC
 * @tc.require: issue23101
 */
HWTEST_F(RSEffectDrawableUtilsTest, ApplySDFShapeToEffect005, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    EXPECT_EQ(properties.GetSDFShape(), nullptr);
    EXPECT_EQ(properties.GetClipRRect(), RRect());
    properties.SetCornerRadius(10.0f);
    NodeId nodeId = 1;
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::SDF_EDGE_LIGHT_EFFECT);
    const auto& effectShader = std::static_pointer_cast<RSNGRenderSDFEdgeLightEffect>(shader);
    EXPECT_EQ(effectShader->Getter<SDFEdgeLightEffectSDFShapeRenderTag>()->stagingValue_, nullptr);
    DrawableV2::RSEffectDrawableUtils::ApplySDFShapeToEffect(properties, shader, nodeId);
    ASSERT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::SDF_EDGE_LIGHT_EFFECT);
    const auto& shaderFromResult = std::static_pointer_cast<RSNGRenderSDFEdgeLightEffect>(shader);
    const auto& rrectShape = std::static_pointer_cast<RSNGRenderSDFRRectShape>(
        shaderFromResult->Getter<SDFEdgeLightEffectSDFShapeRenderTag>()->stagingValue_);
    auto rrectFromShape = rrectShape->Getter<SDFRRectShapeRRectRenderTag>()->stagingValue_;
    EXPECT_TRUE(rrectFromShape.IsNearEqual(RRect(properties.GetRRect().rect_, properties.GetRRect().radius_[0].x_,
        properties.GetRRect().radius_[0].y_)));
}

/**
 * @tc.name: ApplySDFShapeToEffect006
 * @tc.desc: Test ApplySDFShapeToEffect with boundsRect
 * @tc.type: FUNC
 * @tc.require: issue23101
 */
HWTEST_F(RSEffectDrawableUtilsTest, ApplySDFShapeToEffect006, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    EXPECT_EQ(properties.GetSDFShape(), nullptr);
    EXPECT_EQ(properties.GetClipRRect(), RRect());
    EXPECT_EQ(properties.GetCornerRadius(), Vector4f(0.f, 0.f, 0.f, 0.f));
    properties.SetBounds(Vector4f(0.0f, 0.0f, 100.0f, 100.0f));
    properties.SetFrame(Vector4f(0.0f, 0.0f, 100.0f, 100.0f));
    NodeId nodeId = 1;
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::SDF_EDGE_LIGHT_EFFECT);
    const auto& effectShader = std::static_pointer_cast<RSNGRenderSDFEdgeLightEffect>(shader);
    EXPECT_EQ(effectShader->Getter<SDFEdgeLightEffectSDFShapeRenderTag>()->stagingValue_, nullptr);
    DrawableV2::RSEffectDrawableUtils::ApplySDFShapeToEffect(properties, shader, nodeId);
    ASSERT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::SDF_EDGE_LIGHT_EFFECT);
    const auto& shaderFromResult = std::static_pointer_cast<RSNGRenderSDFEdgeLightEffect>(shader);
    const auto& rrectShape = std::static_pointer_cast<RSNGRenderSDFRRectShape>(
        shaderFromResult->Getter<SDFEdgeLightEffectSDFShapeRenderTag>()->stagingValue_);
    auto rrectFromShape = rrectShape->Getter<SDFRRectShapeRRectRenderTag>()->stagingValue_;
    EXPECT_TRUE(rrectFromShape.IsNearEqual(RRect(RectT<float>(0.0f, 0.0f, 100.0f, 100.0f), 0.0f, 0.0f)));
}

/**
 * @tc.name: ApplySDFShapeToEffect007
 * @tc.desc: Test ApplySDFShapeToEffect with SDF_EDGE_LIGHT_EFFECT and valid sdfShape
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, ApplySDFShapeToEffect007, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    NodeId nodeId = 1;
    auto rrectShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    properties.SetSDFShape(rrectShape);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::SDF_EDGE_LIGHT_EFFECT);
    ASSERT_NE(shader, nullptr);
    DrawableV2::RSEffectDrawableUtils::ApplySDFShapeToEffect(properties, shader, nodeId);
    const auto& effectShader = std::static_pointer_cast<RSNGRenderSDFEdgeLightEffect>(shader);
    auto resultShape = effectShader->Getter<SDFEdgeLightEffectSDFShapeRenderTag>()->stagingValue_;
    EXPECT_NE(resultShape, nullptr);
    EXPECT_EQ(resultShape->GetType(), RSNGEffectType::SDF_RRECT_SHAPE);
}

} // namespace OHOS::Rosen

