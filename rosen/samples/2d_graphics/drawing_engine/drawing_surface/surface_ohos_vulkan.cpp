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

#include <vulakn_native_surface_ohos.h>
#include <vulkan_window.h>
#include <vulkan_proc_table.h>
#include <hilog/log.h>
#include <display_type.h>
#include "window.h"

namespace OHOS {
namespace Rosen {
SurfaceOhosVulkan::SurfaceOhosVulkan(const sptr<Surface>& producer)
    : SurfaceOhos(producer), frame_(nullptr)
{
}

SurfaceOhosVulkan::~SurfaceOhosVulkan()
{
    frame_ = nullptr;
    if (mNativeWindow_ != nullptr) {
        DestoryNativeWindow(mNativeWindow_);
    }
    if (mVulkanWindow_ != nullptr) {
        delete mVulkanWindow_;
    }
}

std::unique_ptr<SurfaceFrame> SurfaceOhosVulkan::RequestFrame(int32_t width, int32_t height)
{
    if (mNativeWindow_ == nullptr) {
        mNativeWindow_ = CreateNativeWindowBufferFromSurface(&producer_);
        if (mNativeWindow_ == nullptr) {
            LOGE("SurfaceOhosVulkan nativewindow is null");
            return nullptr;
        }
    }

    if (mVulkanWIndow_ == nullptr) {
        auto vulkan_surface_ohos = std::make_unique<vulkan::VulkanNativeSurfaceOHOS>(mNativeWindow_);
        mVulkanWIndow_ = new vulkan::VulkanWindow(std::move(vulkan_surface_ohos));
    }

    sk_sp<SkSurface> skSurface = mVulkanWIndow_->AcquireSurface();

    frame_ = std::make_unique<SurfaceFrameOhosVulkan>(skSurface, width, height);
    frame_->SetColorSpace(ColorGamut::COLOR_GAMUT_SRGB);

    NativeWindowHandleOpt(mNativeWindow_, SET_BUFFER_GEOMETRY, width, height);
    NativeWindowHandleOpt(mNativeWindow_, SET_COLOR_GAMUT, frame_->GetColorSpace());

    std::unique_ptr<SurfaceFrame> ret(std::move(frame_));
    return ret;
}

bool SurfaceOhosVulkan::FlushFrame(std::unique_ptr<SurfaceFrame>& frame)
{
    if (drawingProxy_ == nullptr) {
        LOGE("drawingProxy_ is nullptr, can not FlushFrame");
        return false;
    }
    // gpu render flush
    drawingProxy_->RenderFrame();
    if (mVulkanWIndow_ != nullptr) {
        drawingProxy_->SwapBuffers();
    } else {
        LOGE("mVulkanWIndow_ is null");
    }
    return true;
}

SkCanvas* SurfaceOhosVulkan::GetCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    if (drawingProxy_ == nullptr) {
        LOGE("drawingProxy_ is nullptr, can not GetCanvas");
        return nullptr;
    }
    return drawingProxy_->AcquireCanvas(frame);
}
} // namespace Rosen
} // namespace OHOS
