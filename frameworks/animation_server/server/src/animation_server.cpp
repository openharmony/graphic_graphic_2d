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

#include "animation_server.h"

#include <cassert>
#include <chrono>
#include <fstream>
#include <multimodal_event_handler.h>
#include <securec.h>
#include <sys/time.h>

#include <cpudraw.h>
#include <graphic_bytrace.h>
#include <gslogger.h>

namespace OHOS {
namespace {
DEFINE_HILOG_LABEL("AnimationServer");
} // namespace

GSError AnimationServer::Init()
{
    handler = AppExecFwk::EventHandler::Current();
    vhelper = VsyncHelper::FromHandler(handler);
    auto wm = WindowManager::GetInstance();
    auto wret = wm->Init();
    if (wret != WM_OK) {
        GSLOG2HI(ERROR) << "WindowManager::Init failed: " << WMErrorStr(wret);
        return static_cast<enum GSError>(wret);
    }

    WindowManagerServiceClient::GetInstance()->Init();

    auto splitOption = WindowOption::Get();
    splitOption->SetWindowType(WINDOW_TYPE_SPLIT_LINE);
    wret = wm->CreateWindow(splitWindow, splitOption);
    if (wret != WM_OK || splitWindow == nullptr) {
        GSLOG2HI(ERROR) << "WindowManager::CreateWindow failed: " << WMErrorStr(wret);
        return static_cast<enum GSError>(wret);
    }
    splitWindow->Hide();
    auto func = std::bind(&AnimationServer::OnSplitStatusChange, this, std::placeholders::_1);
    splitWindow->OnSplitStatusChange(func);

    auto option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_ANIMATION);
    wret = wm->CreateWindow(window, option);
    if (wret != WM_OK || window == nullptr) {
        GSLOG2HI(ERROR) << "WindowManager::CreateWindow failed: " << WMErrorStr(wret);
        return static_cast<enum GSError>(wret);
    }

    window->Hide();
#ifdef ACE_ENABLE_GPU
    auto producer = window->GetProducer();
    eglSurface = EglRenderSurface::CreateEglSurfaceAsProducer(producer);
#endif
    return GSERROR_OK;
}

GSError AnimationServer::StartRotationAnimation(int32_t did, int32_t degree)
{
#ifdef ACE_ENABLE_GPU
    if (isAnimationRunning == false) {
        struct Animation animation {
            .degree = degree,
            .retval = new Promise<GSError>(),
        };
        handler->PostTask(std::bind(&AnimationServer::StartAnimation, this, std::ref(animation)));
        return animation.retval->Await();
    }
    return GSERROR_ANIMATION_RUNNING;
#else
    return GSERROR_NOT_SUPPORT;
#endif
}

GSError AnimationServer::SplitModeCreateBackground()
{
    ScopedBytrace trace(__func__);
    GSLOG2HI(DEBUG);
    splitWindow->Show();
    splitWindow->SwitchTop();
    if (thandler == nullptr) {
        thandler = new TouchEventHandler(this);
        MMIEventHdl.RegisterStandardizedEventHandle(this, splitWindow->GetID(), thandler);
    }
    handler->PostTask(std::bind(&AnimationServer::SplitWindowUpdate, this));
    return GSERROR_OK;
}

GSError AnimationServer::SplitModeCreateMiddleLine()
{
    ScopedBytrace trace(__func__);
    GSLOG2HI(DEBUG);
    midlineY = splitWindow->GetHeight() / 2 + splitWindow->GetY();
    haveMiddleLine = true;
    handler->PostTask(std::bind(&AnimationServer::SplitWindowUpdate, this));
    return GSERROR_OK;
}

#ifdef ACE_ENABLE_GPU
void AnimationServer::StartAnimation(struct Animation &animation)
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
        GSLOG2HI(ERROR) << "OnScreenShot failed: " << WMErrorStr(asinfo.wmimage.wret);
        animation.retval->Resolve(static_cast<enum GSError>(asinfo.wmimage.wret));
        isAnimationRunning = false;
        return;
    }

    assert(asinfo.wmimage.width == window->GetWidth());
    assert(asinfo.wmimage.height == window->GetHeight());
    window->Show();

    auto sret = eglSurface->InitContext();
    if (sret != SURFACE_ERROR_OK) {
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

    struct FrameCallback cb = { .callback_ = std::bind(&AnimationServer::AnimationSync, this, SYNC_FUNC_ARG) };
    animation.retval->Resolve(static_cast<enum GSError>(vhelper->RequestFrameCallback(cb)));
}

void AnimationServer::AnimationSync(int64_t time, void *data)
{
    ScopedBytrace trace(__func__);
    if (ranimation->Draw()) {
        eglSurface->SwapBuffers();
        struct FrameCallback cb = { .callback_ = std::bind(&AnimationServer::AnimationSync, this, SYNC_FUNC_ARG) };
        vhelper->RequestFrameCallback(cb);
        sb.End();
    } else {
        sb.End();
        GSLOG2HI(INFO) << "Animation End";
        window->Hide();
        isAnimationRunning = false;
    }
}

