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

#include "ohos/render_context_ohos_vk.h"

#include "rs_trace.h"
#include "utils/log.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
RenderContextOhosVk::~RenderContextOhosVk()
{
    vulkanWindow_ = nullptr;
}

void RenderContextOhosVk::Init()
{
    vulkan::VulkanWindow::InitializeVulkan();
}

bool RenderContextOhosVk::IsContextReady()
{
    return vulkanWindow_ != nullptr;
}

bool RenderContextOhosVk::CreateSurface(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    if (frame == nullptr) {
        LOGE("Failed to create surface, frame is nullptr");
        return false;
    }
    if (frame->nativeWindow_ == nullptr) {
        frame->nativeWindow_ = CreateNativeWindowFromSurface(&(frame->producer_));
        if (frame->nativeWindow_ == nullptr) {
            LOGE("Failed to create surface, native window is nullptr");
            return false;
        }
    }
    NativeWindowHandleOpt(frame->nativeWindow_, SET_FORMAT, frame->pixelFormat_);
#ifdef RS_ENABLE_AFBC
    if (RSSystemProperties::GetAFBCEnabled()) {
        int32_t format = 0;
        NativeWindowHandleOpt(frame->nativeWindow_, GET_FORMAT, &format);
        if (format == PIXEL_FMT_RGBA_8888 && useAFBC) {
            bufferUsage_ =
                (BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA);
        }
    }
#endif
    NativeWindowHandleOpt(frame->nativeWindow_, SET_USAGE, frame->bufferUsage_);
    NativeWindowHandleOpt(frame->nativeWindow_, SET_BUFFER_GEOMETRY, frame->width, frame->height);
    NativeWindowHandleOpt(frame->nativeWindow_, SET_COLOR_GAMUT, frame->colorSpace_);
    NativeWindowHandleOpt(frame->nativeWindow_, SET_UI_TIMESTAMP, frame->uiTimestamp_);

    if (vulkanWindow_ == nullptr) {
        auto vulkanSurface = std::make_unique<vulkan::VulkanNativeSurfaceOHOS>(nativeWindow_);
        vulkanWindow_ = std::make_unique<vulkan::VulkanWindow>(std::move(vulkanSurface));
    }
    frame->vulkanWindow_ = vulkanWindow_;
    return true;
}

void RenderContextOhosVk::DamageFrame(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    LOGI("Failed to damage frame, not support now");
}

int32_t RenderContextOhosVk::GetBufferAge()
{
    LOGI("Failed to get buffer age, not support now");
    return -1;
}

void RenderContextOhosVk::SwapBuffers(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    if (IsContextReady()) {
        vulkanWindow_->SwapBuffers();
    } else {
        LOGE("Failed to swap buffers, vulkanWindow_ is nullptr");
    }
}
}
}