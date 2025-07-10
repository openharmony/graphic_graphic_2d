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
#include "parameters.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_render_node.h"
#include "skia_adapter/skia_surface.h"
#include "skia_adapter/skia_canvas.h"
#if defined(MODIFIER_NG)
#include "modifier_ng/foreground/rs_env_foreground_color_render_modifier.h"
#endif

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
    node.disappearingChildren_.emplace_back(childOne, 1);
    node.disappearingChildren_.emplace_back(childTwo, 2);
    node.disappearingChildren_.emplace_back(childThree, 3);
    node.GenerateFullChildrenList();
    ASSERT_TRUE(node.fullChildrenList_->size());

    ASSERT_TRUE(UNLIKELY(childOne->GetSharedTransitionParam()));
    ASSERT_FALSE(UNLIKELY(childTwo->GetSharedTransitionParam()));
    ASSERT_FALSE(UNLIKELY(childThree->GetSharedTransitionParam()));
    ASSERT_EQ(DrawableV2::RSChildrenDrawable::OnGenerate(node), nullptr);

    node.GetMutableRenderProperties().SetUseShadowBatching(true);
    ASSERT_FALSE(LIKELY(!node.GetRenderProperties().GetUseShadowBatching()));
    auto nodeTwo = std::make_shared<RSRenderNode>(0);
    auto nodeDrawable = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(nodeTwo);
    childThree->renderDrawable_ = nodeDrawable;
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

#ifndef MODIFIER_NG
/**
 * @tc.name: RSCustomModifierDrawable
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSCustomModifierDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSCanvasDrawingRenderNode node(id);
    ASSERT_EQ(DrawableV2::RSCustomModifierDrawable::OnGenerate(node, RSModifierType::BOUNDS), nullptr);

    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    std::list<std::shared_ptr<RSRenderModifier>> list { std::make_shared<RSDrawCmdListRenderModifier>(property) };

    auto propertyTwo = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    propertyTwo->GetRef() = std::make_shared<Drawing::DrawCmdList>(1, 1);
    list.emplace_back(std::make_shared<RSDrawCmdListRenderModifier>(propertyTwo));

    node.drawCmdModifiers_.emplace(RSModifierType::BOUNDS, list);
    node.AddDirtyType(RSModifierType::BOUNDS);
    auto drawable = std::static_pointer_cast<DrawableV2::RSCustomModifierDrawable>(
        DrawableV2::RSCustomModifierDrawable::OnGenerate(node, RSModifierType::BOUNDS));
    ASSERT_NE(drawable, nullptr);

    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);

    NodeId idTwo = 2;
    RSRenderNode nodeTwo(idTwo);
    nodeTwo.drawCmdModifiers_.emplace(RSModifierType::BOUNDS, list);
    drawable = std::static_pointer_cast<DrawableV2::RSCustomModifierDrawable>(
        DrawableV2::RSCustomModifierDrawable::OnGenerate(nodeTwo, RSModifierType::BOUNDS));
    ASSERT_NE(drawable, nullptr);
}
#else
/**
 * @tc.name: RSCustomModifierDrawable002
 * @tc.desc: Test OnGenerate
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSCustomModifierDrawable002, TestSize.Level1)
{
    NodeId id = 1;
    RSCanvasDrawingRenderNode node(id);
    ASSERT_EQ(DrawableV2::RSCustomModifierDrawable::OnGenerate(node, RSModifierType::CONTENT_STYLE), nullptr);

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
        DrawableV2::RSCustomModifierDrawable::OnGenerate(node, RSModifierType::CONTENT_STYLE));
    ASSERT_NE(drawable, nullptr);

    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);

    NodeId idTwo = 2;
    RSRenderNode nodeTwo(idTwo);
    auto drawCmdList2 = std::make_shared<Drawing::DrawCmdList>(100, 100, Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmdList2->AddDrawOp(std::make_shared<Drawing::FlushOpItem>());
    auto property2 = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property2->GetRef() = drawCmdList2;
    auto modifier3 = std::make_shared<ModifierNG::RSCustomRenderModifier<ModifierNG::RSModifierType::CONTENT_STYLE>>();
    modifier3->AttachProperty(ModifierNG::RSPropertyType::CONTENT_STYLE, property2);
    nodeTwo.AddModifier(modifier3);
    auto drawable2 = std::static_pointer_cast<DrawableV2::RSCustomModifierDrawable>(
        DrawableV2::RSCustomModifierDrawable::OnGenerate(nodeTwo, RSModifierType::CONTENT_STYLE));
    ASSERT_NE(drawable2, nullptr);
}
#endif

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

    system::SetParameter("persist.sys.graphic.openDebugTrace", "2");
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
 * @tc.desc: Test CreateDrawFunc
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSEndBlenderDrawable002, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSEndBlenderDrawable>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(filterCanvas.get(), rect.get());
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
#if defined(MODIFIER_NG)
    auto modifier = std::make_shared<ModifierNG::RSEnvForegroundColorRenderModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::ENV_FOREGROUND_COLOR, property);
    node.AddModifier(modifier);
#else
    std::list<std::shared_ptr<RSRenderModifier>> list { std::make_shared<RSDrawCmdListRenderModifier>(property) };
    node.drawCmdModifiers_.emplace(RSModifierType::ENV_FOREGROUND_COLOR, list);
#endif
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
 * @tc.desc: Test CreateDrawFunc
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSEnvFGColorDrawable002, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSEnvFGColorDrawable>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(filterCanvas.get(), rect.get());
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
#if defined(MODIFIER_NG)
    auto modifier = std::make_shared<ModifierNG::RSEnvForegroundColorRenderModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::ENV_FOREGROUND_COLOR_STRATEGY, property);
    node.AddModifier(modifier);
#else
    std::list<std::shared_ptr<RSRenderModifier>> list { std::make_shared<RSDrawCmdListRenderModifier>(property) };
    node.drawCmdModifiers_.emplace(RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY, list);
#endif
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
 * @tc.desc: Test CreateDrawFunc
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
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
    drawable->envFGColorStrategy_ = ForegroundColorStrategyType::INVERT_BACKGROUNDCOLOR;
    drawFunc(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

#ifndef MODIFIER_NG
/**
 * @tc.name: RSCustomClipToFrameDrawable001
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueIASGKZ
 */
HWTEST_F(RSChildrenDrawableTest, RSCustomClipToFrameDrawable001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    ASSERT_EQ(DrawableV2::RSCustomClipToFrameDrawable::OnGenerate(node), nullptr);
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    std::list<std::shared_ptr<RSRenderModifier>> list { std::make_shared<RSDrawCmdListRenderModifier>(property) };
    node.drawCmdModifiers_.emplace(RSModifierType::CUSTOM_CLIP_TO_FRAME, list);
    auto drawable = std::static_pointer_cast<DrawableV2::RSCustomClipToFrameDrawable>(
        DrawableV2::RSCustomClipToFrameDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
}
#endif

/**
 * @tc.name: RSCustomClipToFrameDrawable002
 * @tc.desc: Test CreateDrawFunc
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSCustomClipToFrameDrawable002, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSCustomClipToFrameDrawable>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}
} // namespace OHOS::Rosen
