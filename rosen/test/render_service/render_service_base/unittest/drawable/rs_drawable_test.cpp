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

#include "common/rs_common_tools.h"
#include "drawable/rs_drawable.h"
#include "drawable/rs_misc_drawable.h"
#include "drawable/rs_property_drawable.h"
#include "drawable/rs_property_drawable_background.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_effect_render_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::TemplateUtils;
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
    RSDrawable::Vec drawableVec;
    std::optional<Vector4f> aiInvert = { Vector4f() };
    node.GetMutableRenderProperties().SetAiInvert(aiInvert);
    ASSERT_TRUE(node.GetRenderProperties().GetAiInvert());
    ModifierNG::ModifierDirtyTypes dirtyTypesNG;
    dirtyTypesNG.set(static_cast<int>(ModifierNG::RSModifierType::SHADOW), true);
    auto dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypesNG, drawableVec);
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::CHILDREN) != dirtySlots.end());
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
    ModifierNG::ModifierDirtyTypes dirtyTypes;
    dirtyTypes.set();
    std::optional<Vector4f> aiInvert = { Vector4f() };
    node.GetMutableRenderProperties().SetAiInvert(aiInvert);
    std::unordered_set<RSDrawableSlot> dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypes, drawableVec);
    std::shared_ptr<RSShader> shader = RSShader::CreateRSShader();
    node.GetMutableRenderProperties().SetBackgroundShader(shader);
    for (auto i = static_cast<int8_t>(RSDrawableSlot::SAVE_ALL); i < static_cast<int8_t>(RSDrawableSlot::MAX); ++i) {
        drawableVec[i] = nullptr;
    }
    for (auto& [slot, drawable] : drawableVec) {
        drawable = DrawableV2::RSBackgroundShaderDrawable::OnGenerate(node);
        ASSERT_TRUE(drawable);
    }
    ASSERT_FALSE(RSDrawable::UpdateDirtySlots(node, drawableVec, dirtySlots));
    node.GetMutableRenderProperties().SetBackgroundShader(nullptr);
    ASSERT_TRUE(RSDrawable::UpdateDirtySlots(node, drawableVec, dirtySlots));

    NodeId idTwo = 2;
    RSRenderNode nodeTwo(idTwo);
    RSDrawable::Vec drawableVecTwo;
    ModifierNG::ModifierDirtyTypes dirtyTypesTwo;
    dirtyTypesTwo.set();
    std::unordered_set<RSDrawableSlot> dirtySlotsTwo = RSDrawable::CalculateDirtySlotsNG(dirtyTypesTwo, drawableVecTwo);
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
    node.GetMutableRenderProperties().GetEffectProperties().GenerateBackgroundMaterialBlurFilter();
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
    properties.clipRRect_ = std::make_unique<RRect>(rect, 1.0, 2.0);
    auto path = std::make_shared<RSPath>();
    properties.SetClipBounds(path);
    properties.GetEffectProperties().GetBlenderEffect().colorBlendMode_ = 1;
    properties.GetEffectProperties().GetBlenderEffect().fgBrightnessParams_ =
        std::make_optional<RSDynamicBrightnessPara>();
    properties.GetEffectProperties().GetBlenderEffect().fgBrightnessParams_->fraction_ = 0;

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
    std::shared_ptr<RSDrawable> drawable = std::make_shared<DrawableV2::RSPixelStretchDrawable>();
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

    std::shared_ptr<RSDrawable> drawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    drawableVec[static_cast<size_t>(RSDrawableSlot::BACKGROUND_FILTER)] = drawable;
    ASSERT_FALSE(RSDrawable::CanFusePixelStretch(drawableVec));

    std::shared_ptr<RSDrawable> drawable2 = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    ASSERT_NE(drawable2, nullptr);
    drawableVec[static_cast<size_t>(RSDrawableSlot::PIXEL_STRETCH)] = drawable2;
    ASSERT_TRUE(RSDrawable::CanFusePixelStretch(drawableVec));
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

    std::shared_ptr<RSDrawable> drawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    drawableVec[static_cast<size_t>(RSDrawableSlot::BACKGROUND_FILTER)] = drawable;

    std::shared_ptr<RSDrawable> drawable2 = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    ASSERT_NE(drawable2, nullptr);
    drawableVec[static_cast<size_t>(RSDrawableSlot::PIXEL_STRETCH)] = drawable2;

    std::shared_ptr<RSDrawable> drawable3 = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    ASSERT_NE(drawable3, nullptr);
    drawableVec[static_cast<size_t>(RSDrawableSlot::BACKGROUND_STYLE)] = drawable3;
    ASSERT_FALSE(RSDrawable::CanFusePixelStretch(drawableVec));
}

