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
    auto prop = std::make_shared<RSProperty<float>>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
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
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
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
    auto prop = std::make_shared<RSPropertyBase>();
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
    auto propTwo = std::make_shared<RSPropertyBase>();
    EXPECT_FALSE(propOne == propTwo);

    propOne = std::make_shared<RSPropertyBase>();
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
    auto propTwo = std::make_shared<RSPropertyBase>();
    EXPECT_FALSE(propOne == propTwo);

    propOne = std::make_shared<RSPropertyBase>();
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
    EXPECT_TRUE(rsAnimatableProperty->GetPropertyType() == RSRenderPropertyType::PROPERTY_VECTOR3F);
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
    EXPECT_TRUE(rsAnimatableProperty->GetPropertyType() == RSRenderPropertyType::PROPERTY_SHADER_PARAM);
    std::vector<float> tempVec2 = {4.0f, 5.0f, 6.0f};
    rsAnimatableProperty->Set(tempVec2);
    EXPECT_EQ(rsAnimatableProperty->Get(), tempVec2);
}
} // namespace OHOS::Rosen
