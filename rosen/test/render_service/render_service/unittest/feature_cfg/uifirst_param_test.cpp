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

#include "uifirst_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class UIFirstParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void UIFirstParamTest::SetUpTestCase() {}
void UIFirstParamTest::TearDownTestCase() {}
void UIFirstParamTest::SetUp() {}
void UIFirstParamTest::TearDown() {}

/**
 * @tc.name: SetUIFirstEnable
 * @tc.desc: Verify the SetUIFirstEnable function
 * @tc.type: FUNC
 * @tc.require: #IBOKAG
 */
HWTEST_F(UIFirstParamTest, SetUIFirstEnable, Function | SmallTest | Level1)
{
    UIFirstParam uifirstParam;
    uifirstParam.SetUIFirstEnable(true);
    ASSERT_EQ(uifirstParam.IsUIFirstEnable(), true);
    uifirstParam.SetUIFirstEnable(false);
    ASSERT_EQ(uifirstParam.IsUIFirstEnable(), false);
}

/**
 * @tc.name: SetCardUIFirstEnable
 * @tc.desc: Verify the SetCardUIFirstEnable function
 * @tc.type: FUNC
 * @tc.require: #IBOKAG
 */
HWTEST_F(UIFirstParamTest, SetCardUIFirstEnable, Function | SmallTest | Level1)
{
    UIFirstParam uifirstParam;
    uifirstParam.SetCardUIFirstEnable(true);
    ASSERT_EQ(uifirstParam.IsCardUIFirstEnable(), true);
    uifirstParam.SetCardUIFirstEnable(false);
    ASSERT_EQ(uifirstParam.IsCardUIFirstEnable(), false);
}

/**
 * @tc.name: SetCacheOptimizeRotateEnable
 * @tc.desc: Verify the SetCacheOptimizeRotateEnable function
 * @tc.type: FUNC
 * @tc.require: #IBOKAG
 */
HWTEST_F(UIFirstParamTest, SetCacheOptimizeRotateEnable, Function | SmallTest | Level1)
{
    UIFirstParam uifirstParam;
    uifirstParam.SetCacheOptimizeRotateEnable(true);
    ASSERT_EQ(uifirstParam.IsCacheOptimizeRotateEnable(), true);
    uifirstParam.SetCacheOptimizeRotateEnable(false);
    ASSERT_EQ(uifirstParam.IsCacheOptimizeRotateEnable(), false);
}

/**
 * @tc.name: SetFreeMultiWindowEnable
 * @tc.desc: Verify the SetFreeMultiWindowEnable function
 * @tc.type: FUNC
 * @tc.require: #IBOKAG
 */
HWTEST_F(UIFirstParamTest, SetFreeMultiWindowEnable, Function | SmallTest | Level1)
{
    UIFirstParam uifirstParam;
    uifirstParam.SetFreeMultiWindowEnable(true);
    ASSERT_EQ(uifirstParam.IsFreeMultiWindowEnable(), true);
    uifirstParam.SetFreeMultiWindowEnable(false);
    ASSERT_EQ(uifirstParam.IsFreeMultiWindowEnable(), false);
}

/**
 * @tc.name: SetUIFirstEnableWindowThreshold
 * @tc.desc: Verify the SetUIFirstEnableWindowThreshold function
 * @tc.type: FUNC
 * @tc.require: #IBOKAG
 */
HWTEST_F(UIFirstParamTest, SetUIFirstEnableWindowThreshold, Function | SmallTest | Level1)
{
    UIFirstParam uifirstParam;
    uifirstParam.SetUIFirstEnableWindowThreshold(2);
    ASSERT_EQ(uifirstParam.GetUIFirstEnableWindowThreshold(), 2);
}
} // namespace Rosen
} // namespace OHOS