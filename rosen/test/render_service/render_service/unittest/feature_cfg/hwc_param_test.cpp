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
 * @tc.name: SetHwcEnable
 * @tc.desc: Verify the SetHwcEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(HwcParamTest, SetHwcEnable, Function | SmallTest | Level1)
{
    HWCParam hwcParam;
    hwcParam.SetHwcEnable(true);
    EXPECT_EQ(hwcParam.isHwcEnable_, true);
    hwcParam.SetHwcEnable(false);
    EXPECT_EQ(hwcParam.isHwcEnable_, false);
}

/**
 * @tc.name: SetHwcMirrorEnable
 * @tc.desc: Verify the SetHwcMirrorEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(HwcParamTest, SetHwcMirrorEnable, Function | SmallTest | Level1)
{
    HWCParam hwcParam;
    hwcParam.SetHwcMirrorEnable(true);
    EXPECT_EQ(hwcParam.isHwcMirrorEnable_, true);
    hwcParam.SetHwcMirrorEnable(false);
    EXPECT_EQ(hwcParam.isHwcMirrorEnable_, false);
}

/**
 * @tc.name: SetSourceTuningForApp
 * @tc.desc: Verify the SetSourceTuningForApp function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(HwcParamTest, SetSourceTuningForApp, Function | SmallTest | Level1)
{
    HWCParam hwcParam;
    std::string appName = "testApp";
    std::string val = "1";
    hwcParam.SetSourceTuningForApp(appName, val);
    ASSERT_EQ(hwcParam.sourceTuningMap_[appName], 1);
}

/**
 * @tc.name: SetSolidColorLayerForApp
 * @tc.desc: Verify the SetSolidColorLayerForApp function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(HwcParamTest, SetSolidColorLayerForApp, Function | SmallTest | Level1)
{
    HWCParam hwcParam;
    std::string appName = "testApp";
    std::string val = "1";
    hwcParam.SetSolidColorLayerForApp(appName, val);
    ASSERT_EQ(hwcParam.solidColorLayerMap_[appName], 1);
}

/**
 * @tc.name: IsHwcEnable
 * @tc.desc: Verify the result of IsHwcEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(HwcParamTest, IsHwcEnable, Function | SmallTest | Level1)
{
    HWCParam hwcParam;
    hwcParam.isHwcEnable_ = true;
    EXPECT_TRUE(hwcParam.IsHwcEnable());
    hwcParam.isHwcEnable_ = false;
    EXPECT_FALSE(hwcParam.IsHwcEnable());
}

/**
 * @tc.name: IsHwcMirrorEnable
 * @tc.desc: Verify the result of IsHwcMirrorEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(HwcParamTest, IsHwcMirrorEnable, Function | SmallTest | Level1)
{
    HWCParam hwcParam;
    hwcParam.isHwcMirrorEnable_ = true;
    EXPECT_TRUE(hwcParam.IsHwcMirrorEnable());
    hwcParam.isHwcMirrorEnable_ = false;
    EXPECT_FALSE(hwcParam.IsHwcMirrorEnable());
}

/**
 * @tc.name: GetSourceTuningForApp
 * @tc.desc: Verify the result of GetSourceTuningForApp function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(HwcParamTest, GetSourceTuningForApp, Function | SmallTest | Level1)
{
    HWCParam hwcParam;
    std::string appName = "testApp";
    int expectedVal = 1;
    hwcParam.sourceTuningMap_[appName] = expectedVal;
    int actualVal = hwcParam.GetSourceTuningForApp(appName);
    EXPECT_EQ(actualVal, expectedVal);
    // Check if not has the appName
    std::string nonExistAppName = "nonExistApp";
    int nonVal = hwcParam.GetSourceTuningForApp(nonExistAppName);
    EXPECT_EQ(nonVal, -1);
}

/**
 * @tc.name: GetSolidColorLayerForApp
 * @tc.desc: Verify the result of GetSolidColorLayerForApp function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(HwcParamTest, GetSolidColorLayerForApp, Function | SmallTest | Level1)
{
    HWCParam hwcParam;
    std::string appName = "testApp";
    int expectedVal = 1;
    hwcParam.solidColorLayerMap_[appName] = expectedVal;
    int actualVal = hwcParam.GetSolidColorLayerForApp(appName);
    EXPECT_EQ(actualVal, expectedVal);
    // Check if not has the appName
    std::string nonExistAppName = "nonExistApp";
    int nonVal = hwcParam.GetSolidColorLayerForApp(nonExistAppName);
    EXPECT_EQ(nonVal, -1);
}
} // namespace Rosen
} // namespace OHOS