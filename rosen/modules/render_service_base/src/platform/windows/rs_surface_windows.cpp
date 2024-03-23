/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_surface_windows.h"

#include <glfw_render_context.h>
#include <include/core/SkColorSpace.h>
#include <include/gpu/gl/GrGLInterface.h>

#include "platform/common/rs_log.h"
#include "rs_surface_frame_windows.h"

namespace OHOS {
namespace Rosen {
RSSurfaceWindows::RSSurfaceWindows(OnRenderFunc onRender)
    : onRender_(onRender)
{
}

RSSurfaceWindows::~RSSurfaceWindows()
{
    if (grContext_) {
        grContext_->ReleaseResourcesAndAbandonContext();
    }
    grContext_ = nullptr;
}

bool RSSurfaceWindows::IsValid() const
{
    return onRender_ != nullptr;
}

void RSSurfaceWindows::SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
}

std::unique_ptr<RSSurfaceFrame> RSSurfaceWindows::RequestFrame(
    int32_t width, int32_t height, uint64_t uiTimestamp, bool useAFBC)
{
    if (onRender_ == nullptr) {
        ROSEN_LOGE("RSSurfaceWindows::RequestFrame, producer is nullptr");
        return nullptr;
    }

    auto frame = std::make_unique<RSSurfaceFrameWindows>(width, height);
    if (SetupGrContext() == false) {
        return frame;
    }

    struct Drawing::FrameBuffer bufferInfo;
    bufferInfo.width = frame->width_;
    bufferInfo.height = frame->height_;
    bufferInfo.FBOID = 0;
    bufferInfo.Format = 0x8058; // GL_RGBA8
    bufferInfo.gpuContext = grContext_;
    bufferInfo.colorSpace = drColorSpace_;
    bufferInfo.colorType = Drawing::COLORTYPE_RGBA_8888;
    frame->surface_ = std::make_shared<Drawing::Surface>();
    if (!frame->surface_->Bind(bufferInfo)) {
        ROSEN_LOGE("RSSurfaceWindows::RequestFrame, surface bind failed");
        return frame;
    }
#ifdef USE_GLFW_WINDOW
    const auto canvas = frame->surface_->GetCanvas();
    if (canvas != nullptr) {
        canvas->Translate(0, frame->height_);
        canvas->Scale(1, -1);
    }
#endif
    return frame;
}

bool RSSurfaceWindows::FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    if (frame == nullptr) {
        ROSEN_LOGE("RSSurfaceFrame::FlushFrame frame is nullptr");
        return false;
    }

    // RSSurfaceWindows is the class for platform Windows,
    // the input pointer should be the pointer to the class RSSurfaceFrameWindows.
    // We use reinterpret_cast instead of RTTI and dynamic_cast which are not permitted.
    auto frameWindows = reinterpret_cast<RSSurfaceFrameWindows *>(frame.get());
    void *addr = nullptr;
    if (frameWindows->addr_ == nullptr) {
        ROSEN_LOGW("RSSurfaceWindows::FlushFrame frame.addr is nullptr");
    } else {
        addr = frameWindows->addr_.get();
    }

    Drawing::BitmapFormat format = { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    Drawing::Bitmap bitmap;
    bitmap.Build(frameWindows->width_, frameWindows->height_, format);
    bitmap.SetPixels(addr);
    if (frameWindows->surface_ != nullptr) {
        auto image = frameWindows->surface_->GetImageSnapshot();
        image->ReadPixels(bitmap, 0, 0);
    }
#ifdef USE_GLFW_WINDOW
    if (frameWindows->surface_ != nullptr) {
        YInvert(addr, frameWindows->width_, frameWindows->height_);
    }
#endif

    int32_t width = frameWindows->width_;
    int32_t height = frameWindows->height_;
    int32_t size = width * height * 0x4;
    onRender_(addr, size, width, height, uiTimestamp);

#ifdef USE_GLFW_WINDOW
    GlfwRenderContext::GetGlobal()->SwapBuffers();
#endif
    return true;
}

RenderContext* RSSurfaceWindows::GetRenderContext()
{
    return renderContext_;
}

void RSSurfaceWindows::SetRenderContext(RenderContext* context)
{
    renderContext_ = context;
}

void RSSurfaceWindows::YInvert(void *addr, int32_t width, int32_t height)
{
    const auto &pixels = reinterpret_cast<uint32_t *>(addr);
    const auto &halfHeight = height / 0x2;
    const auto &tmpPixels = std::make_unique<uint32_t[]>(width * halfHeight);
    for (int32_t i = 0; i < halfHeight; i++) {
        for (int32_t j = 0; j < width; j++) {
            tmpPixels[i * width + j] = pixels[i * width + j];
        }
    }

    for (int32_t i = 0; i < halfHeight; i++) {
        const auto &r = height - 1 - i;
        for (int32_t j = 0; j < width; j++) {
            pixels[i * width + j] = pixels[r * width + j];
        }
    }

    for (int32_t i = 0; i < halfHeight; i++) {
        const auto &r = height - 1 - i;
        for (int32_t j = 0; j < width; j++) {
            pixels[r * width + j] = tmpPixels[i * width + j];
        }
    }
}

bool RSSurfaceWindows::SetupGrContext()
{
    if (grContext_ != nullptr) {
        return true;
    }

    GlfwRenderContext::GetGlobal()->MakeCurrent();
    auto grContext = std::make_shared<Drawing::GPUContext>();
    Drawing::GPUContextOptions options;
    if (!grContext->BuildFromGL(options)) {
        RS_LOGE("grContext is nullptr");
        return false;
    }
    grContext_ = grContext;
    return true;
}

uint32_t RSSurfaceWindows::GetQueueSize() const
{
    return 0x3;
}

void RSSurfaceWindows::ClearBuffer()
{
}

void RSSurfaceWindows::ClearAllBuffer()
{
}

void RSSurfaceWindows::ResetBufferAge()
{
    ROSEN_LOGD("RSSurfaceWindows: Reset Buffer Age!");
}
} // namespace Rosen
} // namespace OHOS
