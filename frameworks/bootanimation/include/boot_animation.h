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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_ANIMATION_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_ANIMATION_H

#include <include/core/SkCanvas.h>
#include <include/core/SkData.h>
#include <include/core/SkImageInfo.h>
#include <include/core/SkImage.h>
#include <include/codec/SkCodec.h>
#ifdef PLAYER_FRAMEWORK_ENABLE
#include <media_errors.h>
#endif
#include <ipc_skeleton.h>
#include <iremote_broker.h>
#include <iservice_registry.h>
#include <platform/ohos/rs_irender_service.h>
#include <parameters.h>
#ifdef NEW_RENDER_CONTEXT
#include "render_context_base.h"
#include "rs_render_surface.h"
#else
#include <render_context/render_context.h>
#endif
#include <system_ability_definition.h>
#include <ui/rs_display_node.h>
#include <ui/rs_surface_extractor.h>

#include "boot_animationconfig.h"
#ifdef PLAYER_FRAMEWORK_ENABLE
#include "boot_videoplayer.h"
#endif
#include "event_handler.h"
#ifdef PLAYER_FRAMEWORK_ENABLE
#include "player.h"
#endif
#include "vsync_receiver.h"
#include "util.h"

namespace OHOS {
class BootAnimation {
public:
    void Init(Rosen::ScreenId defaultId, int32_t width, int32_t height);
    void Draw();
    bool CheckExitAnimation();
#ifdef PLAYER_FRAMEWORK_ENABLE
    void PlaySound();
    void PlayVideo();
    void CloseVideoPlayer();
#endif
    void Run(Rosen::ScreenId id, int screenWidth, int screenHeight);
    ~BootAnimation();
private:
    void OnVsync();
    void OnDraw(SkCanvas* canvas, int32_t curNo);
    void InitRsSurface();
    void InitRsSurfaceNode();
    void InitRsDisplayNode();
    void InitPicCoordinates();
    int32_t windowWidth_;
    int32_t windowHeight_;
    Rosen::ScreenId defaultId_;

#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<OHOS::Rosen::RenderContextBase> renderContext_;
    std::shared_ptr<OHOS::Rosen::RSRenderSurface> rsSurface_;
#else
    std::unique_ptr<OHOS::Rosen::RSSurfaceFrame> framePtr_;
    std::shared_ptr<OHOS::Rosen::RSSurface> rsSurface_;
    OHOS::Rosen::RenderContext* rc_;
#endif
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> rsSurfaceNode_;
    std::shared_ptr<OHOS::Rosen::RSDisplayNode> rsDisplayNode_;
    int32_t freq_ = 30;
    int32_t realHeight_ = 0;
    int32_t realWidth_ = 0;
    int32_t pointX_ = 0;
    int32_t pointY_ = 0;
    int32_t picCurNo_ = -1;
    int32_t imgVecSize_ = 0;
    std::shared_ptr<OHOS::Rosen::VSyncReceiver> receiver_;
#ifdef PLAYER_FRAMEWORK_ENABLE
    std::shared_ptr<Media::Player> soundPlayer_;
    std::shared_ptr<BootVideoPlayer> bootVideoPlayer_;
    OHOS::FrameCallback fcb_;
#endif
    ImageStructVec imageVector_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> mainHandler_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    bool isAnimationEnd_ = false;
    BootAnimationConfig animationConfig_;
};
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_ANIMATION_H
