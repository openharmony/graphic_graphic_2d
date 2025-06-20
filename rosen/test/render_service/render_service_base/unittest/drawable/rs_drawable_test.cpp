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
#include "pipeline/rs_effect_render_node.h"

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
    node.GetMutableRenderProperties().SetAiInvert(aiInvert);
    ASSERT_TRUE(node.GetRenderProperties().GetAiInvert());
    ASSERT_EQ(RSDrawable::CalculateDirtySlots(dirtyTypes, drawableVec).size(), 34);
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
    node.GetMutableRenderProperties().SetAiInvert(aiInvert);
    std::unordered_set<RSDrawableSlot> dirtySlots = RSDrawable::CalculateDirtySlots(dirtyTypes, drawableVec);
    std::shared_ptr<RSShader> shader = RSShader::CreateRSShader();
    node.GetMutableRenderProperties().SetBackgroundShader(shader);
    for (auto& drawable : drawableVec) {
        drawable = DrawableV2::RSBackgroundShaderDrawable::OnGenerate(node);
        ASSERT_TRUE(drawable);
    }
    ASSERT_FALSE(RSDrawable::UpdateDirtySlots(node, drawableVec, dirtySlots));
    node.GetMutableRenderProperties().SetBackgroundShader(nullptr);
    ASSERT_TRUE(RSDrawable::UpdateDirtySlots(node, drawableVec, dirtySlots));

    NodeId idTwo = 2;
    RSRenderNode nodeTwo(idTwo);
    RSDrawable::Vec drawableVecTwo;
    ModifierDirtyTypes dirtyTypesTwo;
    dirtyTypesTwo.set();
    std::unordered_set<RSDrawableSlot> dirtySlotsTwo = RSDrawable::CalculateDirtySlots(dirtyTypesTwo, drawableVecTwo);
    for (int8_t i = 0; i < static_cast<int8_t>(RSDrawableSlot::MAX); i++) {
        drawableVecTwo[i] = nullptr;
    }
    ASSERT_TRUE(RSDrawable::UpdateDirtySlots(nodeTwo, drawableVecTwo, dirtySlotsTwo));
    nodeTwo.GetMutableRenderProperties().SetBackgroundColor(Color(255, 255, 255, 255));
    ASSERT_TRUE(RSDrawable::UpdateDirtySlots(nodeTwo, drawableVecTwo, dirtySlotsTwo));
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

    node.GetMutableRenderProperties().SetBackgroundBlurRadius(10.f);  // 10.f: radius
    std::optional<Vector2f> greyPara = { Vector2f(1.f, 1.f) };  // 1.f: grey coef
    node.GetMutableRenderProperties().SetGreyCoef(greyPara);
    node.GetMutableRenderProperties().GenerateBackgroundMaterialBlurFilter();
    std::shared_ptr<RSDrawable> bgDrawable = DrawableV2::RSBackgroundFilterDrawable::OnGenerate(node);
    drawableVec[static_cast<size_t>(RSDrawableSlot::BACKGROUND_FILTER)] = bgDrawable;
    auto stretchDrawable = std::make_shared<DrawableV2::RSPixelStretchDrawable>();
    drawableVec[static_cast<size_t>(RSDrawableSlot::PIXEL_STRETCH)] = stretchDrawable;
    ASSERT_FALSE(RSDrawable::FuzeDrawableSlots(node, drawableVec));
    
    // -1.f: stretch param
    std::optional<Vector4f> pixelStretchPara = { Vector4f(-1.f, -1.f, -1.f, -1.f) };
    node.GetMutableRenderProperties().SetPixelStretch(pixelStretchPara);
    ASSERT_TRUE(RSDrawable::FuzeDrawableSlots(node, drawableVec));

    auto colorFilterDrawable = std::make_shared<DrawableV2::RSColorFilterDrawable>();
    drawableVec[static_cast<size_t>(RSDrawableSlot::COLOR_FILTER)] = colorFilterDrawable;
    ASSERT_FALSE(RSDrawable::FuzeDrawableSlots(node, drawableVec));
}

