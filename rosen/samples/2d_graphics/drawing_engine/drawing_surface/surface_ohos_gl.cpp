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

#include "surface_ohos_gl.h"

#include <hilog/log.h>
#include "window.h"

namespace OHOS {
namespace Rosen {
SurfaceOhosGl::SurfaceOhosGl(const sptr<Surface>& producer)
    : SurfaceOhos(producer), frame_(nullptr)
{
}

SurfaceOhosGl::~SurfaceOhosGl()
{
    frame_ = nullptr;
}

std::unique_ptr<SurfaceFrame> SurfaceOhosGl::RequestFrame(int32_t width, int32_t height)
{
    if (drawingProxy_ == nullptr) {
        LOGE("drawingProxy_ is nullptr, can not RequestFrame");
        return nullptr;
    }
    struct NativeWindow* nativeWindow = CreateNativeWindowFromSurface(&producer_);
    if (nativeWindow == nullptr) {
        return nullptr;
    }

    frame_ = std::make_unique<SurfaceFrameOhosGl>(width, height);
    frame_->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    frame_->SetSurface(static_cast<EGLSurface>(drawingProxy_->CreateSurface((EGLNativeWindowType)nativeWindow)));
    NativeWindowHandleOpt(nativeWindow, SET_BUFFER_GEOMETRY, frame_->GetWidth(), frame_->GetHeight());
    NativeWindowHandleOpt(nativeWindow, SET_COLOR_GAMUT, frame_->GetColorSpace());
    drawingProxy_->MakeCurrent();

    std::unique_ptr<SurfaceFrame> ret(std::move(frame_));
    DestoryNativeWindow(nativeWindow);
    return ret;
}

std::unique_ptr<SurfaceFrame> SurfaceOhosGl::NativeRequestFrame(int32_t width, int32_t height)
{
    return RequestFrame(width, height);
}

bool SurfaceOhosGl::NativeFlushFrame(std::unique_ptr<SurfaceFrame>& frame)
{
    return FlushFrame(frame);
}

bool SurfaceOhosGl::FlushFrame(std::unique_ptr<SurfaceFrame>& frame)
{
    if (drawingProxy_ == nullptr) {
        LOGE("drawingProxy_ is nullptr, can not FlushFrame");
        return false;
    }
    // gpu render flush
    drawingProxy_->RenderFrame();
    drawingProxy_->SwapBuffers();
    return true;
}

SkCanvas* SurfaceOhosGl::GetSkCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    if (drawingProxy_ == nullptr) {
        LOGE("drawingProxy_ is nullptr, can not GetSkCanvas");
        return nullptr;
    }
    return drawingProxy_->AcquireSkCanvas(frame);
}

Drawing::Canvas* SurfaceOhosGl::GetCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    if (drawingProxy_ == nullptr) {
        LOGE("drawingProxy_ is nullptr, can not GetSkCanvas");
        return nullptr;
    }
    return drawingProxy_->AcquireDrCanvas(frame);
}
} // namespace Rosen
} // namespace OHOS
