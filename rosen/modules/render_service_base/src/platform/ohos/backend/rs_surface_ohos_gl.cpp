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

#include "rs_surface_frame_ohos_gl.h"
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

std::unique_ptr<RSSurfaceFrame> RSSurfaceOhosGl::RequestFrame(int32_t width, int32_t height)
{
    std::unique_ptr<RSSurfaceFrameOhosGl> frame = std::make_unique<RSSurfaceFrameOhosGl>(width, height);
    struct NativeWindow* nWindow = CreateNativeWindowFromSurface(&producer_);
    int w = 0;
    int h = 0;
    NativeWindowHandleOpt(nWindow, SET_BUFFER_GEOMETRY, width, height);
    NativeWindowHandleOpt(nWindow, GET_BUFFER_GEOMETRY, &h, &w);

    RenderContext* context = GetRenderContext();
    if (context == nullptr) {
        ROSEN_LOGE("RSSurfaceOhosGl::RequestFrame, GetRenderContext failed!");
        return nullptr;
    }

    mEglSurface = context->CreateEGLSurface((EGLNativeWindowType)nWindow);
    context->MakeCurrent(mEglSurface);

    ROSEN_LOGD("RSSurfaceOhosGl::RequestFrame, CreateEGLSurface eglsurface is %p, \
        width is %d, height is %d", mEglSurface, w, h);

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
    ROSEN_LOGD("FlushFrame::SwapBuffers eglsurface is %p", mEglSurface);
    return true;
}
} // namespace Rosen
} // namespace OHOS