class BufferDirtyTestNode : public RSRenderNode {
public:
    explicit BufferDirtyTestNode(NodeId id) : RSRenderNode(id) {}
    bool IsBufferDirty() const override
    {
        return bufferDirty_;
    }
    bool bufferDirty_ = false;
};

/**
 * @tc.name: CalculateDrawableVecStatus_BufferDirty
 * @tc.desc: Test CalculateDrawableVecStatus sets FRAME_NOT_EMPTY when IsBufferDirty is true
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawableTest, CalculateDrawableVecStatus_BufferDirty, TestSize.Level1)
{
    NodeId id = 1;
    BufferDirtyTestNode node(id);
    RSDrawable::Vec drawableVec;
    uint8_t drawableVecStatus = 0;
    node.bufferDirty_ = true;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_TRUE(drawableVecStatus & DrawableVecStatus::FRAME_NOT_EMPTY);
    ASSERT_TRUE(drawableVecStatus & DrawableVecStatus::NODE_NOT_EMPTY);
}

/**
 * @tc.name: CalculateDrawableVecStatus_BufferNotDirty
 * @tc.desc: Test CalculateDrawableVecStatus does not set FRAME_NOT_EMPTY when no content and buffer not dirty
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawableTest, CalculateDrawableVecStatus_BufferNotDirty, TestSize.Level1)
{
    NodeId id = 1;
    BufferDirtyTestNode node(id);
    RSDrawable::Vec drawableVec;
    uint8_t drawableVecStatus = 0;
    node.bufferDirty_ = false;
    RSDrawable::UpdateSaveRestore(node, drawableVec, drawableVecStatus);
    ASSERT_FALSE(drawableVecStatus & DrawableVecStatus::FRAME_NOT_EMPTY);
}
/**
 * @tc.name: CalculateDirtySlotsNG_Bounds
 * @tc.desc: Test CalculateDirtySlotsNG with BOUNDS dirty type
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawableTest, CalculateDirtySlotsNG_Bounds, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    RSDrawable::Vec drawableVec;
    drawableVec[static_cast<int8_t>(RSDrawableSlot::MASK)] = std::make_shared<DrawableV2::RSSaveDrawable>(
        std::make_shared<uint32_t>(0));
    ModifierNG::ModifierDirtyTypes dirtyTypes;
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::BOUNDS));
    auto dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypes, drawableVec);
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::CLIP_TO_BOUNDS) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::MASK) != dirtySlots.end());
}

/**
 * @tc.name: CalculateDirtySlotsNG_ClipToBounds
 * @tc.desc: Test CalculateDirtySlotsNG with CLIP_TO_BOUNDS dirty type
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawableTest, CalculateDirtySlotsNG_ClipToBounds, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    RSDrawable::Vec drawableVec;
    drawableVec[static_cast<int8_t>(RSDrawableSlot::BORDER)] = std::make_shared<DrawableV2::RSSaveDrawable>(
        std::make_shared<uint32_t>(0));
    ModifierNG::ModifierDirtyTypes dirtyTypes;
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::CLIP_TO_BOUNDS));
    auto dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypes, drawableVec);
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::CLIP_TO_BOUNDS) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::BORDER) != dirtySlots.end());
}

/**
 * @tc.name: CalculateDirtySlotsNG_Frame
 * @tc.desc: Test CalculateDirtySlotsNG with FRAME dirty type
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawableTest, CalculateDirtySlotsNG_Frame, TestSize.Level1)
{
    RSDrawable::Vec drawableVec;
    ModifierNG::ModifierDirtyTypes dirtyTypes;
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::FRAME));
    auto dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypes, drawableVec);
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::FRAME_OFFSET) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::CONTENT_STYLE) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::FOREGROUND_STYLE) != dirtySlots.end());
}

/**
 * @tc.name: CalculateDirtySlotsNG_CompositingFilter
 * @tc.desc: Test CalculateDirtySlotsNG with COMPOSITING_FILTER dirty type
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawableTest, CalculateDirtySlotsNG_CompositingFilter, TestSize.Level1)
{
    RSDrawable::Vec drawableVec;
    ModifierNG::ModifierDirtyTypes dirtyTypes;
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::COMPOSITING_FILTER));
    auto dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypes, drawableVec);
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::LIGHT_UP_EFFECT) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::BINARIZATION) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::DYNAMIC_DIM) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::COLOR_FILTER) != dirtySlots.end());
}

/**
 * @tc.name: CalculateDirtySlotsNG_EnvForegroundColor
 * @tc.desc: Test CalculateDirtySlotsNG with ENV_FOREGROUND_COLOR dirty type
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawableTest, CalculateDirtySlotsNG_EnvForegroundColor, TestSize.Level1)
{
    RSDrawable::Vec drawableVec;
    ModifierNG::ModifierDirtyTypes dirtyTypes;
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::ENV_FOREGROUND_COLOR));
    auto dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypes, drawableVec);
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::ENV_FOREGROUND_COLOR) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::ENV_FOREGROUND_COLOR_STRATEGY) != dirtySlots.end());
}

/**
 * @tc.name: CalculateDirtySlotsNG_ClipToFrame
 * @tc.desc: Test CalculateDirtySlotsNG with CLIP_TO_FRAME dirty type
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawableTest, CalculateDirtySlotsNG_ClipToFrame, TestSize.Level1)
{
    RSDrawable::Vec drawableVec;
    ModifierNG::ModifierDirtyTypes dirtyTypes;
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::CLIP_TO_FRAME));
    auto dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypes, drawableVec);
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::CLIP_TO_FRAME) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::CUSTOM_CLIP_TO_FRAME) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::FRAME_OFFSET) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::CONTENT_STYLE) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::FOREGROUND_STYLE) != dirtySlots.end());
}

/**
 * @tc.name: CalculateDirtySlotsNG_SpatialEffect
 * @tc.desc: Test CalculateDirtySlotsNG with SPATIAL_EFFECT dirty type
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawableTest, CalculateDirtySlotsNG_SpatialEffect, TestSize.Level1)
{
    RSDrawable::Vec drawableVec;
    ModifierNG::ModifierDirtyTypes dirtyTypes;
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::SPATIAL_EFFECT));
    auto dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypes, drawableVec);
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::SPATIAL_EFFECT) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::MATERIAL_SHADER) != dirtySlots.end());
}

/**
 * @tc.name: CalculateDirtySlotsNG_BorderExpansion
 * @tc.desc: Test CalculateDirtySlotsNG expands BORDER to border dirty types
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawableTest, CalculateDirtySlotsNG_BorderExpansion, TestSize.Level1)
{
    RSDrawable::Vec drawableVec;
    drawableVec[static_cast<int8_t>(RSDrawableSlot::BACKGROUND_COLOR)] =
        std::make_shared<DrawableV2::RSSaveDrawable>(std::make_shared<uint32_t>(0));
    ModifierNG::ModifierDirtyTypes dirtyTypes;
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::BORDER));
    auto dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypes, drawableVec);
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::BORDER) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::BACKGROUND_COLOR) != dirtySlots.end());
}

/**
 * @tc.name: CalculateDirtySlotsNG_ForegroundFilterExpansion
 * @tc.desc: Test CalculateDirtySlotsNG expands FOREGROUND_FILTER to RESTORE_FOREGROUND_FILTER
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawableTest, CalculateDirtySlotsNG_ForegroundFilterExpansion, TestSize.Level1)
{
    RSDrawable::Vec drawableVec;
    ModifierNG::ModifierDirtyTypes dirtyTypes;
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::FOREGROUND_FILTER));
    auto dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypes, drawableVec);
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::FOREGROUND_FILTER) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::RESTORE_FOREGROUND_FILTER) != dirtySlots.end());
}

/**
 * @tc.name: CalculateDirtySlotsNG_PixelStretchExpansion
 * @tc.desc: Test CalculateDirtySlotsNG expands PIXEL_STRETCH to stretch dirty types
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawableTest, CalculateDirtySlotsNG_PixelStretchExpansion, TestSize.Level1)
{
    RSDrawable::Vec drawableVec;
    drawableVec[static_cast<int8_t>(RSDrawableSlot::BACKGROUND_FILTER)] =
        std::make_shared<DrawableV2::RSSaveDrawable>(std::make_shared<uint32_t>(0));
    ModifierNG::ModifierDirtyTypes dirtyTypes;
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::PIXEL_STRETCH));
    auto dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypes, drawableVec);
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::PIXEL_STRETCH) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::BACKGROUND_FILTER) != dirtySlots.end());
}

/**
 * @tc.name: CalculateDirtySlotsNG_BackgroundFilterExpansion
 * @tc.desc: Test CalculateDirtySlotsNG expands BACKGROUND_FILTER to bgfilter dirty types
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawableTest, CalculateDirtySlotsNG_BackgroundFilterExpansion, TestSize.Level1)
{
    RSDrawable::Vec drawableVec;
    drawableVec[static_cast<int8_t>(RSDrawableSlot::PIXEL_STRETCH)] =
        std::make_shared<DrawableV2::RSSaveDrawable>(std::make_shared<uint32_t>(0));
    ModifierNG::ModifierDirtyTypes dirtyTypes;
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::BACKGROUND_FILTER));
    auto dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypes, drawableVec);
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::BACKGROUND_FILTER) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::PIXEL_STRETCH) != dirtySlots.end());
}

/**
 * @tc.name: UpdateDirtySlots_CHILDREN_EraseTest
 * @tc.desc: Test UpdateDirtySlots erases CHILDREN slot when it is null
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawableTest, UpdateDirtySlots_CHILDREN_EraseTest, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    RSDrawable::Vec drawableVec;
    std::unordered_set<RSDrawableSlot> dirtySlots;
    dirtySlots.insert(RSDrawableSlot::CHILDREN);
    drawableVec[static_cast<int8_t>(RSDrawableSlot::CHILDREN)] = nullptr;
    ASSERT_TRUE(RSDrawable::UpdateDirtySlots(node, drawableVec, dirtySlots));
    ASSERT_FALSE(drawableVec.find(static_cast<int8_t>(RSDrawableSlot::CHILDREN)) != drawableVec.end());
}

/**
 * @tc.name: CalculateDirtySlotsNG_MultipleDirtyTypes
 * @tc.desc: Test CalculateDirtySlotsNG with multiple dirty types set
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawableTest, CalculateDirtySlotsNG_MultipleDirtyTypes, TestSize.Level1)
{
    RSDrawable::Vec drawableVec;
    ModifierNG::ModifierDirtyTypes dirtyTypes;
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::BOUNDS));
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::FRAME));
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::SHADOW));
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::COMPOSITING_FILTER));
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::ENV_FOREGROUND_COLOR));
    dirtyTypes.set(static_cast<size_t>(ModifierNG::RSModifierType::SPATIAL_EFFECT));
    auto dirtySlots = RSDrawable::CalculateDirtySlotsNG(dirtyTypes, drawableVec);
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::CLIP_TO_BOUNDS) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::FRAME_OFFSET) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::SHADOW) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::COLOR_FILTER) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::ENV_FOREGROUND_COLOR) != dirtySlots.end());
    ASSERT_TRUE(dirtySlots.find(RSDrawableSlot::MATERIAL_SHADER) != dirtySlots.end());
}

} // namespace OHOS::Rosen