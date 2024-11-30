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
constexpr static float FLOAT_DATA_INIT = 0.5f;

class RSModifierUnitTest : public testing::Test {
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

void RSModifierUnitTest::SetUpTestCase() {}
void RSModifierUnitTest::TearDownTestCase() {}
void RSModifierUnitTest::SetUp() {}
void RSModifierUnitTest::TearDown() {}

/**
 * @tc.name: Modifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierUnitTest, Modifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, Modifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, AddModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, AddModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, BoundsModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, BoundsModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, FrameModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, FrameModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, PositionZModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, PositionZModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, PivotModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, PivotModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, QuaternionModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, QuaternionModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, RotationModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, RotationModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, RotationXModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, RotationXModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, RotationYModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, RotationYModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, ScaleModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, ScaleModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, SkewModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, SkewModifier002, TestSize.Level1)
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
 * @tc.name: PerspModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierUnitTest, PerspModifier001, TestSize.Level1)
{
    auto value = Vector2f(FLOAT_DATA_INIT, FLOAT_DATA_INIT);
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
    auto modifier = std::make_shared<RSPerspModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetPersp(), value);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetPersp(), node1->GetStagingProperties().GetPersp());
}

/**
 * @tc.name: PerspModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierUnitTest, PerspModifier002, TestSize.Level1)
{
    auto value = Vector2f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
    auto modifier = std::make_shared<RSPerspModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetPersp(), value);

    value = Vector2f(FLOAT_DATA_INIT, FLOAT_DATA_INIT);
    prop->Set(value);
    ASSERT_EQ(node->GetStagingProperties().GetPersp(), value);
}

/**
 * @tc.name: TranslateModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierUnitTest, TranslateModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, TranslateModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, TranslateZModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, TranslateZModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, CornerRadiusModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, CornerRadiusModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, AlphaModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, AlphaModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, AlphaOffscreenModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, AlphaOffscreenModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, ForegroundColorModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, ForegroundColorModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, BackgroundColorModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, BackgroundColorModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, BackgroundShaderModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, BackgroundShaderModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, BgImageModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, BgImageModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, BgImageWidthModifier001, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, BgImageWidthModifier002, TestSize.Level1)
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
HWTEST_F(RSModifierUnitTest, BgImageHeightModifier001, TestSize.Level1)
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
} // namespace OHOS::Rosen