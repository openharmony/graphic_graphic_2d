/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include <ostream>
#include "gtest/gtest.h"
#include "animation/rs_animation.h"
#include "core/transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_director.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

namespace {
constexpr float DEFAULT_NEAR_ZERO_THRESHOLD = 1.0f / 256.0f;
constexpr float FLOAT_NEAR_ZERO_COARSE_THRESHOLD = 1.0f / 256.0f;
constexpr float FLOAT_NEAR_ZERO_MEDIUM_THRESHOLD = 1.0f / 1000.0f;
constexpr float FLOAT_NEAR_ZERO_FINE_THRESHOLD = 1.0f / 3072.0f;
constexpr float COLOR_NEAR_ZERO_THRESHOLD = 0.0f;
constexpr float LAYOUT_NEAR_ZERO_THRESHOLD = 0.5f;
constexpr float ZERO = 0.0f;
} // namespace

class RSPropertyTest : public testing::Test {
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

void RSPropertyTest::SetUpTestCase() {}
void RSPropertyTest::TearDownTestCase() {}
void RSPropertyTest::SetUp() {}
void RSPropertyTest::TearDown() {}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, LifeCycle001, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop = std::make_shared<RSProperty<float>>();
    ASSERT_TRUE(prop != nullptr);
    ASSERT_TRUE(prop->GetId() != 0);
}

