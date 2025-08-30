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
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "animation/rs_animation.h"
#include "core/transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_director.h"
#include "modifier/rs_property.h"

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
    EXPECT_TRUE(rsAnimatableProperty->GetPropertyType() == RSPropertyType::VECTOR_FLOAT);
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
 * @tc.name: GetRenderProperty013
 * @tc.desc: test template value is std::shared_ptr<RSNGFilterBase>
 * @tc.type: FUNC
 * @tc.require: issueICAZAW
 */
HWTEST_F(RSPropertyTest, GetRenderProperty013, TestSize.Level1)
{
    auto nullProp = std::make_shared<RSProperty<std::shared_ptr<RSNGFilterBase>>>();
    auto result = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>>(
        nullProp->GetRenderProperty());
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->stagingValue_, nullptr);

    auto blurFilter = std::make_shared<RSNGBlurFilter>();
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSNGFilterBase>>>();
    prop->stagingValue_ = blurFilter;
    result = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSNGRenderFilterBase>>>(
        prop->GetRenderProperty());
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result->stagingValue_, nullptr);
}

/**
 * @tc.name: OnAttachAndOnDetach001
 * @tc.desc: Test OnAttach and OnDetach for RSProperty<std::shared_ptr<RSNGFilterBase>>
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, OnAttachAndOnDetach001, TestSize.Level1)
{
    auto nullProp = std::make_shared<RSProperty<std::shared_ptr<RSNGFilterBase>>>();
    auto node = RSCanvasNode::Create();
    nullProp->OnAttach(*node, {});
    nullProp->OnDetach();

    auto blurFilter = std::make_shared<RSNGBlurFilter>();
    auto filterProp = blurFilter->Getter<BlurRadiusXTag>();
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSNGFilterBase>>>();
    prop->Set(blurFilter);

    prop->OnAttach(*node, {});
    EXPECT_NE(filterProp->target_.lock(), nullptr);
    prop->OnDetach();
}

/**
 * @tc.name: Set001
 * @tc.desc: Test Set for RSProperty<std::shared_ptr<RSNGFilterBase>>
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, Set001, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSProperty<std::shared_ptr<RSNGFilterBase>>>();
    auto blurFilter1 = std::make_shared<RSNGBlurFilter>();
    prop1->Set(blurFilter1);
    EXPECT_EQ(prop1->stagingValue_, blurFilter1);
    prop1->Set(blurFilter1);
    EXPECT_EQ(prop1->stagingValue_, blurFilter1);

    // without node
    auto prop2 = std::make_shared<RSProperty<std::shared_ptr<RSNGFilterBase>>>();
    auto blurFilter2 = std::make_shared<RSNGBlurFilter>();
    prop2->Set(blurFilter2);
    EXPECT_EQ(prop2->stagingValue_, blurFilter2);

    prop2->Set(blurFilter1);
    EXPECT_EQ(prop2->stagingValue_, blurFilter1);

    prop2->Set(nullptr);
    EXPECT_EQ(prop2->stagingValue_, nullptr);

    // with node
    auto node = RSCanvasNode::Create();
    prop2 = std::make_shared<RSProperty<std::shared_ptr<RSNGFilterBase>>>();
    prop2->target_ = node;
    blurFilter2 = std::make_shared<RSNGBlurFilter>();
    prop2->Set(blurFilter2);
    EXPECT_EQ(prop2->stagingValue_, blurFilter2);

    prop2->Set(blurFilter1);
    EXPECT_EQ(prop2->stagingValue_, blurFilter2); // Update value only

    prop2->Set(nullptr);
    EXPECT_EQ(prop2->stagingValue_, nullptr);
}

/**
 * @tc.name: OnAttachAndOnDetach002
 * @tc.desc: Test OnAttach and OnDetach for RSProperty<std::shared_ptr<RSNGShaderBase>>
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, OnAttachAndOnDetach002, TestSize.Level1)
{
    auto nullProp = std::make_shared<RSProperty<std::shared_ptr<RSNGShaderBase>>>();
    auto node = RSCanvasNode::Create();
    nullProp->OnAttach(*node, {});
    nullProp->OnDetach();

    auto shader = std::make_shared<RSNGContourDiagonalFlowLight>();
    auto shaderProp = shader->Getter<ContourDiagonalFlowLightContourTag>();
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSNGShaderBase>>>();
    prop->Set(shader);

    prop->OnAttach(*node, {});
    EXPECT_NE(shaderProp->target_.lock(), nullptr);
    prop->OnDetach();
}

/**
 * @tc.name: Set002
 * @tc.desc: Test Set for RSProperty<std::shared_ptr<RSNGShaderBase>>
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, Set002, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSProperty<std::shared_ptr<RSNGShaderBase>>>();
    auto shader1 = std::make_shared<RSNGContourDiagonalFlowLight>();
    prop1->Set(shader1);
    EXPECT_EQ(prop1->stagingValue_, shader1);
    prop1->Set(shader1);
    EXPECT_EQ(prop1->stagingValue_, shader1);

    // without node
    auto prop2 = std::make_shared<RSProperty<std::shared_ptr<RSNGShaderBase>>>();
    auto shader2 = std::make_shared<RSNGContourDiagonalFlowLight>();
    prop2->Set(shader2);
    EXPECT_EQ(prop2->stagingValue_, shader2);

    prop2->Set(shader1);
    EXPECT_EQ(prop2->stagingValue_, shader1);

    prop2->Set(nullptr);
    EXPECT_EQ(prop2->stagingValue_, nullptr);

    // with node
    auto node = RSCanvasNode::Create();
    prop2 = std::make_shared<RSProperty<std::shared_ptr<RSNGShaderBase>>>();
    prop2->target_ = node;
    shader2 = std::make_shared<RSNGContourDiagonalFlowLight>();
    prop2->Set(shader2);
    EXPECT_EQ(prop2->stagingValue_, shader2);

    prop2->Set(shader1);
    EXPECT_EQ(prop2->stagingValue_, shader2); // Update value only

    prop2->Set(nullptr);
    EXPECT_EQ(prop2->stagingValue_, nullptr);
}

/**
 * @tc.name: OnAttachAndOnDetach003
 * @tc.desc: Test OnAttach and OnDetach for RSProperty<std::shared_ptr<RSNGMaskBase>>
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, OnAttachAndOnDetach003, TestSize.Level1)
{
    auto nullProp = std::make_shared<RSProperty<std::shared_ptr<RSNGMaskBase>>>();
    auto node = RSCanvasNode::Create();
    nullProp->OnAttach(*node, {});
    nullProp->OnDetach();

    auto mask = std::make_shared<RSNGRippleMask>();
    auto maskProp = mask->Getter<RippleMaskCenterTag>();
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSNGMaskBase>>>();
    prop->Set(mask);

    prop->OnAttach(*node, {});
    EXPECT_NE(maskProp->target_.lock(), nullptr);
    prop->OnDetach();
}

/**
 * @tc.name: Set003
 * @tc.desc: Test Set for RSProperty<std::shared_ptr<RSNGMaskBase>>
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, Set003, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSProperty<std::shared_ptr<RSNGMaskBase>>>();
    auto mask1 = std::make_shared<RSNGRippleMask>();
    prop1->Set(mask1);
    EXPECT_EQ(prop1->stagingValue_, mask1);
    prop1->Set(mask1);
    EXPECT_EQ(prop1->stagingValue_, mask1);

    // without node
    auto prop2 = std::make_shared<RSProperty<std::shared_ptr<RSNGMaskBase>>>();
    auto mask2 = std::make_shared<RSNGRippleMask>();
    prop2->Set(mask2);
    EXPECT_EQ(prop2->stagingValue_, mask2);

    prop2->Set(mask1);
    EXPECT_EQ(prop2->stagingValue_, mask1);

    prop2->Set(nullptr);
    EXPECT_EQ(prop2->stagingValue_, nullptr);

    // with node
    auto node = RSCanvasNode::Create();
    prop2 = std::make_shared<RSProperty<std::shared_ptr<RSNGMaskBase>>>();
    prop2->target_ = node;
    mask2 = std::make_shared<RSNGRippleMask>();
    prop2->Set(mask2);
    EXPECT_EQ(prop2->stagingValue_, mask2);

    prop2->Set(mask1);
    EXPECT_EQ(prop2->stagingValue_, mask2); // Update value only

    prop2->Set(nullptr);
    EXPECT_EQ(prop2->stagingValue_, nullptr);
}

/**
 * @tc.name: RSNGShaderBaseGetRenderPropertyTest
 * @tc.desc: Test GetRenderPropertyTest for RSProperty<std::shared_ptr<RSNGShaderBase>>
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, RSNGShaderBaseGetRenderPropertyTest, TestSize.Level1)
{
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSNGShaderBase>>>();
    auto result = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSNGRenderShaderBase>>>(
        prop->GetRenderProperty());
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->stagingValue_, nullptr);

    auto shader = std::make_shared<RSNGContourDiagonalFlowLight>();
    prop->Set(shader);
    result = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSNGRenderShaderBase>>>(
        prop->GetRenderProperty());
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result->stagingValue_, nullptr);
}

/**
 * @tc.name: RSNGMaskBaseGetRenderPropertyTest
 * @tc.desc: Test GetRenderPropertyTest for RSProperty<std::shared_ptr<RSNGMaskBase>>
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, RSNGMaskBaseGetRenderPropertyTest, TestSize.Level1)
{
    auto prop = std::make_shared<RSProperty<std::shared_ptr<RSNGMaskBase>>>();
    auto result = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSNGRenderMaskBase>>>(
        prop->GetRenderProperty());
    EXPECT_NE(result, nullptr);

    EXPECT_EQ(result->stagingValue_, nullptr);

    auto mask = std::make_shared<RSNGRippleMask>();
    prop->Set(mask);
    result = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSNGRenderMaskBase>>>(
        prop->GetRenderProperty());
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result->stagingValue_, nullptr);
}

/**
 * @tc.name: DrawCmdListGetRenderPropertyTest
 * @tc.desc: Test GetRenderPropertyTest for RSProperty<Drawing::DrawCmdListPtr>
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTest, DrawCmdListGetRenderPropertyTest, TestSize.Level1)
{
    auto prop = std::make_shared<RSProperty<Drawing::DrawCmdListPtr>>();
    auto result = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(prop->GetRenderProperty());
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->stagingValue_, nullptr);

    auto cmdList = std::make_shared<Drawing::DrawCmdList>();
    prop->Set(cmdList);
    result = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(prop->GetRenderProperty());
    EXPECT_NE(result, nullptr);

    auto node = RSCanvasNode::Create();
    prop->target_ = node;
    prop->Set(cmdList);
    result = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(prop->GetRenderProperty());
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: GetPropertyTypeNG
 * @tc.desc: test results of GetPropertyTypeNG
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyTest, GetPropertyTypeNG, TestSize.Level1)
{
    auto property = std::make_shared<RSProperty<float>>();
    property->SetPropertyTypeNG(ModifierNG::RSPropertyType::ALPHA);
    EXPECT_EQ(property->GetPropertyTypeNG(), ModifierNG::RSPropertyType::ALPHA);
}
} // namespace OHOS::Rosen
