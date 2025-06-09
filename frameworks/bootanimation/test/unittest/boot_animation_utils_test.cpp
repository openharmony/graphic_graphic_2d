/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "boot_animation_utils.h"
#include "parameters.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BootAnimationUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BootAnimationUtilsTest::SetUpTestCase() {}
void BootAnimationUtilsTest::TearDownTestCase() {}
void BootAnimationUtilsTest::SetUp() {}
void BootAnimationUtilsTest::TearDown() {}

/**
 * @tc.name: SetBootAnimationSoundEnabled
 * @tc.desc: Verify the SetBootAnimationSoundEnabled
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationUtilsTest, SetBootAnimationSoundEnabled, TestSize.Level0)
{
    BootAnimationUtils::SetBootAnimationSoundEnabled(false);
    bool soundEnabled =
        std::atoi((system::GetParameter("persist.graphic.bootsound.enabled", "1")).c_str()) != 0;
    EXPECT_EQ(soundEnabled, false);
}

/**
 * @tc.name: GetBootAnimationSoundEnabled
 * @tc.desc: Verify the GetBootAnimationSoundEnabled
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationUtilsTest, GetBootAnimationSoundEnabled, TestSize.Level0)
{
    bool isEnabled = BootAnimationUtils::GetBootAnimationSoundEnabled();
    bool soundEnabled =
        std::atoi((system::GetParameter("persist.graphic.bootsound.enabled", "1")).c_str()) != 0;
    EXPECT_EQ(soundEnabled, isEnabled);
}
}
