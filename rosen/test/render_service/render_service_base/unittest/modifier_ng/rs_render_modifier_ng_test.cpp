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
#include "common/rs_vector4.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "modifier_ng/appearance/rs_alpha_render_modifier.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"

#include "message_parcel.h"
#include "property/rs_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderModifierNGTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderModifierNGTest::SetUpTestCase() {}
void RSRenderModifierNGTest::TearDownTestCase() {}
void RSRenderModifierNGTest::SetUp() {}
void RSRenderModifierNGTest::TearDown() {}

/**
 * @tc.name: AttachPropertyTest
 * @tc.desc: test the function AttachProperty
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderModifierNGTest, AttachPropertyTest, TestSize.Level1)
{
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    drawCmdList->SetWidth(1024);
    drawCmdList->SetHeight(1090);
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    auto modifier = std::make_shared<ModifierNG::RSCustomRenderModifier<ModifierNG::RSModifierType::CONTENT_STYLE>>();
    NodeId nodeId = 1;
    RSCanvasDrawingRenderNode node(nodeId);
    modifier->OnAttachModifier(node);
    modifier->AttachProperty(ModifierNG::RSPropertyType::CONTENT_STYLE, property);
    EXPECT_NE(property, nullptr);
}

/**
 * @tc.name: DetachPropertyTest
 * @tc.desc: test the function DetachProperty
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderModifierNGTest, DetachPropertyTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSCustomRenderModifier<ModifierNG::RSModifierType::CONTENT_STYLE>>();
    modifier->DetachProperty(ModifierNG::RSPropertyType::CONTENT_STYLE);
    EXPECT_NE(modifier, nullptr);

    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    drawCmdList->SetWidth(1024);
    drawCmdList->SetHeight(1090);
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    NodeId nodeId = 1;
    RSCanvasDrawingRenderNode node(nodeId);
    modifier->OnAttachModifier(node);
    modifier->AttachProperty(ModifierNG::RSPropertyType::CONTENT_STYLE, property);
    modifier->DetachProperty(ModifierNG::RSPropertyType::CONTENT_STYLE);
    EXPECT_NE(property, nullptr);
}

/**
 * @tc.name: GetResetFuncMapTest
 * @tc.desc: test the function GetResetFuncMap
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderModifierNGTest, GetResetFuncMapTest, TestSize.Level1)
{
    auto resetFuncMap = ModifierNG::RSRenderModifier::GetResetFuncMap();
    EXPECT_TRUE(resetFuncMap.count(ModifierNG::RSModifierType::TRANSFORM));
}

/**
 * @tc.name: GetPropertySizeTest
 * @tc.desc: test the function GetPropertySize
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderModifierNGTest, GetPropertySizeTest, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderProperty<float>>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>();
    EXPECT_TRUE(modifier->GetPropertySize() == 0);
    modifier->AttachProperty(ModifierNG::RSPropertyType::ALPHA, property);
    EXPECT_TRUE(modifier->GetPropertySize() > 0);
}

/**
 * @tc.name: SetDirtyTest
 * @tc.desc: test the function SetDirty
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderModifierNGTest, SetDirtyTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>();
    modifier->SetDirty();
    EXPECT_EQ(modifier->target_.lock(), nullptr);

    NodeId nodeId = 1;
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSCanvasDrawingRenderNode>(nodeId);
    std::weak_ptr<RSRenderNode> weakPtr = nodePtr;
    modifier->target_ = weakPtr;
    modifier->SetDirty();
    EXPECT_NE(modifier->target_.lock(), nullptr);
}

/**
 * @tc.name: MarshallingTest
 * @tc.desc: test the function Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderModifierNGTest, MarshallingTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>();
    Parcel parcel;
    EXPECT_TRUE(modifier->Marshalling(parcel));
}

/**
 * @tc.name: UnmarshallingTest
 * @tc.desc: test the function Unmarshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderModifierNGTest, UnmarshallingTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>();
    Parcel parcel;
    auto res = modifier->Unmarshalling(parcel);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name: ApplyLegacyPropertyTest
 * @tc.desc: test the function ApplyLegacyProperty
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderModifierNGTest, ApplyLegacyPropertyTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>();
    RSProperties properties;
    auto property = std::make_shared<RSRenderProperty<float>>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::ALPHA, property);
    modifier->ApplyLegacyProperty(properties);
    EXPECT_NE(modifier, nullptr);
}

/**
 * @tc.name: RSCustomRenderModifier_Apply_Test
 * @tc.desc: test the function Apply of the class RSCustomRenderModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderModifierNGTest, RSCustomRenderModifier_Apply_Test, TestSize.Level1)
{
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    drawCmdList->SetWidth(1024);
    drawCmdList->SetHeight(1090);
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    ModifierId id = 1;
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::CONTENT_STYLE, property, id, ModifierNG::RSPropertyType::CONTENT_STYLE);
    RSProperties properties;
    modifier->Apply(nullptr, properties);
    EXPECT_TRUE(modifier->HasProperty(ModifierNG::RSPropertyType::CONTENT_STYLE));
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    modifier->Apply(&paintFilterCanvas, properties);
    EXPECT_NE(&paintFilterCanvas, nullptr);
}

/**
 * @tc.name: RSCustomRenderModifier_OnSetDirty_Test
 * @tc.desc: test the function OnSetDirty of the class RSCustomRenderModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderModifierNGTest, RSCustomRenderModifier_OnSetDirty_Test, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSCustomRenderModifier<ModifierNG::RSModifierType::CONTENT_STYLE>>();
    NodeId nodeId = 1;
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSCanvasDrawingRenderNode>(nodeId);
    std::weak_ptr<RSRenderNode> weakPtr = nodePtr;
    modifier->target_ = weakPtr;
    modifier->OnSetDirty();
    EXPECT_NE(modifier->target_.lock(), nullptr);
}
}