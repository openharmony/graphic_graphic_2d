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

#include "animation/rs_animation_trace_utils.h"
#include "common/rs_color.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_root_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderAnimationDebugTraceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t NODE_ID = 123456;
    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    static constexpr RSModifierType MODIFIER_TYPE = RSModifierType::BOUNDS;
    static constexpr ImplicitAnimationParamType ANIMATION_TYPE = ImplicitAnimationParamType::CURVE;
    const std::string NODE_NAME = "test";
    const std::string INTERFACE_NAME = "test_name";
    RSUINodeType NODE_TYPE = RSUINodeType::RS_NODE;
};

void RSRenderAnimationDebugTraceTest::SetUpTestCase() {}
void RSRenderAnimationDebugTraceTest::TearDownTestCase() {}
void RSRenderAnimationDebugTraceTest::SetUp() {}
void RSRenderAnimationDebugTraceTest::TearDown() {}

/**
 * @tc.name: AnimationDebugTrace001
 * @tc.desc: Verify the AnimationDebugTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, AnimationDebugTrace001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace001 start";

    RSAnimationTraceUtils::GetInstance().AddAnimationNameTrace(NODE_NAME);
    RSAnimationTraceUtils::GetInstance().AddAnimationFinishTrace(NODE_NAME, NODE_ID, ANIMATION_ID, false);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(100.0f);
    auto renderNode = std::make_shared<RSRenderNode>(NODE_ID);
    int repeatCount = 1;
    RSAnimationTraceUtils::GetInstance().AddAnimationFrameTrace(
        renderNode.get(), ANIMATION_ID, PROPERTY_ID, 0, startValue, 0, 0, repeatCount);
    RSAnimationTraceUtils::GetInstance().AddSpringInitialVelocityTrace(PROPERTY_ID, ANIMATION_ID, startValue, endValue);
    EXPECT_TRUE(renderNode->GetNodeName().empty());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace001 end";
}

/**
 * @tc.name: AnimationDebugTrace002
 * @tc.desc: Verify the AnimationDebugTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, AnimationDebugTrace002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace002 start";

    system("param set persist.rosen.animationtrace.enabled 1");
    RSAnimationTraceUtils::GetInstance().AddAnimationNameTrace(NODE_NAME);
    RSAnimationTraceUtils::GetInstance().AddAnimationFinishTrace(NODE_NAME, NODE_ID, ANIMATION_ID, true);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(100.0f);
    auto renderNode = std::make_shared<RSRenderNode>(NODE_ID);
    RSAnimationTraceUtils::GetInstance().AddAnimationFrameTrace(
        renderNode.get(), ANIMATION_ID, PROPERTY_ID, 0, startValue, 0, 0, 1);
    RSAnimationTraceUtils::GetInstance().AddSpringInitialVelocityTrace(PROPERTY_ID, ANIMATION_ID, startValue, endValue);
    system("param set persist.rosen.animationtrace.enabled 0");
    EXPECT_TRUE(renderNode->GetNodeName().empty());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace002 end";
}

/**
 * @tc.name: AnimationDebugTrace003
 * @tc.desc: Verify the AnimationDebugTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, AnimationDebugTrace003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace003 start";

    RSAnimationTraceUtils::isDebugEnabled_ = true;
    RSAnimationTraceUtils::GetInstance().AddAnimationNameTrace(NODE_NAME);
    RSAnimationTraceUtils::GetInstance().AddAnimationFinishTrace(NODE_NAME, NODE_ID, ANIMATION_ID, true);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(100.0f);
    auto renderNode = std::make_shared<RSRenderNode>(NODE_ID);
    RSAnimationTraceUtils::GetInstance().AddAnimationFrameTrace(
        renderNode.get(), ANIMATION_ID, PROPERTY_ID, 0, startValue, 0, 0, 1);
    RSAnimationTraceUtils::GetInstance().AddSpringInitialVelocityTrace(PROPERTY_ID, ANIMATION_ID, startValue, endValue);
    EXPECT_TRUE(renderNode->GetNodeName().empty());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace003 end";
}

/**
 * @tc.name: AddAnimationFinishTrace
 * @tc.desc: Verify the AddAnimationFinishTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, AddAnimationFinishTrace, TestSize.Level1)
{
    RSAnimationTraceUtils::isDebugEnabled_ = true;
    auto renderNode = std::make_shared<RSRenderNode>(NODE_ID);
    RSAnimationTraceUtils::GetInstance().AddAnimationFinishTrace(NODE_NAME, NODE_ID, ANIMATION_ID, true);

    RSAnimationTraceUtils::isDebugEnabled_ = false;
    OHOS::Rosen::RSSystemProperties::SetDebugFmtTraceEnabled(true);
    EXPECT_TRUE(OHOS::Rosen::RSSystemProperties::GetDebugFmtTraceEnabled());
    RSAnimationTraceUtils::GetInstance().AddAnimationFinishTrace(NODE_NAME, NODE_ID, ANIMATION_ID, true);
    EXPECT_TRUE(renderNode->GetNodeName().empty());
}

/**
 * @tc.name: AddAnimationFrameTrace
 * @tc.desc: Verify the AddAnimationFrameTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, AddAnimationFrameTrace, TestSize.Level1)
{
    RSAnimationTraceUtils::isDebugEnabled_ = true;
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto renderNode = std::make_shared<RSRenderNode>(NODE_ID);
    renderNode->SetNodeName(NODE_NAME);
    RSAnimationTraceUtils::GetInstance().AddAnimationFrameTrace(
        renderNode.get(), ANIMATION_ID, PROPERTY_ID, 0, startValue, 0, 0, -1);
    EXPECT_FALSE(renderNode->GetNodeName().empty());

    // renderNode is nullptr
    RSAnimationTraceUtils::GetInstance().AddAnimationFrameTrace(
        nullptr, ANIMATION_ID, PROPERTY_ID, 0, startValue, 0, 0, -1);

    RSAnimationTraceUtils::isDebugEnabled_ = false;
    OHOS::Rosen::RSSystemProperties::SetDebugFmtTraceEnabled(true);
    EXPECT_TRUE(OHOS::Rosen::RSSystemProperties::GetDebugFmtTraceEnabled());
    RSAnimationTraceUtils::GetInstance().AddAnimationFrameTrace(
        renderNode.get(), ANIMATION_ID, PROPERTY_ID, 0, startValue, 0, 0, -1);

    // isOnTheTree is true
    RSAnimationTraceUtils::isDebugEnabled_ = true;
    renderNode->isOnTheTree_ = true;
    RSAnimationTraceUtils::GetInstance().AddAnimationFrameTrace(
        renderNode.get(), ANIMATION_ID, PROPERTY_ID, 0, startValue, 0, 0, -1);
}

/**
 * @tc.name: ParseRenderPropertyValue001
 * @tc.desc: Verify the ParseRenderPropertyValue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, ParseRenderPropertyValue001, TestSize.Level1)
{
    float val = 1.0f;
    auto floatProperty = std::make_shared<RSRenderAnimatableProperty<float>>(val);
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyValue(floatProperty,
        RSPropertyType::FLOAT), "float:" + std::to_string(val));
    auto red = RgbPalette::Red();
    auto colorProperty = std::make_shared<RSRenderAnimatableProperty<Color>>(red);
    std::string colorString;
    colorProperty->Get().Dump(colorString);
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyValue(colorProperty,
        RSPropertyType::RS_COLOR), colorString);
    auto quaternionProperty = std::make_shared<RSRenderAnimatableProperty<Quaternion>>(
        Quaternion(1.0f, 2.0f, 3.0f, 4.0f));
    EXPECT_NE(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyValue(quaternionProperty,
        RSPropertyType::QUATERNION), "None");
    auto vector2fProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f(1.0f, 2.0f));
    EXPECT_NE(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyValue(vector2fProperty,
        RSPropertyType::VECTOR2F), "None");
    auto vector4fProperty = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(1.0f, 2.0f, 3.0f, 4.0f));
    EXPECT_NE(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyValue(vector4fProperty,
        RSPropertyType::VECTOR4F), "None");

    auto vector4ColorProperty = std::make_shared<RSRenderAnimatableProperty<Vector4<Color>>>(Vector4<Color>(red));
    std::string colorString1;
    vector4ColorProperty->Get().x_.Dump(colorString1);
    std::string colorString2;
    vector4ColorProperty->Get().y_.Dump(colorString2);
    std::string colorString3;
    vector4ColorProperty->Get().z_.Dump(colorString3);
    std::string colorString4;
    vector4ColorProperty->Get().w_.Dump(colorString4);
    std::string vector4ColorString = "Vector4<Color>:x:" + colorString1 + "," + "y:" + colorString2 + "," +
        "z:" + colorString3 + "," + "w:" + colorString4;
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyValue(vector4ColorProperty,
        RSPropertyType::VECTOR4_COLOR), vector4ColorString);

    RRect rect;
    std::string str = "RRECT " + rect.ToString();
    auto rrectProperty = std::make_shared<RSRenderAnimatableProperty<RRect>>(rect);
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyValue(rrectProperty,
        RSPropertyType::RRECT), str);
}

/**
 * @tc.name: AddAnimationCallFinishTrace
 * @tc.desc: Verify the AddAnimationCallFinishTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, AddAnimationCallFinishTrace, TestSize.Level1)
{
    RSAnimationTraceUtils::isDebugEnabled_ = true;
    RSAnimationTraceUtils::GetInstance().AddAnimationCallFinishTrace(NODE_ID, ANIMATION_ID, MODIFIER_TYPE, false);
    RSAnimationTraceUtils::GetInstance().AddAnimationCallFinishTrace(NODE_ID, ANIMATION_ID, MODIFIER_TYPE, true);

    RSAnimationTraceUtils::isDebugEnabled_ = false;
    RSAnimationTraceUtils::GetInstance().AddAnimationCallFinishTrace(NODE_ID, ANIMATION_ID, MODIFIER_TYPE, false);
    RSRenderNode renderNode(NODE_ID);
    EXPECT_TRUE(renderNode.GetNodeName().empty());

    RSAnimationTraceUtils::isDebugEnabled_ = false;
    OHOS::Rosen::RSSystemProperties::SetDebugFmtTraceEnabled(true);
    EXPECT_TRUE(OHOS::Rosen::RSSystemProperties::GetDebugFmtTraceEnabled());
    RSAnimationTraceUtils::GetInstance().AddAnimationCallFinishTrace(NODE_ID, ANIMATION_ID, MODIFIER_TYPE, false);
}

/**
 * @tc.name: AddAnimationCreateTrace
 * @tc.desc: Verify the AddAnimationCreateTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, AddAnimationCreateTrace, TestSize.Level1)
{
    RSAnimationTraceUtils::GetInstance().isDebugEnabled_ = true;
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(100.0f);
    int repeatCount = 1;
    RSAnimationTraceUtils::GetInstance().AddAnimationCreateTrace(NODE_ID, NODE_NAME, PROPERTY_ID, ANIMATION_ID,
        ANIMATION_TYPE, MODIFIER_TYPE, startValue, endValue, 0, 0, repeatCount, INTERFACE_NAME, NODE_ID, INTERFACE_NAME,
        NODE_TYPE);

    // nodeName empty
    RSAnimationTraceUtils::GetInstance().AddAnimationCreateTrace(NODE_ID, "", PROPERTY_ID, ANIMATION_ID, ANIMATION_TYPE,
        MODIFIER_TYPE, startValue, endValue, 0, 0, repeatCount, INTERFACE_NAME, NODE_ID, INTERFACE_NAME, NODE_TYPE);
    // animationDelay != 0
    int animationDelay = 1; // test animationDelay = 1
    RSAnimationTraceUtils::GetInstance().AddAnimationCreateTrace(NODE_ID, "", PROPERTY_ID, ANIMATION_ID, ANIMATION_TYPE,
        MODIFIER_TYPE, startValue, endValue, animationDelay, 0, repeatCount, INTERFACE_NAME, NODE_ID, INTERFACE_NAME,
        NODE_TYPE);
    repeatCount = -1; // test repeatCount = -1
    RSAnimationTraceUtils::GetInstance().AddAnimationCreateTrace(NODE_ID, "", PROPERTY_ID, ANIMATION_ID, ANIMATION_TYPE,
        MODIFIER_TYPE, startValue, endValue, animationDelay, 0, repeatCount, INTERFACE_NAME, NODE_ID, INTERFACE_NAME,
        NODE_TYPE);
    // interfaceName empty
    RSAnimationTraceUtils::GetInstance().AddAnimationCreateTrace(NODE_ID, "", PROPERTY_ID, ANIMATION_ID, ANIMATION_TYPE,
        MODIFIER_TYPE, startValue, endValue, animationDelay, 0, repeatCount, "", NODE_ID, INTERFACE_NAME, NODE_TYPE);

    RSAnimationTraceUtils::GetInstance().isDebugEnabled_ = false;
    RSAnimationTraceUtils::GetInstance().AddAnimationCreateTrace(NODE_ID, NODE_NAME, PROPERTY_ID, ANIMATION_ID,
        ANIMATION_TYPE, MODIFIER_TYPE, startValue, endValue, 0, 0, repeatCount, INTERFACE_NAME, NODE_ID, INTERFACE_NAME,
        NODE_TYPE);
    RSRenderNode renderNode(NODE_ID);
    RSAnimationTraceUtils::GetInstance().AddSpringInitialVelocityTrace(PROPERTY_ID, ANIMATION_ID, startValue, endValue);
    EXPECT_TRUE(renderNode.GetNodeName().empty());
}

/**
 * @tc.name: AddAnimationCancelTrace
 * @tc.desc: Verify the AddAnimationCancelTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, AddAnimationCancelTrace, TestSize.Level1)
{
    RSAnimationTraceUtils::GetInstance().isDebugEnabled_ = true;
    RSAnimationTraceUtils::GetInstance().AddAnimationCancelTrace(NODE_ID, PROPERTY_ID);

    RSAnimationTraceUtils::GetInstance().isDebugEnabled_ = false;
    RSAnimationTraceUtils::GetInstance().AddAnimationCancelTrace(NODE_ID, PROPERTY_ID);
    RSRenderNode renderNode(NODE_ID);
    EXPECT_TRUE(renderNode.GetNodeName().empty());
}

/**
 * @tc.name: AddChangeAnimationValueTrace
 * @tc.desc: Verify the AddChangeAnimationValueTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, AddChangeAnimationValueTrace, TestSize.Level1)
{
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(100.0f);
    RSAnimationTraceUtils::GetInstance().isDebugEnabled_ = true;
    RSAnimationTraceUtils::GetInstance().AddChangeAnimationValueTrace(PROPERTY_ID, endValue);

    RSAnimationTraceUtils::GetInstance().isDebugEnabled_ = false;
    RSAnimationTraceUtils::GetInstance().AddChangeAnimationValueTrace(PROPERTY_ID, endValue);
    RSRenderNode renderNode(NODE_ID);
    EXPECT_TRUE(renderNode.GetNodeName().empty());
}

/**
 * @tc.name: GetColorString
 * @tc.desc: Verify the GetColorString
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, GetColorString, TestSize.Level1)
{
    Color color = Color();
    std::string result = RSAnimationTraceUtils::GetInstance().GetColorString(color);
    std::string substring = "RGBA-0x"; // Color printed string header
    EXPECT_NE(result.find(substring), std::string::npos);
}

/**
 * @tc.name: OnAnimationTraceEnabledChangedCallback
 * @tc.desc: Verify the OnAnimationTraceEnabledChangedCallback
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, OnAnimationTraceEnabledChangedCallback, TestSize.Level1)
{
    const char* key = "wrong_key";
    const char* value = "1";
    RSAnimationTraceUtils::OnAnimationTraceEnabledChangedCallback(key, value, nullptr);
    EXPECT_FALSE(RSAnimationTraceUtils::isDebugEnabled_);

    // "persist.rosen.animationtrace.enabled" is animation debug trace switch
    const char* key1 = "persist.rosen.animationtrace.enabled";
    const char* value1 = "1";
    RSAnimationTraceUtils::OnAnimationTraceEnabledChangedCallback(key1, value1, nullptr);
    EXPECT_TRUE(RSAnimationTraceUtils::isDebugEnabled_);
}

/**
 * @tc.name: GetModifierTypeString
 * @tc.desc: Verify the GetModifierTypeString
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, GetModifierTypeString, TestSize.Level1)
{
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().GetModifierTypeString(RSModifierType::BOUNDS), "Bounds");

    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().GetModifierTypeString(RSModifierType::QUATERNION), "Quaternion");
}

/**
 * @tc.name: GetAnimationTypeString
 * @tc.desc: Verify the GetAnimationTypeString
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, GetAnimationTypeString, TestSize.Level1)
{
    EXPECT_EQ(
        RSAnimationTraceUtils::GetInstance().GetAnimationTypeString(ImplicitAnimationParamType::INVALID), "Invalid");

    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().GetAnimationTypeString(ImplicitAnimationParamType::CURVE), "Curve");

    EXPECT_EQ(
        RSAnimationTraceUtils::GetInstance().GetAnimationTypeString(ImplicitAnimationParamType::KEYFRAME), "Keyframe");

    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().GetAnimationTypeString(ImplicitAnimationParamType::PATH), "Path");

    EXPECT_EQ(
        RSAnimationTraceUtils::GetInstance().GetAnimationTypeString(ImplicitAnimationParamType::SPRING), "Spring");

    EXPECT_EQ(
        RSAnimationTraceUtils::GetInstance().GetAnimationTypeString(ImplicitAnimationParamType::INTERPOLATING_SPRING),
        "InterpolatingSpring");

    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().GetAnimationTypeString(ImplicitAnimationParamType::TRANSITION),
        "Transition");

    EXPECT_EQ(
        RSAnimationTraceUtils::GetInstance().GetAnimationTypeString(ImplicitAnimationParamType::CANCEL), "Cancel");

    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().GetAnimationTypeString(static_cast<ImplicitAnimationParamType>(-1)),
        "Invalid");
}

/**
 * @tc.name: GetNodeTypeString
 * @tc.desc: Verify the GetNodeTypeString
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, GetNodeTypeString, TestSize.Level1)
{
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().GetNodeTypeString(RSUINodeType::UNKNOW), "UNKNOW");

    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().GetNodeTypeString(RSUINodeType::DISPLAY_NODE), "DisplayNode");

    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().GetNodeTypeString(RSUINodeType::RS_NODE), "RsNode");

    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().GetNodeTypeString(RSUINodeType::SURFACE_NODE), "SurfaceNode");

    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().GetNodeTypeString(RSUINodeType::PROXY_NODE), "ProxyNode");

    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().GetNodeTypeString(RSUINodeType::CANVAS_NODE), "CanvasNode");

    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().GetNodeTypeString(RSUINodeType::ROOT_NODE), "RootNode");

    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().GetNodeTypeString(RSUINodeType::EFFECT_NODE), "EffectNode");

    EXPECT_EQ(
        RSAnimationTraceUtils::GetInstance().GetNodeTypeString(RSUINodeType::CANVAS_DRAWING_NODE), "CanvasDrawingNode");

    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().GetNodeTypeString(static_cast<RSUINodeType>(-1)), "UNKNOW");
}
} // namespace Rosen
} // namespace OHOS