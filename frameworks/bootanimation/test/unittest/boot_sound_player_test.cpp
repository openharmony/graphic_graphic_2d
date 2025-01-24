/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "boot_sound_player.h"
#include "parameters.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BootSoundPlayerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BootSoundPlayerTest::SetUpTestCase() {}
void BootSoundPlayerTest::TearDownTestCase() {}
void BootSoundPlayerTest::SetUp() {}
void BootSoundPlayerTest::TearDown() {}

/**
 * @tc.name: BootSoundPlayerTest_001
 * @tc.desc: Verify the Play
 * @tc.type:FUNC
 */
HWTEST_F(BootSoundPlayerTest, BootSoundPlayerTest_001, TestSize.Level1)
{
    PlayerParams params1;
    params1.soundEnabled = false;
    std::shared_ptr<BootSoundPlayer> player1 = std::make_shared<BootSoundPlayer>(params1);
    player1->Play();

    PlayerParams params2;
    params2.soundEnabled = true;
    std::shared_ptr<BootSoundPlayer> player2 = std::make_shared<BootSoundPlayer>(params2);
    player2->Play();

    PlayerParams params3;
    params3.soundEnabled = true;
    std::shared_ptr<BootSoundPlayer> player3 = std::make_shared<BootSoundPlayer>(params3);
    BootAnimationUtils::SetBootAnimationSoundEnabled(false);
    EXPECT_EQ(BootAnimationUtils::GetBootAnimationSoundEnabled(), false);
    player3->Play();
}

/**
 * @tc.name: BootSoundPlayerTest_002
 * @tc.desc: Verify the Play
 * @tc.type:FUNC
 */
HWTEST_F(BootSoundPlayerTest, BootSoundPlayerTest_002, TestSize.Level1)
{
    PlayerParams params;
    params.soundEnabled = true;
    std::shared_ptr<BootSoundPlayer> player = std::make_shared<BootSoundPlayer>(params);
    BootAnimationUtils::SetBootAnimationSoundEnabled(true);
    system::SetParameter(BOOT_SOUND, std::to_string(MIN_VOLUME));
    EXPECT_EQ(BootAnimationUtils::GetBootAnimationSoundEnabled(), true);
    player->Play();
}

/**
 * @tc.name: BootSoundPlayerTest_003
 * @tc.desc: Verify the Play
 * @tc.type:FUNC
 */
HWTEST_F(BootSoundPlayerTest, BootSoundPlayerTest_003, TestSize.Level1)
{
    PlayerParams params;
    params.soundEnabled = true;
    std::shared_ptr<BootSoundPlayer> player = std::make_shared<BootSoundPlayer>(params);
    BootAnimationUtils::SetBootAnimationSoundEnabled(true);
    system::SetParameter(BOOT_SOUND, std::to_string(MIN_VOLUME + 1));
    EXPECT_EQ(BootAnimationUtils::GetBootAnimationSoundEnabled(), true);
    player->Play();
}
}