/**
 * @tc.name: UpdateSaveRestore001
 * @tc.desc: Test UpdateSaveRestore
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSDrawableTest, UpdateSaveRestore001, TestSize.Level1)
{
    NodeId id = 1;
    RSEffectRenderNode node(id); 
    RSDrawable::Vec drawableVec;
    uint8_t drawableVecStatus = 0;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    auto& properties = node.GetMutableRenderProperties();
    properties.clipToBounds_ = true;
    RectF rect = {1.0, 2.0, 3.0, 4.0};
    properties.clipRRect_ = std::make_optional<RRect>(rect, 1.0, 2.0);
    auto path = std::make_shared<RSPath>();
    properties.SetClipBounds(path);
    properties.colorBlendMode_ = 1;
    properties.fgBrightnessParams_ = std::make_optional<RSDynamicBrightnessPara>();
    properties.fgBrightnessParams_->fraction_ = 0;

    drawableVecStatus = 49;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_EQ(drawableVecStatus, 1);
    drawableVecStatus = 8;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_EQ(drawableVecStatus, 1);

    node.GetMutableRenderProperties().SetUseEffect(true);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSUseEffectDrawable::OnGenerate(node);
    drawableVec[static_cast<size_t>(RSDrawableSlot::CONTENT_BEGIN)] = drawable;
    drawableVec[static_cast<size_t>(RSDrawableSlot::BG_PROPERTIES_BEGIN)] = drawable;
    drawableVec[static_cast<size_t>(RSDrawableSlot::FG_PROPERTIES_BEGIN)] = drawable;
    drawableVec[static_cast<size_t>(RSDrawableSlot::TRANSITION_PROPERTIES_BEGIN)] = drawable;
    drawableVec[static_cast<size_t>(RSDrawableSlot::EXTRA_PROPERTIES_BEGIN)] = drawable;
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
    drawableVecStatus = 8;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_EQ(drawableVecStatus, 63);
}

/**
 * @tc.name: UpdateSaveRestore002
 * @tc.desc: Test UpdateSaveRestore
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSDrawableTest, UpdateSaveRestore002, TestSize.Level1)
{
    NodeId id = 1;
    RSEffectRenderNode node(id);
    RSDrawable::Vec drawableVec;
    node.GetMutableRenderProperties().SetUseEffect(true);
    uint8_t drawableVecStatus = 2;
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSUseEffectDrawable::OnGenerate(node);
    drawableVec[static_cast<size_t>(RSDrawableSlot::CONTENT_BEGIN)] = drawable;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_EQ(drawableVecStatus, 48);
    drawableVec[static_cast<size_t>(RSDrawableSlot::CONTENT_BEGIN)] = nullptr;
    drawableVec[static_cast<size_t>(RSDrawableSlot::BG_PROPERTIES_BEGIN)] = drawable;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_EQ(drawableVecStatus, 42);
    drawableVec[static_cast<size_t>(RSDrawableSlot::BG_PROPERTIES_BEGIN)] = nullptr;
    drawableVec[static_cast<size_t>(RSDrawableSlot::FG_PROPERTIES_BEGIN)] = drawable;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_EQ(drawableVecStatus, 36);
    drawableVec[static_cast<size_t>(RSDrawableSlot::FG_PROPERTIES_BEGIN)] = nullptr;
    drawableVec[static_cast<size_t>(RSDrawableSlot::TRANSITION_PROPERTIES_BEGIN)] = drawable;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_EQ(drawableVecStatus, 32);
    drawableVec[static_cast<size_t>(RSDrawableSlot::TRANSITION_PROPERTIES_BEGIN)] = nullptr;
    drawableVec[static_cast<size_t>(RSDrawableSlot::EXTRA_PROPERTIES_BEGIN)] = drawable;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_EQ(drawableVecStatus, 32);
    drawableVec[static_cast<size_t>(RSDrawableSlot::CONTENT_BEGIN)] = drawable;
    drawableVec[static_cast<size_t>(RSDrawableSlot::BG_PROPERTIES_BEGIN)] = drawable;
    drawableVec[static_cast<size_t>(RSDrawableSlot::FG_PROPERTIES_BEGIN)] = drawable;
    drawableVec[static_cast<size_t>(RSDrawableSlot::TRANSITION_PROPERTIES_BEGIN)] = drawable;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_EQ(drawableVecStatus, 62);
}

/**
 * @tc.name: ResetPixelStretchSlotTest
 * @tc.desc: Test
 * @tc.type:FUNC
 * @tc.require: wz
 */

