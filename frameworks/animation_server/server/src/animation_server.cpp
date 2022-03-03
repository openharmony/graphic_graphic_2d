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

#include <multimodal_event_handler.h>
#include <securec.h>

#include <cpudraw.h>
#include <scoped_bytrace.h>
#include <gslogger.h>

namespace OHOS {
namespace {
DEFINE_HILOG_LABEL("AnimationServer");
} // namespace

GSError AnimationServer::Init()
{
    handler = AppExecFwk::EventHandler::Current();
    auto wm = WindowManager::GetInstance();
    auto wret = wm->Init();
    if (wret != GSERROR_OK) {
        GSLOG2HI(ERROR) << "WindowManager::Init failed: " << GSErrorStr(wret);
        return static_cast<enum GSError>(wret);
    }

    WindowManagerServiceClient::GetInstance()->Init();
    animationModule.Init();

    auto splitOption = WindowOption::Get();
    splitOption->SetWindowType(WINDOW_TYPE_SPLIT_LINE);
    wret = wm->CreateWindow(splitWindow, splitOption);
    if (wret != GSERROR_OK || splitWindow == nullptr) {
        GSLOG2HI(ERROR) << "WindowManager::CreateWindow failed: " << GSErrorStr(wret);
        return static_cast<enum GSError>(wret);
    }
    splitWindow->Hide();
    auto func = std::bind(&AnimationServer::OnSplitStatusChange, this, std::placeholders::_1);
    splitWindow->OnSplitStatusChange(func);

    auto launchPageOption = WindowOption::Get();
    launchPageOption->SetWindowType(WINDOW_TYPE_LAUNCH_PAGE);
    wret = wm->CreateWindow(launchPageWindow, launchPageOption);
    if (wret != WM_OK || launchPageWindow == nullptr) {
        GSLOG2HI(ERROR) << "WindowManager::CreateWindow failed: " << WMErrorStr(wret);
        return static_cast<enum GSError>(wret);
    }
    launchPageWindow->Hide();
    return GSERROR_OK;
}

GSError AnimationServer::StartRotationAnimation(int32_t did, int32_t degree)
{
    return animationModule.StartRotationAnimation(did, degree);
}

GSError AnimationServer::SplitModeCreateBackground()
{
    ScopedBytrace trace(__func__);
    GSLOG2HI(DEBUG);
    splitWindow->Show();
    splitWindow->SwitchTop();
    return GSERROR_OK;
}

GSError AnimationServer::SplitModeCreateMiddleLine()
{
    ScopedBytrace trace(__func__);
    GSLOG2HI(DEBUG);
    midlineY = splitWindow->GetHeight() / 0x2 + splitWindow->GetY();
    haveMiddleLine = true;
    handler->PostTask(std::bind(&AnimationServer::SplitWindowUpdate, this));
    return GSERROR_OK;
}

GSError AnimationServer::CreateLaunchPage(const std::string &filename)
{
    GSLOG2HI(DEBUG);
    auto ret = resource.Parse(filename);
    if (ret) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    int32_t width = launchPageWindow->GetDestWidth();
    int32_t height = launchPageWindow->GetDestHeight();

    launchPageWindow->SwitchTop();
    launchPageWindow->Show();
    launchPageWindow->Resize(resource.GetWidth(), resource.GetHeight());
    launchPageWindow->ScaleTo(width, height);

    handler->PostTask(std::bind(&AnimationServer::LaunchPageWindowUpdate, this));
    return GSERROR_OK;
}

GSError AnimationServer::CancelLaunchPage()
{
    GSLOG2HI(DEBUG);
    launchPageWindow->Hide();
    return GSERROR_OK;
}

void AnimationServer::SplitWindowUpdate()
{
    ScopedBytrace trace(__func__);
    sptr<SurfaceBuffer> buffer;
    auto surf = splitWindow->GetSurface();
    BufferRequestConfig rconfig = {
        .width = surf->GetDefaultWidth(),
        .height = surf->GetDefaultHeight(),
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = surf->GetDefaultUsage(),
        .timeout = 0,
    };

    int releaseFence = -1;
    GSError ret = surf->RequestBuffer(buffer, releaseFence, rconfig);
    if (ret == GSERROR_NO_BUFFER) {
        return;
    } else if (ret != GSERROR_OK || buffer == nullptr) {
        return;
    }

    auto addr = buffer->GetVirAddr();
    if (addr == nullptr) {
        surf->CancelBuffer(buffer);
        return;
    }

    static int32_t count = 0;
    SplitWindowDraw(reinterpret_cast<uint32_t *>(buffer->GetVirAddr()), rconfig.width, rconfig.height, count);
    count++;

    BufferFlushConfig fconfig = {
        .damage = {
            .w = rconfig.width,
            .h = rconfig.height,
        },
    };
    surf->FlushBuffer(buffer, -1, fconfig);
}

void AnimationServer::SplitWindowDraw(uint32_t *vaddr, uint32_t width, uint32_t height, uint32_t count)
{
    ScopedBytrace trace(__func__);
    GSLOG2HI(DEBUG) << "midlineY: " << midlineY << ", midlineDown: " << midlineDown;
    Cpudraw draw(vaddr, width, height);

    draw.SetColor(0xff000000);
    draw.DrawRect(0, 0, width, height);
    if (haveMiddleLine == false) {
        draw.SetColor(0xff333333);
        constexpr double left = 0.1;
        constexpr double w = 0.8;
        constexpr double top1 = 0.025;
        constexpr double top2 = 0.575;
        constexpr double h = 0.4;

        draw.DrawRect(left * width, top1 * height, w * width, h * height);
        draw.DrawRect(left * width, top2 * height, w * width, h * height);
    } else {
        auto midlineYlocal = midlineY - splitWindow->GetY();
        draw.SetColor(midlineDown ? 0xffffffff : 0xffcccccc);
        constexpr double lineHeight = 0.1;
        draw.DrawRect(0, midlineYlocal - height * lineHeight / 0x2, width, height * lineHeight);
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

void AnimationServer::LaunchPageWindowUpdate()
{
    auto surf = launchPageWindow->GetSurface();
    sptr<SurfaceBuffer> buffer;
    BufferRequestConfig rconfig = {
        .width = launchPageWindow->GetWidth(),
        .height = launchPageWindow->GetHeight(),
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = surf->GetDefaultUsage(),
        .timeout = 0,
    };

    int releaseFence = -1;
    SurfaceError ret = surf->RequestBuffer(buffer, releaseFence, rconfig);
    if (ret == SURFACE_ERROR_NO_BUFFER) {
        return;
    } else if (ret != SURFACE_ERROR_OK || buffer == nullptr) {
        return;
    }

    auto addr = buffer->GetVirAddr();
    if (addr == nullptr) {
        surf->CancelBuffer(buffer);
        return;
    }

    resource.GetNextData(reinterpret_cast<uint32_t *>(addr));
    BufferFlushConfig fconfig = {
        .damage = {
            .w = rconfig.width,
            .h = rconfig.height,
        },
    };
    surf->FlushBuffer(buffer, -1, fconfig);
}
} // namespace OHOS
