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
#include "boot_video_player.h"

#include "boot_animation_utils.h"
#include "log.h"
#include <media_errors.h>
#include "transaction/rs_interfaces.h"
#include <parameters.h>

using namespace OHOS;
#ifdef PLAYER_FRAMEWORK_ENABLE
static const int CONTENT_TYPE_UNKNOWN = 0;
static const int STREAM_USAGE_ENFORCED_TONE = 15;
#endif

BootVideoPlayer::BootVideoPlayer(const PlayerParams& params)
{
    screenId_ = params.screenId;
    resPath_ = params.resPath;
#ifdef PLAYER_FRAMEWORK_ENABLE
    surface_ = params.surface;
#endif
    SetCallback(params.callback);
    isSoundEnabled_ = params.soundEnabled;
}

void BootVideoPlayer::Play()
{
#ifdef PLAYER_FRAMEWORK_ENABLE
    LOGI("PlayVideo begin");
    while (mediaPlayer_ == nullptr) {
        LOGI("mediaPlayer is nullptr, try create again");
        mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
        usleep(SLEEP_TIME_US);
    }

    std::shared_ptr<VideoPlayerCallback> cb = std::make_shared<VideoPlayerCallback>(shared_from_this());
    int32_t ret = mediaPlayer_->SetPlayerCallback(cb);
    if (ret != 0) {
        LOGE("PlayVideo SetPlayerCallback fail, errorCode: %{public}d", ret);
        return;
    }
    std::string path = GetResPath(TYPE_VIDEO);
    LOGI("video res path: %{public}s", path.c_str());
    ret = mediaPlayer_->SetSource(path);
    if (ret != 0) {
        LOGE("PlayVideo SetSource fail, errorCode: %{public}d", ret);
        return;
    }
    if (surface_ == nullptr) {
        LOGE("PlayVideo surface is null");
        return;
    }
    ret = mediaPlayer_->SetVideoSurface(surface_);
    if (ret != 0) {
        LOGE("PlayVideo SetVideoSurface fail, errorCode: %{public}d", ret);
        return;
    }

    if (!SetVideoSound()) {
        LOGW("SetVideoSound failed");
    }

    ret = mediaPlayer_->Prepare();
    if (ret !=  0) {
        LOGE("PlayVideo Prepare fail, errorCode: %{public}d", ret);
        return;
    }
#else
    LOGI("player framework is disabled");
#endif
}

bool BootVideoPlayer::SetVideoSound()
{
#ifdef PLAYER_FRAMEWORK_ENABLE
    LOGI("SetVideoSound start");
    if (!isSoundEnabled_) {
        LOGI("sound disabled on screen: " BPUBU64 "", screenId_);
        return true;
    }
    Media::Format format;
    format.PutIntValue(Media::PlayerKeys::CONTENT_TYPE, CONTENT_TYPE_UNKNOWN);
    format.PutIntValue(Media::PlayerKeys::STREAM_USAGE, STREAM_USAGE_ENFORCED_TONE);
    format.PutIntValue(Media::PlayerKeys::RENDERER_FLAG, 0);
    int ret = mediaPlayer_->SetParameter(format);
    if (ret != 0) {
        LOGE("PlayVideo SetParameter fail, errorCode:%{public}d", ret);
        return false;
    }

    bool bootSoundEnabled = BootAnimationUtils::GetBootAnimationSoundEnabled();
    if (!bootSoundEnabled) {
        ret = mediaPlayer_->SetVolume(0, 0);
        if (ret != 0) {
            LOGE("PlayVideo SetVolume fail, errorCode:%{public}d", ret);
            return false;
        }
    }
    return true;
#endif
}

void BootVideoPlayer::SetCallback(const BootAnimationCallback* cb)
{
    std::lock_guard<std::mutex> locker(mtx_);
    vSyncCallback_ = cb->callback;
    userData_ = cb->userData;
}

#ifdef PLAYER_FRAMEWORK_ENABLE
std::shared_ptr<Media::Player> BootVideoPlayer::GetMediaPlayer() const
{
    return mediaPlayer_;
}
#endif

void BootVideoPlayer::StopVideo()
{
    vSyncCallback_(userData_);
}

#ifdef PLAYER_FRAMEWORK_ENABLE
// PlayerCallback override
void VideoPlayerCallback::OnError(int32_t errorCode, const std::string &errorMsg)
{
    LOGE("PlayerCallbackError received, errorMsg:%{public}s", errorMsg.c_str());
    boot_->StopVideo();
}
#endif

#ifdef PLAYER_FRAMEWORK_ENABLE
void VideoPlayerCallback::OnInfo(Media::PlayerOnInfoType type, int32_t extra, const Media::Format &infoBody)
{
    switch (type) {
        case Media::INFO_TYPE_SEEKDONE:
            LOGI("PlayerCallback: OnSeekDone currentPositon is: %{public}d", extra);
            break;
        case Media::INFO_TYPE_SPEEDDONE:
            LOGI("PlayerCallback: SpeedDone");
            break;
        case Media::INFO_TYPE_BITRATEDONE:
            LOGI("PlayerCallback: BitRateDone");
            break;
        case Media::INFO_TYPE_EOS: {
            LOGI("PlayerCallback: OnEndOfStream isLooping is: %{public}d", extra);
            boot_->StopVideo();
            break;
        }
        case Media::INFO_TYPE_BUFFERING_UPDATE:
            LOGI("PlayerCallback: Buffering Update");
            break;
        case Media::INFO_TYPE_BITRATE_COLLECT:
            LOGI("PlayerCallback: Bitrate Collect");
            break;
        case Media::INFO_TYPE_STATE_CHANGE:
            LOGI("PlayerCallback: State Change, current state is: %{public}d", extra);
            if (Media::PlayerStates::PLAYER_PREPARED == extra) {
                LOGI("Begin to play");
                boot_->GetMediaPlayer()->Play();
            }
            break;
        case Media::INFO_TYPE_POSITION_UPDATE: {
            LOGD("PlayerCallback: Position Update");
            break;
        }
        case Media::INFO_TYPE_MESSAGE:
            LOGI("PlayerCallback: OnMessage is: %{public}d", extra);
            if (!system::GetBoolParameter(BOOT_ANIMATION_STARTED, false)) {
                system::SetParameter(BOOT_ANIMATION_STARTED, "true");
            }
            break;
        case Media::INFO_TYPE_RESOLUTION_CHANGE:
            LOGI("PlayerCallback: Resolution Change");
            break;
        case Media::INFO_TYPE_VOLUME_CHANGE:
            LOGI("PlayerCallback: Volume Changed");
            break;
        default:
            LOGI("PlayerCallback: Default");
            break;
    }
}
#endif
