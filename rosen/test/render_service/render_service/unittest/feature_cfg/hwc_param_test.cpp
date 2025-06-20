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

#include "hwc_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class HwcParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HwcParamTest::SetUpTestCase() {}
void HwcParamTest::TearDownTestCase() {}
void HwcParamTest::SetUp() {}
void HwcParamTest::TearDown() {}

/**
 * @tc.name: SetDisableHwcOnExpandScreen
 * @tc.desc: Verify the SetDisableHwcOnExpandScreen function
 * @tc.type: FUNC
 * @tc.require: #IBOLM8
 */
HWTEST_F(HwcParamTest, SetDisableHwcOnExpandScreen, Function | SmallTest | Level1)
{
    HWCParam::SetDisableHwcOnExpandScreen(true);
    ASSERT_TRUE(HWCParam::IsDisableHwcOnExpandScreen());
    HWCParam::SetDisableHwcOnExpandScreen(false);
    ASSERT_FALSE(HWCParam::IsDisableHwcOnExpandScreen());
}
 
/**
 * @tc.name: IsDisableHwcOnExpandScreen001
 * @tc.desc: Verify the IsDisableHwcOnExpandScreen function
 * @tc.type: FUNC
 * @tc.require: #IBOLM8
 */
HWTEST_F(HwcParamTest, IsDisableHwcOnExpandScreen001, Function | SmallTest | Level1)
{
    HWCParam::SetDisableHwcOnExpandScreen(true);
    ASSERT_TRUE(HWCParam::IsDisableHwcOnExpandScreen());
    HWCParam::SetDisableHwcOnExpandScreen(false);
    ASSERT_FALSE(HWCParam::IsDisableHwcOnExpandScreen());
}

/**
 * @tc.name: SetSolidColorLayerForApp001
 * @tc.desc: Verify the SetSolidColorLayerForApp function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HwcParamTest, SetSolidColorLayerForApp001, Function | SmallTest | Level1)
{
    HWCParam hwcParam;
    std::string appName = "TestApp";
    std::string val = "1";
    hwcParam.SetSolidColorLayerForApp(appName, val);
    auto it = hwcParam.solidColorLayerMap_.find(appName);
    EXPECT_NE(it, hwcParam.solidColorLayerMap_.end());
    EXPECT_EQ(it->second, val);
}

/**
 * @tc.name: SetSolidColorLayerForApp002
 * @tc.desc: Verify the SetSolidColorLayerForApp function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HwcParamTest, SetSolidColorLayerForApp002, Function | SmallTest | Level1)
{
    HWCParam hwcParam;
    std::string appName = "TestApp";
    std::string initialVal = "0";
    hwcParam.SetSolidColorLayerForApp(appName, initialVal);
    std::string newVal = "1";
    hwcParam.SetSolidColorLayerForApp(appName, newVal);
    auto it = hwcParam.solidColorLayerMap_.find(appName);
    EXPECT_NE(it, hwcParam.solidColorLayerMap_.end());
    EXPECT_EQ(it->second, newVal);
}

/**
 * @tc.name: SetSourceTuningForApp001
 * @tc.desc: Verify the SetSourceTuningForApp function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HwcParamTest, SetSourceTuningForApp001, Function | SmallTest | Level1)
{
    HWCParam hwcParam;
    std::string appName = "TestApp";
    std::string val = "1";
    hwcParam.SetSourceTuningForApp(appName, val);
    auto it = hwcParam.sourceTuningMap_.find(appName);
    EXPECT_NE(it, hwcParam.sourceTuningMap_.end());
    EXPECT_EQ(it->second, val);
}

/**
 * @tc.name: SetSourceTuningForApp002
 * @tc.desc: Verify the SetSourceTuningForApp function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HwcParamTest, SetSourceTuningForApp002, Function | SmallTest | Level1)
{
    HWCParam hwcParam;
    std::string appName = "TestApp";
    std::string initialVal = "0";
    hwcParam.SetSourceTuningForApp(appName, initialVal);
    std::string newVal = "1";
    hwcParam.SetSourceTuningForApp(appName, newVal);
    auto it = hwcParam.sourceTuningMap_.find(appName);
    EXPECT_NE(it, hwcParam.sourceTuningMap_.end());
    EXPECT_EQ(it->second, newVal);
}

/**
 * @tc.name: IsSolidLayerEnable001
 * @tc.desc: Verify the IsSolidLayerEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HwcParamTest, IsSolidLayerEnable001, Function | SmallTest | Level1)
{
    HWCParam::SetSolidLayerEnable(true);
    ASSERT_TRUE(HWCParam::IsSolidLayerEnable());
    HWCParam::SetSolidLayerEnable(false);
    ASSERT_FALSE(HWCParam::IsSolidLayerEnable());
}
} // namespace Rosen
} // namespace OHOS