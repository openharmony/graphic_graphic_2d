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

#include "vulkan_render_backend.h"
#include <iostream>
#include "drawing_utils.h"
#include "skia_adapter/skia_surface.h"
#include "surface_ohos_vulkan.h"

namespace OHOS {
namespace Rosen {

void VulkanRenderBackend::Destroy()
{
    grContext_ = nullptr;
    skSurface_ = nullptr;
}

void VulkanRenderBackend::RenderFrame()
{
    if (drSurface_ == nullptr) {
        LOGE("drSurface is nullptr, can not RenderFrame");
        return;
    }
    skSurface_ = drSurface_->GetImpl<Drawing::SkiaSurface>()->GetSkSurface().get();
    if (skSurface_ == nullptr) {
        std::cout << "skSurface is nullptr, can not RenderFrame" << std::endl;
        return;
    }
    // flush commands
    if (skSurface_->getCanvas() != nullptr) {
        skSurface_->getCanvas()->flush();
        std::cout << "VulkanRenderBackend skia RenderFrame flushing end" << std::endl;
    } else {
        std::cout << "skia canvas is nullptr!!!" << std::endl;
    }
}

SkCanvas* VulkanRenderBackend::AcquireSkCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    auto vulkan_frame = reinterpret_cast<SurfaceFrameOhosVulkan*>(frame.get());
    drSurface_ = vulkan_frame->GetSurface();
    skSurface_ = drSurface_->GetImpl<Drawing::SkiaSurface>()->GetSkSurface().get();
    return skSurface_->getCanvas();
}

Drawing::Canvas* VulkanRenderBackend::AcquireDrCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    auto vulkan_frame = reinterpret_cast<SurfaceFrameOhosVulkan*>(frame.get());
    drSurface_ = vulkan_frame->GetSurface();
    return vulkan_frame->GetCanvas();
}
}
}