void AnimationServer::OnScreenShot(const struct WMImageInfo &info)
{
    int32_t length = info.size;
    struct AnimationScreenshotInfo ainfo = {
        .wmimage = info,
        .ptr = nullptr,
    };
    if (info.wret != WM_OK) {
        screenshotPromise->Resolve(ainfo);
        return;
    }

    ainfo.ptr = std::make_shared<Array>();
    ainfo.ptr->ptr = std::make_unique<uint8_t[]>(length);
    ainfo.wmimage.data = ainfo.ptr.get();
    if (memcpy_s(ainfo.ptr->ptr.get(), length, info.data, info.size) != EOK) {
        GSLOG2HI(ERROR) << "memcpy_s failed: " << strerror(errno);
        ainfo.wmimage.wret = static_cast<enum WMError>(GSERROR_INTERNEL);
        screenshotPromise->Resolve(ainfo);
        return;
    }

    screenshotPromise->Resolve(ainfo);
}
#else
void AnimationServer::StartAnimation(struct Animation &animation)
{
}

void AnimationServer::AnimationSync(int64_t time, void *data)
{
}

void AnimationServer::OnScreenShot(const struct WMImageInfo &info)
{
}
#endif

void AnimationServer::SplitWindowUpdate()
{
    ScopedBytrace trace(__func__);
    sptr<SurfaceBuffer> buffer;
    auto surface = splitWindow->GetSurface();
    BufferRequestConfig rconfig = {
        .width = surface->GetDefaultWidth(),
        .height = surface->GetDefaultHeight(),
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = surface->GetDefaultUsage(),
        .timeout = 0,
    };

    SurfaceError ret = surface->RequestBufferNoFence(buffer, rconfig);
    if (ret == SURFACE_ERROR_NO_BUFFER) {
        return;
    } else if (ret != SURFACE_ERROR_OK || buffer == nullptr) {
        return;
    }

    auto addr = buffer->GetVirAddr();
    if (addr == nullptr) {
        surface->CancelBuffer(buffer);
        return;
    }

    static int32_t count = 0;
    SplitWindowDraw(buffer->GetVirAddr(), rconfig.width, rconfig.height, count);
    count++;

    BufferFlushConfig fconfig = {
        .damage = {
            .w = rconfig.width,
            .h = rconfig.height,
        },
    };
    surface->FlushBuffer(buffer, -1, fconfig);
}

void AnimationServer::SplitWindowDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
{
    ScopedBytrace trace(__func__);
    GSLOG2HI(DEBUG) << "midlineY: " << midlineY << ", midlineDown: " << midlineDown;
    CPUDraw draw(vaddr, width, height);

    draw.SetColor(0xff000000);
    draw.DrawRect(0, 0, width, height);
    if (haveMiddleLine == false) {
        draw.SetColor(0xff333333);
        draw.DrawRect(0.1 * width, 0.025 * height, 0.8 * width, 0.4 * height);
        draw.DrawRect(0.1 * width, 0.575 * height, 0.8 * width, 0.4 * height);
    } else {
        auto midlineYlocal = midlineY - splitWindow->GetY();
        draw.SetColor(midlineDown ? 0xffffffff : 0xffcccccc);
        draw.DrawRect(0, midlineYlocal - height * 0.05, width, height * 0.1);
    }
}

void AnimationServer::OnSplitStatusChange(SplitStatus status)
{
    ScopedBytrace trace(__func__);
    if (status == SPLIT_STATUS_DESTROY) {
        splitWindow->Hide();
        haveMiddleLine = false;
    }
}

bool AnimationServer::OnTouch(const TouchEvent &event)
{
    ScopedBytrace trace(__func__);
    auto wms = WindowManagerServiceClient::GetInstance()->GetService();
    int32_t index = event.GetIndex();
    int32_t x = event.GetPointerPosition(index).GetX();
    int32_t y = event.GetPointerPosition(index).GetY();
    GSLOG2HI(DEBUG) << "touch event: " << event.GetAction() << " " << x << " " << y;
    if (event.GetAction() == TouchEnum::PRIMARY_POINT_DOWN) {
        wms->SetSplitMode(SPLIT_MODE_DIVIDER_TOUCH_DOWN);
        midlineDown = true;
        downX = x;
        downY = y;
        midlineYBackup = midlineY;
        handler->PostTask(std::bind(&AnimationServer::SplitWindowUpdate, this));
    } else if (event.GetAction() == TouchEnum::POINT_MOVE) {
        midlineY = midlineYBackup + y - downY;
        wms->SetSplitMode(SPLIT_MODE_DIVIDER_TOUCH_MOVE, x, midlineY);
        handler->PostTask(std::bind(&AnimationServer::SplitWindowUpdate, this));
    } else if (event.GetAction() == TouchEnum::PRIMARY_POINT_UP) {
        wms->SetSplitMode(SPLIT_MODE_DIVIDER_TOUCH_UP);
        midlineDown = false;
        handler->PostTask(std::bind(&AnimationServer::SplitWindowUpdate, this));
    } else {
        return true;
    }

    return false;
}
} // namespace OHOS