/**
 * @tc.name: Property001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, Property001, TestSize.Level1)
{
    auto prop = std::make_shared<RSProperty<float>>(floatData[0]);
    ASSERT_TRUE(prop != nullptr);
    ASSERT_EQ(prop->Get(), floatData[0]);

    prop->Set(floatData[1]);
    ASSERT_EQ(prop->Get(), floatData[1]);
}

/**
 * @tc.name: Property002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, Property002, TestSize.Level1)
{
    auto prop = std::make_shared<RSProperty<float>>(floatData[0]);
    ASSERT_TRUE(prop != nullptr);
    ASSERT_EQ(prop->Get(), floatData[0]);

    prop->Set(floatData[2]);
    ASSERT_EQ(prop->Get(), floatData[2]);
}

/**
 * @tc.name: Property003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, Property003, TestSize.Level1)
{
    auto prop = std::make_shared<RSProperty<float>>(UINT32_MAX);
    ASSERT_TRUE(prop != nullptr);
    ASSERT_EQ(prop->Get(), UINT32_MAX);

    prop->Set(floatData[2]);
    ASSERT_EQ(prop->Get(), floatData[2]);
}

/**
 * @tc.name: Property004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, Property004, TestSize.Level1)
{
    auto prop = std::make_shared<RSAnimatableProperty<float>>(UINT32_MAX);
    ASSERT_TRUE(prop != nullptr);
    ASSERT_EQ(prop->Get(), UINT32_MAX);

    prop->SetThresholdType(ThresholdType::COARSE);
    ASSERT_EQ(prop->GetThreshold(), 1.0f / 256.0f);
}

/**
 * @tc.name: PropertyOp001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, PropertyOp001, TestSize.Level1)
{
    std::shared_ptr<RSAnimatableProperty<float>> prop1 = nullptr;
    std::shared_ptr<RSAnimatableProperty<float>> prop2 = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    std::shared_ptr<RSAnimatableProperty<float>> prop3 = std::make_shared<RSAnimatableProperty<float>>(floatData[1]);

    prop1 += prop3;
    ASSERT_EQ(prop1, nullptr);

    prop2 += prop3;
    ASSERT_TRUE(ROSEN_EQ(prop2->Get(), floatData[0] + floatData[1]));

    prop1 -= prop3;
    ASSERT_EQ(prop1, nullptr);

    prop2 -= prop3;
    ASSERT_TRUE(ROSEN_EQ(prop2->Get(), floatData[0]));

    ASSERT_EQ(prop1 + prop3, nullptr);
    ASSERT_TRUE(ROSEN_EQ(
        std::static_pointer_cast<RSProperty<float>>(prop2 + prop3)->Get(), floatData[0] + floatData[1]));

    ASSERT_EQ(prop1 - prop3, nullptr);
    ASSERT_TRUE(ROSEN_EQ(
        std::static_pointer_cast<RSProperty<float>>(prop2 - prop3)->Get(), floatData[0] - floatData[1]));

    ASSERT_EQ(prop1 * floatData[1], nullptr);
    ASSERT_TRUE(ROSEN_EQ(
        std::static_pointer_cast<RSProperty<float>>(prop2 * floatData[1])->Get(), floatData[0] * floatData[1]));

    ASSERT_FALSE(prop1 == prop3);
    ASSERT_TRUE(prop1 != prop3);

    ASSERT_FALSE(prop2 == prop3);
    ASSERT_TRUE(prop2 != prop3);
}

/**
 * @tc.name: RSPropertyBase001
 * @tc.desc: operator+=001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator001, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    std::shared_ptr<RSPropertyBase> prop2 = std::make_shared<RSProperty<float>>();
    prop1 = nullptr;
    ASSERT_TRUE(prop2 != nullptr);
    prop1 += prop2;
}

/**
 * @tc.name: RSPropertyBase002
 * @tc.desc: operator+=002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator002, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    std::shared_ptr<RSPropertyBase> prop2 = std::make_shared<RSProperty<float>>();
    ASSERT_TRUE(prop1 != nullptr);
    prop1 += prop2;
}

/**
 * @tc.name: RSPropertyBase003
 * @tc.desc: operator-=001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator003, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    std::shared_ptr<RSPropertyBase> prop2 = std::make_shared<RSProperty<float>>();
    prop1 = nullptr;
    ASSERT_TRUE(prop2 != nullptr);
    prop1 -= prop2;
}

/**
 * @tc.name: RSPropertyBase004
 * @tc.desc: operator-=002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator004, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    std::shared_ptr<RSPropertyBase> prop2 = std::make_shared<RSProperty<float>>();
    ASSERT_TRUE(prop1 != nullptr);
    prop1 -= prop2;
}

/**
 * @tc.name: RSPropertyBase005
 * @tc.desc: operator*=001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator005, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    float scale = 2;
    ASSERT_TRUE(prop1 != nullptr);
    prop1 = nullptr;
    prop1 *= scale;
}

/**
 * @tc.name: RSPropertyBase006
 * @tc.desc: operator*=002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator006, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    float scale = 2;
    ASSERT_TRUE(prop1 != nullptr);
    prop1 *= scale;
}

/**
 * @tc.name: RSPropertyBase007
 * @tc.desc: operator+001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator007, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    std::shared_ptr<RSPropertyBase> prop2 = std::make_shared<RSProperty<float>>();
    prop1 = nullptr;
    ASSERT_TRUE(prop2 != nullptr);
    prop1 = prop1 + prop2;
}

/**
 * @tc.name: RSPropertyBase008
 * @tc.desc: operator+002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator008, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    std::shared_ptr<RSPropertyBase> prop2 = std::make_shared<RSProperty<float>>();
    ASSERT_TRUE(prop1 != nullptr);
    prop1 = prop1 + prop2;
}

/**
 * @tc.name: RSPropertyBase009
 * @tc.desc: operator-001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator009, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    std::shared_ptr<RSPropertyBase> prop2 = std::make_shared<RSProperty<float>>();
    prop1 = nullptr;
    ASSERT_TRUE(prop2 != nullptr);
    prop1 = prop1 - prop2;
}

/**
 * @tc.name: RSPropertyBase010
 * @tc.desc: operator-002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator010, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    std::shared_ptr<RSPropertyBase> prop2 = std::make_shared<RSProperty<float>>();
    ASSERT_TRUE(prop1 != nullptr);
    prop1 = prop1 - prop2;
}

/**
 * @tc.name: RSPropertyBase011
 * @tc.desc: operator*001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator011, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    float scale = 2;
    ASSERT_TRUE(prop1 != nullptr);
    prop1 = nullptr;
    prop1 = prop1 * scale;
}

/**
 * @tc.name: RSPropertyBase012
 * @tc.desc: operator*002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator012, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    float scale = 2;
    ASSERT_TRUE(prop1 != nullptr);
    prop1 = prop1 * scale;
}

/**
 * @tc.name: RSPropertyBase013
 * @tc.desc: operator==001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator013, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    std::shared_ptr<RSPropertyBase> prop2 = std::make_shared<RSProperty<float>>();
    prop1 = nullptr;
    ASSERT_TRUE(prop2 != nullptr);
    ASSERT_TRUE(prop1 != prop2);
}

/**
 * @tc.name: RSPropertyBase014
 * @tc.desc: operator==002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator014, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    std::shared_ptr<RSPropertyBase> prop2 = std::make_shared<RSProperty<float>>();
    ASSERT_TRUE(prop1 != nullptr);
    ASSERT_TRUE(prop1 != prop2);
}

/**
 * @tc.name: RSPropertyBase015
 * @tc.desc: operator!=001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator015, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    std::shared_ptr<RSPropertyBase> prop2 = std::make_shared<RSProperty<float>>();
    prop1 = nullptr;
    ASSERT_TRUE(prop2 != nullptr);
    ASSERT_TRUE(prop1 != prop2);
}

/**
 * @tc.name: RSPropertyBase016
 * @tc.desc: operator!=002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, operator016, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop1 = std::make_shared<RSProperty<float>>();
    std::shared_ptr<RSPropertyBase> prop2 = std::make_shared<RSProperty<float>>();
    ASSERT_TRUE(prop1 != nullptr);
    ASSERT_TRUE(prop1 != prop2);
}

/**
 * @tc.name: GetThresholdByThresholdType001
 * @tc.desc: test results of GetThresholdByThresholdType
 * @tc.type: FUNC
 * @tc.require: issueI9V4DP
 */
