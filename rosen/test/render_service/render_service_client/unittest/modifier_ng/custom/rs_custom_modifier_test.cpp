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

#include <cstdlib>
#include <cstring>
#include <memory>
#include <securec.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "message_parcel.h"

#include "common/rs_vector4.h"
#include "modifier_ng/custom/rs_content_style_modifier.h"
#include "modifier_ng/custom/rs_custom_modifier.h"
#include "ui/rs_display_node.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
namespace {
class RSNodeMock : public RSCanvasNode {
public:
    explicit RSNodeMock() : RSCanvasNode(false, false, nullptr) {}
    ~RSNodeMock() = default;
    bool NeedForcedSendToRemote() const override
    {
        return true;
    }
};
} // namespace
class RSCustomModifierHelperTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

class TestContentStyleModifier : public ModifierNG::RSContentStyleModifier {
public:
    TestContentStyleModifier() = default;
    ~TestContentStyleModifier() = default;

    void Draw(ModifierNG::RSDrawingContext& context) const override
    {
        float left = 0;
        float top = 0;
        float right = 100;
        float bottom = 100;
        Drawing::Rect rect(left, top, right, bottom);
        Drawing::Brush brush;
        brush.SetColor(SK_ColorBLUE);
        context.canvas->AttachBrush(brush);
        context.canvas->DrawRect(rect);
        context.canvas->DetachBrush();
    }
};

/**
 * @tc.name: CreateDrawingContextTest
 * @tc.desc: Test the function CreateDrawingContext
 * @tc.type: FUNC
 */
HWTEST_F(RSCustomModifierHelperTest, CreateDrawingContextTest, TestSize.Level1)
{
    std::shared_ptr<RSNode> node = nullptr;
    ModifierNG::RSDrawingContext context = RSCustomModifierHelper::CreateDrawingContext(node);
    EXPECT_EQ(context.canvas, nullptr);

    RSDisplayNodeConfig config;
    std::shared_ptr<RSDisplayNode> node2 = RSDisplayNode::Create(config);
    RSCustomModifierHelper::CreateDrawingContext(node2);
    EXPECT_FALSE(node2->IsInstanceOf<RSCanvasNode>());

    std::shared_ptr<RSNode> node3 = RSCanvasNode::Create();
    ModifierNG::RSDrawingContext context3 = RSCustomModifierHelper::CreateDrawingContext(node3);
    EXPECT_NE(context3.canvas, nullptr);
}

/**
 * @tc.name: FinishDrawingTest
 * @tc.desc: Test the function FinishDrawing
 * @tc.type: FUNC
 */
HWTEST_F(RSCustomModifierHelperTest, FinishDrawingTest, TestSize.Level1)
{
    float width = 100.0f;
    float height = 100.0f;
    ModifierNG::RSDrawingContext context1 = { nullptr, width, height };
    auto drawCmdList1 = RSCustomModifierHelper::FinishDrawing(context1);
    EXPECT_EQ(drawCmdList1, nullptr);

    auto canvas2 = new ExtendRecordingCanvas(width, height);
    ModifierNG::RSDrawingContext context2 = { canvas2, width, height };
    RSSystemProperties::SetDrawTextAsBitmap(true);
    auto drawCmdList2 = RSCustomModifierHelper::FinishDrawing(context2);
    EXPECT_NE(drawCmdList2, nullptr);

    auto canvas3 = new ExtendRecordingCanvas(width, height);
    ModifierNG::RSDrawingContext context3 = { canvas3, width, height };
    RSSystemProperties::SetDrawTextAsBitmap(false);
    auto drawCmdList3 = RSCustomModifierHelper::FinishDrawing(context3);
    EXPECT_NE(drawCmdList3, nullptr);

    auto canvas4 = new ExtendRecordingCanvas(width, height);
    ModifierNG::RSDrawingContext context4 = { canvas4, width, height };
    canvas4->cmdList_ = nullptr;
    auto drawCmdList4 = RSCustomModifierHelper::FinishDrawing(context4);
    EXPECT_EQ(drawCmdList4, nullptr);
}

/**
 * @tc.name: UpdateDrawCmdListTest
 * @tc.desc: Test the function UpdateDrawCmdList
 * @tc.type: FUNC
 */
