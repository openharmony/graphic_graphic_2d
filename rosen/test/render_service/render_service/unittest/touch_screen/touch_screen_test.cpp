/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "limit_number.h"
#include "parameters.h"

#include "touch_screen/touch_screen.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class TouchScreenTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void TouchScreenTest::SetUpTestCase() {}
void TouchScreenTest::TearDownTestCase() {}
void TouchScreenTest::SetUp() {}
void TouchScreenTest::TearDown() {}

/*
 * @tc.name: InitTouchScreen_001
 * @tc.desc: Test InitTouchScreen
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(TouchScreenTest, InitTouchScreen_001, TestSize.Level1)
{
    ASSERT_EQ(nullptr, TOUCH_SCREEN->tsSetFeatureConfig_);
    TOUCH_SCREEN->InitTouchScreen();
    ASSERT_NE(nullptr, TOUCH_SCREEN->tsSetFeatureConfig_);
}

/*
 * @tc.name: SetFeatureConfig_001
 * @tc.desc: Test SetFeatureConfig
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(TouchScreenTest, SetFeatureConfig_001, TestSize.Level1)
{
    TOUCH_SCREEN->InitTouchScreen();
    ASSERT_NE(nullptr, TOUCH_SCREEN->tsSetFeatureConfig_);

    int32_t feature = 12;
    const char* config = "0";
    bool isFlodScreen = system::GetParameter("const.window.foldscreen.type", "") != "";
    if (isFlodScreen) {
        ASSERT_EQ(TOUCH_SCREEN->tsSetFeatureConfig_(feature, config), 0);
    } else {
        ASSERT_LT(TOUCH_SCREEN->tsSetFeatureConfig_(feature, config), 0);
    }
}

/*
 * @tc.name: SetFeatureConfig_002
 * @tc.desc: Test SetFeatureConfig
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(TouchScreenTest, SetFeatureConfig_002, TestSize.Level1)
{
    TOUCH_SCREEN->InitTouchScreen();
    ASSERT_NE(nullptr, TOUCH_SCREEN->tsSetFeatureConfig_);

    int32_t feature = 12;
    const char* config = "1";
    bool isFlodScreen = system::GetParameter("const.window.foldscreen.type", "") != "";
    if (isFlodScreen) {
        ASSERT_EQ(TOUCH_SCREEN->tsSetFeatureConfig_(feature, config), 0);
    } else {
        ASSERT_LT(TOUCH_SCREEN->tsSetFeatureConfig_(feature, config), 0);
    }
}

/*
 * @tc.name: SetFeatureConfig_003
 * @tc.desc: Test SetFeatureConfig
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(TouchScreenTest, SetFeatureConfig_003, TestSize.Level1)
{
    TOUCH_SCREEN->InitTouchScreen();
    ASSERT_NE(nullptr, TOUCH_SCREEN->tsSetFeatureConfig_);

    int32_t feature = 12;
    const char* config = "-1";
    ASSERT_LT(TOUCH_SCREEN->tsSetFeatureConfig_(feature, config), 0);
}

}