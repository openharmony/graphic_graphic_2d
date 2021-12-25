/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_ANIMATION_MODULE_H
#define FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_ANIMATION_MODULE_H

#include "rotation_animation.h"

#include <egl_surface.h>
#include <vsync_helper.h>
#include <window_manager.h>

#include "array.h"

namespace OHOS {
using PromiseGSError = Promise<GSError>;

struct Animation {
    int32_t degree;
    sptr<PromiseGSError> retval;
};

struct AnimationScreenshotInfo {
    struct WMImageInfo wmimage;
    std::shared_ptr<struct Array> ptr;
};

using PromiseAnimationScreenshotInfo = Promise<struct AnimationScreenshotInfo>;

class AnimationModule : IScreenShotCallback {
public:
    GSError Init();
    GSError StartRotationAnimation(int32_t did, int32_t degree);

private:
    void OnScreenShot(const struct WMImageInfo &info) override;

    void StartAnimation(struct Animation &animation);
    void AnimationSync(int64_t time, void *data);

    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    sptr<VsyncHelper> vhelper = nullptr;
    sptr<Window> window = nullptr;
#ifdef ACE_ENABLE_GPU
    sptr<EglSurface> eglSurface = nullptr;
#endif

    std::atomic<bool> isAnimationRunning = false;
    sptr<PromiseAnimationScreenshotInfo> screenshotPromise = nullptr;
    std::unique_ptr<RotationAnimation> ranimation = nullptr;
};
} // namespace OHOS

#endif // FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_ANIMATION_MODULE_H
