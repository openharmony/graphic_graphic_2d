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
#include "boot_videoplayer.h"

#include "boot_animation.h"
#include "boot_animation_utils.h"
#include "log.h"
#include "util.h"

using namespace OHOS;
#ifdef PLAYER_FRAMEWORK_ENABLE
static const int CONTENT_TYPE_UNKNOWN = 0;
static const int STREAM_USAGE_RINGTONE = 6;
#endif

void BootVideoPlayer::SetVideoPath(const std::string& path)
{
    if (path.empty()) {
        LOGE("set video path is empty");
        return;
    }
    videopath_ = path;
}

#ifdef PLAYER_FRAMEWORK_ENABLE
void BootVideoPlayer::SetPlayerSurface(const sptr<Surface>& surface)
{
    if (surface == nullptr) {
        LOGE("SetPlayerSurface surface is nullptr");
        return;
    }
    surface_ = surface;
}
#endif

bool BootVideoPlayer::PlayVideo()
{
#ifdef PLAYER_FRAMEWORK_ENABLE
    LOGI("PlayVideo begin");
    if (mediaPlayer_ == nullptr) {
        mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
    }
    while (mediaPlayer_ == nullptr) {
        LOGE("CreatePlayer fail, mediaPlayer_ is nullptr");
        mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
        usleep(SLEEP_TIME_US);
    }

    std::shared_ptr<VideoPlayerCallback> cb = std::make_shared<VideoPlayerCallback>(shared_from_this());
    int32_t ret = mediaPlayer_->SetPlayerCallback(cb);
    if (ret != 0) {
        LOGE("PlayVideo SetPlayerCallback fail, errorCode:%{public}d", ret);
        return false;
    }

    std::string uri = "file:/" + videopath_;
    ret = mediaPlayer_->SetSource(uri);
    if (ret != 0) {
        LOGE("PlayVideo SetSource fail, uri:%{public}s, errorCode:%{public}d", uri.c_str(), ret);
        return false;
    }
    if (surface_ == nullptr) {
        LOGE("PlayVideo surface is null");
        return false;
    }
    ret = mediaPlayer_->SetVideoSurface(surface_);
    if (ret != 0) {
        LOGE("PlayVideo SetVideoSurface fail, errorCode:%{public}d", ret);
        return false;
    }

    SetVideoSound();
    
    ret = mediaPlayer_->Prepare();
    if (ret !=  0) {
        LOGE("PlayVideo Prepare fail, errorCode:%{public}d", ret);
        return false;
    }
    mediaPlayer_->Play();
    LOGI("PlayVideo end");
    return true;
#else
    LOGI("player_framework part is not enabled.");
    return false;
#endif
}

void BootVideoPlayer::StopVideo()
{
    LOGI("BootVideoPlayer StopVideo");
    vsyncCallbacks_(userData_);
}

void BootVideoPlayer::SetVideoSound()
{
#ifdef PLAYER_FRAMEWORK_ENABLE
    LOGI("BootVideoPlayer SetVideoSound");
    Media::Format format;
    format.PutIntValue(Media::PlayerKeys::CONTENT_TYPE, CONTENT_TYPE_UNKNOWN);
    format.PutIntValue(Media::PlayerKeys::STREAM_USAGE, STREAM_USAGE_RINGTONE);
    format.PutIntValue(Media::PlayerKeys::RENDERER_FLAG, 0);
    int ret = mediaPlayer_->SetParameter(format);
    if (ret !=  0) {
        LOGE("PlayVideo SetParameter fail, errorCode:%{public}d", ret);
    }

    bool bootSoundEnabled = BootAnimationUtils::GetBootAnimationSoundEnabled();
    if (!bootSoundEnabled) {
        ret = mediaPlayer_->SetVolume(0, 0);
        if (ret !=  0) {
            LOGE("PlayVideo SetVolume fail, errorCode:%{public}d", ret);
        }
    }
#endif
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
            LOGI("PlayerCallback: OnSeekDone currentPositon is:%{public}d", extra);
            break;
        case Media::INFO_TYPE_SPEEDDONE:
            LOGI("PlayerCallback: SpeedDone");
            break;
        case Media::INFO_TYPE_BITRATEDONE:
            LOGI("PlayerCallback: BitRateDone");
            break;
        case Media::INFO_TYPE_EOS: {
            LOGI("PlayerCallback: OnEndOfStream isLooping is:%{public}d", extra);
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
            LOGI("PlayerCallback: State Change");
            break;
        case Media::INFO_TYPE_POSITION_UPDATE: {
            LOGD("PlayerCallback: Position Update");
            break;
        }
        case Media::INFO_TYPE_MESSAGE:
            LOGI("PlayerCallback: OnMessage is:%{public}d", extra);
            system::SetParameter("bootevent.bootanimation.started", "true");
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
