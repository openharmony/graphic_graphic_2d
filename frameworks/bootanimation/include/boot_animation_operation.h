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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_ANIMATION_OPERATION_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_ANIMATION_OPERATION_H

#include <condition_variable>
#include <mutex>
#include <thread>

#include "boot_animation_config.h"
#include "boot_compile_progress.h"
#include "boot_player.h"
#include "event_handler.h"
#ifdef NEW_RENDER_CONTEXT
#include "render_context_factory.h"
#include "rs_surface_factory.h"
#endif
#include <render_context/render_context.h>
#include <ui/rs_display_node.h>
#include <ui/rs_surface_extractor.h>
#include "util.h"

namespace OHOS {
class BootAnimationOperation {
public:
    BootAnimationOperation() = default;

    virtual ~BootAnimationOperation();

    void Init(const BootAnimationConfig& config, int32_t width, int32_t height, int32_t duration);

    void SetSoundEnable(bool isEnabled);

    std::thread& GetThread();

private:
    void StartEventHandler(const BootAnimationConfig& config);
    bool IsBootVideoEnabled(const BootAnimationConfig& config);
    bool InitRsDisplayNode();
    bool InitRsSurfaceNode(int32_t degree);
    bool InitRsSurface();
    void PlayVideo(const std::string& path);
    void PlayPicture(const std::string& path);
    void PlaySound(const std::string& path);
    void StopBootAnimation();

private:
    bool isSoundEnabled_ = true;
    int32_t windowWidth_;
    int32_t windowHeight_;
    int32_t duration_;
    Rosen::ScreenId currentScreenId_;
    OHOS::BootAnimationCallback callback_;

    std::mutex eventMutex_;
    std::thread eventThread_;
    std::condition_variable eventCon_;

#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<OHOS::Rosen::RSRenderSurface> rsSurface_;
#endif
    std::shared_ptr<OHOS::Rosen::RSSurface> rsSurface_;
    std::shared_ptr<OHOS::Rosen::RSDisplayNode> rsDisplayNode_;
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> rsSurfaceNode_;

    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> mainHandler_;

    std::shared_ptr<BootPlayer> picPlayer_;
    std::shared_ptr<BootPlayer> soundPlayer_;
    std::shared_ptr<BootPlayer> videoPlayer_;
};
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_ANIMATION_OPERATION_H
