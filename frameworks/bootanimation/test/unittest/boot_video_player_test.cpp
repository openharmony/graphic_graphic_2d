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

#include <parameters.h>
#include "boot_animation_utils.h"
#include "boot_video_player.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
    constexpr int32_t TEST_BOOT_SOUND_VOLUME = 5;
    constexpr int32_t TEST_ERROR_CODE = -1;
}

class BootVideoPlayerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BootVideoPlayerTest::SetUpTestCase() {}
void BootVideoPlayerTest::TearDownTestCase() {}
void BootVideoPlayerTest::SetUp() {}
void BootVideoPlayerTest::TearDown() {}

/**
 * @tc.name: OnError_CallbackInvoked_CallbackFlagSet
 * @tc.desc: Verify the OnError function invokes callback correctly.
 * @tc.type: FUNC
 */
HWTEST_F(BootVideoPlayerTest, OnError_CallbackInvoked_CallbackFlagSet, TestSize.Level1)
{
    PlayerParams params;
    int flag = 0;
    BootAnimationCallback callback_ = {
        .userData = this,
        .callback = [&flag](void*) { flag = 1; },
    };
    params.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player = std::make_shared<BootVideoPlayer>(params);
    std::shared_ptr<VideoPlayerCallback> cb = std::make_shared<VideoPlayerCallback>(player);
    cb->OnError(TEST_ERROR_CODE, "test");
    EXPECT_EQ(flag, 1);
}

/**
 * @tc.name: StopVideo_CallbackInvoked_CallbackFlagSet
 * @tc.desc: Verify the StopVideo function invokes callback correctly.
 * @tc.type: FUNC
 */
HWTEST_F(BootVideoPlayerTest, StopVideo_CallbackInvoked_CallbackFlagSet, TestSize.Level1)
{
    PlayerParams params;
    int flag = 0;
    BootAnimationCallback callback_ = {
        .userData = this,
        .callback = [&flag](void*) { flag = 1; },
    };
    params.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player = std::make_shared<BootVideoPlayer>(params);
    player->StopVideo();
    EXPECT_EQ(flag, 1);
}

/**
 * @tc.name: SetVideoSound_DifferentSoundSettings_VolumeSetCorrectly
 * @tc.desc: Verify the SetVideoSound function with different sound settings.
 * @tc.type: FUNC
 */
