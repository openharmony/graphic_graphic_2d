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
#include "boot_sound_player.h"

#include "boot_animation_utils.h"
#include "log.h"
#include <media_errors.h>

using namespace OHOS;
BootSoundPlayer::BootSoundPlayer(const PlayerParams& params)
{
    resPath_ = params.resPath;
    screenId_ = params.screenId;
    isSoundEnabled_ = params.soundEnabled;
}

#ifdef PLAYER_FRAMEWORK_ENABLE
void BootSoundPlayer::Play()
{
    LOGI("PlaySound start");
    if (!isSoundEnabled_) {
        LOGI("sound disabled on screen: " BPUBU64 "", screenId_);
        return;
    }
    bool bootSoundEnabled = BootAnimationUtils::GetBootAnimationSoundEnabled();
    if (!bootSoundEnabled) {
        LOGI("boot animation sound disabled");
        return;
    }

    while (mediaPlayer_ == nullptr) {
        LOGI("mediaPlayer is nullptr, try create again");
        mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
        usleep(SLEEP_TIME_US);
    }
    std::string path = GetResPath(TYPE_SOUND);
    LOGI("sound res path: %{public}s", path.c_str());
    mediaPlayer_->SetSource(path);
    mediaPlayer_->SetLooping(false);
    mediaPlayer_->PrepareAsync();
    mediaPlayer_->Play();
}
#endif
