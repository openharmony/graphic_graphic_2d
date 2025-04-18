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

#include "prevalidate_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class PrevalidateParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void PrevalidateParamTest::SetUpTestCase() {}
void PrevalidateParamTest::TearDownTestCase() {}
void PrevalidateParamTest::SetUp() {}
void PrevalidateParamTest::TearDown() {}

/**
 * @tc.name: ParamSettingAndGettingTest
 * @tc.desc: Verify the SetPrevalidateEnable and IsPrevalidateEnable function
 * @tc.type: FUNC
 * @tc.require: #IBMVNU
 */
HWTEST_F(PrevalidateParamTest, ParamSettingAndGettingTest, Function | SmallTest | Level1)
{
    PrevalidateParam::SetPrevalidateEnable(false);
    ASSERT_FALSE(PrevalidateParam::IsPrevalidateEnable());
    PrevalidateParam::SetPrevalidateEnable(true);
    ASSERT_TRUE(PrevalidateParam::IsPrevalidateEnable());
}
} // namespace Rosen
} // namespace OHOS