HWTEST_F(RSPropertyTest, GetThresholdByThresholdType001, TestSize.Level1)
{
    std::shared_ptr<RSPropertyBase> prop = std::make_shared<RSProperty<float>>();
    float res = prop->GetThresholdByThresholdType(ThresholdType::LAYOUT);
    EXPECT_TRUE(res == LAYOUT_NEAR_ZERO_THRESHOLD);
    
    res = prop->GetThresholdByThresholdType(ThresholdType::COARSE);
    EXPECT_TRUE(res == FLOAT_NEAR_ZERO_COARSE_THRESHOLD);

    res = prop->GetThresholdByThresholdType(ThresholdType::MEDIUM);
    EXPECT_TRUE(res == FLOAT_NEAR_ZERO_MEDIUM_THRESHOLD);

    res = prop->GetThresholdByThresholdType(ThresholdType::FINE);
    EXPECT_TRUE(res == FLOAT_NEAR_ZERO_FINE_THRESHOLD);

    res = prop->GetThresholdByThresholdType(ThresholdType::COLOR);
    EXPECT_TRUE(res == COLOR_NEAR_ZERO_THRESHOLD);

    res = prop->GetThresholdByThresholdType(ThresholdType::ZERO);
    EXPECT_TRUE(res == ZERO);

    res = prop->GetThresholdByThresholdType(ThresholdType::DEFAULT);
    EXPECT_TRUE(res == DEFAULT_NEAR_ZERO_THRESHOLD);
}

/**
 * @tc.name: Equality001
 * @tc.desc: test results of operator==
 * @tc.type: FUNC
 * @tc.require: issueI9V4DP
 */
HWTEST_F(RSPropertyTest, Equality001, TestSize.Level1)
{
    std::shared_ptr<const RSPropertyBase> propOne = nullptr;
    std::shared_ptr<RSPropertyBase> propTwo = std::make_shared<RSProperty<float>>();
    EXPECT_FALSE(propOne == propTwo);

    propOne = std::make_shared<RSProperty<float>>();
    EXPECT_FALSE(propOne == propTwo);
}

