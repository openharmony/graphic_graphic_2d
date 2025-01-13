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

#include "dvsync_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class DvsyncParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DvsyncParamTest::SetUpTestCase() {}
void DvsyncParamTest::TearDownTestCase() {}
void DvsyncParamTest::SetUp() {}
void DvsyncParamTest::TearDown() {}

/**
 * @tc.name: SetDvsyncEnable
 * @tc.desc: Verify the SetDvsyncEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DvsyncParamTest, SetDvsyncEnable, Function | SmallTest | Level1)
{
    DvsyncParam dvsyncParam;
    dvsyncParam.SetDvsyncEnable(true);
    EXPECT_EQ(dvsyncParam.isDvsyncEnable_, true);
    dvsyncParam.SetDvsyncEnable(false);
    EXPECT_EQ(dvsyncParam.isDvsyncEnable_, false);
}

/**
 * @tc.name: SetUiDvsyncEnable
 * @tc.desc: Verify the SetUiDvsyncEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DvsyncParamTest, SetUiDvsyncEnable, Function | SmallTest | Level1)
{
    DvsyncParam dvsyncParam;
    dvsyncParam.SetUiDvsyncEnable(true);
    EXPECT_EQ(dvsyncParam.isUiDvsyncEnable_, true);
    dvsyncParam.SetUiDvsyncEnable(false);
    EXPECT_EQ(dvsyncParam.isUiDvsyncEnable_, false);
}

/**
 * @tc.name: SetNativeDvsyncEnable
 * @tc.desc: Verify the SetNativeDvsyncEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DvsyncParamTest, SetNativeDvsyncEnable, Function | SmallTest | Level1)
{
    DvsyncParam dvsyncParam;
    dvsyncParam.SetNativeDvsyncEnable(true);
    EXPECT_EQ(dvsyncParam.isNativeDvsyncEnable_, true);
    dvsyncParam.SetNativeDvsyncEnable(false);
    EXPECT_EQ(dvsyncParam.isNativeDvsyncEnable_, false);
}

/**
 * @tc.name: SetRsPreCount
 * @tc.desc: Verify the SetRsPreCount function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DvsyncParamTest, SetRsPreCount, Function | SmallTest | Level1)
{
    DvsyncParam dvsyncParam;
    int32_t expCnt = 10;
    dvsyncParam.SetRsPreCount(expCnt);
    EXPECT_EQ(dvsyncParam.rsPreCnt_, expCnt);
}


/**
 * @tc.name: SetAppPreCount
 * @tc.desc: Verify the SetAppPreCount function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DvsyncParamTest, SetAppPreCount, Function | SmallTest | Level1)
{
    DvsyncParam dvsyncParam;
    int32_t expCnt = 10;
    dvsyncParam.SetAppPreCount(expCnt);
    EXPECT_EQ(dvsyncParam.appPreCnt_, expCnt);
}

/**
 * @tc.name: SetNativePreCount
 * @tc.desc: Verify the SetNativePreCount function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DvsyncParamTest, SetNativePreCount, Function | SmallTest | Level1)
{
    DvsyncParam dvsyncParam;
    int32_t expCnt = 10;
    dvsyncParam.SetNativePreCount(expCnt);
    EXPECT_EQ(dvsyncParam.nativePreCnt_, expCnt);
}

/**
 * @tc.name: isDvsyncEnable
 * @tc.desc: Verify the result of isDvsyncEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DvsyncParamTest, isDvsyncEnable, Function | SmallTest | Level1)
{
    DvsyncParam dvsyncParam;
    dvsyncParam.isDvsyncEnable_ = true;
    EXPECT_TRUE(dvsyncParam.IsDvsyncEnable());
    dvsyncParam.isDvsyncEnable_ = false;
    EXPECT_FALSE(dvsyncParam.IsDvsyncEnable());
}

/**
 * @tc.name: isUiDvsyncEnable
 * @tc.desc: Verify the result of isUiDvsyncEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DvsyncParamTest, isUiDvsyncEnable, Function | SmallTest | Level1)
{
    DvsyncParam dvsyncParam;
    dvsyncParam.isUiDvsyncEnable_ = true;
    EXPECT_TRUE(dvsyncParam.IsUiDvsyncEnable());
    dvsyncParam.isUiDvsyncEnable_ = false;
    EXPECT_FALSE(dvsyncParam.IsUiDvsyncEnable());
}

/**
 * @tc.name: isNativeDvsyncEnable
 * @tc.desc: Verify the result of isNativeDvsyncEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DvsyncParamTest, isNativeDvsyncEnable, Function | SmallTest | Level1)
{
    DvsyncParam dvsyncParam;
    dvsyncParam.isNativeDvsyncEnable_ = true;
    EXPECT_TRUE(dvsyncParam.IsNativeDvsyncEnable());
    dvsyncParam.isNativeDvsyncEnable_ = false;
    EXPECT_FALSE(dvsyncParam.IsNativeDvsyncEnable());
}

/**
 * @tc.name: GetRsPreCount
 * @tc.desc: Verify the result of GetRsPreCount function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DvsyncParamTest, GetRsPreCount, Function | SmallTest | Level1)
{
    DvsyncParam dvsyncParam;
    dvsyncParam.rsPreCnt_ = 5;
    EXPECT_EQ(dvsyncParam.GetRsPreCount(), 5);
}

/**
 * @tc.name: GetAppPreCount
 * @tc.desc: Verify the result of GetAppPreCount function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DvsyncParamTest, GetAppPreCount, Function | SmallTest | Level1)
{
    DvsyncParam dvsyncParam;
    dvsyncParam.appPreCnt_ = 5;
    EXPECT_EQ(dvsyncParam.GetAppPreCount(), 5);
}

/**
 * @tc.name: GetNativePreCount
 * @tc.desc: Verify the result of GetNativePreCount function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DvsyncParamTest, GetNativePreCount, Function | SmallTest | Level1)
{
    DvsyncParam dvsyncParam;
    dvsyncParam.nativePreCnt_ = 5;
    EXPECT_EQ(dvsyncParam.GetNativePreCount(), 5);
}
} // namespace Rosen
} // namespace OHOS