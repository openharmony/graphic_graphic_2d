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

class DVSyncParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DVSyncParamTest::SetUpTestCase() {}
void DVSyncParamTest::TearDownTestCase() {}
void DVSyncParamTest::SetUp() {}
void DVSyncParamTest::TearDown() {}

/**
 * @tc.name: SetDVSyncEnable
 * @tc.desc: Verify the SetDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.SetDVSyncEnable(false);
    ASSERT_EQ(dvsyncParam.isRsDVSyncEnabled_, false);
    dvsyncParam.SetDVSyncEnable(true);
    ASSERT_EQ(dvsyncParam.isRsDVSyncEnabled_, true);
}

/**
 * @tc.name: SetUiDVSyncEnable
 * @tc.desc: Verify the SetUiDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetUiDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.SetUiDVSyncEnable(false);
    ASSERT_EQ(dvsyncParam.isUiDVSyncEnabled_, false);
    dvsyncParam.SetUiDVSyncEnable(true);
    ASSERT_EQ(dvsyncParam.isUiDVSyncEnabled_, true);
}

/**
 * @tc.name: SetNativeDVSyncEnable
 * @tc.desc: Verify the SetNativeDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetNativeDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.SetNativeDVSyncEnable(false);
    ASSERT_EQ(dvsyncParam.isNativeDVSyncEnabled_, false);
    dvsyncParam.SetNativeDVSyncEnable(true);
    ASSERT_EQ(dvsyncParam.isNativeDVSyncEnabled_, true);
}

/**
 * @tc.name: SetAdaptiveDVSyncEnable
 * @tc.desc: Verify the SetAdaptiveDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetAdaptiveDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.SetAdaptiveDVSyncEnable(false);
    ASSERT_EQ(dvsyncParam.isAdaptiveDVSyncEnabled_, false);
    dvsyncParam.SetAdaptiveDVSyncEnable(true);
    ASSERT_EQ(dvsyncParam.isAdaptiveDVSyncEnabled_, true);
}

/**
 * @tc.name: SetRsBufferCount
 * @tc.desc: Verify the SetRsBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetRsBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.SetRsBufferCount(1);
    ASSERT_EQ(dvsyncParam.rsBufferCount_, 1);
}

/**
 * @tc.name: SetUiBufferCount
 * @tc.desc: Verify the SetUiBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetUiBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.SetUiBufferCount(3);
    ASSERT_EQ(dvsyncParam.uiBufferCount_, 3);
}

/**
 * @tc.name: SetNativeBufferCount
 * @tc.desc: Verify the SetNativeBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetNativeBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.SetNativeBufferCount(2);
    ASSERT_EQ(dvsyncParam.nativeBufferCount_, 2);
}

/**
 * @tc.name: SetWebBufferCount
 * @tc.desc: Verify the SetWebBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetWebBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.SetWebBufferCount(1);
    ASSERT_EQ(dvsyncParam.webBufferCount_, 1);
}

/**
 * @tc.name: IsDVSyncEnable
 * @tc.desc: Verify the IsDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, IsDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.isRsDVSyncEnabled_ = true;
    ASSERT_EQ(dvsyncParam.IsDVSyncEnable(), true);
    dvsyncParam.isRsDVSyncEnabled_ = false;
    ASSERT_EQ(dvsyncParam.IsDVSyncEnable(), false);
}

/**
 * @tc.name: IsUiDVSyncEnable
 * @tc.desc: Verify the IsUiDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, IsUiDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.isUiDVSyncEnabled_ = true;
    ASSERT_EQ(dvsyncParam.IsUiDVSyncEnable(), true);
    dvsyncParam.isUiDVSyncEnabled_ = false;
    ASSERT_EQ(dvsyncParam.IsUiDVSyncEnable(), false);
}

/**
 * @tc.name: IsNativeDVSyncEnable
 * @tc.desc: Verify the IsNativeDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, IsNativeDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.isNativeDVSyncEnabled_ = true;
    ASSERT_EQ(dvsyncParam.IsNativeDVSyncEnable(), true);
    dvsyncParam.isNativeDVSyncEnabled_ = false;
    ASSERT_EQ(dvsyncParam.IsNativeDVSyncEnable(), false);
}

/**
 * @tc.name: IsAdaptiveDVSyncEnable
 * @tc.desc: Verify the IsAdaptiveDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, IsAdaptiveDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.isAdaptiveDVSyncEnabled_ = true;
    ASSERT_EQ(dvsyncParam.IsAdaptiveDVSyncEnable(), true);
    dvsyncParam.isAdaptiveDVSyncEnabled_ = false;
    ASSERT_EQ(dvsyncParam.IsAdaptiveDVSyncEnable(), false);
}

/**
 * @tc.name: GetUiBufferCount
 * @tc.desc: Verify the GetUiBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, GetUiBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.uiBufferCount_ = 3;
    ASSERT_EQ(dvsyncParam.GetUiBufferCount(), 3);
}

/**
 * @tc.name: GetRsBufferCount
 * @tc.desc: Verify the GetRsBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, GetRsBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.rsBufferCount_ = 1;
    ASSERT_EQ(dvsyncParam.GetRsBufferCount(), 1);
}

/**
 * @tc.name: GetNativeBufferCount
 * @tc.desc: Verify the GetNativeBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, GetNativeBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.nativeBufferCount_ = 2;
    ASSERT_EQ(dvsyncParam.GetNativeBufferCount(), 2);
}

/**
 * @tc.name: GetWebBufferCount
 * @tc.desc: Verify the GetWebBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, GetWebBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam dvsyncParam;
    dvsyncParam.webBufferCount_ = 1;
    ASSERT_EQ(dvsyncParam.GetWebBufferCount(), 1);
}
} // namespace Rosen
} // namespace OHOS