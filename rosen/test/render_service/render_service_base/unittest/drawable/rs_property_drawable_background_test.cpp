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

#include "gtest/gtest.h"

#include "drawable/rs_property_drawable_background.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "render/rs_drawing_filter.h"
#include "third_party/skia/include/core/SkStream.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRSBinarizationDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

void RSRSBinarizationDrawableTest::SetUpTestCase() {}
void RSRSBinarizationDrawableTest::TearDownTestCase() {}
void RSRSBinarizationDrawableTest::SetUp() {}
void RSRSBinarizationDrawableTest::TearDown() {}

/**
 * @tc.name: RSBinarizationDrawable
 * @tc.desc: Test OnSync
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBinarizationDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSShadowDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    node.renderContent_->GetMutableRenderProperties().SetShadowIsFilled(true);
    node.renderContent_->GetMutableRenderProperties().shadow_->radius_ = 1.0f;
    ASSERT_TRUE(node.GetRenderProperties().IsShadowValid());
    node.renderContent_->GetMutableRenderProperties().shadow_->SetMask(true);
    std::shared_ptr<RSDrawable> drawableTwo = DrawableV2::RSShadowDrawable::OnGenerate(node);
    node.renderContent_->GetMutableRenderProperties().shadow_->SetMask(false);
    node.renderContent_->GetMutableRenderProperties().shadow_->SetElevation(1.0f);
    ASSERT_TRUE(node.GetRenderProperties().GetShadowElevation() > 0.f);
    std::shared_ptr<RSDrawable> drawableThree = DrawableV2::RSShadowDrawable::OnGenerate(node);
    std::shared_ptr<DrawableV2::RSShadowDrawable> rsShadowDrawable =
        std::static_pointer_cast<DrawableV2::RSShadowDrawable>(drawableThree);
    ASSERT_NE(rsShadowDrawable, nullptr);
    rsShadowDrawable->OnSync();
    ASSERT_FALSE(rsShadowDrawable->needSync_);
    rsShadowDrawable->needSync_ = true;
    rsShadowDrawable->OnSync();
    ASSERT_FALSE(rsShadowDrawable->needSync_);
    node.renderContent_->GetMutableRenderProperties().shadow_->SetElevation(0);
    std::shared_ptr<RSDrawable> drawableFour = DrawableV2::RSShadowDrawable::OnGenerate(node);
    ASSERT_NE(drawableFour, nullptr);
}

/**
 * @tc.name: RSMaskDrawable
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSMaskDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSMaskDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    std::shared_ptr<RSMask> mask =  std::make_shared<RSMask>();
    node.renderContent_->GetMutableRenderProperties().SetMask(mask);
    mask->type_ = MaskType::SVG;
    ASSERT_EQ(DrawableV2::RSMaskDrawable::OnGenerate(node), nullptr);
    mask->type_ = MaskType::GRADIENT;
    node.renderContent_->GetMutableRenderProperties().mask_.reset();
    node.renderContent_->GetMutableRenderProperties().SetMask(mask);
    ASSERT_NE(DrawableV2::RSMaskDrawable::OnGenerate(node), nullptr);
    mask->type_ = MaskType::PATH;
    node.renderContent_->GetMutableRenderProperties().mask_.reset();
    node.renderContent_->GetMutableRenderProperties().SetMask(mask);
    ASSERT_NE(DrawableV2::RSMaskDrawable::OnGenerate(node), nullptr);
}

/**
 * @tc.name: RSBackgroundColorDrawable
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundColorDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSBackgroundColorDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    node.renderContent_->GetMutableRenderProperties().SetBackgroundColor(Color(1, 1, 1, 1));
    ASSERT_NE(DrawableV2::RSBackgroundColorDrawable::OnGenerate(node), nullptr);
    std::optional<RSDynamicBrightnessPara> params = RSDynamicBrightnessPara();
    node.renderContent_->GetMutableRenderProperties().SetBgBrightnessParams(params);
    node.renderContent_->GetMutableRenderProperties().SetBgBrightnessFract(0.0f);
    ASSERT_NE(DrawableV2::RSBackgroundColorDrawable::OnGenerate(node), nullptr);
}

/**
 * @tc.name: RSBackgroundShaderDrawable
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundShaderDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSBackgroundShaderDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    std::shared_ptr<RSShader> shader = RSShader::CreateRSShader();
    node.renderContent_->GetMutableRenderProperties().SetBackgroundShader(shader);
    ASSERT_NE(DrawableV2::RSBackgroundShaderDrawable::OnGenerate(node), nullptr);
}

/**
 * @tc.name: RSBackgroundImageDrawable
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundImageDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSBackgroundImageDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    std::shared_ptr<RSImage> shader = std::make_shared<RSImage>();
    node.renderContent_->GetMutableRenderProperties().SetBgImage(shader);
    ASSERT_NE(DrawableV2::RSBackgroundImageDrawable::OnGenerate(node), nullptr);
}

/**
 * @tc.name: RSBackgroundFilterDrawable
 * @tc.desc: Test OnSync
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundFilterDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSBackgroundFilterDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    std::shared_ptr<RSFilter> backgroundFilter = std::make_shared<RSDrawingFilter>(std::make_shared<RSShaderFilter>());
    node.renderContent_->GetMutableRenderProperties().SetBackgroundFilter(backgroundFilter);
    ASSERT_NE(DrawableV2::RSBackgroundFilterDrawable::OnGenerate(node), nullptr);
    RSEffectRenderNode nodeTwo(id);
    nodeTwo.renderContent_->GetMutableRenderProperties().SetBackgroundFilter(backgroundFilter);
    ASSERT_TRUE(nodeTwo.IsInstanceOf<RSEffectRenderNode>());
    ASSERT_TRUE(nodeTwo.GetRenderProperties().GetBackgroundFilter());
    auto drawableTwo = std::static_pointer_cast<DrawableV2::RSBackgroundEffectDrawable>(
        DrawableV2::RSBackgroundFilterDrawable::OnGenerate(nodeTwo));
    ASSERT_NE(drawableTwo, nullptr);
    drawableTwo->stagingHasEffectChildren_ = true;
    drawableTwo->OnSync();
    ASSERT_FALSE(drawableTwo->stagingHasEffectChildren_);
    drawableTwo->OnSync();
    ASSERT_FALSE(drawableTwo->stagingHasEffectChildren_);
    ASSERT_TRUE(drawableTwo->CreateDrawFunc());
}

/**
 * @tc.name: RSUseEffectDrawable
 * @tc.desc: Test OnUpdate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSUseEffectDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSUseEffectDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    node.renderContent_->GetMutableRenderProperties().SetUseEffect(true);
    auto drawableTwo = std::static_pointer_cast<DrawableV2::RSUseEffectDrawable>(
        DrawableV2::RSUseEffectDrawable::OnGenerate(node));
    ASSERT_NE(drawableTwo, nullptr);
    ASSERT_TRUE(drawableTwo->OnUpdate(node));
    node.renderContent_->GetMutableRenderProperties().SetUseEffect(false);
    ASSERT_FALSE(drawableTwo->OnUpdate(node));
}

/**
 * @tc.name: RSDynamicLightUpDrawable
 * @tc.desc: Test OnSync
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSDynamicLightUpDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSDynamicLightUpDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    std::optional<float> rate = { 1.0f };
    node.renderContent_->GetMutableRenderProperties().SetDynamicLightUpRate(rate);
    std::optional<float> lightUpDegree = { 0.0f };
    node.renderContent_->GetMutableRenderProperties().SetDynamicLightUpDegree(lightUpDegree);
    ASSERT_TRUE(node.renderContent_->GetMutableRenderProperties().IsDynamicLightUpValid());
    auto drawableTwo = std::static_pointer_cast<DrawableV2::RSDynamicLightUpDrawable>(
        DrawableV2::RSDynamicLightUpDrawable::OnGenerate(node));
    ASSERT_NE(drawableTwo, nullptr);
    ASSERT_TRUE(drawableTwo->OnUpdate(node));
    node.renderContent_->GetMutableRenderProperties().SetDynamicLightUpRate(lightUpDegree);
    ASSERT_FALSE(drawableTwo->OnUpdate(node));
    drawableTwo->OnSync();
    ASSERT_FALSE(drawableTwo->needSync_);
    drawableTwo->OnSync();
    ASSERT_FALSE(drawableTwo->needSync_);
    ASSERT_TRUE(DrawableV2::RSDynamicLightUpDrawable::MakeDynamicLightUpBlender(1.0f, 1.0f, 1.0f));
    ASSERT_TRUE(DrawableV2::RSDynamicLightUpDrawable::MakeDynamicLightUpBlender(1.0f, 1.0f, 1.0f));
}
} // namespace OHOS::Rosen