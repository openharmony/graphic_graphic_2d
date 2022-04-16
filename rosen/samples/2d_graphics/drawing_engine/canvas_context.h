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

#ifndef CANVAS_CONTEXT_H
#define CANVAS_CONTEXT_H

#include "surface.h"
#include "interface_render_backend.h"

namespace OHOS {
namespace Rosen {
class CanvasContext {
public:
    CanvasContext() = delete;
    static CanvasContext* Create();
    virtual ~CanvasContext();
    void RenderFrame();
    SkCanvas* AcquireCanvas(std::unique_ptr<SurfaceFrame>& frame);
    void InitDrawContext();
    void MakeCurrent();
    void* CreateSurface(void* window);
    void SwapBuffers();
private:
    explicit CanvasContext(std::unique_ptr<IRenderBackend> renderBackend);
    std::unique_ptr<IRenderBackend> renderBackend_;
};
}
}
#endif