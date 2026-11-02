/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * the distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rs_surface_linux.h"

#include <glfw_render_context.h>
#include <include/core/SkColorSpace.h>
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/gl/GrGLInterface.h"
#else
#include <include/gpu/gl/GrGLInterface.h>
#endif

#include "platform/common/rs_log.h"
#include "rs_surface_frame_linux.h"

namespace OHOS {
namespace Rosen {
RSSurfaceLinux::RSSurfaceLinux(OnRenderFunc onRender)
    : onRender_(onRender)
{
}

RSSurfaceLinux::~RSSurfaceLinux()
{
    if (grContext_) {
        grContext_->ReleaseResourcesAndAbandonContext();
    }
    grContext_ = nullptr;
}

bool RSSurfaceLinux::IsValid() const
{
    return onRender_ != nullptr;
}

void RSSurfaceLinux::SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
}

std::unique_ptr<RSSurfaceFrame> RSSurfaceLinux::RequestFrame(
    int32_t width, int32_t height, uint64_t uiTimestamp, bool useAFBC, bool isProtected)
{
    if (onRender_ == nullptr) {
        ROSEN_LOGE("RSSurfaceLinux::RequestFrame, producer is nullptr");
        return nullptr;
    }

    auto frame = std::make_unique<RSSurfaceFrameLinux>(width, height);
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
        ROSEN_LOGE("RSSurfaceLinux::RequestFrame, surface bind failed");
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

bool RSSurfaceLinux::FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    if (frame == nullptr) {
        ROSEN_LOGE("RSSurfaceLinux::FlushFrame frame is nullptr");
        return false;
    }

    auto frameLinux = reinterpret_cast<RSSurfaceFrameLinux *>(frame.get());
    void *addr = nullptr;
    if (frameLinux->addr_ == nullptr) {
        ROSEN_LOGW("RSSurfaceLinux::FlushFrame frame.addr is nullptr");
    } else {
        addr = frameLinux->addr_.get();
    }

    Drawing::BitmapFormat format = { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    Drawing::Bitmap bitmap;
    bitmap.Build(frameLinux->width_, frameLinux->height_, format);
    bitmap.SetPixels(addr);
    if (frameLinux->surface_ != nullptr) {
        auto image = frameLinux->surface_->GetImageSnapshot();
        if (image) {
            image->ReadPixels(bitmap, 0, 0);
        }
    }
#ifdef USE_GLFW_WINDOW
    if (frameLinux->surface_ != nullptr) {
        YInvert(addr, frameLinux->width_, frameLinux->height_);
    }
#endif

    int32_t width = frameLinux->width_;
    int32_t height = frameLinux->height_;
    int32_t size = width * height * 0x4;
    if (onRender_ != nullptr) {
        onRender_(addr, size, width, height, uiTimestamp);
    }

#ifdef USE_GLFW_WINDOW
    GlfwRenderContext::GetGlobal()->SwapBuffers();
#endif
    return true;
}

std::shared_ptr<RenderContext> RSSurfaceLinux::GetRenderContext()
{
    return renderContext_;
}

void RSSurfaceLinux::SetRenderContext(std::shared_ptr<RenderContext> context)
{
    renderContext_ = context;
}

void RSSurfaceLinux::YInvert(void *addr, int32_t width, int32_t height)
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

bool RSSurfaceLinux::SetupGrContext()
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

uint32_t RSSurfaceLinux::GetQueueSize() const
{
    return 0x3;
}

void RSSurfaceLinux::ClearBuffer()
{
}

void RSSurfaceLinux::ClearAllBuffer()
{
}

void RSSurfaceLinux::ResetBufferAge()
{
    ROSEN_LOGD("RSSurfaceLinux: Reset Buffer Age!");
}
} // namespace Rosen
} // namespace OHOS
