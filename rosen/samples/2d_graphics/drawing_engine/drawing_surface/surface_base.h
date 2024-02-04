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

#ifndef SURFACE_BASE_H
#define SURFACE_BASE_H

#include <memory>

#include "surface_frame.h"
#include "surface_type.h"
#include "draw/canvas.h"

class SkCanvas;

namespace OHOS {
namespace Rosen {
class DrawingProxy;
class SurfaceBase {
public:
    SurfaceBase() = default;

    virtual ~SurfaceBase() = default;

    virtual bool IsValid() const = 0;

    virtual std::unique_ptr<SurfaceFrame> RequestFrame(int32_t width, int32_t height) = 0;

    virtual std::unique_ptr<SurfaceFrame> NativeRequestFrame(int32_t width, int32_t height) = 0;

    virtual bool FlushFrame(std::unique_ptr<SurfaceFrame>& frame) = 0;

    virtual bool NativeFlushFrame(std::unique_ptr<SurfaceFrame>& frame) = 0;
#ifdef USE_ROSEN_DRAWING
    virtual Drawing::Canvas* GetCanvas(std::unique_ptr<SurfaceFrame>& frame) = 0;
#endif
    virtual SkCanvas* GetSkCanvas(std::unique_ptr<SurfaceFrame>& frame) = 0;

    void SetDrawingProxy(DrawingProxy* proxy)
    {
        drawingProxy_ = proxy;
    }
protected:
    DrawingProxy* drawingProxy_;
};
} // namespace Rosen
} // namespace OHOS

#endif
