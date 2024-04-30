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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_VIDEO_PLAYER_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_VIDEO_PLAYER_H

#include "boot_player.h"

namespace OHOS {
class BootVideoPlayer : public BootPlayer, public std::enable_shared_from_this<BootVideoPlayer> {
public:
    BootVideoPlayer(const PlayerParams& params);

    virtual ~BootVideoPlayer() = default;

    void Play() override;

private:
    void SetCallback(const BootAnimationCallback* cb);
    bool SetVideoSound();
    std::shared_ptr<Media::Player> GetMediaPlayer() const;
    void StopVideo();

#ifdef PLAYER_FRAMEWORK_ENABLE
    OHOS::sptr<OHOS::Surface> surface_;
#endif

    VSyncCallback vSyncCallback_;
    void *userData_;
    std::mutex mtx_;

#ifdef PLAYER_FRAMEWORK_ENABLE
    friend class VideoPlayerCallback;
#endif
};

#ifdef PLAYER_FRAMEWORK_ENABLE
class VideoPlayerCallback : public Media::PlayerCallback, public NoCopyable {
public:
    VideoPlayerCallback(std::shared_ptr<BootVideoPlayer> boot)
    {
        boot_ = boot;
    };
    virtual ~VideoPlayerCallback() = default;

    void OnError(int32_t errorCode, const std::string &errorMsg) override;
    void OnInfo(Media::PlayerOnInfoType type, int32_t extra, const Media::Format &infoBody) override;
private:
    std::shared_ptr<BootVideoPlayer> boot_;
};
#endif
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_VIDEO_PLAYER_H
