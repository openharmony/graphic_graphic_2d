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

#include "subwindow_option_impl_test.h"

namespace OHOS {
void SubwindowOptionImplTest::SetUp()
{
}

void SubwindowOptionImplTest::TearDown()
{
}

void SubwindowOptionImplTest::SetUpTestCase()
{
}

void SubwindowOptionImplTest::TearDownTestCase()
{
}

namespace {
/*
 * Feature: SubwindowOptionImpl Getter, Setter
 * Function: SubwindowOptionImpl
 * SubFunction: Getter, Setter
 * FunctionPoints: SubwindowOptionImpl Getter, Setter
 * EnvConditions: N/A
 * CaseDescription: 1. Check Set and Get WindowType
 *                  2. Check Set and Get X
 *                  3. Check Set and Get Y
 *                  4. Check Set and Get Width
 *                  5. Check Set and Get Height
 */
HWTEST_F(SubwindowOptionImplTest, Create, testing::ext::TestSize.Level0)
{
    SubwindowOptionImpl soi;

    // 1. Check Set and Get WindowType
    constexpr SubwindowType type1 = static_cast<SubwindowType>(0);
    constexpr SubwindowType type2 = static_cast<SubwindowType>(1);
    soi.SetWindowType(type1);
    ASSERT_EQ(soi.GetWindowType(), type1) << "CaseDescription: "
        << "1. Check Set and Get WindowType (soi.GetWindowType() == type1)";
    soi.SetWindowType(type2);
    ASSERT_EQ(soi.GetWindowType(), type2) << "CaseDescription: "
        << "1. Check Set and Get WindowType (soi.GetWindowType() == type2)";

    // 2. Check Set and Get X
    constexpr int32_t x1 = -1;
    constexpr int32_t x2 = 1;
    soi.SetX(x1);
    ASSERT_EQ(soi.GetX(), x1) << "CaseDescription: "
        << "2. Check Set and Get X (soi.GetX() == x1)";
    soi.SetX(x2);
    ASSERT_EQ(soi.GetX(), x2) << "CaseDescription: "
        << "2. Check Set and Get X (soi.GetY() == x2)";

    // 3. Check Set and Get Y
    constexpr int32_t y1 = -1;
    constexpr int32_t y2 = 1;
    soi.SetY(y1);
    ASSERT_EQ(soi.GetY(), y1) << "CaseDescription: "
        << "3. Check Set and Get Y (soi.GetY() == y1)";
    soi.SetY(y2);
    ASSERT_EQ(soi.GetY(), y2) << "CaseDescription: "
        << "3. Check Set and Get Y (soi.GetY() == y2)";

    // 4. Check Set and Get Width
    constexpr uint32_t width1 = 1;
    constexpr uint32_t width2 = 2;
    soi.SetWidth(width1);
    ASSERT_EQ(soi.GetWidth(), width1) << "CaseDescription: "
        << "4. Check Set and Get Width (soi.GetWidth() == width1)";
    soi.SetWidth(width2);
    ASSERT_EQ(soi.GetWidth(), width2) << "CaseDescription: "
        << "4. Check Set and Get Width (soi.GetWidth() == width2)";

    // 5. Check Set and Get Height
    constexpr uint32_t height1 = 1;
    constexpr uint32_t height2 = 2;
    soi.SetHeight(height1);
    ASSERT_EQ(soi.GetHeight(), height1) << "CaseDescription: "
        << "5. Check Set and Get Height (soi.GetHeight() == height1)";
    soi.SetHeight(height2);
    ASSERT_EQ(soi.GetHeight(), height2) << "CaseDescription: "
        << "5. Check Set and Get Height (soi.GetHeight() == height2)";
}

/*
 * Feature: SubwindowOptionImpl Invalid Set
 * Function: SubwindowOptionImpl
 * SubFunction: Invalid Set
 * FunctionPoints: SubwindowOptionImpl Invalid Set
 * EnvConditions: N/A
 * CaseDescription: 1. set invalid WindowType, check
 *                  2. set invalid width, check
 *                  3. set invalid height, check
 */
HWTEST_F(SubwindowOptionImplTest, InvalidSet, testing::ext::TestSize.Level0)
{
    SubwindowOptionImpl soi;

    // 1. set invalid WindowType, check
    constexpr auto invalidWindowType = static_cast<SubwindowType>(-1);
    ASSERT_EQ(soi.SetWindowType(invalidWindowType), GSERROR_INVALID_ARGUMENTS) << "CaseDescription: "
        << "1. set invalid WindowType, check (soi.SetWindowType() == GSERROR_INVALID_ARGUMENTS)";

    // 2. set invalid width, check
    constexpr auto invalidWidth = 0u;
    ASSERT_EQ(soi.SetWidth(invalidWidth), GSERROR_INVALID_ARGUMENTS) << "CaseDescription: "
        << "2. set invalid width, check (soi.SetWidth() == GSERROR_INVALID_ARGUMENTS)";

    // 3. set invalid height, check
    constexpr auto invalidHeight = 0u;
    ASSERT_EQ(soi.SetHeight(invalidHeight), GSERROR_INVALID_ARGUMENTS) << "CaseDescription: "
        << "3. set invalid height, check (soi.SetHeight() == GSERROR_INVALID_ARGUMENTS)";
}
} // namespace
} // namespace OHOS
