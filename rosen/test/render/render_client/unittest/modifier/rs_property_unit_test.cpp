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
class RSPropertyUnitTest : public testing::Test {
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

void RSPropertyUnitTest::SetUpTestCase() {}
void RSPropertyUnitTest::TearDownTestCase() {}
void RSPropertyUnitTest::SetUp() {}
void RSPropertyUnitTest::TearDown() {}

/**
 * @tc.name: LifeCycle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, LifeCycle002, TestSize.Level1)
{
    auto prop = std::make_shared<RSProperty<float>>();
    ASSERT_TRUE(prop != nullptr);
    ASSERT_TRUE(prop->GetId() != 0);
}

/**
 * @tc.name: Property002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, Property002, TestSize.Level1)
{
    auto prop = std::make_shared<RSProperty<float>>(floatData[0]);
    ASSERT_TRUE(prop != nullptr);
    ASSERT_EQ(prop->Get(), floatData[0]);

    prop->Set(floatData[1]);
    ASSERT_EQ(prop->Get(), floatData[1]);
}

/**
 * @tc.name: Property003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, Property003, TestSize.Level1)
{
    auto prop = std::make_shared<RSProperty<float>>(floatData[0]);
    ASSERT_TRUE(prop != nullptr);
    ASSERT_EQ(prop->Get(), floatData[0]);

    prop->Set(floatData[2]);
    ASSERT_EQ(prop->Get(), floatData[2]);
}

/**
 * @tc.name: Property0031
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, Property0031, TestSize.Level1)
{
    auto prop = std::make_shared<RSProperty<float>>(UINT32_MAX);
    ASSERT_TRUE(prop != nullptr);
    ASSERT_EQ(prop->Get(), UINT32_MAX);

    prop->Set(floatData[2]);
    ASSERT_EQ(prop->Get(), floatData[2]);
}

/**
 * @tc.name: Property0041
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, Property0041, TestSize.Level1)
{
    auto prop = std::make_shared<RSAnimatableProperty<float>>(UINT32_MAX);
    ASSERT_TRUE(prop != nullptr);
    ASSERT_EQ(prop->Get(), UINT32_MAX);

    prop->SetThresholdType(ThresholdType::COARSE);
    ASSERT_EQ(prop->GetThreshold(), 1.0f / 256.0f);
}

/**
 * @tc.name: PropertyOp0011
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, PropertyOp0011, TestSize.Level1)
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
 * @tc.name: RSPropertyBase0011
 * @tc.desc: operator+=001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0011, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
    prop1 = nullptr;
    ASSERT_TRUE(prop1 == nullptr);
    prop1 += prop2;
}

/**
 * @tc.name: RSPropertyBase0021
 * @tc.desc: operator+=002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0021, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
    ASSERT_TRUE(prop1 != nullptr);
    prop1 += prop2;
}

/**
 * @tc.name: RSPropertyBase0031
 * @tc.desc: operator-=001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0031, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
    prop1 = nullptr;
    ASSERT_TRUE(prop1 == nullptr);
    prop1 -= prop2;
}

/**
 * @tc.name: RSPropertyBase0041
 * @tc.desc: operator-=002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0041, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
    ASSERT_TRUE(prop1 != nullptr);
    prop1 -= prop2;
}

/**
 * @tc.name: RSPropertyBase0051
 * @tc.desc: operator*=001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0051, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    float scale = 2;
    prop1 = nullptr;
    ASSERT_TRUE(prop1 == nullptr);
    prop1 *= scale;
}

/**
 * @tc.name: RSPropertyBase0061
 * @tc.desc: operator*=002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0061, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    float scale = 2;
    ASSERT_TRUE(prop1 != nullptr);
    prop1 *= scale;
}

/**
 * @tc.name: RSPropertyBase0071
 * @tc.desc: operator+001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0071, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
    prop1 = nullptr;
    ASSERT_TRUE(prop1 == nullptr);
    prop1 = prop1 + prop2;
}

/**
 * @tc.name: RSPropertyBase0081
 * @tc.desc: operator+002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0081, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
    ASSERT_TRUE(prop1 != nullptr);
    prop1 = prop1 + prop2;
}

/**
 * @tc.name: RSPropertyBase0091
 * @tc.desc: operator-001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0091, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
    prop1 = nullptr;
    ASSERT_TRUE(prop1 == nullptr);
    prop1 = prop1 - prop2;
}

/**
 * @tc.name: RSPropertyBase0101
 * @tc.desc: operator-002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0101, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
    ASSERT_TRUE(prop1 != nullptr);
    prop1 = prop1 - prop2;
}

/**
 * @tc.name: RSPropertyBase0111
 * @tc.desc: operator*001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0111, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    float scale = 2;
    prop1 = nullptr;
    ASSERT_TRUE(prop1 == nullptr);
    prop1 = prop1 * scale;
}

/**
 * @tc.name: RSPropertyBase0121
 * @tc.desc: operator*002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0121, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    float scale = 2;
    ASSERT_TRUE(prop1 != nullptr);
    prop1 = prop1 * scale;
}

/**
 * @tc.name: RSPropertyBase0131
 * @tc.desc: operator==001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0131, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
    prop1 = nullptr;
    ASSERT_TRUE(prop1 == nullptr);
    ASSERT_TRUE(prop1 != prop2);
}

/**
 * @tc.name: RSPropertyBase0141
 * @tc.desc: operator==002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0141, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
    ASSERT_TRUE(prop1 != nullptr);
    ASSERT_TRUE(prop1 != prop2);
}

/**
 * @tc.name: RSPropertyBase0151
 * @tc.desc: operator!=001
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0151, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
    prop1 = nullptr;
    ASSERT_TRUE(prop1 == nullptr);
    ASSERT_TRUE(prop1 != prop2);
}

/**
 * @tc.name: RSPropertyBase0161
 * @tc.desc: operator!=002
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyUnitTest, operator0161, TestSize.Level1)
{
    auto prop1 = std::make_shared<RSPropertyBase>();
    auto prop2 = std::make_shared<RSPropertyBase>();
    ASSERT_TRUE(prop1 != nullptr);
    ASSERT_TRUE(prop1 != prop2);
}

/**
 * @tc.name: GetThresholdByThresholdType0011
 * @tc.desc: test results of GetThresholdByThresholdType
 * @tc.type: FUNC
 * @tc.require: issueI9V4DP
 */
HWTEST_F(RSPropertyUnitTest, GetThresholdByThresholdType0011, TestSize.Level1)
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
 * @tc.name: Equality0011
 * @tc.desc: test results of operator==
 * @tc.type: FUNC
 * @tc.require: issueI9V4DP
 */
HWTEST_F(RSPropertyUnitTest, Equality0011, TestSize.Level1)
{
    std::shared_ptr<const RSPropertyBase> propOne = nullptr;
    auto propTwo = std::make_shared<RSPropertyBase>();
    EXPECT_FALSE(propOne == propTwo);

    propOne = std::make_shared<RSPropertyBase>();
    EXPECT_FALSE(propOne == propTwo);
}

/**
 * @tc.name: Unlikeness0011
 * @tc.desc: test results of operator!=
 * @tc.type: FUNC
 * @tc.require: issueI9V4DP
 */
HWTEST_F(RSPropertyUnitTest, Unlikeness0011, TestSize.Level1)
{
    std::shared_ptr<const RSPropertyBase> propOne = nullptr;
    auto propTwo = std::make_shared<RSPropertyBase>();
    EXPECT_FALSE(propOne == propTwo);

    propOne = std::make_shared<RSPropertyBase>();
    EXPECT_TRUE(propOne != propTwo);
}
} // namespace OHOS::Rosen
