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
 
#include "hard_cursor_param.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HardCursorParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HardCursorParamTest::SetUpTestCase() {}
void HardCursorParamTest::TearDownTestCase() {}
void HardCursorParamTest::SetUp() {}
void HardCursorParamTest::TearDown() {}

/**
 * @tc.name: SetHardCursorEnable
 * @tc.desc: Verify the SetHardCursorEnable function
 * @tc.type: FUNC
 * @tc.require: #IBMVNU
 */
HWTEST_F(HardCursorParamTest, SetHardCursorEnable, Function | SmallTest | Level1)
{
    HardCursorParam hardCursorParam;
    hardCursorParam.SetHardCursorEnable(true);
    ASSERT_EQ(hardCursorParam.isHardCursorEnable_, true);
    hardCursorParam.SetHardCursorEnable(false);
    ASSERT_EQ(hardCursorParam.isHardCursorEnable_, false);
}

/**
 * @tc.name: IsHardCursorEnable
 * @tc.desc: Verify the result of IsHardCursorEnable function
 * @tc.type: FUNC
 * @tc.require: #IBMVNU
 */
HWTEST_F(HardCursorParamTest, IsHardCursorEnable, Function | SmallTest | Level1)
{
    HardCursorParam hardCursorParam;
    hardCursorParam.isHardCursorEnable_ = true;
    ASSERT_TRUE(hardCursorParam.IsHardCursorEnable());
}
} // namespace Rosen
} // namespace OHOS