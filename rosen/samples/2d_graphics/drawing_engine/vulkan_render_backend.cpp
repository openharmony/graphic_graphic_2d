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

#include "surface_ohos_vulkan.h"
#include "vulkan_render_backend.h"

#include "drawing_utils.h"
#include "iostream"

namespace OHOS {
namespace Rosen {
VulkanRenderBackend::VulkanRenderBackend() noexcept
{
}

VulkanRenderBackend::~VulkanRenderBackend()
{
}

void VulkanRenderBackend::InitDrawContext()
{
}

bool VulkanRenderBackend::SetUpGrContext()
{
    return true;
}

void VulkanRenderBackend::MakeCurrent()
{
}

void VulkanRenderBackend::SwapBuffers()
{
}

void* VulkanRenderBackend::CreateSurface(void* window)
{
    return nullptr;
}

void VulkanRenderBackend::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
}

void VulkanRenderBackend::Destroy()
{
    grContext_ = nullptr;
    skSurface_ = nullptr;
}

void VulkanRenderBackend::RenderFrame()
{
    if (skSurface == nullptr) {
        LOGE("skSurface is nullptr, can not RenderFrame");
        return;
    }
    // flush commands
    if (skSurface_->getCanvas() != nullptr) {
        LOGD("RenderFrame: Canvas");
        skSurface_->getCanvas()->flush();
    } else {
        LOGW("canvas is nullptr!!!");
    }
}

SkCanvas* VulkanRenderBackend::AcquireCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    auto vulkan_frame = reinterpret_cast<SurfaceFrameOhosVulkan*>(frame.get());
    skSurface_ = vulkan_frame->GetSurface();
    return skSurface_->getCanvas();
}
}
}