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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_PICTURE_PLAYER_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_PICTURE_PLAYER_H

#include "boot_player.h"
#include "util.h"
#include <ui/rs_surface_extractor.h>
#include "vsync_receiver.h"

namespace OHOS {
class BootPicturePlayer : public BootPlayer {
public:
    BootPicturePlayer(const PlayerParams& params);

    virtual ~BootPicturePlayer() = default;

    void Play() override;

private:
    void OnVsync();
    bool Draw();
    bool OnDraw(Rosen::Drawing::CoreCanvas* canvas, int32_t curNo);
    void InitPicCoordinates(const Rosen::ScreenId screenId);
    bool ReadPicZipFile(ImageStructVec& imgVec, int32_t& freq);
    bool CheckFrameRateValid(int32_t frameRate);
    std::string GetPicZipPath();

private:
    int32_t windowWidth_;
    int32_t windowHeight_;
    int32_t picCurNo_ = 0;
    int32_t realHeight_ = 0;
    int32_t realWidth_ = 0;
    int32_t pointX_ = 0;
    int32_t pointY_ = 0;
    int32_t imgVecSize_ = 0;
    int32_t freq_ = 30;
    ImageStructVec imageVector_;

#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<OHOS::Rosen::RSRenderSurface> rsSurface_;
#endif
    std::shared_ptr<OHOS::Rosen::RSSurface> rsSurface_;
    std::unique_ptr<OHOS::Rosen::RSSurfaceFrame> rsSurfaceFrame_;
    std::shared_ptr<OHOS::Rosen::VSyncReceiver> receiver_;
};
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_PICTURE_PLAYER_H
