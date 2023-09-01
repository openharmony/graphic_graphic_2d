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
    ASSERT_TRUE(prop1 == nullptr);
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
    ASSERT_TRUE(prop1 == nullptr);
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
    prop1 = nullptr;
    ASSERT_TRUE(prop1 == nullptr);
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
    ASSERT_TRUE(prop1 == nullptr);
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
    ASSERT_TRUE(prop1 == nullptr);
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
    prop1 = nullptr;
    ASSERT_TRUE(prop1 == nullptr);
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
    ASSERT_TRUE(prop1 == nullptr);
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
    ASSERT_TRUE(prop1 == nullptr);
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
} // namespace OHOS::Rosen
