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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_VIDEOPLAYER_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_VIDEOPLAYER_H

#ifdef PLAYER_FRAMEWORK_ENABLE
#include <media_errors.h>
#endif
#include <parameters.h>

#include "event_handler.h"
#ifdef PLAYER_FRAMEWORK_ENABLE
#include "player.h"
#endif

namespace OHOS {
using VSyncCallback = std::function<void(void*)>;
struct FrameCallback {
    void *userData_;
    VSyncCallback callback_;
};
class BootVideoPlayer : public std::enable_shared_from_this<BootVideoPlayer> {
public:
    void SetVideoPath(const std::string& path);
#ifdef PLAYER_FRAMEWORK_ENABLE
    void SetPlayerSurface(const OHOS::sptr<OHOS::Surface>& surface);
#endif
    void SetCallback(const FrameCallback* cb)
    {
        std::lock_guard<std::mutex> locker(mtx_);
        vsyncCallbacks_ = cb->callback_;
        userData_ = cb->userData_;
    }
    bool PlayVideo();
    void StopVideo();
    void SetVideoSound();
private:
#ifdef PLAYER_FRAMEWORK_ENABLE
    std::shared_ptr<Media::Player> mediaPlayer_;
    OHOS::sptr<OHOS::Surface> surface_;
#endif
    std::string videopath_;
    VSyncCallback vsyncCallbacks_;
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

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_VIDEOPLAYER_H
