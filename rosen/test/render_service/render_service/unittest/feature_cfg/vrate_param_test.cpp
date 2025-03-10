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

#include "vrate_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class VRateParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void VRateParamTest::SetUpTestCase() {}
void VRateParamTest::TearDownTestCase() {}
void VRateParamTest::SetUp() {}
void VRateParamTest::TearDown() {}

/**
 * @tc.name: SetVRateEnable
 * @tc.desc: Verify the SetVRateEnable function
 * @tc.type: FUNC
 * @tc.require: #IBOKAG
 */
HWTEST_F(VRateParamTest, SetVRateEnable, Function | SmallTest | Level1)
{
    VRateParam vrateParam;
    vrateParam.SetVRateEnable(true);
    ASSERT_EQ(vrateParam.GetVRateEnable(), true);
    vrateParam.SetVRateEnable(false);
    ASSERT_EQ(vrateParam.GetVRateEnable(), false);
}
} // namespace Rosen
} // namespace OHOS