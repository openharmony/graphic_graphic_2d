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

#include "render_backend_utils.h"

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
    if (!RenderBackendUtils::IsValidFrame(frame)) {
        LOGE("Failed to create surface, frame is invalid");
        return false;
    }

    std::shared_ptr<SurfaceConfig> surfaceConfig = frame->surfaceConfig;
    if (surfaceConfig->producer == nullptr) {
        LOGE("Failed to create surface, producer is nullptr");
        return false;
    }
    if (surfaceConfig->nativeWindow == nullptr) {
        sptr<Surface> producer = surfaceConfig->producer;
        surfaceConfig->nativeWindow = CreateNativeWindowFromSurface(&producer);
        if (surfaceConfig->nativeWindow == nullptr) {
            LOGE("Failed to create surface, native window is nullptr");
            return false;
        }
    }

    std::shared_ptr<FrameConfig> frameConfig = frame->frameConfig;
    NativeWindowHandleOpt(surfaceConfig->nativeWindow, SET_FORMAT, frameConfig->pixelFormat);
#ifdef RS_ENABLE_AFBC
    if (RSSystemProperties::GetAFBCEnabled()) {
        int32_t format = 0;
        bool useAFBC = frameConfig->useAFBC;
        NativeWindowHandleOpt(surfaceConfig->nativeWindow, GET_FORMAT, &format);
        if (format == GRAPHIC_PIXEL_FMT_RGBA_8888 && useAFBC) {
            frameConfig->bufferUsage =
                (BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA);
        }
    }
#endif
    NativeWindowHandleOpt(surfaceConfig->nativeWindow, SET_USAGE, frameConfig->bufferUsage);
    NativeWindowHandleOpt(surfaceConfig->nativeWindow, SET_BUFFER_GEOMETRY, frameConfig->width, frameConfig->height);
    NativeWindowHandleOpt(surfaceConfig->nativeWindow, SET_COLOR_GAMUT, frameConfig->colorSpace);
    NativeWindowHandleOpt(surfaceConfig->nativeWindow, SET_UI_TIMESTAMP, frameConfig->uiTimestamp);

    VulkanState* vulkanState = frame->vulkanState;
    if (vulkanState == nullptr) {
        LOGE("Failed to acquire surface in vulkan, vulkanState is nullptr");
        return nullptr;
    }
    if (vulkanState->vulkanWindow == nullptr) {
        auto vulkanSurface = std::make_unique<vulkan::VulkanNativeSurfaceOHOS>(surfaceConfig->nativeWindow);
        vulkanState->vulkanWindow = std::make_unique<vulkan::VulkanWindow>(std::move(vulkanSurface));
    }
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
        LOGD("SwapBuffers in vulkan successfully");
    } else {
        LOGE("Failed to swap buffers, vulkanWindow_ is nullptr");
    }
}
}
}