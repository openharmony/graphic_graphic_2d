/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <test_header.h>

#include "rotateoffscreen_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RotateOffScreenParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RotateOffScreenParamTest::SetUpTestCase() {}
void RotateOffScreenParamTest::TearDownTestCase() {}
void RotateOffScreenParamTest::SetUp() {}
void RotateOffScreenParamTest::TearDown() {}

/**
 * @tc.name: SetRotateOffScreenDisplayNodeEnable
 * @tc.desc: Verify the SetRotateOffScreenDisplayNodeEnable function
 * @tc.type: FUNC
 * @tc.require: #IBOKAG
 */
HWTEST_F(RotateOffScreenParamTest, SetRotateOffScreenDisplayNodeEnable, Function | SmallTest | Level1)
{
    RotateOffScreenParam::SetRotateOffScreenDisplayNodeEnable (true);
    ASSERT_EQ(RotateOffScreenParam::GetRotateOffScreenDisplayNodeEnable(), true);
    RotateOffScreenParam::SetRotateOffScreenDisplayNodeEnable(false);
    ASSERT_EQ(RotateOffScreenParam::GetRotateOffScreenDisplayNodeEnable(), false);
}

/**
 * @tc.name: SetRotateOffScreenSurfaceNodeEnable
 * @tc.desc: Verify the SetRotateOffScreenSurfaceNodeEnable function
 * @tc.type: FUNC
 * @tc.require: #IBOKAG
 */
HWTEST_F(RotateOffScreenParamTest, SetRotateOffScreenSurfaceNodeEnable, Function | SmallTest | Level1)
{
    RotateOffScreenParam::SetRotateOffScreenSurfaceNodeEnable (true);
    ASSERT_EQ(RotateOffScreenParam::GetRotateOffScreenSurfaceNodeEnable(), true);
    RotateOffScreenParam::SetRotateOffScreenSurfaceNodeEnable(false);
    ASSERT_EQ(RotateOffScreenParam::GetRotateOffScreenSurfaceNodeEnable(), false);
}

/**
 * @tc.name: SetRotateOffScreenDowngradeEnabled
 * @tc.desc: Verify the SetRotateOffScreenDowngradeEnabled function
 * @tc.type: FUNC
 * @tc.require: #
 */
HWTEST_F(RotateOffScreenParamTest, SetRotateOffScreenSurfaceNodeEnable, Function | SmallTest | Level1)
{
    RotateOffScreenParam::SetRotateOffScreenDowngradeEnabled (true);
    ASSERT_EQ(RotateOffScreenParam::GetRotateOffScreenDowngradeEnabled(), true);
    RotateOffScreenParam::SetRotateOffScreenDowngradeEnabled(false);
    ASSERT_EQ(RotateOffScreenParam::GetRotateOffScreenDowngradeEnabled(), false);
}
} // namespace Rosen
} // namespace OHOS