/**
 * @tc.name: Unlikeness001
 * @tc.desc: test results of operator!=
 * @tc.type: FUNC
 * @tc.require: issueI9V4DP
 */
HWTEST_F(RSPropertyTest, Unlikeness001, TestSize.Level1)
{
    std::shared_ptr<const RSPropertyBase> propOne = nullptr;
    std::shared_ptr<RSPropertyBase> propTwo = std::make_shared<RSProperty<float>>();
    EXPECT_FALSE(propOne == propTwo);

    propOne = std::make_shared<RSProperty<float>>();
    EXPECT_TRUE(propOne != propTwo);
}

/**
 * @tc.name: GetPropertyType001
 * @tc.desc: test results of GetPropertyType
 * @tc.type: FUNC
 * @tc.require: issueIAP7XJ
 */
HWTEST_F(RSPropertyTest, GetPropertyType001, TestSize.Level1)
{
    auto rsAnimatableProperty = std::make_shared<RSAnimatableProperty<Vector3f>>(Vector3f(1.0f, 2.0f, 3.0f));
    EXPECT_TRUE(rsAnimatableProperty->GetPropertyType() == RSPropertyType::VECTOR3F);
    rsAnimatableProperty->Set(Vector3f(4.0f, 5.0f, 6.0f));
    EXPECT_EQ(rsAnimatableProperty->Get(), Vector3f(4.0f, 5.0f, 6.0f));
}

/**
 * @tc.name: GetPropertyType002
 * @tc.desc: test results of GetPropertyType
 * @tc.type: FUNC
 * @tc.require: issueIC7PB4
 */
HWTEST_F(RSPropertyTest, GetPropertyType002, TestSize.Level1)
{
    std::vector<float> tempVec = {1.0f, 2.0f, 3.0f};
    auto rsAnimatableProperty = std::make_shared<RSAnimatableProperty<std::vector<float>>>(tempVec);
    EXPECT_TRUE(rsAnimatableProperty->GetPropertyType() == RSPropertyType::SHADER_PARAM);
    std::vector<float> tempVec2 = {4.0f, 5.0f, 6.0f};
    rsAnimatableProperty->Set(tempVec2);
    EXPECT_EQ(rsAnimatableProperty->Get(), tempVec2);
}

/**
 * @tc.name: GetRenderProperty001
 * @tc.desc: test template value is integral
 * @tc.type: FUNC
 * @tc.require: issueICAZAW
 */
HWTEST_F(RSPropertyTest, GetRenderProperty001, TestSize.Level1)
{
    RSAnimatableProperty<int> property(0);
    property.isCustom_ = true;
    property.renderProperty_ = std::make_shared<RSRenderAnimatableProperty<int>>();
    auto rsRenderProperty1 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty1, nullptr);

    property.renderProperty_ = nullptr;
    auto rsRenderProperty2 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty2, nullptr);

    property.isCustom_ = false;
    property.renderProperty_ = nullptr;
    auto rsRenderProperty3 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty3, nullptr);
}

/**
 * @tc.name: GetRenderProperty002
 * @tc.desc: test template value is float
 * @tc.type: FUNC
 * @tc.require: issueICAZAW
 */
HWTEST_F(RSPropertyTest, GetRenderProperty002, TestSize.Level1)
{
    RSAnimatableProperty<float> property(0.f);
    property.isCustom_ = true;
    property.renderProperty_ = std::make_shared<RSRenderAnimatableProperty<float>>();
    auto rsRenderProperty1 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty1, nullptr);

    property.renderProperty_ = nullptr;
    auto rsRenderProperty2 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty2, nullptr);

    property.isCustom_ = false;
    property.renderProperty_ = nullptr;
    auto rsRenderProperty3 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty3, nullptr);
}

/**
 * @tc.name: GetRenderProperty003
 * @tc.desc: test template value is RSColor
 * @tc.type: FUNC
 * @tc.require: issueICAZAW
 */
