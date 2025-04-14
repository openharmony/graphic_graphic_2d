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
    ASSERT_EQ(hwcParam.sourceTuningMap_[appName], "1");
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
    ASSERT_EQ(hwcParam.solidColorLayerMap_[appName], "1");
}

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
} // namespace Rosen
} // namespace OHOS