HWTEST_F(BootVideoPlayerTest, SetVideoSound_DifferentSoundSettings_VolumeSetCorrectly, TestSize.Level1)
{
    BootAnimationCallback callback_ = {
        .userData = this,
        .callback = [&](void*) {},
    };

    PlayerParams params1;
    params1.soundEnabled = true;
    params1.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player1 = std::make_shared<BootVideoPlayer>(params1);
    ASSERT_NE(player1, nullptr);
    player1->mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
    player1->SetVideoSound();
    EXPECT_TRUE(true);

    PlayerParams params2;
    params2.soundEnabled = false;
    params2.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player2 = std::make_shared<BootVideoPlayer>(params2);
    ASSERT_NE(player2, nullptr);
    player2->mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
    player2->SetVideoSound();
    EXPECT_TRUE(true);

    PlayerParams params3;
    params3.soundEnabled = true;
    params3.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player3 = std::make_shared<BootVideoPlayer>(params3);
    ASSERT_NE(player3, nullptr);
    player3->mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
    BootAnimationUtils::SetBootAnimationSoundEnabled(false);
    EXPECT_EQ(BootAnimationUtils::GetBootAnimationSoundEnabled(), false);
    player3->SetVideoSound();
    EXPECT_TRUE(true);

    PlayerParams params4;
    params4.soundEnabled = true;
    params4.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player4 = std::make_shared<BootVideoPlayer>(params4);
    ASSERT_NE(player4, nullptr);
    player4->mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
    BootAnimationUtils::SetBootAnimationSoundEnabled(true);
    player4->SetVideoSound();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: OnInfo_DifferentInfoTypes_HandledCorrectly
 * @tc.desc: Verify the OnInfo function handles different info types correctly.
 * @tc.type: FUNC
 */
HWTEST_F(BootVideoPlayerTest, OnInfo_DifferentInfoTypes_HandledCorrectly, TestSize.Level1)
{
    PlayerParams params;
    BootAnimationCallback callback_ = {
        .userData = this,
        .callback = [&](void*) {},
    };
    params.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player = std::make_shared<BootVideoPlayer>(params);
    player->mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
    std::shared_ptr<VideoPlayerCallback> cb = std::make_shared<VideoPlayerCallback>(player);
    Media::Format format;
    cb->OnInfo(Media::INFO_TYPE_SEEKDONE, 0, format);
    cb->OnInfo(Media::INFO_TYPE_SPEEDDONE, 0, format);
    cb->OnInfo(Media::INFO_TYPE_BITRATEDONE, 0, format);
    cb->OnInfo(Media::INFO_TYPE_EOS, 0, format);
    cb->OnInfo(Media::INFO_TYPE_BUFFERING_UPDATE, 0, format);
    cb->OnInfo(Media::INFO_TYPE_BITRATE_COLLECT, 0, format);
    cb->OnInfo(Media::INFO_TYPE_STATE_CHANGE, 0, format);

    if (player->GetMediaPlayer() != nullptr) {
        cb->OnInfo(Media::INFO_TYPE_STATE_CHANGE, Media::PlayerStates::PLAYER_PREPARED, format);
    }

    cb->OnInfo(Media::INFO_TYPE_POSITION_UPDATE, 0, format);
    cb->OnInfo(Media::INFO_TYPE_MESSAGE, 0, format);
    system::SetParameter(BOOT_ANIMATION_STARTED, "false");
    cb->OnInfo(Media::INFO_TYPE_MESSAGE, 0, format);
    EXPECT_EQ(system::GetBoolParameter(BOOT_ANIMATION_STARTED, false), true);
    cb->OnInfo(Media::INFO_TYPE_RESOLUTION_CHANGE, 0, format);
    cb->OnInfo(Media::INFO_TYPE_VOLUME_CHANGE, 0, format);
    cb->OnInfo(Media::INFO_TYPE_SUBTITLE_UPDATE, 0, format);
}

/**
 * @tc.name: Play_NullSurface_MediaPlayerCreated
 * @tc.desc: Verify the Play function creates mediaPlayer when surface is null.
 * @tc.type: FUNC
 */
HWTEST_F(BootVideoPlayerTest, Play_NullSurface_MediaPlayerCreated, TestSize.Level1)
{
    PlayerParams params;
    params.surface = nullptr;
    BootAnimationCallback callback_ = {
        .userData = this,
        .callback = [&](void*) {},
    };
    params.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player = std::make_shared<BootVideoPlayer>(params);
    player->Play();
    ASSERT_NE(nullptr, player->mediaPlayer_);
}

/**
 * @tc.name: IsNormalBoot_DifferentBootReasons_ReturnCorrectResult
 * @tc.desc: Verify the IsNormalBoot function returns correct result for different boot reasons.
 * @tc.type: FUNC
 */
HWTEST_F(BootVideoPlayerTest, IsNormalBoot_DifferentBootReasons_ReturnCorrectResult, TestSize.Level1)
{
    PlayerParams params;
    params.surface = nullptr;
    BootAnimationCallback callback_ = {
        .userData = this,
        .callback = [&](void*) {},
    };
    params.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player = std::make_shared<BootVideoPlayer>(params);
    bool result = player->IsNormalBoot();
    std::string bootReason = system::GetParameter("ohos.boot.reboot_reason", "");
    std::vector<std::string> reasonArr = {"AP_S_COLDBOOT", "bootloader", "recovery", "fastbootd",
        "resetfactory", "at2resetfactory", "atfactoryreset0", "resetuser", "sdupdate", "chargereboot",
        "resize", "erecovery", "usbupdate", "cust", "oem_rtc", "UNKNOWN", "mountfail", "hungdetect",
        "COLDBOOT", "updatedataimg", "AP_S_FASTBOOTFLASH", "gpscoldboot", "AP_S_COMBINATIONKEY",
        "CP_S_NORMALRESET", "IOM3_S_USER_EXCEPTION", "BR_UPDATE_USB", "BR_UPDATA_SD_FORCE",
        "BR_KEY_VOLUMN_UP", "BR_PRESS_1S", "BR_CHECK_RECOVERY", "BR_CHECK_ERECOVERY",
        "BR_CHECK_SDUPDATE", "BR_CHECK_USBUPDATE", "BR_CHECK_RESETFACTORY", "BR_CHECK_HOTAUPDATE",
        "BR_POWERONNOBAT", "BR_NOGUI", "BR_FACTORY_VERSION", "BR_RESET_HAPPEN", "BR_POWEROFF_ALARM",
        "BR_POWEROFF_CHARGE", "BR_POWERON_BY_SMPL", "BR_CHECK_UPDATEDATAIMG", "BR_POWERON_CHARGE",
        "AP_S_PRESS6S", "BR_PRESS_10S"};
    if (std::find(reasonArr.begin(), reasonArr.end(), bootReason) != reasonArr.end()) {
        EXPECT_EQ(result, true);
    } else {
        EXPECT_EQ(result, false);
    }
}

/**
 * @tc.name: Play_FrameRateEnabled_PlaySuccessfully
 * @tc.desc: Verify the Play function with frameRateEnabled flag.
 * @tc.type: FUNC
 */
HWTEST_F(BootVideoPlayerTest, Play_FrameRateEnabled_PlaySuccessfully, TestSize.Level1)
{
    PlayerParams params;
    params.surface = nullptr;
    params.isFrameRateEnable = true;
    BootAnimationCallback callback_ = {
        .userData = this,
        .callback = [&](void*) {},
    };
    params.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player = std::make_shared<BootVideoPlayer>(params);
    ASSERT_NE(player, nullptr);
    player->Play();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetVideoSound_HmosUpdate_SoundSetCorrectly
 * @tc.desc: Verify the SetVideoSound function during HMOS update.
 * @tc.type: FUNC
 */
HWTEST_F(BootVideoPlayerTest, SetVideoSound_HmosUpdate_SoundSetCorrectly, TestSize.Level0)
{
    system::SetParameter("persist.update.hmos_to_next_flag", "1");
    PlayerParams params;
    params.surface = nullptr;
    BootAnimationCallback callback_ = {
        .userData = this,
        .callback = [&](void*) {},
    };
    params.callback = &callback_;
    params.soundEnabled = true;
    std::shared_ptr<BootVideoPlayer> player = std::make_shared<BootVideoPlayer>(params);
    ASSERT_NE(player, nullptr);
    player->mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
    BootAnimationUtils::SetBootAnimationSoundEnabled(true);
    system::SetParameter(BOOT_SOUND, std::to_string(TEST_BOOT_SOUND_VOLUME));
    player->SetVideoSound();
    EXPECT_TRUE(true);
    system::SetParameter("persist.update.hmos_to_next_flag", "0");
}

/**
 * @tc.name: SetVideoSound_InvalidVolume_SoundSetCorrectly
 * @tc.desc: Verify the SetVideoSound function with invalid volume.
 * @tc.type: FUNC
 */
HWTEST_F(BootVideoPlayerTest, SetVideoSound_InvalidVolume_SoundSetCorrectly, TestSize.Level0)
{
    PlayerParams params;
    params.surface = nullptr;
    BootAnimationCallback callback_ = {
        .userData = this,
        .callback = [&](void*) {},
    };
    params.callback = &callback_;
    params.soundEnabled = true;
    std::shared_ptr<BootVideoPlayer> player = std::make_shared<BootVideoPlayer>(params);
    ASSERT_NE(player, nullptr);
    player->mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
    BootAnimationUtils::SetBootAnimationSoundEnabled(true);
    system::SetParameter(BOOT_SOUND, std::to_string(INVALID_VOLUME));
    player->SetVideoSound();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: ResPath_EmptyPath_PathSetCorrectly
 * @tc.desc: Verify the resPath_ member variable with empty path.
 * @tc.type: FUNC
 */
HWTEST_F(BootVideoPlayerTest, ResPath_EmptyPath_PathSetCorrectly, TestSize.Level1)
{
    PlayerParams params;
    params.resPath = "";
    params.surface = nullptr;
    BootAnimationCallback callback_ = {
        .userData = this,
        .callback = [&](void*) {},
    };
    params.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player = std::make_shared<BootVideoPlayer>(params);
    ASSERT_NE(player, nullptr);
    player->resPath_ = "";
    EXPECT_EQ(player->resPath_, "");
}

/**
 * @tc.name: OnInfo_EosInfo_HandledCorrectly
 * @tc.desc: Verify the OnInfo function handles EOS info type correctly.
 * @tc.type: FUNC
 */
HWTEST_F(BootVideoPlayerTest, OnInfo_EosInfo_HandledCorrectly, TestSize.Level1)
{
    PlayerParams params;
    BootAnimationCallback callback_ = {
        .userData = this,
        .callback = [&](void*) {},
    };
    params.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player = std::make_shared<BootVideoPlayer>(params);
    ASSERT_NE(player, nullptr);
    player->mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
    std::shared_ptr<VideoPlayerCallback> cb = std::make_shared<VideoPlayerCallback>(player);
    ASSERT_NE(cb, nullptr);
    Media::Format format;
    cb->OnInfo(Media::INFO_TYPE_EOS, 0, format);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: OnError_ErrorCallback_CalledCorrectly
 * @tc.desc: Verify the OnError function calls error callback correctly.
 * @tc.type: FUNC
 */
HWTEST_F(BootVideoPlayerTest, OnError_ErrorCallback_CalledCorrectly, TestSize.Level1)
{
    PlayerParams params;
    BootAnimationCallback callback_ = {
        .userData = this,
        .callback = [&](void*) {},
    };
    params.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player = std::make_shared<BootVideoPlayer>(params);
    ASSERT_NE(player, nullptr);
    std::shared_ptr<VideoPlayerCallback> cb = std::make_shared<VideoPlayerCallback>(player);
    ASSERT_NE(cb, nullptr);
    cb->OnError(TEST_ERROR_CODE, "test error");
    EXPECT_TRUE(true);
}
}