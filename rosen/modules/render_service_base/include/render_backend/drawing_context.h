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

#ifndef DRAWING_CONTEXT_H
#define DRAWING_CONTEXT_H

#if defined(NEW_SKIA)
#include <include/gpu/GrDirectContext.h>
#else
#include <include/gpu/GrContext.h>
#endif

#include "rs_render_surface_frame.h"

namespace OHOS {
namespace Rosen {
class DrawingContext {
public:
    explicit DrawingContext() = default;
    ~DrawingContext() = default;
    sk_sp<SkSurface> AcquireSurface(const std::shared_ptr<RSRenderSurfaceFrame>& frame, const RenderType& renderType);
    bool SetUpDrawingContext();
#if defined(NEW_SKIA)
    GrDirectContext* GetDrawingContext() const;
#else
    GrContext* GetDrawingContext() const;
#endif
private:
    sk_sp<SkSurface> AcquireSurfaceInGLES(const std::shared_ptr<RSRenderSurfaceFrame>& frame);
    sk_sp<SkSurface> AcquireSurfaceInRaster(const std::shared_ptr<RSRenderSurfaceFrame>& frame);
    sk_sp<SkSurface> AcquireSurfaceInVulkan(const std::shared_ptr<RSRenderSurfaceFrame>& frame);
    sk_sp<SkColorSpace> GetSkColorSpace(const std::shared_ptr<RSRenderSurfaceFrame>& frame);
#if defined(NEW_SKIA)
    sk_sp<GrDirectContext> grContext_ = nullptr;
#else
    sk_sp<GrContext> grContext_ = nullptr;
#endif
};
}
}
#endif