HWTEST_F(RSCustomModifierHelperTest, UpdateDrawCmdListTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSContentStyleModifier>();
    modifier->UpdateDrawCmdList();
    EXPECT_EQ(modifier->node_.lock(), nullptr);
    auto node = std::make_shared<RSCanvasNode>(true);
    modifier->OnAttach(*node);
    modifier->UpdateDrawCmdList();
    EXPECT_NE(modifier->node_.lock(), nullptr);
    modifier->UpdateDrawCmdList();
    auto property = std::static_pointer_cast<RSProperty<Drawing::DrawCmdListPtr>>(
        modifier->GetProperty(ModifierNG::RSPropertyType::CONTENT_STYLE));
    EXPECT_NE(modifier->node_.lock(), nullptr);
    EXPECT_NE(property, nullptr);
    EXPECT_NE(property->Get(), nullptr);
    property->isCustom_ = true;
    modifier->UpdateDrawCmdList();
    property = std::static_pointer_cast<RSProperty<Drawing::DrawCmdListPtr>>(
        modifier->GetProperty(ModifierNG::RSPropertyType::CONTENT_STYLE));
    EXPECT_NE(modifier->node_.lock(), nullptr);
    EXPECT_NE(property, nullptr);
    EXPECT_NE(property->Get(), nullptr);
}

/**
 * @tc.name: UpdatePropertyTest001
 * @tc.desc: Test the function UpdateProperty
 * @tc.type: FUNC
 */
HWTEST_F(RSCustomModifierHelperTest, UpdatePropertyTest001, TestSize.Level1)
{
    auto rsCustomModifier = std::make_shared<ModifierNG::RSContentStyleModifier>();
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(1, 1);
    RSAnimationTimingProtocol timingProtocol(1);

    // case1: OPACITY and INTERPOLATING
    auto timingCurve = RSAnimationTimingCurve::LINEAR;
    rsCustomModifier->SetContentTransitionParam(ContentTransitionType::OPACITY, timingProtocol, timingCurve);
    rsCustomModifier->UpdateProperty(node, drawCmdList, 0);
    ASSERT_EQ(rsCustomModifier->contentTransitionType_, ContentTransitionType::OPACITY);
    ASSERT_EQ(rsCustomModifier->timingCurve_.type_, RSAnimationTimingCurve::CurveType::INTERPOLATING);

    // case2: not OPACITY and INTERPOLATING
    rsCustomModifier->SetContentTransitionParam(ContentTransitionType::IDENTITY, timingProtocol, timingCurve);
    rsCustomModifier->UpdateProperty(node, drawCmdList, 0);
    ASSERT_NE(rsCustomModifier->contentTransitionType_, ContentTransitionType::OPACITY);
    ASSERT_EQ(rsCustomModifier->timingCurve_.type_, RSAnimationTimingCurve::CurveType::INTERPOLATING);

    // case3: OPACITY and not INTERPOLATING
    RSAnimationTimingCurve springTimeCurve(1.0f, 1.f, 1.f, 1.f);
    rsCustomModifier->SetContentTransitionParam(ContentTransitionType::OPACITY, timingProtocol, springTimeCurve);
    rsCustomModifier->UpdateProperty(node, drawCmdList, 0);
    ASSERT_EQ(rsCustomModifier->contentTransitionType_, ContentTransitionType::OPACITY);
    ASSERT_NE(rsCustomModifier->timingCurve_.type_, RSAnimationTimingCurve::CurveType::INTERPOLATING);

    // case4: not OPACITY and not INTERPOLATING
    rsCustomModifier->SetContentTransitionParam(ContentTransitionType::IDENTITY, timingProtocol, springTimeCurve);
    rsCustomModifier->UpdateProperty(node, drawCmdList, 0);
    ASSERT_NE(rsCustomModifier->contentTransitionType_, ContentTransitionType::OPACITY);
    ASSERT_NE(rsCustomModifier->timingCurve_.type_, RSAnimationTimingCurve::CurveType::INTERPOLATING);
}

/**
 * @tc.name: UpdatePropertyTest002
 * @tc.desc: Test the function UpdateProperty
 * @tc.type: FUNC
 */
HWTEST_F(RSCustomModifierHelperTest, UpdatePropertyTest002, TestSize.Level1)
{
    auto rsCustomModifier = std::make_shared<ModifierNG::RSContentStyleModifier>();
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(1, 1);
    rsCustomModifier->SetContentTransitionParam(ContentTransitionType::IDENTITY);

    // case1: not send to remote
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    rsCustomModifier->UpdateProperty(node, drawCmdList, 0);
    ASSERT_FALSE(node->NeedForcedSendToRemote());

    // case2: send to remote
    node = std::make_shared<RSNodeMock>();
    rsCustomModifier->UpdateProperty(node, drawCmdList, 0);
    ASSERT_TRUE(node->NeedForcedSendToRemote());
}

