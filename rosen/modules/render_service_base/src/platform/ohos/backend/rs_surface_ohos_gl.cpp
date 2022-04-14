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

#include "rs_surface_ohos_gl.h"
#include "platform/common/rs_log.h"
#include "window.h"
#include <hilog/log.h>
#include <display_type.h>
#include "pipeline/rs_render_thread.h"

namespace OHOS {
namespace Rosen {

RSSurfaceOhosGl::RSSurfaceOhosGl(const sptr<Surface>& producer) : RSSurfaceOhos(producer)
{
}

RSSurfaceOhosGl::~RSSurfaceOhosGl()
{
    DestoryNativeWindow(mWindow);
    if (context_ != nullptr) {
        context_->DestroyEGLSurface(mEglSurface);
    }
    mWindow = nullptr;
    mEglSurface = EGL_NO_SURFACE;
}

std::unique_ptr<RSSurfaceFrame> RSSurfaceOhosGl::RequestFrame(int32_t width, int32_t height)
{
    RenderContext* context = GetRenderContext();
    if (context == nullptr) {
        ROSEN_LOGE("RSSurfaceOhosGl::RequestFrame, GetRenderContext failed!");
        return nullptr;
    }
    context->SetColorSpace(colorSpace_);
    if (mWindow == nullptr) {
        mWindow = CreateNativeWindowFromSurface(&producer_);
        mEglSurface = context->CreateEGLSurface((EGLNativeWindowType)mWindow);
        ROSEN_LOGI("RSSurfaceOhosGl: create and Init EglSurface %p", mEglSurface);
    }

    if (mEglSurface == EGL_NO_SURFACE) {
        ROSEN_LOGE("RSSurfaceOhosGl: Invalid eglSurface, return");
        return nullptr;
    }

    std::unique_ptr<RSSurfaceFrameOhosGl> frame = std::make_unique<RSSurfaceFrameOhosGl>(width, height);

    NativeWindowHandleOpt(mWindow, SET_BUFFER_GEOMETRY, width, height);
    NativeWindowHandleOpt(mWindow, GET_BUFFER_GEOMETRY, &mHeight, &mWidth);
    NativeWindowHandleOpt(mWindow, SET_COLOR_GAMUT, colorSpace_);

    context->MakeCurrent(mEglSurface);

    ROSEN_LOGI("RSSurfaceOhosGl:RequestFrame, eglsurface is %p, width is %d, height is %d",
        mEglSurface, mWidth, mHeight);

    frame->SetRenderContext(context);

    std::unique_ptr<RSSurfaceFrame> ret(std::move(frame));

    return ret;
}

bool RSSurfaceOhosGl::FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame)
{
    RenderContext* context = GetRenderContext();
    if (context == nullptr) {
        ROSEN_LOGE("RSSurfaceOhosGl::FlushFrame, GetRenderContext failed!");
        return false;
    }

    // gpu render flush
    context->RenderFrame();
    context->SwapBuffers(mEglSurface);
    ROSEN_LOGD("RSSurfaceOhosGl: FlushFrame, SwapBuffers eglsurface is %p", mEglSurface);
    return true;
}
} // namespace Rosen
} // namespace OHOS