HWTEST_F(RSPropertyTest, GetRenderProperty003, TestSize.Level1)
{
    RSColor color = RSColor(0, 0, 0, 255);
    RSAnimatableProperty<RSColor> property(color);
    property.isCustom_ = true;
    property.renderProperty_ = std::make_shared<RSRenderAnimatableProperty<RSColor>>();
    auto rsRenderProperty1 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty1, nullptr);

    property.renderProperty_ = nullptr;
    auto rsRenderProperty2 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty2, nullptr);

    property.isCustom_ = false;
    property.renderProperty_ = nullptr;
    auto rsRenderProperty3 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty3, nullptr);
}

/**
 * @tc.name: GetRenderProperty004
 * @tc.desc: test template value is Matrix3f
 * @tc.type: FUNC
 * @tc.require: issueICAZAW
 */
HWTEST_F(RSPropertyTest, GetRenderProperty004, TestSize.Level1)
{
    RSAnimatableProperty<Matrix3f> property(Matrix3f::ZERO);
    property.isCustom_ = true;
    property.renderProperty_ = std::make_shared<RSRenderAnimatableProperty<Matrix3f>>();
    auto rsRenderProperty1 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty1, nullptr);

    property.renderProperty_ = nullptr;
    auto rsRenderProperty2 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty2, nullptr);

    property.isCustom_ = false;
    property.renderProperty_ = nullptr;
    auto rsRenderProperty3 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty3, nullptr);
}

/**
 * @tc.name: GetRenderProperty005
 * @tc.desc: test template value is Vector2f
 * @tc.type: FUNC
 * @tc.require: issueICAZAW
 */
HWTEST_F(RSPropertyTest, GetRenderProperty005, TestSize.Level1)
{
    RSAnimatableProperty<Vector2f> property(Vector2f(0.f, 0.f));
    property.isCustom_ = true;
    property.renderProperty_ = std::make_shared<RSRenderAnimatableProperty<Vector2f>>();
    auto rsRenderProperty1 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty1, nullptr);

    property.renderProperty_ = nullptr;
    auto rsRenderProperty2 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty2, nullptr);

    property.isCustom_ = false;
    property.renderProperty_ = nullptr;
    auto rsRenderProperty3 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty3, nullptr);
}

/**
 * @tc.name: GetRenderProperty006
 * @tc.desc: test template value is Vector3f
 * @tc.type: FUNC
 * @tc.require: issueICAZAW
 */
HWTEST_F(RSPropertyTest, GetRenderProperty006, TestSize.Level1)
{
    RSAnimatableProperty<Vector3f> property(Vector3f(0.f, 0.f, 0.f));
    property.isCustom_ = true;
    property.renderProperty_ = std::make_shared<RSRenderAnimatableProperty<Vector3f>>();
    auto rsRenderProperty1 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty1, nullptr);

    property.renderProperty_ = nullptr;
    auto rsRenderProperty2 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty2, nullptr);

    property.isCustom_ = false;
    property.renderProperty_ = nullptr;
    auto rsRenderProperty3 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty3, nullptr);
}

/**
 * @tc.name: GetRenderProperty007
 * @tc.desc: test template value is Vector4f
 * @tc.type: FUNC
 * @tc.require: issueICAZAW
 */
HWTEST_F(RSPropertyTest, GetRenderProperty007, TestSize.Level1)
{
    RSAnimatableProperty<Vector4f> property(Vector4f(0.f, 0.f, 0.f, 0.f));
    property.isCustom_ = true;
    property.renderProperty_ = std::make_shared<RSRenderAnimatableProperty<Vector4f>>();
    auto rsRenderProperty1 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty1, nullptr);

    property.renderProperty_ = nullptr;
    auto rsRenderProperty2 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty2, nullptr);

    property.isCustom_ = false;
    property.renderProperty_ = nullptr;
    auto rsRenderProperty3 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty3, nullptr);
}

/**
 * @tc.name: GetRenderProperty008
 * @tc.desc: test template value is Quaternion
 * @tc.type: FUNC
 * @tc.require: issueICAZAW
 */
