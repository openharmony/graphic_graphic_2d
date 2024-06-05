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
#include "pipeline/rs_render_node.h"
#include "drawable/rs_render_node_drawable_adapter.h"

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

void RSChildrenDrawableTest::SetUpTestCase() {}
void RSChildrenDrawableTest::TearDownTestCase() {}
void RSChildrenDrawableTest::SetUp() {}
void RSChildrenDrawableTest::TearDown() {}

/**
 * @tc.name: RSChildrenDrawable
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSChildrenDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSRenderNode> child = std::make_shared<RSRenderNode>(2);
    ASSERT_EQ(DrawableV2::RSChildrenDrawable::OnGenerate(node), nullptr);

    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSRenderNode>(2);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSRenderNode>(2);
    outNode->isOnTheTree_ = true;
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode);
    sharedTransitionParam->relation_ = SharedTransitionParam::NodeHierarchyRelation::IN_NODE_BELOW_OUT_NODE;
    ASSERT_TRUE(sharedTransitionParam->UpdateHierarchyAndReturnIsLower(child->GetId()));
    child->SetSharedTransitionParam(sharedTransitionParam);

    ASSERT_TRUE(LIKELY(!node.GetRenderProperties().GetUseShadowBatching()));
    node.AddChild(child, -1);
    node.disappearingChildren_.emplace_back(child, 1);
    node.GenerateFullChildrenList();
    ASSERT_TRUE(node.fullChildrenList_->size());

    ASSERT_TRUE(UNLIKELY(child->GetSharedTransitionParam()));
    ASSERT_EQ(DrawableV2::RSChildrenDrawable::OnGenerate(node), nullptr);
    
    node.GetMutableRenderProperties().SetUseShadowBatching(true);
    ASSERT_FALSE(LIKELY(!node.GetRenderProperties().GetUseShadowBatching()));
    auto drawableTwo = std::static_pointer_cast<DrawableV2::RSChildrenDrawable>(
        DrawableV2::RSChildrenDrawable::OnGenerate(node));
    ASSERT_EQ(drawableTwo, nullptr);
}

/**
 * @tc.name: RSCustomModifierDrawable
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSCustomModifierDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    ASSERT_EQ(DrawableV2::RSCustomModifierDrawable::OnGenerate(node, RSModifierType::BOUNDS), nullptr);

    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    std::list<std::shared_ptr<RSRenderModifier>> list { std::make_shared<RSDrawCmdListRenderModifier>(property) };

    auto propertyTwo = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = std::make_shared<Drawing::DrawCmdList>(1, 1);
    list.emplace_back(std::make_shared<RSDrawCmdListRenderModifier>(propertyTwo));

    node.renderContent_->drawCmdModifiers_.emplace(RSModifierType::BOUNDS, list);
    auto drawable = std::static_pointer_cast<DrawableV2::RSCustomModifierDrawable>(
        DrawableV2::RSCustomModifierDrawable::OnGenerate(node, RSModifierType::BOUNDS));
    ASSERT_NE(drawable, nullptr);
    
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: RSBeginBlenderDrawable
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSBeginBlenderDrawable, TestSize.Level1)
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
 * @tc.name: RSEndBlenderDrawable
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSEndBlenderDrawable, TestSize.Level1)
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
 * @tc.name: RSEnvFGColorDrawable
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSEnvFGColorDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    ASSERT_EQ(DrawableV2::RSEnvFGColorDrawable::OnGenerate(node), nullptr);
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    std::list<std::shared_ptr<RSRenderModifier>> list { std::make_shared<RSDrawCmdListRenderModifier>(property) };
    node.renderContent_->drawCmdModifiers_.emplace(RSModifierType::ENV_FOREGROUND_COLOR, list);
    node.GetMutableRenderProperties().SetColorBlendApplyType(2);
    auto drawable = std::static_pointer_cast<DrawableV2::RSEnvFGColorDrawable>(
        DrawableV2::RSEnvFGColorDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: RSEnvFGColorStrategyDrawable
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSChildrenDrawableTest, RSEnvFGColorStrategyDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    ASSERT_EQ(DrawableV2::RSEnvFGColorStrategyDrawable::OnGenerate(node), nullptr);
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    std::list<std::shared_ptr<RSRenderModifier>> list { std::make_shared<RSDrawCmdListRenderModifier>(property) };
    node.renderContent_->drawCmdModifiers_.emplace(RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY, list);
    auto drawable = std::static_pointer_cast<DrawableV2::RSEnvFGColorStrategyDrawable>(
        DrawableV2::RSEnvFGColorStrategyDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
}
} // namespace OHOS::Rosen