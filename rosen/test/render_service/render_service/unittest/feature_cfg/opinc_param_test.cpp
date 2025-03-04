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

#include "opinc_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class OPIncParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void OPIncParamTest::SetUpTestCase() {}
void OPIncParamTest::TearDownTestCase() {}
void OPIncParamTest::SetUp() {}
void OPIncParamTest::TearDown() {}

/**
 * @tc.name: SetOPIncEnable
 * @tc.desc: Verify the SetOPIncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBOKAG
 */
HWTEST_F(OPIncParamTest, SetOPIncEnable, Function | SmallTest | Level1)
{
    OPIncParam opincParam;
    opincParam.SetOPIncEnable(true);
    ASSERT_EQ(opincParam.IsOPIncEnable(), true);
    opincParam.SetOPIncEnable(false);
    ASSERT_EQ(opincParam.IsOPIncEnable(), false);
}

/**
 * @tc.name: SetCacheWidthThresholdPercentValue
 * @tc.desc: Verify the SetCacheWidthThresholdPercentValue function
 * @tc.type: FUNC
 * @tc.require: #IBOKAG
 */
HWTEST_F(OPIncParamTest, SetCacheWidthThresholdPercentValue, Function | SmallTest | Level1)
{
    OPIncParam opincParam;
    opincParam.SetCacheWidthThresholdPercentValue(100);
    ASSERT_EQ(opincParam.GetCacheWidthThresholdPercentValue(), 100);
}
} // namespace Rosen
} // namespace OHOS