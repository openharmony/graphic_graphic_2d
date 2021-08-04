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

#include "window_option_impl_test.h"

namespace OHOS {
void WindowOptionImplTest::SetUp()
{
}

void WindowOptionImplTest::TearDown()
{
}

void WindowOptionImplTest::SetUpTestCase()
{
}

void WindowOptionImplTest::TearDownTestCase()
{
}

namespace {
/*
 * Feature: WindowOptionImpl Getter, Setter
 * Function: WindowOptionImpl
 * SubFunction: Getter, Setter
 * FunctionPoints: WindowOptionImpl Getter, Setter
 * EnvConditions: N/A
 * CaseDescription: 1. Check Set and Get WindowType
 *                  2. Check Set and Get WindowMode
 *                  3. Check Set and Get Display
 *                  4. Check Set and Get ConsumerSurface
 */
HWTEST_F(WindowOptionImplTest, Create, testing::ext::TestSize.Level0)
{
    WindowOptionImpl woi;

    // 1. Check Set and Get WindowType
    constexpr WindowType type1 = static_cast<WindowType>(0);
    constexpr WindowType type2 = static_cast<WindowType>(1);
    woi.SetWindowType(type1);
    ASSERT_EQ(woi.GetWindowType(), type1) << "CaseDescription: "
        << "1. Check Set and Get WindowType (woi.GetWindowType() == type1)";
    woi.SetWindowType(type2);
    ASSERT_EQ(woi.GetWindowType(), type2) << "CaseDescription: "
        << "1. Check Set and Get WindowType (woi.GetWindowType() == type2)";

    // 2. Check Set and Get WindowMode
    constexpr WindowMode mode1 = static_cast<WindowMode>(0);
    constexpr WindowMode mode2 = static_cast<WindowMode>(1);
    woi.SetWindowMode(mode1);
    ASSERT_EQ(woi.GetWindowMode(), mode1) << "CaseDescription: "
        << "2. Check Set and Get WindowMode (woi.GetWindowMode() == mode1)";
    woi.SetWindowMode(mode2);
    ASSERT_EQ(woi.GetWindowMode(), mode2) << "CaseDescription: "
        << "2. Check Set and Get WindowMode (woi.GetWindowMode() == mode2)";

    // 3. Check Set and Get Display
    constexpr int32_t display1 = 1;
    constexpr int32_t display2 = 2;
    woi.SetDisplay(display1);
    ASSERT_EQ(woi.GetDisplay(), display1) << "CaseDescription: "
        << "3. Check Set and Get Display (woi.GetDisplay() == display1)";
    woi.SetDisplay(display2);
    ASSERT_EQ(woi.GetDisplay(), display2) << "CaseDescription: "
        << "3. Check Set and Get Display (woi.GetY() == display2)";

    // 4. Check Set and Get ConsumerSurface
    auto csurface1 = Surface::CreateSurfaceAsConsumer();
    auto csurface2 = Surface::CreateSurfaceAsConsumer();
    woi.SetConsumerSurface(csurface1);
    ASSERT_EQ(woi.GetConsumerSurface(), csurface1) << "CaseDescription: "
        << "4. Check Set and Get ConsumerSurface (woi.GetConsumerSurface() == csurface1)";
    woi.SetConsumerSurface(csurface2);
    ASSERT_EQ(woi.GetConsumerSurface(), csurface2) << "CaseDescription: "
        << "4. Check Set and Get ConsumerSurface (woi.GetConsumerSurface() == csurface2)";
}

/*
 * Feature: WindowOptionImpl Invalid Set
 * Function: WindowOptionImpl
 * SubFunction: Invalid Set
 * FunctionPoints: WindowOptionImpl Invalid Set
 * EnvConditions: N/A
 * CaseDescription: 1. set invalid WindowType, check
 *                  2. set invalid WindowMode, check
 *                  3. set invalid Display, check
 *                  4. set invalid ConsumerSurface, check
 */
HWTEST_F(WindowOptionImplTest, InvalidSet, testing::ext::TestSize.Level0)
{
    WindowOptionImpl woi;

    // 1. set invalid WindowType, check
    constexpr auto invalidWindowType = static_cast<WindowType>(-1);
    ASSERT_EQ(woi.SetWindowType(invalidWindowType), WM_ERROR_INVALID_PARAM) << "CaseDescription: "
        << "1. set invalid WindowType, check (woi.SetWindowType() == WM_ERROR_INVALID_PARAM)";

    // 2. set invalid WindowMode, check
    constexpr auto invalidWindowMode = static_cast<WindowMode>(-1);
    ASSERT_EQ(woi.SetWindowMode(invalidWindowMode), WM_ERROR_INVALID_PARAM) << "CaseDescription: "
        << "2. set invalid WindowMode, check (woi.SetWindowMode() == WM_ERROR_INVALID_PARAM)";

    // 3. set invalid display, check
    constexpr auto invalidDisplay = -1;
    ASSERT_EQ(woi.SetDisplay(invalidDisplay), WM_ERROR_INVALID_PARAM) << "CaseDescription: "
        << "3. set invalid display, check (woi.SetDisplay() == WM_ERROR_INVALID_PARAM)";

    // 4. set invalid consumerSurface, check
    auto csurface = Surface::CreateSurfaceAsConsumer();
    auto producer = csurface->GetProducer();
    auto invalidConsumerSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_EQ(woi.SetConsumerSurface(invalidConsumerSurface), WM_ERROR_INVALID_PARAM) << "CaseDescription: "
        << "4. set invalid consumerSurface, check (woi.SetConsumerSurface() == WM_ERROR_INVALID_PARAM)";
}
} // namespace
} // namespace OHOS
