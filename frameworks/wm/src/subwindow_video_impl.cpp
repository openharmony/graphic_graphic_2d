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

#include "subwindow_video_impl.h"

#include <display_type.h>

#include "window_impl.h"
#include "window_manager_hilog.h"
#include "wl_shm_buffer_factory.h"
#include "wl_subsurface_factory.h"
#include "wl_surface_factory.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WMSubwindowVideoImpl"};
}

GSError SubwindowVideoImpl::CreateWlSurface(const sptr<WlSurface> &parentWlSurface)
{
    wlSurface = SingletonContainer::Get<WlSurfaceFactory>()->Create();
    if (wlSurface == nullptr) {
        WMLOGFE("WlSurfaceFactory::Create return nullptr");
        return GSERROR_API_FAILED;
    }

    auto subsurfaceFactory = SingletonContainer::Get<WlSubsurfaceFactory>();
    wlSubsurf = subsurfaceFactory->Create(wlSurface, parentWlSurface);
    if (wlSubsurf == nullptr) {
        WMLOGFE("WlSubsurf::Create return nullptr");
        return GSERROR_API_FAILED;
    }

    wlSubsurf->SetPosition(attr.GetX(), attr.GetY());
    wlSubsurf->PlaceBelow(parentWlSurface);
    wlSubsurf->SetDesync();
    return GSERROR_OK;
}

GSError SubwindowVideoImpl::CreateLayer()
{
#ifdef TARGET_CPU_ARM
    LayerInfo layerInfo = {
        .width = attr.GetWidth(),
        .height = attr.GetHeight(),
        .type = LAYER_TYPE_SDIEBAND,
        .bpp = 0x8,
        .pixFormat = PIXEL_FMT_RGBA_8888,
    };
    int32_t ret = VideoDisplayManager::CreateLayer(layerInfo, layerId, csurf);
#else
    layerId = -1;
    int32_t ret = DISPLAY_FAILURE;
#endif
    if (ret) {
        WMLOGFE("SubwindowVideoImpl::CreateLayer return nullptr");
        return GSERROR_API_FAILED;
    }

    if (csurf != nullptr) {
        csurf->SetDefaultWidthAndHeight(attr.GetWidth(), attr.GetHeight());
        csurf->SetDefaultUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);
    } else {
        WMLOGFE("SubwindowVideoImpl::CreateLayer csurf is nullptr");
    }
    return GSERROR_OK;
}

