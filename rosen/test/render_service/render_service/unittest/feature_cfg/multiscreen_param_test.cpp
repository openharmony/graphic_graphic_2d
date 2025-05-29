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

#include "multiscreen_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class MultiScreenParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void MultiScreenParamTest::SetUpTestCase() {}
void MultiScreenParamTest::TearDownTestCase() {}
void MultiScreenParamTest::SetUp() {}
void MultiScreenParamTest::TearDown() {}

/**
 * @tc.name: SetExternalScreenSecure
 * @tc.desc: Verify the SetExternalScreenSecure/IsExternalScreenSecure function
 * @tc.type: FUNC
 * @tc.require: #IBOA5Q
 */
HWTEST_F(MultiScreenParamTest, SetExternalScreenSecure, Function | SmallTest | Level1)
{
    MultiScreenParam::SetExternalScreenSecure(true);
    EXPECT_TRUE(MultiScreenParam::IsExternalScreenSecure());
    MultiScreenParam::SetExternalScreenSecure(false);
    EXPECT_FALSE(MultiScreenParam::IsExternalScreenSecure());
}

/**
 * @tc.name: SetSlrScaleEnabled
 * @tc.desc: Verify the SetSlrScaleEnabled/IsSlrScaleEnabled function
 * @tc.type: FUNC
 * @tc.require: #IBOA5Q
 */
HWTEST_F(MultiScreenParamTest, SetSlrScaleEnabled, Function | SmallTest | Level1)
{
    MultiScreenParam::SetSlrScaleEnabled(true);
    EXPECT_TRUE(MultiScreenParam::IsSlrScaleEnabled());
    MultiScreenParam::SetSlrScaleEnabled(false);
    EXPECT_FALSE(MultiScreenParam::IsSlrScaleEnabled());
}

/**
 * @tc.name: SetRsReportHwcDead
 * @tc.desc: Verify the SetRsReportHwcDead/IsRsReportHwcDead function
 * @tc.type: FUNC
 * @tc.require: #IBOA5Q
 */
HWTEST_F(MultiScreenParamTest, SetRsReportHwcDead, Function | SmallTest | Level1)
{
    MultiScreenParam::SetRsReportHwcDead(true);
    EXPECT_TRUE(MultiScreenParam::IsRsReportHwcDead());
    MultiScreenParam::SetRsReportHwcDead(false);
    EXPECT_FALSE(MultiScreenParam::IsRsReportHwcDead());
}

/**
 * @tc.name: SetRsSetScreenPowerStatus
 * @tc.desc: Verify the SetRsSetScreenPowerStatus/IsRsSetScreenPowerStatus function
 * @tc.type: FUNC
 * @tc.require: #IBOA5Q
 */
HWTEST_F(MultiScreenParamTest, SetRsSetScreenPowerStatus, Function | SmallTest | Level1)
{
    MultiScreenParam::SetRsSetScreenPowerStatus(true);
    EXPECT_TRUE(MultiScreenParam::IsRsSetScreenPowerStatus());
    MultiScreenParam::SetRsSetScreenPowerStatus(false);
    EXPECT_FALSE(MultiScreenParam::IsRsSetScreenPowerStatus());
}

/**
 * @tc.name: SetMirrorDisplayCloseP3
 * @tc.desc: Verify the SetMirrorDisplayCloseP3/IsMirrorDisplayCloseP3 function
 * @tc.type: FUNC
 * @tc.require: #IBOA5Q
 */
HWTEST_F(MultiScreenParamTest, SetMirrorDisplayCloseP3, Function | SmallTest | Level1)
{
    MultiScreenParam::SetMirrorDisplayCloseP3(true);
    EXPECT_TRUE(MultiScreenParam::IsMirrorDisplayCloseP3());
    MultiScreenParam::SetMirrorDisplayCloseP3(false);
    EXPECT_FALSE(MultiScreenParam::IsMirrorDisplayCloseP3());
}
} // namespace Rosen
} // namespace OHOS