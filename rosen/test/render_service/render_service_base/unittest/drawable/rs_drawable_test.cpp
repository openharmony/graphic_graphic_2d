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

#include "drawable/rs_drawable.h"
#include "drawable/rs_misc_drawable.h"
#include "drawable/rs_property_drawable.h"
#include "drawable/rs_property_drawable_background.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
    static inline std::weak_ptr<RSContext> context = {};
};

void RSDrawableTest::SetUpTestCase() {}
void RSDrawableTest::TearDownTestCase() {}
void RSDrawableTest::SetUp() {}
void RSDrawableTest::TearDown() {}

/**
 * @tc.name: CalculateDirtySlots
 * @tc.desc: Test CalculateDirtySlots
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSDrawableTest, CalculateDirtySlots, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    ModifierDirtyTypes dirtyTypes;
    RSDrawable::Vec drawableVec;
    dirtyTypes.set();
    std::optional<Vector4f> aiInvert = { Vector4f() };
    node.renderContent_->GetMutableRenderProperties().SetAiInvert(aiInvert);
    ASSERT_TRUE(node.GetRenderProperties().GetAiInvert());
    ASSERT_EQ(RSDrawable::CalculateDirtySlots(dirtyTypes, drawableVec).size(), 33);
}

/**
 * @tc.name: UpdateDirtySlots
 * @tc.desc: Test UpdateDirtySlots
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSDrawableTest, UpdateDirtySlots, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    RSDrawable::Vec drawableVec;
    ModifierDirtyTypes dirtyTypes;
    dirtyTypes.set();
    std::optional<Vector4f> aiInvert = { Vector4f() };
    node.renderContent_->GetMutableRenderProperties().SetAiInvert(aiInvert);
    std::unordered_set<RSDrawableSlot> dirtySlots = RSDrawable::CalculateDirtySlots(dirtyTypes, drawableVec);
    std::shared_ptr<RSShader> shader = RSShader::CreateRSShader();
    node.renderContent_->GetMutableRenderProperties().SetBackgroundShader(shader);
    for (auto& drawable : drawableVec) {
        drawable = DrawableV2::RSBackgroundShaderDrawable::OnGenerate(node);
        ASSERT_TRUE(drawable);
    }
    ASSERT_FALSE(RSDrawable::UpdateDirtySlots(node, drawableVec, dirtySlots));
}

/**
 * @tc.name: FuzeDrawableSlots
 * @tc.desc: Test FuzeDrawableSlots
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawableTest, FuzeDrawableSlots, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    RSDrawable::Vec drawableVec;
    ASSERT_FALSE(RSDrawable::FuzeDrawableSlots(node, drawableVec));

    node.renderContent_->GetMutableRenderProperties().SetBackgroundBlurRadius(10.f);  // 10.f: radius
    std::optional<Vector2f> greyPara = { Vector2f(1.f, 1.f) };  // 1.f: grey coef
    node.renderContent_->GetMutableRenderProperties().SetGreyCoef(greyPara);
    node.renderContent_->GetMutableRenderProperties().GenerateBackgroundMaterialBlurFilter();
    std::shared_ptr<RSDrawable> bgDrawable = DrawableV2::RSBackgroundFilterDrawable::OnGenerate(node);
    drawableVec[static_cast<size_t>(RSDrawableSlot::BACKGROUND_FILTER)] = bgDrawable;
    auto stretchDrawable = std::make_shared<DrawableV2::RSPixelStretchDrawable>();
    drawableVec[static_cast<size_t>(RSDrawableSlot::PIXEL_STRETCH)] = stretchDrawable;
    ASSERT_FALSE(RSDrawable::FuzeDrawableSlots(node, drawableVec));
    
    // -1.f: stretch param
    std::optional<Vector4f> pixelStretchPara = { Vector4f(-1.f, -1.f, -1.f, -1.f) };
    node.renderContent_->GetMutableRenderProperties().SetPixelStretch(pixelStretchPara);
    ASSERT_TRUE(RSDrawable::FuzeDrawableSlots(node, drawableVec));

    auto colorFilterDrawable = std::make_shared<DrawableV2::RSColorFilterDrawable>();
    drawableVec[static_cast<size_t>(RSDrawableSlot::COLOR_FILTER)] = colorFilterDrawable;
    ASSERT_FALSE(RSDrawable::FuzeDrawableSlots(node, drawableVec));
}

/**
 * @tc.name: UpdateSaveRestore
 * @tc.desc: Test UpdateSaveRestore
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSDrawableTest, UpdateSaveRestore, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceRenderNode node(id, context);
    RSDrawable::Vec drawableVec;
    auto& properties = node.GetMutableRenderProperties();
    properties.clipToBounds_ = true;
    RectF rect = {1.0, 2.0, 3.0, 4.0};
    properties.clipRRect_ = std::make_optional<RRect>(rect, 1.0, 2.0);
    auto path = std::make_shared<RSPath>();
    properties.SetClipBounds(path);
    properties.colorBlendMode_ = 1;
    properties.fgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    properties.fgBrightnessParams_->fraction_ = 0;
    node.renderContent_->GetMutableRenderProperties().SetUseEffect(true);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSUseEffectDrawable::OnGenerate(node);
    drawableVec[static_cast<size_t>(RSDrawableSlot::CONTENT_BEGIN)] = drawable;
    drawableVec[static_cast<size_t>(RSDrawableSlot::BG_PROPERTIES_BEGIN)] = drawable;
    drawableVec[static_cast<size_t>(RSDrawableSlot::FG_PROPERTIES_BEGIN)] = drawable;
    drawableVec[static_cast<size_t>(RSDrawableSlot::TRANSITION_PROPERTIES_BEGIN)] = drawable;
    drawableVec[static_cast<size_t>(RSDrawableSlot::EXTRA_PROPERTIES_BEGIN)] = drawable;
    uint8_t drawableVecStatus = 0;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_EQ(drawableVecStatus, 63);
    drawableVecStatus = 53;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_EQ(drawableVecStatus, 63);
    drawableVecStatus = 51;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_EQ(drawableVecStatus, 63);
    drawableVecStatus = 49;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_EQ(drawableVecStatus, 63);
}
} // namespace OHOS::Rosen