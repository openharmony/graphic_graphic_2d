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
#include <display_type.h>
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
    struct NativeWindow* nativeWindow = CreateNativeWindowFromSurface(&producer_);
    if (nativeWindow == nullptr) {
        return nullptr;
    }

    frame_ = std::make_unique<SurfaceFrameOhosGl>(width, height);
    frame_->SetColorSpace(ColorGamut::COLOR_GAMUT_SRGB);
    frame_->SetSurface(static_cast<EGLSurface>(drawingProxy_->CreateSurface((EGLNativeWindowType)nativeWindow)));
    NativeWindowHandleOpt(nativeWindow, SET_BUFFER_GEOMETRY, frame_->GetWidth(), frame_->GetHeight());
    NativeWindowHandleOpt(nativeWindow, SET_COLOR_GAMUT, frame_->GetColorSpace());
    drawingProxy_->MakeCurrent();

    std::unique_ptr<SurfaceFrame> ret(std::move(frame_));
    DestoryNativeWindow(nativeWindow);
    return ret;
}

bool SurfaceOhosGl::FlushFrame(std::unique_ptr<SurfaceFrame>& frame)
{
    // gpu render flush
    drawingProxy_->RenderFrame();
    drawingProxy_->SwapBuffers();
    return true;
}

SkCanvas* SurfaceOhosGl::GetCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    return drawingProxy_->AcquireCanvas(frame);
}
} // namespace Rosen
} // namespace OHOS
