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

#ifndef DRAWING_PROXY_H
#define DRAWING_PROXY_H
#include "canvas_context.h"
#include "draw/canvas.h"

namespace OHOS {
namespace Rosen {
class DrawingProxy {
public:
    DrawingProxy();
    virtual ~DrawingProxy();
    SkCanvas* AcquireSkCanvas(std::unique_ptr<SurfaceFrame>& frame);
    Drawing::Canvas* AcquireDrCanvas(std::unique_ptr<SurfaceFrame>& frame);
    void* CreateSurface(void* window);
    void MakeCurrent();
    void InitDrawContext();
    void SwapBuffers();
    void Destroy();
    void RenderFrame();
private:
    CanvasContext* canvasContext_;
};
}
}
#endif