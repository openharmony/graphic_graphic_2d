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

#include "canvas_context.h"

#include <iostream>

#include "drawing_utils.h"
#include "software_render_backend.h"
#include "gles_render_backend.h"
#include "vulkan_render_backend.h"

namespace OHOS {
namespace Rosen {
CanvasContext* CanvasContext::Create()
{
    auto type = Setting::GetRenderBackendType();
    switch (type) {
        case RenderBackendType::VULKAN:
#ifdef ACE_ENABLE_VK
            LOGI("CanvasContext::Create with vulkan backend");
            std::cout << "CanvasContext::Create with vulkan backend" << std::endl;
            return new CanvasContext(std::make_unique<VulkanRenderBackend>());
#endif
        case RenderBackendType::GLES:
#ifdef ACE_ENABLE_GL
            LOGI("CanvasContext::Create with gles backend");
            std::cout << "CanvasContext::Create with gles backend" << std::endl;
            return new CanvasContext(std::make_unique<GLESRenderBackend>());
#endif
        case RenderBackendType::SOFTWARE:
            LOGE("CanvasContext::Create with software backend");
            std::cout << "CanvasContext::Create with software backend" << std::endl;
            return new CanvasContext(std::make_unique<SoftwareRenderBackend>());
        default:
            return nullptr;
    }
}

CanvasContext::CanvasContext(std::unique_ptr<IRenderBackend> renderBackend)
    : renderBackend_(std::move(renderBackend))
{
}

CanvasContext::~CanvasContext()
{
    renderBackend_->Destroy();
}

void* CanvasContext::CreateSurface(void* window)
{
    return renderBackend_->CreateSurface(window);
}

void CanvasContext::RenderFrame()
{
    renderBackend_->RenderFrame();
}

SkCanvas* CanvasContext::AcquireCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    return renderBackend_->AcquireCanvas(frame);
}

void CanvasContext::InitDrawContext()
{
    renderBackend_->InitDrawContext();
}

void CanvasContext::MakeCurrent()
{
    renderBackend_->MakeCurrent();
}

void CanvasContext::SwapBuffers()
{
    renderBackend_->SwapBuffers();
}
}
}