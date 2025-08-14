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
#include <util.h>

using namespace OHOS;

namespace {
    const std::vector<std::string> NORMAL_REBOOT_REASON_ARR = {"AP_S_COLDBOOT", "bootloader", "recovery", "fastbootd",
        "resetfactory", "at2resetfactory", "atfactoryreset0", "resetuser", "sdupdate", "chargereboot", "resize",
        "erecovery", "usbupdate", "cust", "oem_rtc", "UNKNOWN", "mountfail", "hungdetect", "COLDBOOT", "updatedataimg",
        "AP_S_FASTBOOTFLASH", "gpscoldboot", "AP_S_COMBINATIONKEY", "CP_S_NORMALRESET", "IOM3_S_USER_EXCEPTION",
        "BR_UPDATE_USB", "BR_UPDATA_SD_FORCE", "BR_KEY_VOLUMN_UP", "BR_PRESS_1S", "BR_CHECK_RECOVERY",
        "BR_CHECK_ERECOVERY", "BR_CHECK_SDUPDATE", "BR_CHECK_USBUPDATE", "BR_CHECK_RESETFACTORY",
        "BR_CHECK_HOTAUPDATE", "BR_POWERONNOBAT", "BR_NOGUI", "BR_FACTORY_VERSION", "BR_RESET_HAPPEN",
        "BR_POWEROFF_ALARM", "BR_POWEROFF_CHARGE", "BR_POWERON_BY_SMPL", "BR_CHECK_UPDATEDATAIMG",
        "BR_POWERON_CHARGE", "AP_S_PRESS6S", "BR_PRESS_10S"};
}

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
    CheckAndCreateMedia();

    if (mediaPlayer_ == nullptr) {
        LOGI("mediaPlayer create fail");
        return;
    }

    std::shared_ptr<VideoPlayerCallback> cb = std::make_shared<VideoPlayerCallback>(shared_from_this());
    int32_t ret = mediaPlayer_->SetPlayerCallback(cb);
    if (ret != 0) {
        LOGE("PlayVideo SetPlayerCallback fail, errorCode: %{public}d", ret);
        return;
    }
    std::string path = GetResPath(TYPE_VIDEO);
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
    LOGI("PlayVideo end");
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
        mediaPlayer_->SetVolume(0, 0);
        return true;
    }

    int ret = mediaPlayer_->SetParameter(buildMediaFormat());
    if (ret != 0) {
        LOGE("PlayVideo SetParameter fail, errorCode:%{public}d", ret);
        return false;
    }

    bool bootSoundEnabled = BootAnimationUtils::GetBootAnimationSoundEnabled();
    if (!bootSoundEnabled || !IsNormalBoot()) {
        if (!SetCustomizedVolume(0)) {
            return false;
        }
    } else {
        int customizedVolume = system::GetIntParameter(BOOT_SOUND, INVALID_VOLUME, MIN_VOLUME, MAX_VOLUME);
        if (customizedVolume != INVALID_VOLUME && !SetCustomizedVolume(customizedVolume)) {
            return false;
        }
    }
    return true;
#endif
}

void BootVideoPlayer::SetCallback(const BootAnimationCallback* cb)
{
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

bool BootVideoPlayer::IsNormalBoot()
{
    std::string bootReason = system::GetParameter("ohos.boot.reboot_reason", "");
    LOGI("bootReason: %{public}s", bootReason.c_str());
    if (std::find(NORMAL_REBOOT_REASON_ARR.begin(), NORMAL_REBOOT_REASON_ARR.end(), bootReason)
        != NORMAL_REBOOT_REASON_ARR.end()) {
        LOGI("normal boot");
        return true;
    }
    return false;
}

#ifdef PLAYER_FRAMEWORK_ENABLE
// PlayerCallback override
void VideoPlayerCallback::OnError(int32_t errorCode, const std::string &errorMsg)
{
    LOGE("PlayerCallbackError received, errorMsg:%{public}s", errorMsg.c_str());
    auto boot = boot_.lock();
    if (boot) {
        boot->StopVideo();
    }
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
        case Media::INFO_TYPE_BUFFERING_UPDATE:
            LOGI("PlayerCallback: Buffering Update");
            break;
        case Media::INFO_TYPE_BITRATE_COLLECT:
            LOGI("PlayerCallback: Bitrate Collect");
            break;
        case Media::INFO_TYPE_POSITION_UPDATE:
            LOGD("PlayerCallback: Position Update");
            break;
        case Media::INFO_TYPE_RESOLUTION_CHANGE:
            LOGI("PlayerCallback: Resolution Change");
            break;
        case Media::INFO_TYPE_VOLUME_CHANGE:
            LOGI("PlayerCallback: Volume Changed");
            break;
        default:
            OnOperateInfo(type, extra);
            break;
    }
}
#endif

#ifdef PLAYER_FRAMEWORK_ENABLE
void VideoPlayerCallback::OnOperateInfo(Media::PlayerOnInfoType type, int32_t extra)
{
    auto boot = boot_.lock();
    if (!boot) {
        LOGI("PlayerCallback: boot error");
        return;
    }
    switch (type) {
        case Media::INFO_TYPE_EOS: {
            LOGI("PlayerCallback: OnEndOfStream isLooping is: %{public}d", extra);
            boot->StopVideo();
            break;
        }
        case Media::INFO_TYPE_STATE_CHANGE:
            LOGI("PlayerCallback: State Change, current state is: %{public}d", extra);
            if (Media::PlayerStates::PLAYER_PREPARED == extra) {
                LOGI("Begin to play");
                boot->GetMediaPlayer()->Play();
            }
            break;
        case Media::INFO_TYPE_MESSAGE:
            LOGI("PlayerCallback: OnMessage is: %{public}d", extra);
            if (!system::GetBoolParameter(BOOT_ANIMATION_STARTED, false)) {
                system::SetParameter(BOOT_ANIMATION_STARTED, "true");
            }
            break;
        default:
            LOGI("PlayerCallback: Default");
            break;
    }
}
#endif