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

class RsModifiersTest : public testing::Test {
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

void RsModifiersTest::SetUpTestCase() {}
void RsModifiersTest::TearDownTestCase() {}
void RsModifiersTest::SetUp() {}
void RsModifiersTest::TearDown() {}

/**
 * @tc.name: AddModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, AddModifier01, TestSize.Level1)
{
    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    auto node = RSCanvasNode::Create();

    node->AddModifier(modifier);
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
}

/**
 * @tc.name: AddModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, AddModifier02, TestSize.Level1)
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
 * @tc.name: Modifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, Modifier01, TestSize.Level1)
{
    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    ASSERT_TRUE(modifier != nullptr);
    ASSERT_EQ(modifier->GetPropertyId(), prop->GetId());
}


/**
 * @tc.name: Modifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, Modifier02, TestSize.Level1)
{
    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    ASSERT_TRUE(modifier != nullptr);
    ASSERT_EQ(std::static_pointer_cast<RSAnimatableProperty<float>>(modifier->GetProperty())->Get(), floatData[0]);

    prop->Set(floatData[1]);
    ASSERT_EQ(std::static_pointer_cast<RSAnimatableProperty<float>>(modifier->GetProperty())->Get(), floatData[1]);
}
/**
 * @tc.name: FrameModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, FrameModifier01, TestSize.Level1)
{
    auto val = Vector4f(10.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(val);
    auto modifier = std::make_shared<RSFrameModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetFrame(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetFrame(), node1->GetStagingProperties().GetFrame());
}

/**
 * @tc.name: FrameModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, FrameModifier02, TestSize.Level1)
{
    auto val = Vector4f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(val);
    auto modifier = std::make_shared<RSFrameModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetFrame(), val);
    auto str = node->GetStagingProperties().Dump();

    val = Vector4f(0.f, 0.f, 50.f, 60.f);
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetFrame(), val);
    str = node->GetStagingProperties().Dump();
}

/**
 * @tc.name: PositionZModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, PositionZModifier01, TestSize.Level1)
{
    auto val = 2.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSPositionZModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetPositionZ(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetPositionZ(), node1->GetStagingProperties().GetPositionZ());
}

/**
 * @tc.name: PositionZModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, PositionZModifier02, TestSize.Level1)
{
    auto val = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSPositionZModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetPositionZ(), val);

    val = -1.f;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetPositionZ(), val);
}

/**
 * @tc.name: BoundsModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BoundsModifier01, TestSize.Level1)
{
    auto val = Vector4f(10.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(val);
    auto modifier = std::make_shared<RSBoundsModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBounds(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBounds(), node1->GetStagingProperties().GetBounds());
}

/**
 * @tc.name: BoundsModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BoundsModifier02, TestSize.Level1)
{
    auto val = Vector4f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(val);
    auto modifier = std::make_shared<RSBoundsModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBounds(), val);
    auto str = node->GetStagingProperties().Dump();

    val = Vector4f(0.f, 0.f, 50.f, 60.f);
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetBounds(), val);
    str = node->GetStagingProperties().Dump();
}

/**
 * @tc.name: PivotModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, PivotModifier01, TestSize.Level1)
{
    auto val = Vector2f(1.f, 1.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(val);
    auto modifier = std::make_shared<RSPivotModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetPivot(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetPivot(), node1->GetStagingProperties().GetPivot());
}

/**
 * @tc.name: PivotModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, PivotModifier02, TestSize.Level1)
{
    auto val = Vector2f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(val);
    auto modifier = std::make_shared<RSPivotModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetPivot(), val);

    val = Vector2f(0.5f, 0.5f);
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetPivot(), val);
}

/**
 * @tc.name: RotationModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, RotationModifier01, TestSize.Level1)
{
    auto val = 90.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSRotationModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetRotation(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetRotation(), node1->GetStagingProperties().GetRotation());
}

/**
 * @tc.name: RotationModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, RotationModifier02, TestSize.Level1)
{
    auto val = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSRotationModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetRotation(), val);

    val = -360.f;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetRotation(), val);
}

/**
 * @tc.name: RotationXModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, RotationXModifier01, TestSize.Level1)
{
    auto val = 90.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSRotationXModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetRotationX(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetRotationX(), node1->GetStagingProperties().GetRotationX());
}

/**
 * @tc.name: RotationXModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, RotationXModifier02, TestSize.Level1)
{
    auto val = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSRotationXModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetRotationX(), val);

    val = -360.f;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetRotationX(), val);
}

/**
 * @tc.name: QuaternionModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, QuaternionModifier01, TestSize.Level1)
{
    auto val = Quaternion(0.382683, 0, 0, 0.92388);
    auto prop = std::make_shared<RSAnimatableProperty<Quaternion>>(val);
    auto modifier = std::make_shared<RSQuaternionModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetQuaternion(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetQuaternion(), node1->GetStagingProperties().GetQuaternion());
}

/**
 * @tc.name: QuaternionModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, QuaternionModifier02, TestSize.Level1)
{
    auto val = Quaternion();
    auto prop = std::make_shared<RSAnimatableProperty<Quaternion>>(val);
    auto modifier = std::make_shared<RSQuaternionModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetQuaternion(), val);

    val = Quaternion(0, 0, 0.382683, 0.92388);
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetQuaternion(), val);
}

/**
 * @tc.name: RotationYModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, RotationYModifier01, TestSize.Level1)
{
    auto val = 90.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSRotationYModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetRotationY(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetRotationY(), node1->GetStagingProperties().GetRotationY());
}

/**
 * @tc.name: RotationYModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, RotationYModifier02, TestSize.Level1)
{
    auto val = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSRotationYModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetRotationY(), val);

    val = -360.f;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetRotationY(), val);
}

/**
 * @tc.name: SkewModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, SkewModifier01, TestSize.Level1)
{
    auto val = Vector2f(2.f, 2.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(val);
    auto modifier = std::make_shared<RSSkewModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetSkew(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetSkew(), node1->GetStagingProperties().GetSkew());
}

/**
 * @tc.name: SkewModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, SkewModifier02, TestSize.Level1)
{
    auto val = Vector2f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(val);
    auto modifier = std::make_shared<RSSkewModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetSkew(), val);

    val = Vector2f(0.5f, 0.5f);
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetSkew(), val);
}

/**
 * @tc.name: PerspModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, PerspModifier01, TestSize.Level1)
{
    auto val = Vector2f(FLOAT_DATA_INIT, FLOAT_DATA_INIT);
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(val);
    auto modifier = std::make_shared<RSPerspModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetPersp(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetPersp(), node1->GetStagingProperties().GetPersp());
}

/**
 * @tc.name: PerspModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, PerspModifier02, TestSize.Level1)
{
    auto val = Vector2f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(val);
    auto modifier = std::make_shared<RSPerspModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetPersp(), val);

    val = Vector2f(FLOAT_DATA_INIT, FLOAT_DATA_INIT);
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetPersp(), val);
}

/**
 * @tc.name: ScaleModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ScaleModifier01, TestSize.Level1)
{
    auto val = Vector2f(2.f, 2.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(val);
    auto modifier = std::make_shared<RSScaleModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetScale(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetScale(), node1->GetStagingProperties().GetScale());
}

/**
 * @tc.name: ScaleModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ScaleModifier02, TestSize.Level1)
{
    auto val = Vector2f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(val);
    auto modifier = std::make_shared<RSScaleModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetScale(), val);

    val = Vector2f(0.5f, 0.5f);
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetScale(), val);
}

/**
 * @tc.name: TranslateModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, TranslateModifier01, TestSize.Level1)
{
    auto val = Vector2f(20.f, 30.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(val);
    auto modifier = std::make_shared<RSTranslateModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetTranslate(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetTranslate(), node1->GetStagingProperties().GetTranslate());
}

/**
 * @tc.name: TranslateModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, TranslateModifier02, TestSize.Level1)
{
    auto val = Vector2f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(val);
    auto modifier = std::make_shared<RSTranslateModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetTranslate(), val);

    val = Vector2f(-20.f, -30.f);
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetTranslate(), val);
}

/**
 * @tc.name: TranslateZModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, TranslateZModifier01, TestSize.Level1)
{
    auto val = 20.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSTranslateZModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetTranslateZ(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetTranslateZ(), node1->GetStagingProperties().GetTranslateZ());
}

/**
 * @tc.name: TranslateZModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, TranslateZModifier02, TestSize.Level1)
{
    auto val = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSTranslateZModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetTranslateZ(), val);

    val = -10.f;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetTranslateZ(), val);
}

/**
 * @tc.name: CornerRadiusModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, CornerRadiusModifier01, TestSize.Level1)
{
    auto val = Vector4f(20.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(val);
    auto modifier = std::make_shared<RSCornerRadiusModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetCornerRadius(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetCornerRadius(), node1->GetStagingProperties().GetCornerRadius());
}

/**
 * @tc.name: CornerRadiusModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, CornerRadiusModifier02, TestSize.Level1)
{
    auto val = Vector4f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(val);
    auto modifier = std::make_shared<RSCornerRadiusModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetCornerRadius(), val);

    val = Vector4f(-10.f);
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetCornerRadius(), val);
}

/**
 * @tc.name: AlphaModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, AlphaModifier01, TestSize.Level1)
{
    auto val = 0.5f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetAlpha(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetAlpha(), node1->GetStagingProperties().GetAlpha());
}

/**
 * @tc.name: AlphaModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, AlphaModifier02, TestSize.Level1)
{
    auto val = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetAlpha(), val);
    auto str = node->GetStagingProperties().Dump();

    val = -1.f;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetAlpha(), val);
    str = node->GetStagingProperties().Dump();
}

/**
 * @tc.name: AlphaOffscreenModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, AlphaOffscreenModifier01, TestSize.Level1)
{
    bool val = false;
    auto prop = std::make_shared<RSProperty<bool>>(val);
    auto modifier = std::make_shared<RSAlphaOffscreenModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetAlphaOffscreen(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetAlphaOffscreen(), node1->GetStagingProperties().GetAlphaOffscreen());
}

/**
 * @tc.name: AlphaOffscreenModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, AlphaOffscreenModifier02, TestSize.Level1)
{
    bool val = false;
    auto prop = std::make_shared<RSProperty<bool>>(val);
    auto modifier = std::make_shared<RSAlphaOffscreenModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetAlphaOffscreen(), val);

    val = true;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetAlphaOffscreen(), val);
}

/**
 * @tc.name: ForegroundColorModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ForegroundColorModifier01, TestSize.Level1)
{
    auto val = RgbPalette::Black();
    auto prop = std::make_shared<RSAnimatableProperty<Color>>(val);
    auto modifier = std::make_shared<RSForegroundColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetForegroundColor(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetForegroundColor(), node1->GetStagingProperties().GetForegroundColor());
}

/**
 * @tc.name: ForegroundColorModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ForegroundColorModifier02, TestSize.Level1)
{
    auto val = RgbPalette::Transparent();
    auto prop = std::make_shared<RSAnimatableProperty<Color>>(val);
    auto modifier = std::make_shared<RSForegroundColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetForegroundColor(), val);

    val = RgbPalette::Red();
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetForegroundColor(), val);
}

/**
 * @tc.name: BackgroundColorModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BackgroundColorModifier01, TestSize.Level1)
{
    auto val = RgbPalette::Black();
    auto prop = std::make_shared<RSAnimatableProperty<Color>>(val);
    auto modifier = std::make_shared<RSBackgroundColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundColor(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundColor(), node1->GetStagingProperties().GetBackgroundColor());
}

/**
 * @tc.name: BackgroundColorModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BackgroundColorModifier02, TestSize.Level1)
{
    auto val = RgbPalette::Transparent();
    auto prop = std::make_shared<RSAnimatableProperty<Color>>(val);
    auto modifier = std::make_shared<RSBackgroundColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundColor(), val);
    auto str = node->GetStagingProperties().Dump();

    val = RgbPalette::Green();
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundColor(), val);
    str = node->GetStagingProperties().Dump();
}

/**
 * @tc.name: BackgroundShaderModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BackgroundShaderModifier01, TestSize.Level1)
{
    auto val = RSShader::CreateRSShader();
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSShader>>>(val);
    auto modifier = std::make_shared<RSBackgroundShaderModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundShader(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundShader(), node1->GetStagingProperties().GetBackgroundShader());
}

/**
 * @tc.name: BackgroundShaderModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BackgroundShaderModifier02, TestSize.Level1)
{
    std::shared_ptr<RSShader> val = nullptr;
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSShader>>>(val);
    auto modifier = std::make_shared<RSBackgroundShaderModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundShader(), val);

    val = RSShader::CreateRSShader();
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundShader(), val);
}

/**
 * @tc.name: BgImageModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BgImageModifier01, TestSize.Level1)
{
    auto val = std::make_shared<RSImage>();
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSImage>>>(val);
    auto modifier = std::make_shared<RSBgImageModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImage(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBgImage(), node1->GetStagingProperties().GetBgImage());
}

/**
 * @tc.name: BgImageModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BgImageModifier02, TestSize.Level1)
{
    std::shared_ptr<RSImage> val = nullptr;
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSImage>>>(val);
    auto modifier = std::make_shared<RSBgImageModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImage(), val);

    val = std::make_shared<RSImage>();
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetBgImage(), val);
}

/**
 * @tc.name: BgImageWidthModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BgImageWidthModifier01, TestSize.Level1)
{
    auto val = 20.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSBgImageWidthModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImageWidth(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBgImageWidth(), node1->GetStagingProperties().GetBgImageWidth());
}

/**
 * @tc.name: BgImageWidthModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BgImageWidthModifier02, TestSize.Level1)
{
    auto val = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSBgImageWidthModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImageWidth(), val);

    val = -10.f;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetBgImageWidth(), val);
}

/**
 * @tc.name: BgImageHeightModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BgImageHeightModifier01, TestSize.Level1)
{
    auto val = 20.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSBgImageHeightModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImageHeight(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBgImageHeight(), node1->GetStagingProperties().GetBgImageHeight());
}

/**
 * @tc.name: BgImageHeightModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BgImageHeightModifier02, TestSize.Level1)
{
    auto val = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSBgImageHeightModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImageHeight(), val);

    val = -10.f;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetBgImageHeight(), val);
}

/**
 * @tc.name: BgImagePositionXModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BgImagePositionXModifier01, TestSize.Level1)
{
    auto val = 20.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSBgImagePositionXModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionX(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionX(), node1->GetStagingProperties().GetBgImagePositionX());
}

/**
 * @tc.name: BgImagePositionXModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BgImagePositionXModifier02, TestSize.Level1)
{
    auto val = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSBgImagePositionXModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionX(), val);

    val = -10.f;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionX(), val);
}

/**
 * @tc.name: BgImagePositionYModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BgImagePositionYModifier01, TestSize.Level1)
{
    auto val = 20.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSBgImagePositionYModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionY(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionY(), node1->GetStagingProperties().GetBgImagePositionY());
}

/**
 * @tc.name: BgImagePositionYModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BgImagePositionYModifier02, TestSize.Level1)
{
    auto val = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSBgImagePositionYModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionY(), val);

    val = -10.f;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetBgImagePositionY(), val);
}

/**
 * @tc.name: BorderColorModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BorderColorModifier01, TestSize.Level1)
{
    auto val = Vector4<Color>(RgbPalette::Black());
    auto prop = std::make_shared<RSAnimatableProperty<Vector4<Color>>>(val);
    auto modifier = std::make_shared<RSBorderColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBorderColor(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBorderColor(), node1->GetStagingProperties().GetBorderColor());
}

/**
 * @tc.name: BorderColorModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BorderColorModifier02, TestSize.Level1)
{
    auto val = Vector4<Color>(RgbPalette::Transparent());
    auto prop = std::make_shared<RSAnimatableProperty<Vector4<Color>>>(val);
    auto modifier = std::make_shared<RSBorderColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBorderColor(), val);

    val = Vector4<Color>(RgbPalette::Transparent(), RgbPalette::Red(), RgbPalette::Green(), RgbPalette::Blue());
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetBorderColor(), val);
}

/**
 * @tc.name: BorderWidthModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BorderWidthModifier01, TestSize.Level1)
{
    auto val = Vector4f(10.f, 10.f, 20.f, 30.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(val);
    auto modifier = std::make_shared<RSBorderWidthModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBorderWidth(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBorderWidth(), node1->GetStagingProperties().GetBorderWidth());
}

/**
 * @tc.name: BorderWidthModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BorderWidthModifier02, TestSize.Level1)
{
    auto val = Vector4f();
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(val);
    auto modifier = std::make_shared<RSBorderWidthModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBorderWidth(), val);

    val = Vector4f(-10.f);
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetBorderWidth(), val);
}

/**
 * @tc.name: BorderStyleModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BorderStyleModifier01, TestSize.Level1)
{
    auto val = Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::SOLID));
    auto prop = std::make_shared<RSProperty<Vector4<uint32_t>>>(val);
    auto modifier = std::make_shared<RSBorderStyleModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBorderStyle(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBorderStyle(), node1->GetStagingProperties().GetBorderStyle());
}

/**
 * @tc.name: BorderStyleModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BorderStyleModifier02, TestSize.Level1)
{
    auto val = Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::NONE));
    auto prop = std::make_shared<RSProperty<Vector4<uint32_t>>>(val);
    auto modifier = std::make_shared<RSBorderStyleModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBorderStyle(), val);

    val = Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::DOTTED));
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetBorderStyle(), val);
}

/**
 * @tc.name: FilterModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, FilterModifier01, TestSize.Level1)
{
    auto val = RSFilter::CreateBlurFilter(10.f, 10.f);
    auto prop = std::make_shared<RSAnimatableProperty<std::shared_ptr<RSFilter>>>(val);
    auto modifier = std::make_shared<RSFilterModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetFilter(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetFilter(), node1->GetStagingProperties().GetFilter());
}

/**
 * @tc.name: FilterModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, FilterModifier02, TestSize.Level1)
{
    std::shared_ptr<RSFilter> val = nullptr;
    auto prop = std::make_shared<RSAnimatableProperty<std::shared_ptr<RSFilter>>>(val);
    auto modifier = std::make_shared<RSFilterModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetFilter(), val);

    val = RSFilter::CreateBlurFilter(20.f, 30.f);
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetFilter(), val);
}

/**
 * @tc.name: BackgroundFilterModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BackgroundFilterModifier01, TestSize.Level1)
{
    auto val = RSFilter::CreateBlurFilter(10.f, 10.f);
    auto prop = std::make_shared<RSAnimatableProperty<std::shared_ptr<RSFilter>>>(val);
    auto modifier = std::make_shared<RSBackgroundFilterModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundFilter(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundFilter(), node1->GetStagingProperties().GetBackgroundFilter());
}

/**
 * @tc.name: BackgroundFilterModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, BackgroundFilterModifier02, TestSize.Level1)
{
    std::shared_ptr<RSFilter> val = nullptr;
    auto prop = std::make_shared<RSAnimatableProperty<std::shared_ptr<RSFilter>>>(val);
    auto modifier = std::make_shared<RSBackgroundFilterModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundFilter(), val);

    val = RSFilter::CreateBlurFilter(20.f, 30.f);
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetBackgroundFilter(), val);
}

/**
 * @tc.name: FrameGravityModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, FrameGravityModifier01, TestSize.Level1)
{
    auto val = Gravity::CENTER;
    auto prop = std::make_shared<RSProperty<Gravity>>(val);
    auto modifier = std::make_shared<RSFrameGravityModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetFrameGravity(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetFrameGravity(), node1->GetStagingProperties().GetFrameGravity());
}

/**
 * @tc.name: FrameGravityModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, FrameGravityModifier02, TestSize.Level1)
{
    auto val = Gravity::DEFAULT;
    auto prop = std::make_shared<RSProperty<Gravity>>(val);
    auto modifier = std::make_shared<RSFrameGravityModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetFrameGravity(), val);

    val = Gravity::RESIZE_ASPECT;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetFrameGravity(), val);
}

/**
 * @tc.name: ClipBoundsModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ClipBoundsModifier01, TestSize.Level1)
{
    auto val = RSPath::CreateRSPath();
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSPath>>>(val);
    auto modifier = std::make_shared<RSClipBoundsModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetClipBounds(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetClipBounds(), node1->GetStagingProperties().GetClipBounds());
}

/**
 * @tc.name: ClipBoundsModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ClipBoundsModifier02, TestSize.Level1)
{
    std::shared_ptr<RSPath> val = nullptr;
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSPath>>>(val);
    auto modifier = std::make_shared<RSClipBoundsModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetClipBounds(), val);

    val = RSPath::CreateRSPath();
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetClipBounds(), val);
}

/**
 * @tc.name: ClipToBoundsModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ClipToBoundsModifier01, TestSize.Level1)
{
    auto val = true;
    auto prop = std::make_shared<RSProperty<bool>>(val);
    auto modifier = std::make_shared<RSClipToBoundsModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetClipToBounds(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetClipToBounds(), node1->GetStagingProperties().GetClipToBounds());
}

/**
 * @tc.name: ClipToBoundsModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ClipToBoundsModifier02, TestSize.Level1)
{
    auto val = false;
    auto prop = std::make_shared<RSProperty<bool>>(val);
    auto modifier = std::make_shared<RSClipToBoundsModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetClipToBounds(), val);

    val = true;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetClipToBounds(), val);
}

/**
 * @tc.name: VisibleModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, VisibleModifier01, TestSize.Level1)
{
    auto val = false;
    auto prop = std::make_shared<RSProperty<bool>>(val);
    auto modifier = std::make_shared<RSVisibleModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetVisible(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetVisible(), node1->GetStagingProperties().GetVisible());
}

/**
 * @tc.name: VisibleModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, VisibleModifier02, TestSize.Level1)
{
    auto val = true;
    auto prop = std::make_shared<RSProperty<bool>>(val);
    auto modifier = std::make_shared<RSVisibleModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetVisible(), val);
    auto str = node->GetStagingProperties().Dump();

    val = false;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetVisible(), val);
    str = node->GetStagingProperties().Dump();
}

/**
 * @tc.name: ShadowColorModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ShadowColorModifier01, TestSize.Level1)
{
    auto val = RgbPalette::Black();
    auto prop = std::make_shared<RSAnimatableProperty<Color>>(val);
    auto modifier = std::make_shared<RSShadowColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowColor(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetShadowColor(), node1->GetStagingProperties().GetShadowColor());
}

/**
 * @tc.name: ShadowColorModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ShadowColorModifier02, TestSize.Level1)
{
    auto val = Color::FromArgbInt(DEFAULT_SPOT_COLOR);
    auto prop = std::make_shared<RSAnimatableProperty<Color>>(val);
    auto modifier = std::make_shared<RSShadowColorModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowColor(), val);

    val = RgbPalette::Red();
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetShadowColor(), val);
}

/**
 * @tc.name: ClipToFrameModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ClipToFrameModifier01, TestSize.Level1)
{
    auto val = true;
    auto prop = std::make_shared<RSProperty<bool>>(val);
    auto modifier = std::make_shared<RSClipToFrameModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetClipToFrame(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetClipToFrame(), node1->GetStagingProperties().GetClipToFrame());
}

/**
 * @tc.name: ClipToFrameModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ClipToFrameModifier02, TestSize.Level1)
{
    auto val = false;
    auto prop = std::make_shared<RSProperty<bool>>(val);
    auto modifier = std::make_shared<RSClipToFrameModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetClipToFrame(), val);

    val = true;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetClipToFrame(), val);
}

/**
 * @tc.name: ShadowOffsetXModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ShadowOffsetXModifier01, TestSize.Level1)
{
    auto val = 5.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSShadowOffsetXModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetX(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetX(), node1->GetStagingProperties().GetShadowOffsetX());
}

/**
 * @tc.name: ShadowOffsetXModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ShadowOffsetXModifier02, TestSize.Level1)
{
    auto val = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSShadowOffsetXModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetX(), val);

    val = -1.f;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetX(), val);
}

/**
 * @tc.name: ShadowOffsetYModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ShadowOffsetYModifier01, TestSize.Level1)
{
    auto val = 5.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSShadowOffsetYModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(prop->Get(), val);
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetY(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetY(), node1->GetStagingProperties().GetShadowOffsetY());
}

/**
 * @tc.name: ShadowOffsetYModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ShadowOffsetYModifier02, TestSize.Level1)
{
    auto val = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSShadowOffsetYModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetY(), val);

    val = -1.f;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetShadowOffsetY(), val);
}

/**
 * @tc.name: ShadowAlphaModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ShadowAlphaModifier01, TestSize.Level1)
{
    auto val = 0.8f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSShadowAlphaModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowAlpha(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetShadowAlpha(), node1->GetStagingProperties().GetShadowAlpha());
}

/**
 * @tc.name: ShadowAlphaModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ShadowAlphaModifier02, TestSize.Level1)
{
    auto val = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSShadowAlphaModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowAlpha(), val);

    val = -0.2f;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetShadowAlpha(), val);
}

/**
 * @tc.name: ShadowElevationModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ShadowElevationModifier01, TestSize.Level1)
{
    auto val = 5.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSShadowElevationModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowElevation(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetShadowElevation(), node1->GetStagingProperties().GetShadowElevation());
}

/**
 * @tc.name: ShadowElevationModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ShadowElevationModifier02, TestSize.Level1)
{
    auto val = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSShadowElevationModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowElevation(), val);

    val = -1.f;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetShadowElevation(), val);
}

/**
 * @tc.name: ShadowRadiusModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ShadowRadiusModifier01, TestSize.Level1)
{
    auto val = 10.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSShadowRadiusModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowRadius(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetShadowRadius(), node1->GetStagingProperties().GetShadowRadius());
}

/**
 * @tc.name: ShadowRadiusModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ShadowRadiusModifier02, TestSize.Level1)
{
    auto val = 0.f;
    auto prop = std::make_shared<RSAnimatableProperty<float>>(val);
    auto modifier = std::make_shared<RSShadowRadiusModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowRadius(), val);

    val = -10.f;
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetShadowRadius(), val);
}

/**
 * @tc.name: ShadowPathModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ShadowPathModifier01, TestSize.Level1)
{
    auto val = RSPath::CreateRSPath();
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSPath>>>(val);
    auto modifier = std::make_shared<RSShadowPathModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowPath(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetShadowPath(), node1->GetStagingProperties().GetShadowPath());
}

/**
 * @tc.name: ShadowPathModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ShadowPathModifier02, TestSize.Level1)
{
    std::shared_ptr<RSPath> val = nullptr;
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSPath>>>(val);
    auto modifier = std::make_shared<RSShadowPathModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetShadowPath(), val);

    val = RSPath::CreateRSPath();
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetShadowPath(), val);
}

/**
 * @tc.name: MaskModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, MaskModifier01, TestSize.Level1)
{
    auto val = RSMask::CreateGradientMask(Drawing::Brush());
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSMask>>>(val);
    auto modifier = std::make_shared<RSMaskModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetMask(), val);

    node->RemoveModifier(modifier);
    auto node1 = RSCanvasNode::Create();
    ASSERT_EQ(node->GetStagingProperties().GetMask(), node1->GetStagingProperties().GetMask());
}

/**
 * @tc.name: MaskModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, MaskModifier02, TestSize.Level1)
{
    std::shared_ptr<RSMask> val = nullptr;
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSMask>>>(val);
    auto modifier = std::make_shared<RSMaskModifier>(prop);

    auto node = RSCanvasNode::Create();
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetMask(), val);

    val = RSMask::CreateGradientMask(Drawing::Brush());
    prop->Set(val);
    ASSERT_EQ(node->GetStagingProperties().GetMask(), val);
}

/**
 * @tc.name: ModifierManager01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ModifierManager01, TestSize.Level1)
{
    RSModifierManager manager;
    manager.Draw();

    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    manager.AddModifier(modifier);
    manager.Draw();
}

/**
 * @tc.name: ModifierManager02
 * @tc.desc: animation is exit
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ModifierManager02, TestSize.Level1)
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
 * @tc.name: ModifierManager03
 * @tc.desc: animation is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ModifierManager03, TestSize.Level1)
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
 * @tc.name: ModifierManager04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ModifierManager04, TestSize.Level1)
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
 * @tc.name: ModifierManager05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, ModifierManager05, TestSize.Level1)
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

} // namespace OHOS::Rosen
