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
 * @tc.name: BootVideoPlayerTest_001
 * @tc.desc: Verify the OnError
 * @tc.type:FUNC
 */
HWTEST_F(BootVideoPlayerTest, BootVideoPlayerTest_001, TestSize.Level1)
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
    cb->OnError(-1, "test");
    EXPECT_EQ(flag, 1);
}

/**
 * @tc.name: BootVideoPlayerTest_002
 * @tc.desc: Verify the StopVideo
 * @tc.type:FUNC
 */
HWTEST_F(BootVideoPlayerTest, BootVideoPlayerTest_002, TestSize.Level1)
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
 * @tc.name: BootVideoPlayerTest_003
 * @tc.desc: Verify the SetVideoSound
 * @tc.type:FUNC
 */
HWTEST_F(BootVideoPlayerTest, BootVideoPlayerTest_003, TestSize.Level1)
{
    BootAnimationCallback callback_ = {
        .userData = this,
        .callback = [&](void*) {},
    };

    PlayerParams params1;
    params1.soundEnabled = true;
    params1.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player1 = std::make_shared<BootVideoPlayer>(params1);
    player1->mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
    player1->SetVideoSound();

    PlayerParams params2;
    params2.soundEnabled = false;
    params2.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player2 = std::make_shared<BootVideoPlayer>(params2);
    player2->mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
    player2->SetVideoSound();

    PlayerParams params3;
    params3.soundEnabled = true;
    params3.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player3 = std::make_shared<BootVideoPlayer>(params3);
    player3->mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
    BootAnimationUtils::SetBootAnimationSoundEnabled(false);
    EXPECT_EQ(BootAnimationUtils::GetBootAnimationSoundEnabled(), false);
    player3->SetVideoSound();

    PlayerParams params4;
    params4.soundEnabled = true;
    params4.callback = &callback_;
    std::shared_ptr<BootVideoPlayer> player4 = std::make_shared<BootVideoPlayer>(params4);
    player4->mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
    BootAnimationUtils::SetBootAnimationSoundEnabled(true);
    player4->SetVideoSound();
}

/**
 * @tc.name: BootVideoPlayerTest_004
 * @tc.desc: Verify the OnInfo
 * @tc.type:FUNC
 */
HWTEST_F(BootVideoPlayerTest, BootVideoPlayerTest_004, TestSize.Level1)
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
 * @tc.name: BootVideoPlayerTest_005
 * @tc.desc: Verify the Play
 * @tc.type:FUNC
 */
HWTEST_F(BootVideoPlayerTest, BootVideoPlayerTest_005, TestSize.Level1)
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
/**/
}
