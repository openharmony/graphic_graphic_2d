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

#include "drawing_proxy.h"

#include "drawing_utils.h"

namespace OHOS {
namespace Rosen {
DrawingProxy::DrawingProxy()
{
    canvasContext_ = CanvasContext::Create();
}

DrawingProxy::~DrawingProxy()
{
    if (canvasContext_) {
        delete canvasContext_;
        canvasContext_ = nullptr;
    }
}

void DrawingProxy::InitDrawContext()
{
    canvasContext_->InitDrawContext();
}

SkCanvas* DrawingProxy::AcquireCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    return canvasContext_->AcquireCanvas(frame);
}

void* DrawingProxy::CreateSurface(void* window)
{
    return canvasContext_->CreateSurface(window);
}

void DrawingProxy::MakeCurrent()
{
    canvasContext_->MakeCurrent();
}

void DrawingProxy::SwapBuffers()
{
    canvasContext_->SwapBuffers();
}

void DrawingProxy::RenderFrame()
{
    canvasContext_->RenderFrame();
}

void DrawingProxy::Destroy()
{
    if (canvasContext_) {
        delete canvasContext_;
        canvasContext_ = nullptr;
    }
}
}
}