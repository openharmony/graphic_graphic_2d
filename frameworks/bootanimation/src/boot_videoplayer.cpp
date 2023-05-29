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
#include "boot_animation.h"
#include "boot_videoplayer.h"
#include "log.h"
#include "util.h"

using namespace OHOS;

void BootVideoPlayer::SetVideoPath(const std::string& path)
{
    if (path.empty()) {
        LOGE("set video path is empty");
        return;
    }
    videopath_ = path;
}

void BootVideoPlayer::SetPlayerWindow(const OHOS::sptr<OHOS::Rosen::Window>& window)
{
    if (window == nullptr) {
        LOGE("SetPlayerWindow window is nullptr");
        return;
    }
    window_ = window;
}

bool BootVideoPlayer::PlayVideo()
{
    LOGI("PlayVideo begin");
    if (mediaPlayer_ == nullptr) {
        mediaPlayer_ = Media::PlayerFactory::CreatePlayer();
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
    auto surface = window_->GetSurfaceNode()->GetSurface();
    if (surface == nullptr) {
        LOGE("PlayVideo surface is null");
        return false;
    }
    ret = mediaPlayer_->SetVideoSurface(surface);
    if (ret != 0) {
        LOGE("PlayVideo SetVideoSurface fail, errorCode:%{public}d", ret);
        return false;
    }
    ret = mediaPlayer_->Prepare();
    if (ret !=  0) {
        LOGE("PlayVideo Prepare fail, errorCode:%{public}d", ret);
        return false;
    }
    mediaPlayer_->Play();
    LOGI("PlayVideo end");
    return true;
}

void BootVideoPlayer::StopVideo()
{
    mediaPlayer_->Stop();
    vsyncCallbacks_(userData_);
}

// PlayerCallback override
void VideoPlayerCallback::OnError(int32_t errorCode, const std::string &errorMsg)
{
    std::string err = Media::MSErrorToString(static_cast<Media::MediaServiceErrCode>(errorCode));
    LOGE("PlayerCallbackError received, errorCode:%{public}s", err.c_str());
    boot_->StopVideo();
}

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
            LOGI("PlayerCallback: Position Update");
            break;
        }
        case Media::INFO_TYPE_MESSAGE:
            LOGI("PlayerCallback: OnMessage is:%{public}d", extra);
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
