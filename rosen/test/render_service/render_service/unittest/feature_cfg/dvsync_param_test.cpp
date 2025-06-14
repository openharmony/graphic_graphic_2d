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
    DVSyncParam param;
    param.SetDVSyncEnable(false);
    ASSERT_EQ(param.IsDVSyncEnable(), false);
    param.SetDVSyncEnable(true);
    ASSERT_EQ(param.IsDVSyncEnable(), true);
}

/**
 * @tc.name: SetUiDVSyncEnable
 * @tc.desc: Verify the SetUiDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetUiDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam param;
    param.SetUiDVSyncEnable(true);
    ASSERT_EQ(param.IsUiDVSyncEnable(), true);
    param.SetUiDVSyncEnable(false);
    ASSERT_EQ(param.IsUiDVSyncEnable(), false);
}

/**
 * @tc.name: SetNativeDVSyncEnable
 * @tc.desc: Verify the SetNativeDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetNativeDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam param;
    param.SetNativeDVSyncEnable(false);
    ASSERT_EQ(param.IsNativeDVSyncEnable(), false);
    param.SetNativeDVSyncEnable(true);
    ASSERT_EQ(param.IsNativeDVSyncEnable(), true);
}

/**
 * @tc.name: SetAdaptiveDVSyncEnable
 * @tc.desc: Verify the SetAdaptiveDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetAdaptiveDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam param;
    param.SetAdaptiveDVSyncEnable(false);
    ASSERT_EQ(param.IsAdaptiveDVSyncEnable(), false);
    param.SetAdaptiveDVSyncEnable(true);
    ASSERT_EQ(param.IsAdaptiveDVSyncEnable(), true);
}

/**
 * @tc.name: SetRsBufferCount
 * @tc.desc: Verify the SetRsBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetRsBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam param;
    param.SetRsBufferCount(1);
    ASSERT_EQ(param.rsBufferCount_, 1);
}

/**
 * @tc.name: SetUiBufferCount
 * @tc.desc: Verify the SetUiBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetUiBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam param;
    param.SetUiBufferCount(3);
    ASSERT_EQ(param.uiBufferCount_, 3);
}

/**
 * @tc.name: SetNativeBufferCount
 * @tc.desc: Verify the SetNativeBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetNativeBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam param;
    param.SetNativeBufferCount(2);
    ASSERT_EQ(param.nativeBufferCount_, 2);
}

/**
 * @tc.name: SetWebBufferCount
 * @tc.desc: Verify the SetWebBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetWebBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam param;
    param.SetWebBufferCount(1);
    ASSERT_EQ(param.webBufferCount_, 1);
}

/**
 * @tc.name: IsDVSyncEnable
 * @tc.desc: Verify the IsDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, IsDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam param;
    param.SetDVSyncEnable(true);
    ASSERT_EQ(param.IsDVSyncEnable(), true);
    param.SetDVSyncEnable(false);
    ASSERT_EQ(param.IsDVSyncEnable(), false);
}

/**
 * @tc.name: IsUiDVSyncEnable
 * @tc.desc: Verify the IsUiDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, IsUiDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam param;
    param.SetUiDVSyncEnable(true);
    ASSERT_EQ(param.IsUiDVSyncEnable(), true);
    param.SetUiDVSyncEnable(false);
    ASSERT_EQ(param.IsUiDVSyncEnable(), false);
}

/**
 * @tc.name: IsNativeDVSyncEnable
 * @tc.desc: Verify the IsNativeDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, IsNativeDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam param;
    param.SetNativeDVSyncEnable(true);
    ASSERT_EQ(param.IsNativeDVSyncEnable(), true);
    param.SetNativeDVSyncEnable(false);
    ASSERT_EQ(param.IsNativeDVSyncEnable(), false);
}

/**
 * @tc.name: IsAdaptiveDVSyncEnable
 * @tc.desc: Verify the IsAdaptiveDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, IsAdaptiveDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam param;
    param.SetAdaptiveDVSyncEnable(true);
    ASSERT_EQ(param.IsAdaptiveDVSyncEnable(), true);
    param.SetAdaptiveDVSyncEnable(false);
    ASSERT_EQ(param.IsAdaptiveDVSyncEnable(), false);
}

/**
 * @tc.name: GetUiBufferCount
 * @tc.desc: Verify the GetUiBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, GetUiBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam param;
    param.uiBufferCount_ = 3;
    ASSERT_EQ(param.GetUiBufferCount(), 3);
}

/**
 * @tc.name: GetRsBufferCount
 * @tc.desc: Verify the GetRsBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, GetRsBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam param;
    param.rsBufferCount_ = 1;
    ASSERT_EQ(param.GetRsBufferCount(), 1);
}

/**
 * @tc.name: GetNativeBufferCount
 * @tc.desc: Verify the GetNativeBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, GetNativeBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam param;
    param.nativeBufferCount_ = 2;
    ASSERT_EQ(param.GetNativeBufferCount(), 2);
}

/**
 * @tc.name: GetWebBufferCount
 * @tc.desc: Verify the GetWebBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, GetWebBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam param;
    param.webBufferCount_ = 1;
    ASSERT_EQ(param.GetWebBufferCount(), 1);
}

/**
* @tc.name: SetAdaptiveConfig
* @tc.desc: Verify the SetAdaptiveConfig function
* @tc.type: FUNC
* @tc.require: #IBIE4T
*/
HWTEST_F(DVSyncParamTest, SetAdaptiveConfig, Function | SmallTest | Level1)
{
    DVSyncParam param;
    std::string name = "test";
    std::string val = "1";
    param.SetAdaptiveConfig(name, val);
    auto adaptiveConfig = param.GetAdaptiveConfig();
    ASSERT_EQ(adaptiveConfig[name], val);
}

/**
* @tc.name: GetAdaptiveConfig
* @tc.desc: Verify the GetAdaptiveConfig function
* @tc.type: FUNC
* @tc.require: #IBIE4T
*/
HWTEST_F(DVSyncParamTest, GetAdaptiveConfig, Function | SmallTest | Level1)
{
    DVSyncParam param;
    std::string name = "test";
    std::string val = "1";
    param.SetAdaptiveConfig(name, val);
    std::unordered_map<std::string, std::string> adaptiveConfig = param.GetAdaptiveConfig();
    ASSERT_EQ(adaptiveConfig[name], val);
}
} // namespace Rosen
} // namespace OHOS