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

#ifndef IRENDER_BACKEND_H
#define IRENDER_BACKEND_H

#include <surface.h>
#include "surface_base.h"

class SkCanvas;

namespace OHOS {
namespace Rosen {
class IRenderBackend {
public:
    IRenderBackend() {}
    virtual ~IRenderBackend() {}
    virtual void InitDrawContext() = 0;
    virtual void MakeCurrent() = 0;
    virtual void SwapBuffers() = 0;
    virtual void* CreateSurface(void* window) = 0;
    virtual void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) = 0;
    virtual void Destroy() = 0;
    virtual void RenderFrame() = 0;
    virtual SkCanvas* AcquireSkCanvas(std::unique_ptr<SurfaceFrame>& frame) = 0;
    virtual Drawing::Canvas* AcquireDrCanvas(std::unique_ptr<SurfaceFrame>& frame) = 0;
};
}
}
#endif