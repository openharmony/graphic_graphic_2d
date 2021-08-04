/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "window_attribute_test.h"

#include "wl_display.h"

namespace OHOS {
void WindowAttributeTest::SetUp()
{
}

void WindowAttributeTest::TearDown()
{
}

void WindowAttributeTest::SetUpTestCase()
{
}

void WindowAttributeTest::TearDownTestCase()
{
}

namespace {
/*
 * Feature: WindowAttribute getter and setter (1/2)
 * Function: WindowAttribute
 * SubFunction: getter and setter
 * FunctionPoints: WindowAttribute getter and setter
 * EnvConditions: N/A
 * CaseDescription: 1. set all attribute and check
 */
HWTEST_F(WindowAttributeTest, GetterSetter, testing::ext::TestSize.Level0)
{
    WindowAttribute attribute;
    // 1. set all attribute and check
    constexpr auto id1 = 1;
    constexpr auto id2 = 2;
    attribute.SetID(id1);
    ASSERT_EQ(attribute.GetID(), id1) << "CaseDescription: "
        << "1. set all attribute and check (GetID() == id1)";
    attribute.SetID(id2);
    ASSERT_EQ(attribute.GetID(), id2) << "CaseDescription: "
        << "1. set all attribute and check (GetID() == id2)";

    constexpr auto x1 = 1;
    constexpr auto x2 = 2;
    attribute.SetXY(x1, 0);
    ASSERT_EQ(attribute.GetX(), x1) << "CaseDescription: "
        << "1. set all attribute and check (GetX() == x1)";
    attribute.SetXY(x2, 0);
    ASSERT_EQ(attribute.GetX(), x2) << "CaseDescription: "
        << "1. set all attribute and check (GetX() == x2)";

    constexpr auto y1 = 1;
    constexpr auto y2 = 2;
    attribute.SetXY(0, y1);
    ASSERT_EQ(attribute.GetY(), y1) << "CaseDescription: "
        << "1. set all attribute and check (GetY() == y1)";
    attribute.SetXY(0, y2);
    ASSERT_EQ(attribute.GetY(), y2) << "CaseDescription: "
        << "1. set all attribute and check (GetY() == y2)";

    constexpr auto width1 = 1u;
    constexpr auto width2 = 2u;
    attribute.SetWidthHeight(width1, 0);
    ASSERT_EQ(attribute.GetWidth(), width1) << "CaseDescription: "
        << "1. set all attribute and check (GetWidth() == width1)";
    attribute.SetWidthHeight(width2, 0);
    ASSERT_EQ(attribute.GetWidth(), width2) << "CaseDescription: "
        << "1. set all attribute and check (GetWidth() == width2)";

    constexpr auto height1 = 1u;
    constexpr auto height2 = 2u;
    attribute.SetWidthHeight(0, height1);
    ASSERT_EQ(attribute.GetHeight(), height1) << "CaseDescription: "
        << "1. set all attribute and check (GetHeight() == height1)";
    attribute.SetWidthHeight(0, height2);
    ASSERT_EQ(attribute.GetHeight(), height2) << "CaseDescription: "
        << "1. set all attribute and check (GetHeight() == height2)";
}

/*
 * Feature: WindowAttribute getter and setter (2/2)
 * Function: WindowAttribute
 * SubFunction: getter and setter
 * FunctionPoints: WindowAttribute getter and setter
 * EnvConditions: N/A
 * CaseDescription: 1. set all attribute and check
 */
HWTEST_F(WindowAttributeTest, GetterSetter2, testing::ext::TestSize.Level0)
{
    WindowAttribute attribute;
    // 1. set all attribute and check
    constexpr auto destWidth1 = 1u;
    constexpr auto destWidth2 = 2u;
    attribute.SetDestWidthHeight(destWidth1, 0);
    ASSERT_EQ(attribute.GetDestWidth(), destWidth1) << "CaseDescription: "
        << "1. set all attribute and check (GetDestWidth() == destWidth1)";
    attribute.SetDestWidthHeight(destWidth2, 0);
    ASSERT_EQ(attribute.GetDestWidth(), destWidth2) << "CaseDescription: "
        << "1. set all attribute and check (GetDestWidth() == destWidth2)";

    constexpr auto destHeight1 = 1u;
    constexpr auto destHeight2 = 2u;
    attribute.SetDestWidthHeight(0, destHeight1);
    ASSERT_EQ(attribute.GetDestHeight(), destHeight1) << "CaseDescription: "
        << "1. set all attribute and check (GetDestHeight() == destHeight1)";
    attribute.SetDestWidthHeight(0, destHeight2);
    ASSERT_EQ(attribute.GetDestHeight(), destHeight2) << "CaseDescription: "
        << "1. set all attribute and check (GetDestHeight() == destHeight2)";

    constexpr auto visibility1 = true;
    constexpr auto visibility2 = false;
    attribute.SetVisibility(visibility1);
    ASSERT_EQ(attribute.GetVisibility(), visibility1) << "CaseDescription: "
        << "1. set all attribute and check (GetVisibility() == visibility1)";
    attribute.SetVisibility(visibility2);
    ASSERT_EQ(attribute.GetVisibility(), visibility2) << "CaseDescription: "
        << "1. set all attribute and check (GetVisibility() == visibility2)";

    constexpr auto type1 = static_cast<WindowType>(1);
    constexpr auto type2 = static_cast<WindowType>(2);
    attribute.SetType(type1);
    ASSERT_EQ(attribute.GetType(), type1) << "CaseDescription: "
        << "1. set all attribute and check (GetType() == type1)";
    attribute.SetType(type2);
    ASSERT_EQ(attribute.GetType(), type2) << "CaseDescription: "
        << "1. set all attribute and check (GetType() == type2)";
}

/*
 * Feature: WindowAttribute listeners
 * Function: WindowAttribute
 * SubFunction: listeners
 * FunctionPoints: WindowAttribute listeners
 * EnvConditions: N/A
 * CaseDescription: 1. add listener
 *                  2. change attr, check
 */
HWTEST_F(WindowAttributeTest, AddListeners, testing::ext::TestSize.Level0)
{
    WindowAttribute attribute;

    // 1. add listener
    callCount = 0;
    constexpr uint32_t callPosition = 1;
    constexpr uint32_t callSize = 2;
    constexpr uint32_t callVisibility = 3;
    constexpr uint32_t callType = 4;
    attribute.OnPositionChange([&](int32_t, int32_t) { callCount += callPosition; });
    attribute.OnSizeChange([&](uint32_t, uint32_t) { callCount += callSize; });
    attribute.OnVisibilityChange([&](bool) { callCount += callVisibility; });
    attribute.OnTypeChange([&](WindowType) { callCount += callType; });
    ASSERT_EQ(callCount, 0u) << "CaseDescription: 1. add listener (callCount == 0)";

    // 2. change attr, check
    callCount = 0;
    attribute.SetXY(1, 1);
    ASSERT_EQ(callCount, callPosition) << "CaseDescription: "
        << "2. change attr, check (callCount == callPosition)";

    callCount = 0;
    attribute.SetWidthHeight(1, 1);
    ASSERT_EQ(callCount, callSize) << "CaseDescription: "
        << "2. change attr, check (callCount == callSize)";

    callCount = 0;
    attribute.SetVisibility(true);
    ASSERT_EQ(callCount, callVisibility) << "CaseDescription: "
        << "2. change attr, check (callCount == callVisibility)";

    callCount = 0;
    attribute.SetType(static_cast<WindowType>(1));
    ASSERT_EQ(callCount, callType) << "CaseDescription: "
        << "2. change attr, check (callCount == callType)";
}
} // namespace
} // namespace OHOS
