/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "animation/rs_animation.h"
#include "core/transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_director.h"
#include "modifier/rs_modifier_manager.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSModifierTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f, 485.44f, -34.4f,
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
        };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSModifierTest::SetUpTestCase() {}
void RSModifierTest::TearDownTestCase() {}
void RSModifierTest::SetUp() {}
void RSModifierTest::TearDown() {}

/**
 * @tc.name: Modifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, Modifier001, TestSize.Level1)
{
    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    ASSERT_TRUE(modifier != nullptr);
    ASSERT_EQ(modifier->GetPropertyId(), prop->GetId());
}


/**
 * @tc.name: Modifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, Modifier002, TestSize.Level1)
{
    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    ASSERT_TRUE(modifier != nullptr);
    ASSERT_EQ(std::static_pointer_cast<RSAnimatableProperty<float>>(modifier->GetProperty())->Get(), floatData[0]);

    prop->Set(floatData[1]);
    ASSERT_EQ(std::static_pointer_cast<RSAnimatableProperty<float>>(modifier->GetProperty())->Get(), floatData[1]);
}
/**
 * @tc.name: AddModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, AddModifier001, TestSize.Level1)
{
    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    auto node = RSCanvasNode::Create();

    node->AddModifier(modifier);
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
}

/**
 * @tc.name: AddModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, AddModifier002, TestSize.Level1)
{
    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetAlpha(), floatData[0]);

    prop->Set(floatData[1]);
    ASSERT_EQ(node->GetStagingProperties().GetAlpha(), floatData[1]);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetAlpha(), node1->GetStagingProperties().GetAlpha());
}

/**
 * @tc.name: BoundsModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BoundsModifier001, TestSize.Level1)
{
    auto value = Vector4f(100.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSBoundsModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBounds(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBounds(), node1->GetStagingProperties().GetBounds());
}

/**
 * @tc.name: BoundsModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BoundsModifier002, TestSize.Level1)
{
    auto value = Vector4f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSBoundsModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBounds(), value);
    auto str = node->GetStagingProperties().Dump();

    value = Vector4f(0.f, 0.f, 500.f, 600.f);
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetBounds(), value);
    str = node->GetStagingProperties().Dump();
}

/**
 * @tc.name: FrameModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, FrameModifier001, TestSize.Level1)
{
    auto value = Vector4f(100.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSFrameModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetFrame(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetFrame(), node1->GetStagingProperties().GetFrame());
}

/**
 * @tc.name: FrameModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, FrameModifier002, TestSize.Level1)
{
    auto value = Vector4f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSFrameModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetFrame(), value);
    auto str = node->GetStagingProperties().Dump();

    value = Vector4f(0.f, 0.f, 500.f, 600.f);
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetFrame(), value);
    str = node->GetStagingProperties().Dump();
}

/**
 * @tc.name: PositionZModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, PositionZModifier001, TestSize.Level1)
{
    auto value = 2.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSPositionZModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetPositionZ(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetPositionZ(), node1->GetStagingProperties().GetPositionZ());
}

/**
 * @tc.name: PositionZModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, PositionZModifier002, TestSize.Level1)
{
    auto value = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSPositionZModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetPositionZ(), value);

    value = -1.f;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetPositionZ(), value);
}

/**
 * @tc.name: PivotModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, PivotModifier001, TestSize.Level1)
{
    auto value = Vector2f(1.f, 1.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
    auto modifier = std::make_shared<RSPivotModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetPivot(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetPivot(), node1->GetStagingProperties().GetPivot());
}

/**
 * @tc.name: PivotModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, PivotModifier002, TestSize.Level1)
{
    auto value = Vector2f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
    auto modifier = std::make_shared<RSPivotModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetPivot(), value);

    value = Vector2f(0.5f, 0.5f);
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetPivot(), value);
}

/**
 * @tc.name: QuaternionModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, QuaternionModifier001, TestSize.Level1)
{
    auto value = Quaternion(0.382683, 0, 0, 0.92388);
    auto prop = std::make_shared<RSAnimatableProperty<Quaternion>>(value);
    auto modifier = std::make_shared<RSQuaternionModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetQuaternion(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetQuaternion(), node1->GetStagingProperties().GetQuaternion());
}

/**
 * @tc.name: QuaternionModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, QuaternionModifier002, TestSize.Level1)
{
    auto value = Quaternion();
    auto prop = std::make_shared<RSAnimatableProperty<Quaternion>>(value);
    auto modifier = std::make_shared<RSQuaternionModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetQuaternion(), value);

    value = Quaternion(0, 0, 0.382683, 0.92388);
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetQuaternion(), value);
}

/**
 * @tc.name: RotationModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, RotationModifier001, TestSize.Level1)
{
    auto value = 90.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSRotationModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetRotation(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetRotation(), node1->GetStagingProperties().GetRotation());
}

/**
 * @tc.name: RotationModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, RotationModifier002, TestSize.Level1)
{
    auto value = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSRotationModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetRotation(), value);

    value = -360.f;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetRotation(), value);
}

/**
 * @tc.name: RotationXModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, RotationXModifier001, TestSize.Level1)
{
    auto value = 90.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSRotationXModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetRotationX(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetRotationX(), node1->GetStagingProperties().GetRotationX());
}

/**
 * @tc.name: RotationXModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, RotationXModifier002, TestSize.Level1)
{
    auto value = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSRotationXModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetRotationX(), value);

    value = -360.f;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetRotationX(), value);
}

/**
 * @tc.name: RotationYModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, RotationYModifier001, TestSize.Level1)
{
    auto value = 90.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSRotationYModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetRotationY(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetRotationY(), node1->GetStagingProperties().GetRotationY());
}

/**
 * @tc.name: RotationYModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, RotationYModifier002, TestSize.Level1)
{
    auto value = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSRotationYModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetRotationY(), value);

    value = -360.f;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetRotationY(), value);
}

/**
 * @tc.name: ScaleModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ScaleModifier001, TestSize.Level1)
{
    auto value = Vector2f(2.f, 2.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
    auto modifier = std::make_shared<RSScaleModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetScale(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetScale(), node1->GetStagingProperties().GetScale());
}

/**
 * @tc.name: ScaleModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ScaleModifier002, TestSize.Level1)
{
    auto value = Vector2f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
    auto modifier = std::make_shared<RSScaleModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetScale(), value);

    value = Vector2f(0.5f, 0.5f);
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetScale(), value);
}

/**
 * @tc.name: SkewModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, SkewModifier001, TestSize.Level1)
{
    auto value = Vector2f(2.f, 2.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
    auto modifier = std::make_shared<RSSkewModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetSkew(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetSkew(), node1->GetStagingProperties().GetSkew());
}

/**
 * @tc.name: SkewModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, SkewModifier002, TestSize.Level1)
{
    auto value = Vector2f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
    auto modifier = std::make_shared<RSSkewModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetSkew(), value);

    value = Vector2f(0.5f, 0.5f);
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetSkew(), value);
}

/**
 * @tc.name: TranslateModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, TranslateModifier001, TestSize.Level1)
{
    auto value = Vector2f(200.f, 300.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
    auto modifier = std::make_shared<RSTranslateModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetTranslate(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetTranslate(), node1->GetStagingProperties().GetTranslate());
}

/**
 * @tc.name: TranslateModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, TranslateModifier002, TestSize.Level1)
{
    auto value = Vector2f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
    auto modifier = std::make_shared<RSTranslateModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetTranslate(), value);

    value = Vector2f(-200.f, -300.f);
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetTranslate(), value);
}

/**
 * @tc.name: TranslateZModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, TranslateZModifier001, TestSize.Level1)
{
    auto value = 20.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSTranslateZModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetTranslateZ(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetTranslateZ(), node1->GetStagingProperties().GetTranslateZ());
}

/**
 * @tc.name: TranslateZModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, TranslateZModifier002, TestSize.Level1)
{
    auto value = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSTranslateZModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetTranslateZ(), value);

    value = -10.f;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetTranslateZ(), value);
}

/**
 * @tc.name: CornerRadiusModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, CornerRadiusModifier001, TestSize.Level1)
{
    auto value = Vector4f(20.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSCornerRadiusModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetCornerRadius(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetCornerRadius(), node1->GetStagingProperties().GetCornerRadius());
}

/**
 * @tc.name: CornerRadiusModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, CornerRadiusModifier002, TestSize.Level1)
{
    auto value = Vector4f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSCornerRadiusModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetCornerRadius(), value);

    value = Vector4f(-10.f);
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetCornerRadius(), value);
}

/**
 * @tc.name: AlphaModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, AlphaModifier001, TestSize.Level1)
{
    auto value = 0.5f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetAlpha(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetAlpha(), node1->GetStagingProperties().GetAlpha());
}

/**
 * @tc.name: AlphaModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, AlphaModifier002, TestSize.Level1)
{
    auto value = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetAlpha(), value);
    auto str = node->GetStagingProperties().Dump();

    value = -1.f;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetAlpha(), value);
    str = node->GetStagingProperties().Dump();
}

/**
 * @tc.name: AlphaOffscreenModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, AlphaOffscreenModifier001, TestSize.Level1)
{
    bool value = false;
    auto prop = std::make_shared<RSProperty<bool>>(value);
    auto modifier = std::make_shared<RSAlphaOffscreenModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetAlphaOffscreen(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetAlphaOffscreen(), node1->GetStagingProperties().GetAlphaOffscreen());
}

/**
 * @tc.name: AlphaOffscreenModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, AlphaOffscreenModifier002, TestSize.Level1)
{
    bool value = false;
    auto prop = std::make_shared<RSProperty<bool>>(value);
    auto modifier = std::make_shared<RSAlphaOffscreenModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetAlphaOffscreen(), value);

    value = true;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetAlphaOffscreen(), value);
}

/**
 * @tc.name: ForegroundColorModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ForegroundColorModifier001, TestSize.Level1)
{
    auto value = RgbPalette::Black();
    auto prop = std::make_shared<RSAnimatableProperty<Color>>(value);
    auto modifier = std::make_shared<RSForegroundColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetForegroundColor(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetForegroundColor(), node1->GetStagingProperties().GetForegroundColor());
}

/**
 * @tc.name: ForegroundColorModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ForegroundColorModifier002, TestSize.Level1)
{
    auto value = RgbPalette::Transparent();
    auto prop = std::make_shared<RSAnimatableProperty<Color>>(value);
    auto modifier = std::make_shared<RSForegroundColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetForegroundColor(), value);

    value = RgbPalette::Red();
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetForegroundColor(), value);
}

/**
 * @tc.name: BackgroundColorModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BackgroundColorModifier001, TestSize.Level1)
{
    auto value = RgbPalette::Black();
    auto prop = std::make_shared<RSAnimatableProperty<Color>>(value);
    auto modifier = std::make_shared<RSBackgroundColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundColor(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundColor(), node1->GetStagingProperties().GetBackgroundColor());
}

/**
 * @tc.name: BackgroundColorModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BackgroundColorModifier002, TestSize.Level1)
{
    auto value = RgbPalette::Transparent();
    auto prop = std::make_shared<RSAnimatableProperty<Color>>(value);
    auto modifier = std::make_shared<RSBackgroundColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundColor(), value);
    auto str = node->GetStagingProperties().Dump();

    value = RgbPalette::Green();
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundColor(), value);
    str = node->GetStagingProperties().Dump();
}

/**
 * @tc.name: BackgroundShaderModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BackgroundShaderModifier001, TestSize.Level1)
{
    auto value = RSShader::CreateRSShader();
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSShader>>>(value);
    auto modifier = std::make_shared<RSBackgroundShaderModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundShader(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundShader(), node1->GetStagingProperties().GetBackgroundShader());
}

/**
 * @tc.name: BackgroundShaderModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BackgroundShaderModifier002, TestSize.Level1)
{
    std::shared_ptr<RSShader> value = nullptr;
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSShader>>>(value);
    auto modifier = std::make_shared<RSBackgroundShaderModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundShader(), value);

    value = RSShader::CreateRSShader();
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundShader(), value);
}

/**
 * @tc.name: BgImageModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BgImageModifier001, TestSize.Level1)
{
    auto value = std::make_shared<RSImage>();
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSImage>>>(value);
    auto modifier = std::make_shared<RSBgImageModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImage(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBgImage(), node1->GetStagingProperties().GetBgImage());
}

/**
 * @tc.name: BgImageModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BgImageModifier002, TestSize.Level1)
{
    std::shared_ptr<RSImage> value = nullptr;
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSImage>>>(value);
    auto modifier = std::make_shared<RSBgImageModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImage(), value);

    value = std::make_shared<RSImage>();
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetBgImage(), value);
}

/**
 * @tc.name: BgImageWidthModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BgImageWidthModifier001, TestSize.Level1)
{
    auto value = 200.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgImageWidthModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImageWidth(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBgImageWidth(), node1->GetStagingProperties().GetBgImageWidth());
}

/**
 * @tc.name: BgImageWidthModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BgImageWidthModifier002, TestSize.Level1)
{
    auto value = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgImageWidthModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImageWidth(), value);

    value = -100.f;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetBgImageWidth(), value);
}

/**
 * @tc.name: BgImageHeightModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BgImageHeightModifier001, TestSize.Level1)
{
    auto value = 200.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgImageHeightModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImageHeight(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBgImageHeight(), node1->GetStagingProperties().GetBgImageHeight());
}

/**
 * @tc.name: BgImageHeightModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BgImageHeightModifier002, TestSize.Level1)
{
    auto value = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgImageHeightModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImageHeight(), value);

    value = -100.f;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetBgImageHeight(), value);
}

/**
 * @tc.name: BgImagePositionXModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BgImagePositionXModifier001, TestSize.Level1)
{
    auto value = 200.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgImagePositionXModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionX(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionX(), node1->GetStagingProperties().GetBgImagePositionX());
}

/**
 * @tc.name: BgImagePositionXModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BgImagePositionXModifier002, TestSize.Level1)
{
    auto value = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgImagePositionXModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionX(), value);

    value = -100.f;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionX(), value);
}

/**
 * @tc.name: BgImagePositionYModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BgImagePositionYModifier001, TestSize.Level1)
{
    auto value = 200.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgImagePositionYModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionY(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionY(), node1->GetStagingProperties().GetBgImagePositionY());
}

/**
 * @tc.name: BgImagePositionYModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BgImagePositionYModifier002, TestSize.Level1)
{
    auto value = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgImagePositionYModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionY(), value);

    value = -100.f;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionY(), value);
}

/**
 * @tc.name: BorderColorModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BorderColorModifier001, TestSize.Level1)
{
    auto value = Vector4<Color>(RgbPalette::Black());
    auto prop = std::make_shared<RSAnimatableProperty<Vector4<Color>>>(value);
    auto modifier = std::make_shared<RSBorderColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBorderColor(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBorderColor(), node1->GetStagingProperties().GetBorderColor());
}

/**
 * @tc.name: BorderColorModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BorderColorModifier002, TestSize.Level1)
{
    auto value = Vector4<Color>(RgbPalette::Transparent());
    auto prop = std::make_shared<RSAnimatableProperty<Vector4<Color>>>(value);
    auto modifier = std::make_shared<RSBorderColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBorderColor(), value);

    value = Vector4<Color>(RgbPalette::Transparent(), RgbPalette::Red(), RgbPalette::Green(), RgbPalette::Blue());
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetBorderColor(), value);
}

/**
 * @tc.name: BorderWidthModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BorderWidthModifier001, TestSize.Level1)
{
    auto value = Vector4f(100.f, 100.f, 200.f, 300.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSBorderWidthModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBorderWidth(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBorderWidth(), node1->GetStagingProperties().GetBorderWidth());
}

/**
 * @tc.name: BorderWidthModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BorderWidthModifier002, TestSize.Level1)
{
    auto value = Vector4f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSBorderWidthModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBorderWidth(), value);

    value = Vector4f(-100.f);
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetBorderWidth(), value);
}

/**
 * @tc.name: BorderStyleModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BorderStyleModifier001, TestSize.Level1)
{
    auto value = Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::SOLID));
    auto prop = std::make_shared<RSProperty<Vector4<uint32_t>>>(value);
    auto modifier = std::make_shared<RSBorderStyleModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBorderStyle(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBorderStyle(), node1->GetStagingProperties().GetBorderStyle());
}

/**
 * @tc.name: BorderStyleModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BorderStyleModifier002, TestSize.Level1)
{
    auto value = Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::NONE));
    auto prop = std::make_shared<RSProperty<Vector4<uint32_t>>>(value);
    auto modifier = std::make_shared<RSBorderStyleModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBorderStyle(), value);

    value = Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::DOTTED));
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetBorderStyle(), value);
}

/**
 * @tc.name: FilterModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, FilterModifier001, TestSize.Level1)
{
    auto value = RSFilter::CreateBlurFilter(10.f, 10.f);
    auto prop = std::make_shared<RSAnimatableProperty<std::shared_ptr<RSFilter>>>(value);
    auto modifier = std::make_shared<RSFilterModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetFilter(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetFilter(), node1->GetStagingProperties().GetFilter());
}

/**
 * @tc.name: FilterModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, FilterModifier002, TestSize.Level1)
{
    std::shared_ptr<RSFilter> value = nullptr;
    auto prop = std::make_shared<RSAnimatableProperty<std::shared_ptr<RSFilter>>>(value);
    auto modifier = std::make_shared<RSFilterModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetFilter(), value);

    value = RSFilter::CreateBlurFilter(20.f, 30.f);
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetFilter(), value);
}

/**
 * @tc.name: BackgroundFilterModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BackgroundFilterModifier001, TestSize.Level1)
{
    auto value = RSFilter::CreateBlurFilter(10.f, 10.f);
    auto prop = std::make_shared<RSAnimatableProperty<std::shared_ptr<RSFilter>>>(value);
    auto modifier = std::make_shared<RSBackgroundFilterModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundFilter(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundFilter(), node1->GetStagingProperties().GetBackgroundFilter());
}

/**
 * @tc.name: BackgroundFilterModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, BackgroundFilterModifier002, TestSize.Level1)
{
    std::shared_ptr<RSFilter> value = nullptr;
    auto prop = std::make_shared<RSAnimatableProperty<std::shared_ptr<RSFilter>>>(value);
    auto modifier = std::make_shared<RSBackgroundFilterModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundFilter(), value);

    value = RSFilter::CreateBlurFilter(20.f, 30.f);
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundFilter(), value);
}

/**
 * @tc.name: FrameGravityModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, FrameGravityModifier001, TestSize.Level1)
{
    auto value = Gravity::CENTER;
    auto prop = std::make_shared<RSProperty<Gravity>>(value);
    auto modifier = std::make_shared<RSFrameGravityModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetFrameGravity(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetFrameGravity(), node1->GetStagingProperties().GetFrameGravity());
}

/**
 * @tc.name: FrameGravityModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, FrameGravityModifier002, TestSize.Level1)
{
    auto value = Gravity::DEFAULT;
    auto prop = std::make_shared<RSProperty<Gravity>>(value);
    auto modifier = std::make_shared<RSFrameGravityModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetFrameGravity(), value);

    value = Gravity::RESIZE_ASPECT;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetFrameGravity(), value);
}

/**
 * @tc.name: ClipBoundsModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ClipBoundsModifier001, TestSize.Level1)
{
    auto value = RSPath::CreateRSPath();
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSPath>>>(value);
    auto modifier = std::make_shared<RSClipBoundsModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetClipBounds(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetClipBounds(), node1->GetStagingProperties().GetClipBounds());
}

/**
 * @tc.name: ClipBoundsModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ClipBoundsModifier002, TestSize.Level1)
{
    std::shared_ptr<RSPath> value = nullptr;
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSPath>>>(value);
    auto modifier = std::make_shared<RSClipBoundsModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetClipBounds(), value);

    value = RSPath::CreateRSPath();
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetClipBounds(), value);
}

/**
 * @tc.name: ClipToBoundsModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ClipToBoundsModifier001, TestSize.Level1)
{
    auto value = true;
    auto prop = std::make_shared<RSProperty<bool>>(value);
    auto modifier = std::make_shared<RSClipToBoundsModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetClipToBounds(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetClipToBounds(), node1->GetStagingProperties().GetClipToBounds());
}

/**
 * @tc.name: ClipToBoundsModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ClipToBoundsModifier002, TestSize.Level1)
{
    auto value = false;
    auto prop = std::make_shared<RSProperty<bool>>(value);
    auto modifier = std::make_shared<RSClipToBoundsModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetClipToBounds(), value);

    value = true;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetClipToBounds(), value);
}

/**
 * @tc.name: ClipToFrameModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ClipToFrameModifier001, TestSize.Level1)
{
    auto value = true;
    auto prop = std::make_shared<RSProperty<bool>>(value);
    auto modifier = std::make_shared<RSClipToFrameModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetClipToFrame(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetClipToFrame(), node1->GetStagingProperties().GetClipToFrame());
}

/**
 * @tc.name: ClipToFrameModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ClipToFrameModifier002, TestSize.Level1)
{
    auto value = false;
    auto prop = std::make_shared<RSProperty<bool>>(value);
    auto modifier = std::make_shared<RSClipToFrameModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetClipToFrame(), value);

    value = true;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetClipToFrame(), value);
}

/**
 * @tc.name: VisibleModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, VisibleModifier001, TestSize.Level1)
{
    auto value = false;
    auto prop = std::make_shared<RSProperty<bool>>(value);
    auto modifier = std::make_shared<RSVisibleModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetVisible(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetVisible(), node1->GetStagingProperties().GetVisible());
}

/**
 * @tc.name: VisibleModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, VisibleModifier002, TestSize.Level1)
{
    auto value = true;
    auto prop = std::make_shared<RSProperty<bool>>(value);
    auto modifier = std::make_shared<RSVisibleModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetVisible(), value);
    auto str = node->GetStagingProperties().Dump();

    value = false;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetVisible(), value);
    str = node->GetStagingProperties().Dump();
}

/**
 * @tc.name: ShadowColorModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ShadowColorModifier001, TestSize.Level1)
{
    auto value = RgbPalette::Black();
    auto prop = std::make_shared<RSAnimatableProperty<Color>>(value);
    auto modifier = std::make_shared<RSShadowColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowColor(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetShadowColor(), node1->GetStagingProperties().GetShadowColor());
}

/**
 * @tc.name: ShadowColorModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ShadowColorModifier002, TestSize.Level1)
{
    auto value = Color::FromArgbInt(DEFAULT_SPOT_COLOR);
    auto prop = std::make_shared<RSAnimatableProperty<Color>>(value);
    auto modifier = std::make_shared<RSShadowColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowColor(), value);

    value = RgbPalette::Red();
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetShadowColor(), value);
}

/**
 * @tc.name: ShadowOffsetXModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ShadowOffsetXModifier001, TestSize.Level1)
{
    auto value = 5.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowOffsetXModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetX(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetX(), node1->GetStagingProperties().GetShadowOffsetX());
}

/**
 * @tc.name: ShadowOffsetXModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ShadowOffsetXModifier002, TestSize.Level1)
{
    auto value = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowOffsetXModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetX(), value);

    value = -1.f;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetX(), value);
}

/**
 * @tc.name: ShadowOffsetYModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ShadowOffsetYModifier001, TestSize.Level1)
{
    auto value = 5.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowOffsetYModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(prop->Get(), value);
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetY(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetY(), node1->GetStagingProperties().GetShadowOffsetY());
}

/**
 * @tc.name: ShadowOffsetYModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ShadowOffsetYModifier002, TestSize.Level1)
{
    auto value = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowOffsetYModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetY(), value);

    value = -1.f;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetY(), value);
}

/**
 * @tc.name: ShadowAlphaModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ShadowAlphaModifier001, TestSize.Level1)
{
    auto value = 0.8f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowAlphaModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowAlpha(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetShadowAlpha(), node1->GetStagingProperties().GetShadowAlpha());
}

/**
 * @tc.name: ShadowAlphaModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ShadowAlphaModifier002, TestSize.Level1)
{
    auto value = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowAlphaModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowAlpha(), value);

    value = -0.2f;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetShadowAlpha(), value);
}

/**
 * @tc.name: ShadowElevationModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ShadowElevationModifier001, TestSize.Level1)
{
    auto value = 5.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowElevationModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowElevation(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetShadowElevation(), node1->GetStagingProperties().GetShadowElevation());
}

/**
 * @tc.name: ShadowElevationModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ShadowElevationModifier002, TestSize.Level1)
{
    auto value = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowElevationModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowElevation(), value);

    value = -1.f;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetShadowElevation(), value);
}

/**
 * @tc.name: ShadowRadiusModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ShadowRadiusModifier001, TestSize.Level1)
{
    auto value = 10.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowRadiusModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowRadius(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetShadowRadius(), node1->GetStagingProperties().GetShadowRadius());
}

/**
 * @tc.name: ShadowRadiusModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ShadowRadiusModifier002, TestSize.Level1)
{
    auto value = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowRadiusModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowRadius(), value);

    value = -10.f;
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetShadowRadius(), value);
}

/**
 * @tc.name: ShadowPathModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ShadowPathModifier001, TestSize.Level1)
{
    auto value = RSPath::CreateRSPath();
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSPath>>>(value);
    auto modifier = std::make_shared<RSShadowPathModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowPath(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetShadowPath(), node1->GetStagingProperties().GetShadowPath());
}

/**
 * @tc.name: ShadowPathModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ShadowPathModifier002, TestSize.Level1)
{
    std::shared_ptr<RSPath> value = nullptr;
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSPath>>>(value);
    auto modifier = std::make_shared<RSShadowPathModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowPath(), value);

    value = RSPath::CreateRSPath();
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetShadowPath(), value);
}

/**
 * @tc.name: MaskModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, MaskModifier001, TestSize.Level1)
{
    auto value = RSMask::CreateGradientMask(Drawing::Brush());
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSMask>>>(value);
    auto modifier = std::make_shared<RSMaskModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetMask(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetMask(), node1->GetStagingProperties().GetMask());
}

/**
 * @tc.name: MaskModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, MaskModifier002, TestSize.Level1)
{
    std::shared_ptr<RSMask> value = nullptr;
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSMask>>>(value);
    auto modifier = std::make_shared<RSMaskModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetMask(), value);

    value = RSMask::CreateGradientMask(Drawing::Brush());
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetMask(), value);
}

/**
 * @tc.name: ModifierManager001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ModifierManager001, TestSize.Level1)
{
    RSModifierManager manager;
    manager.Draw();

    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    manager.AddModifier(modifier);
    manager.Draw();
}

/**
 * @tc.name: ModifierManager002
 * @tc.desc: animation is exit
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ModifierManager002, TestSize.Level1)
{
    RSModifierManager manager;
    manager.Draw();

    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    auto animation = std::make_shared<RSRenderAnimation>();
    ASSERT_NE(animation, nullptr);
    manager.AddAnimation(animation);
    manager.AddModifier(modifier);
    manager.Draw();
}

/**
 * @tc.name: ModifierManager003
 * @tc.desc: animation is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ModifierManager003, TestSize.Level1)
{
    RSModifierManager manager;
    AnimationId id = 0;
    manager.Draw();

    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    auto animation = std::make_shared<RSRenderAnimation>(id);
    manager.AddAnimation(animation);
    manager.AddModifier(modifier);
    manager.Draw();
}

/**
 * @tc.name: ModifierManager004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ModifierManager004, TestSize.Level1)
{
    RSModifierManager manager;
    AnimationId id = 10;
    manager.Draw();

    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    auto animation = std::make_shared<RSRenderAnimation>();
    ASSERT_NE(animation, nullptr);
    manager.AddAnimation(animation);
    manager.RemoveAnimation(id);
    manager.AddModifier(modifier);
    manager.Draw();
}

/**
 * @tc.name: ModifierManager005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ModifierManager005, TestSize.Level1)
{
    RSModifierManager manager;
    AnimationId id = 0;
    manager.Draw();

    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    auto animation = std::make_shared<RSRenderAnimation>(id);
    manager.AddAnimation(animation);
    manager.RemoveAnimation(id);
    manager.AddModifier(modifier);
    manager.Draw();
}

/**
 * @tc.name: ModifierManager006
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ModifierManager006, TestSize.Level1)
{
    RSModifierManager manager;
    int64_t time = 0;
    manager.Draw();

    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    manager.Animate(time);
    manager.AddModifier(modifier);
    manager.Draw();
}

/**
 * @tc.name: ModifierManager007
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, ModifierManager007, TestSize.Level1)
{
    RSModifierManager manager;
    int64_t time = 10;
    manager.Draw();

    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    manager.Animate(time);
    manager.AddModifier(modifier);
    manager.Draw();
}
} // namespace OHOS::Rosen