HWTEST_F(RSPropertyTest, GetRenderProperty008, TestSize.Level1)
{
    RSAnimatableProperty<Quaternion> property(Quaternion(0.f, 0.1f, 0.2f, 0.3f));
    property.isCustom_ = true;
    property.renderProperty_ = std::make_shared<RSRenderAnimatableProperty<Quaternion>>();
    auto rsRenderProperty1 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty1, nullptr);

    property.renderProperty_ = nullptr;
    auto rsRenderProperty2 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty2, nullptr);

    property.isCustom_ = false;
    property.renderProperty_ = nullptr;
    auto rsRenderProperty3 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty3, nullptr);
}

/**
 * @tc.name: GetRenderProperty010
 * @tc.desc: test template value is Vector4<Color>
 * @tc.type: FUNC
 * @tc.require: issueICAZAW
 */
HWTEST_F(RSPropertyTest, GetRenderProperty010, TestSize.Level1)
{
    auto colorVec = Vector4<RSColor>(
        RSColor(0, 0, 0, 255),
        RSColor(0, 0, 255, 255),
        RSColor(0, 255, 255, 255),
        RSColor(255, 255, 255, 255)
    );
    RSAnimatableProperty<Vector4<RSColor>> property(colorVec);
    property.isCustom_ = true;
    property.renderProperty_ = std::make_shared<RSRenderAnimatableProperty<Vector4<RSColor>>>();
    auto rsRenderProperty1 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty1, nullptr);

    property.renderProperty_ = nullptr;
    auto rsRenderProperty2 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty2, nullptr);

    property.isCustom_ = false;
    property.renderProperty_ = nullptr;
    auto rsRenderProperty3 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty3, nullptr);
}

/**
 * @tc.name: GetRenderProperty011
 * @tc.desc: test template value is RRect
 * @tc.type: FUNC
 * @tc.require: issueICAZAW
 */
HWTEST_F(RSPropertyTest, GetRenderProperty011, TestSize.Level1)
{
    RRect rrect = RRect({0, 0, 0, 0}, 10.f, 10.f);
    RSAnimatableProperty<RRect> property(rrect);
    property.isCustom_ = true;
    property.renderProperty_ = std::make_shared<RSRenderAnimatableProperty<RRect>>();
    auto rsRenderProperty1 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty1, nullptr);

    property.renderProperty_ = nullptr;
    auto rsRenderProperty2 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty2, nullptr);

    property.isCustom_ = false;
    property.renderProperty_ = nullptr;
    auto rsRenderProperty3 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty3, nullptr);
}

/**
 * @tc.name: GetRenderProperty012
 * @tc.desc: test template value is std::vector<float>
 * @tc.type: FUNC
 * @tc.require: issueICAZAW
 */
HWTEST_F(RSPropertyTest, GetRenderProperty012, TestSize.Level1)
{
    std::vector<float> floatVec = {0.0f, 0.2f, 0.3f};
    RSAnimatableProperty<std::vector<float>> property(floatVec);
    property.isCustom_ = true;
    property.renderProperty_ = std::make_shared<RSRenderAnimatableProperty<std::vector<float>>>();
    auto rsRenderProperty1 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty1, nullptr);

    property.renderProperty_ = nullptr;
    auto rsRenderProperty2 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty2, nullptr);

    property.isCustom_ = false;
    property.renderProperty_ = nullptr;
    auto rsRenderProperty3 = property.GetRenderProperty();
    EXPECT_NE(rsRenderProperty3, nullptr);
}

/**
 * @tc.name: GetPropertyTypeNG
 * @tc.desc: test results of GetPropertyTypeNG
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyTest, GetPropertyTypeNG, TestSize.Level1)
{
    auto property2 = std::make_shared<RSProperty<float>>();
    property2->SetPropertyTypeNG(ModifierNG::RSPropertyType::ALPHA);
    EXPECT_EQ(property2->GetPropertyTypeNG(), ModifierNG::RSPropertyType::ALPHA);
}
} // namespace OHOS::Rosen
