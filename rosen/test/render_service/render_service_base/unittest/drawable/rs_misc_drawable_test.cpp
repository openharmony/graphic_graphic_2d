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

#include "drawable/rs_misc_drawable.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_render_node.h"
#include "skia_adapter/skia_surface.h"
#include "skia_adapter/skia_canvas.h"
#include "modifier_ng/foreground/rs_env_foreground_color_render_modifier.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSChildrenDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

class ConcreteRSRenderNodeDrawableAdapter : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit ConcreteRSRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node))
    {}
    void Draw(Drawing::Canvas& canvas) {}
};

void RSChildrenDrawableTest::SetUpTestCase() {}
void RSChildrenDrawableTest::TearDownTestCase() {}
void RSChildrenDrawableTest::SetUp() {}
void RSChildrenDrawableTest::TearDown() {}

/**
 * @tc.name: RSChildrenDrawable001
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSChildrenDrawable001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSRenderNode> childOne = std::make_shared<RSRenderNode>(2);
    std::shared_ptr<RSRenderNode> childTwo = std::make_shared<RSRenderNode>(3);
    std::shared_ptr<RSRenderNode> childThree = std::make_shared<RSRenderNode>(4);
    std::shared_ptr<RSRenderNode> childFour = std::make_shared<RSRenderNode>(5);
    std::shared_ptr<RSRenderNode> childFive = std::make_shared<RSRenderNode>(6);
    ASSERT_EQ(DrawableV2::RSChildrenDrawable::OnGenerate(node), nullptr);

    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSRenderNode>(2);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSRenderNode>(2);
    outNode->isOnTheTree_ = true;
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    sharedTransitionParam->relation_ = SharedTransitionParam::NodeHierarchyRelation::IN_NODE_BELOW_OUT_NODE;

    ASSERT_TRUE(sharedTransitionParam->IsLower(childOne->GetId()));
    childOne->SetSharedTransitionParam(sharedTransitionParam);
    ASSERT_TRUE(LIKELY(!node.GetRenderProperties().GetUseShadowBatching()));
    node.AddChild(childOne, -1);
    node.AddChild(childTwo, -1);
    node.AddChild(childThree, -1);
    node.AddChild(childFour, -1);
    node.AddChild(childFive, -1);
    node.disappearingChildren_.emplace_back(childOne, 1);
    node.disappearingChildren_.emplace_back(childTwo, 2);
    node.disappearingChildren_.emplace_back(childThree, 3);
    node.disappearingChildren_.emplace_back(childFour, 4);
    node.disappearingChildren_.emplace_back(childFive, 5);
    node.GenerateFullChildrenList();
    ASSERT_TRUE(node.fullChildrenList_->size());

    ASSERT_TRUE(UNLIKELY(childOne->GetSharedTransitionParam()));
    ASSERT_FALSE(UNLIKELY(childTwo->GetSharedTransitionParam()));
    ASSERT_FALSE(UNLIKELY(childThree->GetSharedTransitionParam()));
    ASSERT_FALSE(UNLIKELY(childFour->GetSharedTransitionParam()));
    ASSERT_FALSE(UNLIKELY(childFive->GetSharedTransitionParam()));
    ASSERT_EQ(DrawableV2::RSChildrenDrawable::OnGenerate(node), nullptr);

    node.GetMutableRenderProperties().SetUseShadowBatching(true);
    ASSERT_FALSE(LIKELY(!node.GetRenderProperties().GetUseShadowBatching()));
    auto nodeTwo = std::make_shared<RSRenderNode>(0);
    auto nodeDrawable = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(nodeTwo);
    childThree->renderDrawable_ = nodeDrawable;

    auto nodeThree = std::make_shared<RSRenderNode>(10);
    auto nodeDrawableThree = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(nodeThree);
    childFour->renderDrawable_ = nodeDrawableThree;

    auto nodeFour = std::make_shared<RSRenderNode>(11);
    auto nodeDrawableFour = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(nodeFour);
    childFive->renderDrawable_ = nodeDrawableFour;
    ASSERT_NE(DrawableV2::RSChildrenDrawable::OnGenerate(node), nullptr);

    node.GetMutableRenderProperties().SetUseShadowBatching(false);
    auto drawable =
        std::static_pointer_cast<DrawableV2::RSChildrenDrawable>(DrawableV2::RSChildrenDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    ASSERT_TRUE(drawable->needSync_);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);

    auto canvas = std::make_shared<Drawing::Canvas>();
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(canvas.get(), rect.get());
    EXPECT_NE(canvas, nullptr);
}

/**
 * @tc.name: RSChildrenDrawable002
 * @tc.desc: Test OnSharedTransition
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSChildrenDrawable002, TestSize.Level1)
{
    NodeId id = 1;
    auto node = std::make_shared<RSRenderNode>(id);
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSRenderNode>(1);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    node->sharedTransitionParam_ = sharedTransitionParam;
    DrawableV2::RSChildrenDrawable childrenDrawable;
    ASSERT_FALSE(childrenDrawable.OnSharedTransition(node));
    inNode->isOnTheTree_ = true;
    outNode->isOnTheTree_ = true;
    ASSERT_FALSE(childrenDrawable.OnSharedTransition(node));
    sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    node->sharedTransitionParam_ = sharedTransitionParam;
    ASSERT_TRUE(childrenDrawable.OnSharedTransition(node));
    inNode->isOnTheTree_ = false;
    inNode->SetIsOnTheTree(true, 1);
    auto nodeTwo = std::make_shared<RSRenderNode>(0);
    auto nodeDrawable = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(nodeTwo);
    outNode->renderDrawable_ = nodeDrawable;
    sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    node->sharedTransitionParam_ = sharedTransitionParam;
    ASSERT_TRUE(childrenDrawable.OnSharedTransition(node));
}

/**
 * @tc.name: RSCustomModifierDrawable002
 * @tc.desc: Test OnDraw for RSCustomModifierDrawable
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSCustomModifierDrawable002, TestSize.Level1)
{
    NodeId id = 1;
    RSCanvasDrawingRenderNode node(id);
    ASSERT_EQ(
        DrawableV2::RSCustomModifierDrawable::OnGenerate(node, ModifierNG::RSModifierType::CONTENT_STYLE), nullptr);

    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    auto modifier1 = std::make_shared<ModifierNG::RSCustomRenderModifier<ModifierNG::RSModifierType::CONTENT_STYLE>>();
    ASSERT_NE(modifier1, nullptr);
    modifier1->properties_[ModifierNG::RSPropertyType::CONTENT_STYLE] = property;
    auto indexProperty = std::make_shared<RSRenderProperty<int16_t>>(2, 0);
    modifier1->properties_[ModifierNG::RSPropertyType::CUSTOM_INDEX] = indexProperty;
    node.AddModifier(modifier1);

    auto propertyTwo = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    propertyTwo->GetRef() = std::make_shared<Drawing::DrawCmdList>(1, 1);
    auto modifier2 = std::make_shared<ModifierNG::RSCustomRenderModifier<ModifierNG::RSModifierType::CONTENT_STYLE>>();
    ASSERT_NE(modifier2, nullptr);
    modifier2->properties_[ModifierNG::RSPropertyType::CONTENT_STYLE] = propertyTwo;
    auto indexProperty2 = std::make_shared<RSRenderProperty<int16_t>>(1, 1);
    modifier2->properties_[ModifierNG::RSPropertyType::CUSTOM_INDEX] = indexProperty2;
    node.AddModifier(modifier2);

    node.AddDirtyType(ModifierNG::RSModifierType::CONTENT_STYLE);
    auto drawable = std::static_pointer_cast<DrawableV2::RSCustomModifierDrawable>(
        DrawableV2::RSCustomModifierDrawable::OnGenerate(node, ModifierNG::RSModifierType::CONTENT_STYLE));
    ASSERT_NE(drawable, nullptr);

    auto canvas = std::make_shared<Drawing::Canvas>();
    auto rect = std::make_shared<Drawing::Rect>();

    drawable->OnSync();
    drawable->needClearOp_ = true;
    drawable->modifierTypeNG_ = ModifierNG::RSModifierType::INVALID;
    drawable->OnDraw(canvas.get(), rect.get());

    drawable->needClearOp_ = false;
    drawable->modifierTypeNG_ = ModifierNG::RSModifierType::INVALID;
    drawable->OnDraw(canvas.get(), rect.get());

    drawable->needClearOp_ = true;
    drawable->modifierTypeNG_ = ModifierNG::RSModifierType::CONTENT_STYLE;
    drawable->OnDraw(canvas.get(), rect.get());

    drawable->needClearOp_ = false;
    drawable->modifierTypeNG_ = ModifierNG::RSModifierType::CONTENT_STYLE;
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_NE(drawable, nullptr);

    node.GetMutableRenderProperties().SetFrameGravity(Gravity::CENTER);
    auto customModifiers = node.GetModifiersNG(drawable->modifierTypeNG_);
    EXPECT_EQ(customModifiers.empty(), false);
    drawable->OnUpdate(node);
    drawable->OnSync();
    EXPECT_EQ(drawable->isCanvasNode_, false);
    EXPECT_EQ(drawable->gravity_, Gravity::CENTER);
    drawable->needClearOp_ = true;
    drawable->modifierTypeNG_ = ModifierNG::RSModifierType::CONTENT_STYLE;
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: RSCustomModifierDrawable003
 * @tc.desc: Test OnDraw for RSCustomModifierDrawable on isCanvasNode_ = true branch;
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSCustomModifierDrawable003, TestSize.Level1)
{
    NodeId id = 1;
    RSCanvasDrawingRenderNode node(id);
    ASSERT_EQ(
        DrawableV2::RSCustomModifierDrawable::OnGenerate(node, ModifierNG::RSModifierType::CONTENT_STYLE), nullptr);

    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    auto modifier1 = std::make_shared<ModifierNG::RSCustomRenderModifier<ModifierNG::RSModifierType::CONTENT_STYLE>>();
    ASSERT_NE(modifier1, nullptr);
    modifier1->properties_[ModifierNG::RSPropertyType::CONTENT_STYLE] = property;
    auto indexProperty = std::make_shared<RSRenderProperty<int16_t>>(2, 0);
    modifier1->properties_[ModifierNG::RSPropertyType::CUSTOM_INDEX] = indexProperty;
    node.AddModifier(modifier1);

    auto propertyTwo = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    propertyTwo->GetRef() = std::make_shared<Drawing::DrawCmdList>(1, 1);
    auto modifier2 = std::make_shared<ModifierNG::RSCustomRenderModifier<ModifierNG::RSModifierType::CONTENT_STYLE>>();
    ASSERT_NE(modifier2, nullptr);
    modifier2->properties_[ModifierNG::RSPropertyType::CONTENT_STYLE] = propertyTwo;
    auto indexProperty2 = std::make_shared<RSRenderProperty<int16_t>>(1, 1);
    modifier2->properties_[ModifierNG::RSPropertyType::CUSTOM_INDEX] = indexProperty2;
    node.AddModifier(modifier2);

    node.AddDirtyType(ModifierNG::RSModifierType::CONTENT_STYLE);
    auto drawable = std::static_pointer_cast<DrawableV2::RSCustomModifierDrawable>(
        DrawableV2::RSCustomModifierDrawable::OnGenerate(node, ModifierNG::RSModifierType::CONTENT_STYLE));
    ASSERT_NE(drawable, nullptr);

    drawable->isCanvasNode_ = true;
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto rect = std::make_shared<Drawing::Rect>();

    drawable->needClearOp_ = true;
    drawable->modifierTypeNG_ = ModifierNG::RSModifierType::CONTENT_STYLE;
    drawable->OnDraw(canvas.get(), rect.get());

    drawable->needClearOp_ = false;
    drawable->modifierTypeNG_ = ModifierNG::RSModifierType::CONTENT_STYLE;
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_NE(drawable, nullptr);

    node.GetMutableRenderProperties().SetFrameGravity(Gravity::CENTER);
    auto customModifiers = node.GetModifiersNG(drawable->modifierTypeNG_);
    EXPECT_EQ(customModifiers.empty(), false);
    drawable->OnUpdate(node);
    drawable->OnSync();
    EXPECT_EQ(drawable->isCanvasNode_, false);
    EXPECT_EQ(drawable->gravity_, Gravity::CENTER);
    drawable->needClearOp_ = true;
    drawable->modifierTypeNG_ = ModifierNG::RSModifierType::CONTENT_STYLE;
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: RSBeginBlenderDrawable001
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSBeginBlenderDrawable001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    ASSERT_EQ(DrawableV2::RSBeginBlenderDrawable::OnGenerate(node), nullptr);

    std::optional<RSDynamicBrightnessPara> params = { RSDynamicBrightnessPara() };
    node.GetMutableRenderProperties().SetFgBrightnessParams(params);
    node.GetMutableRenderProperties().SetFgBrightnessFract(0.0f);
    ASSERT_NE(DrawableV2::RSBeginBlenderDrawable::OnGenerate(node), nullptr);
    
    std::optional<RSDynamicBrightnessPara> paramsTwo = std::nullopt;
    node.GetMutableRenderProperties().SetFgBrightnessParams(paramsTwo);
    node.GetMutableRenderProperties().SetColorBlendMode(12);
    auto drawable = std::static_pointer_cast<DrawableV2::RSBeginBlenderDrawable>(
        DrawableV2::RSBeginBlenderDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);

    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: RSBeginBlenderDrawable002
 * @tc.desc: Test OnGenerate for shadow blender
 * @tc.type:FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSChildrenDrawableTest, RSBeginBlenderDrawable002, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    ASSERT_EQ(DrawableV2::RSBeginBlenderDrawable::OnGenerate(node), nullptr);

    auto params = std::optional<RSShadowBlenderPara>({0, 0, 0, 0});
    node.GetMutableRenderProperties().SetShadowBlenderParams(params);
    auto drawable = std::static_pointer_cast<DrawableV2::RSBeginBlenderDrawable>(
        DrawableV2::RSBeginBlenderDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);

    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: RSBeginBlenderDrawable003
 * @tc.desc: Test OnDraw for RSBeginBlenderDrawable
 * @tc.type:FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSChildrenDrawableTest, RSBeginBlenderDrawable003, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetColorBlendMode(12);
    auto drawable = std::static_pointer_cast<DrawableV2::RSBeginBlenderDrawable>(
        DrawableV2::RSBeginBlenderDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    drawable->OnSync();

    auto canvas = std::make_shared<Drawing::Canvas>();
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(canvas.get(), rect.get());
    EXPECT_NE(canvas, nullptr);

    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    drawable->OnDraw(filterCanvas.get(), rect.get());
    EXPECT_NE(filterCanvas->envStack_.top().blender_, nullptr);
}

/**
 * @tc.name: RSEndBlenderDrawable001
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSEndBlenderDrawable001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    ASSERT_EQ(DrawableV2::RSEndBlenderDrawable::OnGenerate(node), nullptr);
    node.GetMutableRenderProperties().SetColorBlendMode(12);
    ASSERT_EQ(DrawableV2::RSEndBlenderDrawable::OnGenerate(node), nullptr);
    node.GetMutableRenderProperties().SetColorBlendApplyType(2);
    auto drawable = std::static_pointer_cast<DrawableV2::RSEndBlenderDrawable>(
        DrawableV2::RSEndBlenderDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: RSEndBlenderDrawable002
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSEndBlenderDrawable002, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSEndBlenderDrawable>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSEnvFGColorDrawable001
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSEnvFGColorDrawable001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    ASSERT_EQ(DrawableV2::RSEnvFGColorDrawable::OnGenerate(node), nullptr);
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    auto modifier = std::make_shared<ModifierNG::RSEnvForegroundColorRenderModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::ENV_FOREGROUND_COLOR, property);
    node.AddModifier(modifier);
    node.GetMutableRenderProperties().SetColorBlendApplyType(2);
    auto drawable =
        std::static_pointer_cast<DrawableV2::RSEnvFGColorDrawable>(DrawableV2::RSEnvFGColorDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: RSEnvFGColorDrawable002
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSEnvFGColorDrawable002, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSEnvFGColorDrawable>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSEnvFGColorStrategyDrawable001
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSEnvFGColorStrategyDrawable001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    ASSERT_EQ(DrawableV2::RSEnvFGColorStrategyDrawable::OnGenerate(node), nullptr);
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    auto modifier = std::make_shared<ModifierNG::RSEnvForegroundColorRenderModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::ENV_FOREGROUND_COLOR_STRATEGY, property);
    node.AddModifier(modifier);
    auto drawable = std::static_pointer_cast<DrawableV2::RSEnvFGColorStrategyDrawable>(
        DrawableV2::RSEnvFGColorStrategyDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: RSEnvFGColorStrategyDrawable002
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSEnvFGColorStrategyDrawable002, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSEnvFGColorStrategyDrawable>();
    auto surface = std::make_shared<Drawing::Surface>();
    surface->impl_ = std::make_shared<Drawing::SkiaSurface>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    canvas->impl_ = std::make_shared<Drawing::SkiaCanvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    filterCanvas->surface_ = surface.get();
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 10, 10);
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
    drawable->envFGColorStrategy_ = ForegroundColorStrategyType::INVERT_BACKGROUNDCOLOR;
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSCustomClipToFrameDrawable002
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSCustomClipToFrameDrawable002, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSCustomClipToFrameDrawable>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSColorPickerDrawable
 * @tc.desc: Test Func of RSColorPickerDrawable
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    ASSERT_EQ(DrawableV2::RSColorPickerDrawable::OnGenerate(node), nullptr);

    node.GetMutableRenderProperties().SetColorPickerPlaceholder(static_cast<int>(ColorPlaceholder::TEXT_CONTRAST));
    node.GetMutableRenderProperties().SetColorPickerStrategy(static_cast<int>(ColorPickStrategyType::AVERAGE));
    node.GetMutableRenderProperties().SetColorPickerInterval(1000);
    auto drawable = std::static_pointer_cast<DrawableV2::RSColorPickerDrawable>(
        DrawableV2::RSColorPickerDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);

    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);

    drawable->OnDraw(nullptr, rect.get());
    ASSERT_TRUE(true);

    drawable->colorPickerManager_ = nullptr;
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSColorPickerDrawable002
 * @tc.desc: Test RSColorPickerDrawable with CLIENT_CALLBACK strategy (useAlt = true)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable002, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    // Set CLIENT_CALLBACK strategy to trigger useAlt = true branch
    node.GetMutableRenderProperties().SetColorPickerPlaceholder(static_cast<int>(ColorPlaceholder::SURFACE));
    node.GetMutableRenderProperties().SetColorPickerStrategy(static_cast<int>(ColorPickStrategyType::CLIENT_CALLBACK));
    node.GetMutableRenderProperties().SetColorPickerInterval(500);

    auto drawable = std::static_pointer_cast<DrawableV2::RSColorPickerDrawable>(
        DrawableV2::RSColorPickerDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    ASSERT_NE(drawable->colorPickerManager_, nullptr);

    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);

    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSColorPickerDrawable003
 * @tc.desc: Test OnSync when needSync_ is false (early return branch)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable003, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    node.GetMutableRenderProperties().SetColorPickerPlaceholder(static_cast<int>(ColorPlaceholder::ACCENT));
    node.GetMutableRenderProperties().SetColorPickerStrategy(static_cast<int>(ColorPickStrategyType::DOMINANT));
    node.GetMutableRenderProperties().SetColorPickerInterval(2000);

    auto drawable = std::static_pointer_cast<DrawableV2::RSColorPickerDrawable>(
        DrawableV2::RSColorPickerDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);

    // First sync should work
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);

    // Second sync should hit early return branch (needSync_ is false)
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: RSColorPickerDrawable004
 * @tc.desc: Test OnSync when stagingColorPicker_ is nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable004, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    node.GetMutableRenderProperties().SetColorPickerPlaceholder(static_cast<int>(ColorPlaceholder::FOREGROUND));
    node.GetMutableRenderProperties().SetColorPickerStrategy(static_cast<int>(ColorPickStrategyType::CONTRAST));
    node.GetMutableRenderProperties().SetColorPickerInterval(1500);

    auto drawable = std::static_pointer_cast<DrawableV2::RSColorPickerDrawable>(
        DrawableV2::RSColorPickerDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);

    // Clear stagingColorPicker_ to test nullptr branch
    drawable->stagingColorPicker_ = nullptr;
    drawable->needSync_ = true;

    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    // params_ should be default constructed when stagingColorPicker_ is nullptr
    EXPECT_EQ(drawable->params_.placeholder, ColorPlaceholder::NONE);
    EXPECT_EQ(drawable->params_.strategy, ColorPickStrategyType::NONE);
    EXPECT_EQ(drawable->params_.interval, 0);
}

/**
 * @tc.name: RSColorPickerDrawable005
 * @tc.desc: Test OnUpdate returns false when colorPicker is nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable005, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    // Don't set any color picker properties, so GetColorPicker() returns nullptr
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // OnUpdate should return false when stagingColorPicker_ is nullptr
    bool result = drawable->OnUpdate(node);
    EXPECT_FALSE(result);
    EXPECT_TRUE(drawable->needSync_);
}

/**
 * @tc.name: RSColorPickerDrawable006
 * @tc.desc: Test different ColorPlaceholder and ColorPickStrategyType combinations
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable006, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    // Test SURFACE_CONTRAST placeholder with CONTRAST strategy
    node.GetMutableRenderProperties().SetColorPickerPlaceholder(static_cast<int>(ColorPlaceholder::SURFACE_CONTRAST));
    node.GetMutableRenderProperties().SetColorPickerStrategy(static_cast<int>(ColorPickStrategyType::CONTRAST));
    node.GetMutableRenderProperties().SetColorPickerInterval(3000);

    auto drawable = std::static_pointer_cast<DrawableV2::RSColorPickerDrawable>(
        DrawableV2::RSColorPickerDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);

    drawable->OnSync();
    EXPECT_EQ(drawable->params_.placeholder, ColorPlaceholder::SURFACE_CONTRAST);
    EXPECT_EQ(drawable->params_.strategy, ColorPickStrategyType::CONTRAST);
    EXPECT_EQ(drawable->params_.interval, 3000);

    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSSaveDrawable001
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSSaveDrawable001, TestSize.Level1)
{
    auto count = std::make_shared<uint32_t>(std::numeric_limits<uint32_t>::max());
    auto drawable = std::make_shared<DrawableV2::RSSaveDrawable>(count);
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSRestoreDrawable001
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSRestoreDrawable001, TestSize.Level1)
{
    auto count = std::make_shared<uint32_t>(std::numeric_limits<uint32_t>::max());
    auto drawable = std::make_shared<DrawableV2::RSRestoreDrawable>(count);
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSCustomRestoreDrawable001
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSCustomRestoreDrawable001, TestSize.Level1)
{
    auto status = std::make_shared<RSPaintFilterCanvas::SaveStatus>();
    auto drawable = std::make_shared<DrawableV2::RSCustomRestoreDrawable>(status);
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSColorPickerDrawable007
 * @tc.desc: Test Prepare with nullptr stagingColorPicker
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable007, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Clear stagingColorPicker_ to test nullptr branch
    drawable->stagingColorPicker_ = nullptr;

    // Prepare should return early and not set needColorPick
    uint64_t vsyncTime = 1000000000; // 1 second in nanoseconds
    auto [needColorPick, needSync] = drawable->PrepareForExecution(vsyncTime, false);

    EXPECT_FALSE(needColorPick);
    EXPECT_FALSE(needSync);
}

/**
 * @tc.name: RSColorPickerDrawable008
 * @tc.desc: Test Prepare with NONE strategy
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable008, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Create a ColorPickerParam with NONE strategy
    auto params = std::make_shared<ColorPickerParam>();
    params->strategy = ColorPickStrategyType::NONE;
    params->placeholder = ColorPlaceholder::NONE;
    params->interval = 0;
    drawable->stagingColorPicker_ = params;

    // Prepare should return early and not set needColorPick
    uint64_t vsyncTime = 1000000000;
    auto [needColorPick, needSync] = drawable->PrepareForExecution(vsyncTime, false);

    EXPECT_FALSE(needColorPick);
    EXPECT_FALSE(needSync);
}

/**
 * @tc.name: RSColorPickerDrawable009
 * @tc.desc: Test Prepare when interval not elapsed
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable009, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Create a ColorPickerParam with 2000ms interval
    auto params = std::make_shared<ColorPickerParam>();
    params->strategy = ColorPickStrategyType::AVERAGE;
    params->placeholder = ColorPlaceholder::TEXT_CONTRAST;
    params->interval = 2000;
    drawable->stagingColorPicker_ = params;
    drawable->lastUpdateTime_ = 1000; // Last update at 1000ms

    // vsyncTime is 1500ms (1000000000 ns = 1000ms), interval not elapsed
    uint64_t vsyncTime = 1500000000;
    auto [needColorPick, needSync] = drawable->PrepareForExecution(vsyncTime, false);

    EXPECT_FALSE(needColorPick);
    EXPECT_FALSE(needSync);
}

/**
 * @tc.name: RSColorPickerDrawable010
 * @tc.desc: Test Prepare when interval elapsed
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable010, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Create a ColorPickerParam with 1000ms interval
    auto params = std::make_shared<ColorPickerParam>();
    params->strategy = ColorPickStrategyType::AVERAGE;
    params->placeholder = ColorPlaceholder::TEXT_CONTRAST;
    params->interval = 1000;
    drawable->stagingColorPicker_ = params;
    drawable->lastUpdateTime_ = 1000; // Last update at 1000ms

    // vsyncTime is 2500ms (2500000000 ns), interval has elapsed (2500 >= 1000 + 1000)
    uint64_t vsyncTime = 2500000000;
    auto [needColorPick, needSync] = drawable->PrepareForExecution(vsyncTime, false);

    EXPECT_TRUE(needColorPick);
    EXPECT_TRUE(needSync); // stagingNeedColorPick_ changed from false to true
    EXPECT_EQ(drawable->lastUpdateTime_, 2500u); // lastUpdateTime_ should be updated
}

/**
 * @tc.name: RSColorPickerDrawable011
 * @tc.desc: Test Prepare with task scheduling
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable011, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Create a ColorPickerParam with 1000ms interval
    auto params = std::make_shared<ColorPickerParam>();
    params->strategy = ColorPickStrategyType::DOMINANT;
    params->placeholder = ColorPlaceholder::FOREGROUND;
    params->interval = 1000;
    drawable->stagingColorPicker_ = params;
    drawable->lastUpdateTime_ = 1000;
    drawable->isTaskScheduled_ = false;

    // vsyncTime is 1500ms, interval not elapsed (1500 < 1000 + 1000) so task should be scheduled
    uint64_t vsyncTime = 1500000000;
    auto [needColorPick, needSync] = drawable->PrepareForExecution(vsyncTime, false);

    // Task should be scheduled (isTaskScheduled_ set to true)
    // Note: We can't easily verify the actual task was posted without mocking
    EXPECT_FALSE(needColorPick);
    EXPECT_FALSE(needSync); // stagingNeedColorPick_ stays false, darkMode unchanged
    EXPECT_TRUE(drawable->isTaskScheduled_);
}

/**
 * @tc.name: RSColorPickerDrawable012
 * @tc.desc: Test OnDraw with needColorPick logic and OnSync
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable012, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    node.GetMutableRenderProperties().SetColorPickerPlaceholder(
        static_cast<int>(ColorPlaceholder::TEXT_CONTRAST));
    node.GetMutableRenderProperties().SetColorPickerStrategy(
        static_cast<int>(ColorPickStrategyType::AVERAGE));
    node.GetMutableRenderProperties().SetColorPickerInterval(1000);

    auto drawable = std::static_pointer_cast<DrawableV2::RSColorPickerDrawable>(
        DrawableV2::RSColorPickerDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);

    // Test OnSync syncs needColorPick_ from stagingNeedColorPick_
    drawable->stagingNeedColorPick_ = true;
    drawable->OnSync();
    EXPECT_TRUE(drawable->needColorPick_);

    drawable->stagingNeedColorPick_ = false;
    drawable->OnSync();
    EXPECT_FALSE(drawable->needColorPick_);

    // Test with different needColorPick values
    drawable->needColorPick_ = true;
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();

    // OnDraw should execute color picking when needColorPick is true
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);

    // Set needColorPick to false and test again
    drawable->needColorPick_ = false;
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);

    // Test with null paintFilterCanvas
    drawable->OnDraw(nullptr, rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSColorPickerDrawable013
 * @tc.desc: Test SetIsSystemDarkColorMode sets needSync_ when value changes
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable013, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Initial state: stagingIsSystemDarkColorMode_ = false, needSync_ = false
    EXPECT_FALSE(drawable->stagingIsSystemDarkColorMode_);
    EXPECT_FALSE(drawable->needSync_);

    // Set to true when current value is false
    drawable->SetIsSystemDarkColorMode(true);

    // Verify: stagingIsSystemDarkColorMode_ should be true, needSync_ should be true
    EXPECT_TRUE(drawable->stagingIsSystemDarkColorMode_);
    EXPECT_TRUE(drawable->needSync_);
}

/**
 * @tc.name: RSColorPickerDrawable014
 * @tc.desc: Test SetIsSystemDarkColorMode does not set needSync_ when value unchanged (false -> false)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable014, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Initial state: stagingIsSystemDarkColorMode_ = false
    EXPECT_FALSE(drawable->stagingIsSystemDarkColorMode_);

    // Set needSync_ to false explicitly
    drawable->needSync_ = false;

    // Set to same value (false)
    drawable->SetIsSystemDarkColorMode(false);

    // Verify: stagingIsSystemDarkColorMode_ should still be false, needSync_ should still be false
    EXPECT_FALSE(drawable->stagingIsSystemDarkColorMode_);
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: RSColorPickerDrawable015
 * @tc.desc: Test OnSync calls SetSystemDarkColorMode on colorPickerManager_
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable015, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    node.GetMutableRenderProperties().SetColorPickerPlaceholder(
        static_cast<int>(ColorPlaceholder::TEXT_CONTRAST));
    node.GetMutableRenderProperties().SetColorPickerStrategy(
        static_cast<int>(ColorPickStrategyType::AVERAGE));
    node.GetMutableRenderProperties().SetColorPickerInterval(1000);

    auto drawable = std::static_pointer_cast<DrawableV2::RSColorPickerDrawable>(
        DrawableV2::RSColorPickerDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    ASSERT_NE(drawable->colorPickerManager_, nullptr);

    // Set dark mode via SetIsSystemDarkColorMode
    drawable->SetIsSystemDarkColorMode(true);
    EXPECT_TRUE(drawable->stagingIsSystemDarkColorMode_);
    EXPECT_TRUE(drawable->needSync_);

    // Call OnSync to sync the value to colorPickerManager_
    drawable->OnSync();

    // After sync, needSync_ should be false
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: RSColorPickerDrawable016
 * @tc.desc: Test OnSync handles null colorPickerManager_ gracefully
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSChildrenDrawableTest, RSColorPickerDrawable016, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Set needSync_ to true
    drawable->SetIsSystemDarkColorMode(true);
    EXPECT_TRUE(drawable->needSync_);

    // Set colorPickerManager_ to nullptr
    drawable->colorPickerManager_ = nullptr;

    // OnSync should handle null manager gracefully (no crash)
    drawable->OnSync();

    // Verify needSync_ is cleared even with null manager
    EXPECT_FALSE(drawable->needSync_);
}
} // namespace OHOS::Rosen
