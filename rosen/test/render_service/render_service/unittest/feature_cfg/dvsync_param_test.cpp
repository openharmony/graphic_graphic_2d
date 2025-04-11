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
    DVSyncParam::SetDVSyncEnable(false);
    ASSERT_EQ(DVSyncParam::isRsDVSyncEnabled_, false);
    DVSyncParam::SetDVSyncEnable(true);
    ASSERT_EQ(DVSyncParam::isRsDVSyncEnabled_, true);
}

/**
 * @tc.name: SetUiDVSyncEnable
 * @tc.desc: Verify the SetUiDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetUiDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam::SetUiDVSyncEnable(false);
    ASSERT_EQ(DVSyncParam::isUiDVSyncEnabled_, false);
    DVSyncParam::SetUiDVSyncEnable(true);
    ASSERT_EQ(DVSyncParam::isUiDVSyncEnabled_, true);
}

/**
 * @tc.name: SetNativeDVSyncEnable
 * @tc.desc: Verify the SetNativeDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetNativeDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam::SetNativeDVSyncEnable(false);
    ASSERT_EQ(DVSyncParam::isNativeDVSyncEnabled_, false);
    DVSyncParam::SetNativeDVSyncEnable(true);
    ASSERT_EQ(DVSyncParam::isNativeDVSyncEnabled_, true);
}

/**
 * @tc.name: SetAdaptiveDVSyncEnable
 * @tc.desc: Verify the SetAdaptiveDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetAdaptiveDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam::SetAdaptiveDVSyncEnable(false);
    ASSERT_EQ(DVSyncParam::isAdaptiveDVSyncEnabled_, false);
    DVSyncParam::SetAdaptiveDVSyncEnable(true);
    ASSERT_EQ(DVSyncParam::isAdaptiveDVSyncEnabled_, true);
}

/**
 * @tc.name: SetRsBufferCount
 * @tc.desc: Verify the SetRsBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetRsBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam::SetRsBufferCount(1);
    ASSERT_EQ(DVSyncParam::rsBufferCount_, 1);
}

/**
 * @tc.name: SetUiBufferCount
 * @tc.desc: Verify the SetUiBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetUiBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam::SetUiBufferCount(3);
    ASSERT_EQ(DVSyncParam::uiBufferCount_, 3);
}

/**
 * @tc.name: SetNativeBufferCount
 * @tc.desc: Verify the SetNativeBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetNativeBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam::SetNativeBufferCount(2);
    ASSERT_EQ(DVSyncParam::nativeBufferCount_, 2);
}

/**
 * @tc.name: SetWebBufferCount
 * @tc.desc: Verify the SetWebBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, SetWebBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam::SetWebBufferCount(1);
    ASSERT_EQ(DVSyncParam::webBufferCount_, 1);
}

/**
 * @tc.name: IsDVSyncEnable
 * @tc.desc: Verify the IsDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, IsDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam::isRsDVSyncEnabled_ = true;
    ASSERT_EQ(DVSyncParam::IsDVSyncEnable(), true);
    DVSyncParam::isRsDVSyncEnabled_ = false;
    ASSERT_EQ(DVSyncParam::IsDVSyncEnable(), false);
}

/**
 * @tc.name: IsUiDVSyncEnable
 * @tc.desc: Verify the IsUiDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, IsUiDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam::isUiDVSyncEnabled_ = true;
    ASSERT_EQ(DVSyncParam::IsUiDVSyncEnable(), true);
    DVSyncParam::isUiDVSyncEnabled_ = false;
    ASSERT_EQ(DVSyncParam::IsUiDVSyncEnable(), false);
}

/**
 * @tc.name: IsNativeDVSyncEnable
 * @tc.desc: Verify the IsNativeDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, IsNativeDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam::isNativeDVSyncEnabled_ = true;
    ASSERT_EQ(DVSyncParam::IsNativeDVSyncEnable(), true);
    DVSyncParam::isNativeDVSyncEnabled_ = false;
    ASSERT_EQ(DVSyncParam::IsNativeDVSyncEnable(), false);
}

/**
 * @tc.name: IsAdaptiveDVSyncEnable
 * @tc.desc: Verify the IsAdaptiveDVSyncEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, IsAdaptiveDVSyncEnable, Function | SmallTest | Level1)
{
    DVSyncParam::isAdaptiveDVSyncEnabled_ = true;
    ASSERT_EQ(DVSyncParam::IsAdaptiveDVSyncEnable(), true);
    DVSyncParam::isAdaptiveDVSyncEnabled_ = false;
    ASSERT_EQ(DVSyncParam::IsAdaptiveDVSyncEnable(), false);
}

/**
 * @tc.name: GetUiBufferCount
 * @tc.desc: Verify the GetUiBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, GetUiBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam::uiBufferCount_ = 3;
    ASSERT_EQ(DVSyncParam::GetUiBufferCount(), 3);
}

/**
 * @tc.name: GetRsBufferCount
 * @tc.desc: Verify the GetRsBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, GetRsBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam::rsBufferCount_ = 1;
    ASSERT_EQ(DVSyncParam::GetRsBufferCount(), 1);
}

/**
 * @tc.name: GetNativeBufferCount
 * @tc.desc: Verify the GetNativeBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, GetNativeBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam::nativeBufferCount_ = 2;
    ASSERT_EQ(DVSyncParam::GetNativeBufferCount(), 2);
}

/**
 * @tc.name: GetWebBufferCount
 * @tc.desc: Verify the GetWebBufferCount function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DVSyncParamTest, GetWebBufferCount, Function | SmallTest | Level1)
{
    DVSyncParam::webBufferCount_ = 1;
    ASSERT_EQ(DVSyncParam::GetWebBufferCount(), 1);
}

/**
* @tc.name: SetAdaptiveConfig
* @tc.desc: Verify the SetAdaptiveConfig function
* @tc.type: FUNC
* @tc.require: #IBIE4T
*/
HWTEST_F(DVSyncParamTest, SetAdaptiveConfig, Function | SmallTest | Level1)
{
    std::string name = "test";
    std::string val = "1";
    DVSyncParam::SetAdaptiveConfig(name, val);
    ASSERT_EQ(DVSyncParam::adaptiveConfig_[name], val);
}

/**
* @tc.name: GetAdaptiveConfig
* @tc.desc: Verify the GetAdaptiveConfig function
* @tc.type: FUNC
* @tc.require: #IBIE4T
*/
HWTEST_F(DVSyncParamTest, GetAdaptiveConfig, Function | SmallTest | Level1)
{
    std::string name = "test";
    std::string val = "1";
    DVSyncParam::SetAdaptiveConfig(name, val);
    std::unordered_map<std::string, std::string> adaptiveConfig = DVSyncParam::GetAdaptiveConfig();
    ASSERT_EQ(adaptiveConfig[name], val);
}
} // namespace Rosen
} // namespace OHOS