/**
 * @tc.name: UpdateToRenderTest001
 * @tc.desc: Test the function UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSCustomModifierHelperTest, UpdateToRenderTest001, TestSize.Level1)
{
    auto rsCustomModifier = std::make_shared<ModifierNG::RSContentStyleModifier>();

    // case1: node is null
    rsCustomModifier->UpdateToRender();
    ASSERT_FALSE(rsCustomModifier->node_.lock());

    // case2: last DrawCmdList empty and DrawCmdList not empty
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    rsCustomModifier->node_ = node;
    rsCustomModifier->lastDrawCmdListEmpty_ = true;
    rsCustomModifier->UpdateToRender();
    ASSERT_FALSE(rsCustomModifier->lastDrawCmdListEmpty_);

    // case3: last DrawCmdList not empty and DrawCmdList not empty
    rsCustomModifier->lastDrawCmdListEmpty_ = false;
    rsCustomModifier->UpdateToRender();
    ASSERT_FALSE(rsCustomModifier->lastDrawCmdListEmpty_);

    // case4: last DrawCmdList empty and DrawCmdList empty
    RSDisplayNodeConfig displayNodeConfig = {};
    node = RSDisplayNode::Create(displayNodeConfig);
    ASSERT_TRUE(node);
    rsCustomModifier->node_ = node;
    rsCustomModifier->lastDrawCmdListEmpty_ = true;
    rsCustomModifier->UpdateToRender();
    ASSERT_TRUE(rsCustomModifier->lastDrawCmdListEmpty_);

    // case5: last DrawCmdList not empty and DrawCmdList empty
    rsCustomModifier->lastDrawCmdListEmpty_ = false;
    rsCustomModifier->UpdateToRender();
    ASSERT_TRUE(rsCustomModifier->lastDrawCmdListEmpty_);

    auto type = rsCustomModifier->GetInnerPropertyType();
    auto it = rsCustomModifier->properties_.find(type);
    ASSERT_EQ(it, rsCustomModifier->properties_.end());

    // case6: invalid property
    node = RSCanvasNode::Create();
    rsCustomModifier->node_ = node;
    rsCustomModifier->properties_[type] = nullptr;
    rsCustomModifier->UpdateToRender();
    it = rsCustomModifier->properties_.find(type);
    ASSERT_NE(it, rsCustomModifier->properties_.end());
    ASSERT_FALSE(it->second);

    // case7: valid property
    auto drawCmdList7 = std::make_shared<Drawing::DrawCmdList>(1, 1);
    auto property = std::make_shared<RSAnimatableProperty<Drawing::DrawCmdListPtr>>(drawCmdList7);
    rsCustomModifier->lastDrawCmdListEmpty_ = false;
    rsCustomModifier->properties_[type] = property;
    rsCustomModifier->UpdateToRender();
    it = rsCustomModifier->properties_.find(type);
    ASSERT_NE(it, rsCustomModifier->properties_.end());
    ASSERT_TRUE(it->second);

    // case8: valid custom property
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(1, 1);
    auto cmdListProperty = std::make_shared<RSAnimatableProperty<std::shared_ptr<Drawing::DrawCmdList>>>(drawCmdList);
    cmdListProperty->isCustom_ = true;
    rsCustomModifier->lastDrawCmdListEmpty_ = false;
    rsCustomModifier->properties_[type] = cmdListProperty;
    rsCustomModifier->UpdateToRender();
    it = rsCustomModifier->properties_.find(type);
    ASSERT_NE(it, rsCustomModifier->properties_.end());
    ASSERT_TRUE(it->second);
}

/**
 * @tc.name: UpdateToRenderTest002
 * @tc.desc: Test the function UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSCustomModifierHelperTest, UpdateToRenderTest002, TestSize.Level1)
{
    auto modifier = std::make_shared<TestContentStyleModifier>();
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    modifier->node_ = node;
    ASSERT_TRUE(modifier->node_.lock());
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(1, 1);
    auto property = std::make_shared<RSProperty<Drawing::DrawCmdListPtr>>(drawCmdList);
    modifier->properties_.emplace(modifier->GetInnerPropertyType(), property);
    modifier->UpdateToRender();
    ASSERT_NE(property->stagingValue_, nullptr);
}

/**
 * @tc.name: ClearDrawCmdListTest
 * @tc.desc: Test the function ClearDrawCmdList
 * @tc.type: FUNC
 */
HWTEST_F(RSCustomModifierHelperTest, ClearDrawCmdListTest, TestSize.Level1)
{
    auto rsCustomModifier = std::make_shared<ModifierNG::RSContentStyleModifier>();

    // case1: property is null
    rsCustomModifier->ClearDrawCmdList();

    // case2: property exists
    auto node = RSCanvasNode::Create();
    rsCustomModifier->OnAttach(*node);
    rsCustomModifier->UpdateDrawCmdList();
    rsCustomModifier->ClearDrawCmdList();

    auto type = rsCustomModifier->GetInnerPropertyType();
    auto it = rsCustomModifier->properties_.find(type);
    ASSERT_NE(it, rsCustomModifier->properties_.end());
    if (it->second) {
        auto property = std::static_pointer_cast<RSAnimatableProperty<Drawing::DrawCmdListPtr>>(it->second);
        EXPECT_EQ(property->showingValue_, nullptr);
        EXPECT_EQ(property->stagingValue_, nullptr);
    }
}
} // namespace OHOS::Rosen