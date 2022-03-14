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

#include "animation_module.h"

#include <cerrno>

#include <gslogger.h>
#include <scoped_bytrace.h>
#include <securec.h>

namespace OHOS {
namespace {
DEFINE_HILOG_LABEL("AnimationModule");
} // namespace

GSError AnimationModule::Init()
{
    handler = AppExecFwk::EventHandler::Current();
    vhelper = VsyncHelper::Current();
    auto wm = WindowManager::GetInstance();
    auto option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_ANIMATION);
    auto wret = wm->CreateWindow(window, option);
    if (wret != GSERROR_OK || window == nullptr) {
        GSLOG2HI(ERROR) << "WindowManager::CreateWindow failed: " << GSErrorStr(wret);
        return wret;
    }

    window->Hide();
    auto producer = window->GetProducer();
    eglSurface = EglSurface::CreateEglSurfaceAsProducer(producer);
    return GSERROR_OK;
}

GSError AnimationModule::StartRotationAnimation(int32_t did, int32_t degree)
{
    if (isAnimationRunning == false) {
        struct Animation animation {
            .degree = degree,
            .retval = new Promise<GSError>(),
        };
        handler->PostTask(std::bind(&AnimationModule::StartAnimation, this, std::ref(animation)));
        return animation.retval->Await();
    }
    return GSERROR_ANIMATION_RUNNING;
}

void AnimationModule::OnScreenShot(const struct WMImageInfo &info)
{
    int32_t length = info.size;
    struct AnimationScreenshotInfo ainfo = {
        .wmimage = info,
        .ptr = nullptr,
    };
    if (info.wret != GSERROR_OK) {
        screenshotPromise->Resolve(ainfo);
        return;
    }

    ainfo.ptr = std::make_shared<struct Array>();
    ainfo.ptr->ptr = std::make_unique<uint8_t[]>(length);
    ainfo.wmimage.data = ainfo.ptr.get();
    if (memcpy_s(ainfo.ptr->ptr.get(), length, info.data, info.size) != EOK) {
        GSLOG2HI(ERROR) << "memcpy_s failed: " << strerror(errno);
        ainfo.wmimage.wret = static_cast<enum GSError>(GSERROR_INTERNEL);
        screenshotPromise->Resolve(ainfo);
        return;
    }

    screenshotPromise->Resolve(ainfo);
}

void AnimationModule::StartAnimation(struct Animation &animation)
{
    if (isAnimationRunning) {
        animation.retval->Resolve(GSERROR_ANIMATION_RUNNING);
        return;
    }

    ScopedBytrace trace(__func__);
    isAnimationRunning = true;
    GSLOG2HI(INFO) << "Animation Start";
    window->Hide();
    auto wm = WindowManager::GetInstance();
    screenshotPromise = new Promise<struct AnimationScreenshotInfo>();
    wm->ListenNextScreenShot(0, this);
    auto asinfo = screenshotPromise->Await();
    if (asinfo.wmimage.wret) {
        GSLOG2HI(ERROR) << "OnScreenShot failed: " << GSErrorStr(asinfo.wmimage.wret);
        animation.retval->Resolve(static_cast<enum GSError>(asinfo.wmimage.wret));
        isAnimationRunning = false;
        return;
    }

    window->Show();

    auto sret = eglSurface->InitContext();
    if (sret != GSERROR_OK) {
        GSLOG2HI(ERROR) << "EGLSurface InitContext failed: " << sret;
        animation.retval->Resolve(static_cast<enum GSError>(sret));
        isAnimationRunning = false;
        return;
    }

    ranimation = std::make_unique<RotationAnimation>();
    constexpr int32_t rotationAnimationDuration = 500;
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    struct RotationAnimationParam param = {
        .data = asinfo.ptr,
        .width = window->GetWidth(),
        .height = window->GetHeight(),
        .startTime = now,
        .duration = rotationAnimationDuration,
        .degree = animation.degree,
    };
    auto gret = ranimation->Init(param);
    if (gret != GSERROR_OK) {
        GSLOG2HI(ERROR) << "RotationAnimation Init failed: " << GSErrorStr(gret);
        animation.retval->Resolve(gret);
        isAnimationRunning = false;
        return;
    }

    struct FrameCallback cb = { .callback_ = std::bind(&AnimationModule::AnimationSync, this, SYNC_FUNC_ARG) };
    animation.retval->Resolve(static_cast<enum GSError>(vhelper->RequestFrameCallback(cb)));
}

void AnimationModule::AnimationSync(int64_t time, void *data)
{
    ScopedBytrace trace(__func__);
    if (ranimation->Draw()) {
        eglSurface->SwapBuffers();
        struct FrameCallback cb = { .callback_ = std::bind(&AnimationModule::AnimationSync, this, SYNC_FUNC_ARG) };
        vhelper->RequestFrameCallback(cb);
        trace.End();
    } else {
        trace.End();
        GSLOG2HI(INFO) << "Animation End";
        window->Hide();
        isAnimationRunning = false;
    }
}
} // namespace OHOS