HWTEST_F(RSDrawableTest, ResetPixelStretchSlotTest, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    RSDrawable::Vec drawableVec;
    RSDrawable::ResetPixelStretchSlot(node, drawableVec);
    std::shared_ptr<RSDrawable> drawable = std::shared_ptr<RSDrawable>(new DrawableV2::RSPixelStrectchDrawbale());
    ASSERT_NE(drawable, nullptr);
    drawableVec[static_cast<size_t>(RSDrawableSlot::PIXEL_STRETCH)] = drawable;
    RSDrawable::ResetPixelStretchSlot(node, drawableVec);
}

/**
 * @tc.name: CanFusePixelStretchTest
 * @tc.desc: Test
 * @tc.type:FUNC
 * @tc.require: wz
 */

HWTEST_F(RSDrawableTest, CanFusePixelStretchTest, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    RSDrawable::Vec drawableVec;
    ASSERT_FALSE(RSDrawable::CanFusePixelStretch(drawableVec));

    std::shared_ptr<RSDrawable> drawable = std::shared_ptr<RSDrawable>(new DrawableV2::RSBackgroundFilterDrawbale());
    ASSERT_NE(drawable, nullptr);
    drawableVec[static_cast<size_t>(RSDrawableSlot::PIXEL_STRETCH)] = drawable;
    ASSERT_FALSE(RSDrawable::CanFusePixelStretch(drawableVec));

    std::shared_ptr<RSDrawable> drawable2 = std::shared_ptr<RSDrawable>(new DrawableV2::RSBackgroundFilterDrawbale());
    ASSERT_NE(drawable2, nullptr);
    drawableVec[static_cast<size_t>(RSDrawableSlot::PIXEL_STRETCH)] = drawable2;
    ASSERT_FALSE(RSDrawable::CanFusePixelStretch(drawableVec));
}

/**
 * @tc.name: CanFusePixelStretchTest002
 * @tc.desc: Test
 * @tc.type:FUNC
 * @tc.require: wz
 */

HWTEST_F(RSDrawableTest, CanFusePixelStretchTest002, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    RSDrawable::Vec drawableVec;

    std::shared_ptr<RSDrawable> drawable = std::shared_ptr<RSDrawable>(new DrawableV2::RSBackgroundFilterDrawbale());
    ASSERT_NE(drawable, nullptr);
    drawableVec[static_cast<size_t>(RSDrawableSlot::PIXEL_STRETCH)] = drawable;

    std::shared_ptr<RSDrawable> drawable2 = std::shared_ptr<RSDrawable>(new DrawableV2::RSBackgroundFilterDrawbale());
    ASSERT_NE(drawable2, nullptr);
    drawableVec[static_cast<size_t>(RSDrawableSlot::PIXEL_STRETCH)] = drawable2;

    std::shared_ptr<RSDrawable> drawable3 = std::shared_ptr<RSDrawable>(new DrawableV2::RSBackgroundFilterDrawbale());
    ASSERT_NE(drawable3, nullptr);
    drawableVec[static_cast<size_t>(RSDrawableSlot::PIXEL_STRETCH)] = drawable3;
    ASSERT_FALSE(RSDrawable::CanFusePixelStretch(drawableVec));
}
} // namespace OHOS::Rosen