GSError SubwindowVideoImpl::CreateSHMBuffer()
{
    auto width = attr.GetWidth();
    auto height = attr.GetHeight();
    auto format = WL_SHM_FORMAT_XRGB8888;
    wlBuffer = SingletonContainer::Get<WlSHMBufferFactory>()->Create(width, height, format);
    if (wlBuffer == nullptr) {
        WMLOGFE("WlSHMBufferFactory::Create return nullptr");
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
}

GSError SubwindowVideoImpl::CreateSurface()
{
#ifdef TARGET_CPU_ARM
    display = new (std::nothrow) VideoDisplayManager();
    if (display == nullptr) {
        WMLOGFE("new VideoDisplayManager failed");
        return GSERROR_NO_MEM;
    }

    auto producer = display->AttachLayer(csurf, layerId);
    if (producer == nullptr) {
        WMLOGFE("VideoDisplayManager attach layer failed");
        return GSERROR_API_FAILED;
    }

    psurf = Surface::CreateSurfaceAsProducer(producer);
    return GSERROR_OK;
#else
    return GSERROR_API_FAILED;
#endif
}

GSError SubwindowVideoImpl::Init(const sptr<Window> &window,
                                 const sptr<SubwindowOption> &option)
{
    auto windowImpl = static_cast<WindowImpl *>(window.GetRefPtr());
    if (windowImpl == nullptr) {
        WMLOGFE("WindowImpl is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }
    if (option == nullptr) {
        WMLOGFE("option is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }

    pw = window;
    attr.SetWidthHeight(option->GetWidth(), option->GetHeight());
    attr.SetXY(option->GetX(), option->GetY());

    auto wret = CreateWlSurface(windowImpl->GetWlSurface());
    if (wret != GSERROR_OK) {
        return wret;
    }

    wret = CreateLayer();
    if (wret != GSERROR_OK) {
        return wret;
    }

    wret = CreateSHMBuffer();
    if (wret != GSERROR_OK) {
        return wret;
    }

    wlSurface->SetSurfaceType(WL_SURFACE_TYPE_VIDEO);
    wlSurface->Attach(wlBuffer, 0, 0);
    wlSurface->Damage(0, 0, attr.GetWidth(), attr.GetHeight());
    wlSurface->Commit();

    wret = CreateSurface();
    if (wret != GSERROR_OK) {
        return wret;
    }

    wret = Move(option->GetX(), option->GetY());
    if (wret != GSERROR_OK) {
        return wret;
    }

    WMLOGFI("Create Video Subwindow Success");
    return GSERROR_OK;
}

sptr<Surface> SubwindowVideoImpl::GetSurface() const
{
    return psurf;
}

GSError SubwindowVideoImpl::Move(int32_t x, int32_t y)
{
    WMLOGFI("(subwindow video) x: %{public}d, y: %{public}d", x, y);
    if (display == nullptr) {
        WMLOGFE("display layer is not create");
        return GSERROR_NOT_INIT;
    }

#ifdef TARGET_CPU_ARM
    IRect rect = {};
    int32_t ret = display->GetRect(layerId, rect);
    if (ret != DISPLAY_SUCCESS) {
        WMLOGFE("get rect fail, ret:%{public}d", ret);
        return GSERROR_API_FAILED;
    }

    attr.SetXY(x, y);

    rect.x = x;
    rect.y = y;
    auto parentWindow = pw.promote();
    if (parentWindow != nullptr) {
        rect.x += parentWindow->GetX();
        rect.y += parentWindow->GetY();
    }

    WMLOGFI("(subwindow video) rect.x: %{public}d, rect.y: %{public}d", rect.x, rect.y);
    ret = display->SetRect(layerId, rect);
    if (ret != DISPLAY_SUCCESS) {
        WMLOGFE("set rect fail, ret:%{public}d", ret);
        return GSERROR_API_FAILED;
    }

    wlSubsurf->SetPosition(attr.GetX(), attr.GetY());
#endif
    return GSERROR_OK;
}

GSError SubwindowVideoImpl::Resize(uint32_t width, uint32_t height)
{
    WMLOGFI("(subwindow video) %{public}u x %{public}u", width, height);
    if (display == nullptr) {
        WMLOGFE("display layer is not create");
        return GSERROR_NOT_INIT;
    }
#ifdef TARGET_CPU_ARM
    IRect rect = {};
    int32_t ret = display->GetRect(layerId, rect);
    if (ret != DISPLAY_SUCCESS) {
        WMLOGFE("get rect fail, ret:%d", ret);
        return GSERROR_API_FAILED;
    }

    attr.SetWidthHeight(width, height);
    rect.w = width;
    rect.h = height;
    ret = display->SetRect(layerId, rect);
    if (ret != DISPLAY_SUCCESS) {
        WMLOGFE("set rect fail, ret:%{public}d", ret);
        return GSERROR_API_FAILED;
    }
#endif
    return GSERROR_OK;
}

GSError SubwindowVideoImpl::Destroy()
{
    WMLOGFI("(subwindow video) Destroy");
    csurf = nullptr;
    psurf = nullptr;
    wlSubsurf = nullptr;
    wlSurface = nullptr;
    wlBuffer = nullptr;
#ifdef TARGET_CPU_ARM
    if (display != nullptr) {
        display->DetachLayer(layerId);
    }
    VideoDisplayManager::DestroyLayer(layerId);
#endif
    display = nullptr;
    return GSERROR_OK;
}

void SubwindowVideoImpl::OnPositionChange(WindowPositionChangeFunc func)
{
    attr.OnPositionChange(func);
}

void SubwindowVideoImpl::OnSizeChange(WindowSizeChangeFunc func)
{
    attr.OnSizeChange(func);
}

SubwindowVideoImpl::~SubwindowVideoImpl()
{
    Destroy();
}
} // namespace OHOS
