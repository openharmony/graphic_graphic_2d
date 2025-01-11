/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawing_surface.h"

#include <mutex>
#include <unordered_map>

#include "drawing_canvas_utils.h"
#ifdef RS_ENABLE_GPU
#include "drawing_surface_utils.h"
#endif

#include "draw/surface.h"
#include "image/gpu_context.h"
#include "image/image_info.h"
#include "utils/log.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Drawing::Surface* CastToSurface(OH_Drawing_Surface* cSurface)
{
    return reinterpret_cast<Drawing::Surface*>(cSurface);
}
#ifdef RS_ENABLE_GPU
static GPUContext* CastToGpuContext(OH_Drawing_GpuContext* cGpuContext)
{
    return reinterpret_cast<GPUContext*>(cGpuContext);
}
#endif
OH_Drawing_Surface* OH_Drawing_SurfaceCreateFromGpuContext(OH_Drawing_GpuContext* cGpuContext,
    bool budgeted, OH_Drawing_Image_Info cImageInfo)
{
#ifdef RS_ENABLE_GPU
    if (cGpuContext == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    ImageInfo imageInfo(cImageInfo.width, cImageInfo.height,
        static_cast<ColorType>(cImageInfo.colorType), static_cast<AlphaType>(cImageInfo.alphaType));
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRenderTarget(CastToGpuContext(cGpuContext),
        budgeted, imageInfo);
    if (surface == nullptr) {
        return nullptr;
    }
    DrawingSurfaceUtils::InsertSurface(surface, CastToGpuContext(cGpuContext));
    return (OH_Drawing_Surface*)(surface.get());
#else
    return nullptr;
#endif
}

OH_Drawing_Surface* OH_Drawing_SurfaceCreateOnScreen(
    OH_Drawing_GpuContext* cGpuContext, OH_Drawing_Image_Info cImageInfo, void* window)
{
#ifdef RS_ENABLE_GPU
    if (cGpuContext == nullptr || window == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }

    ImageInfo imageInfo(cImageInfo.width, cImageInfo.height,
        static_cast<ColorType>(cImageInfo.colorType), static_cast<AlphaType>(cImageInfo.alphaType));
    std::shared_ptr<Drawing::Surface> surface = DrawingSurfaceUtils::CreateFromWindow(
        CastToGpuContext(cGpuContext), imageInfo, window);
    if (!surface) {
        LOGE("OH_Drawing_SurfaceCreateOnScreen: create surface failed.");
        return nullptr;
    }
    return (OH_Drawing_Surface*)(surface.get());
#else
    return nullptr;
#endif
}

OH_Drawing_Canvas* OH_Drawing_SurfaceGetCanvas(OH_Drawing_Surface* cSurface)
{
    Drawing::Surface* surface = CastToSurface(cSurface);
    if (surface == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    return (OH_Drawing_Canvas*)(surface->GetCanvas().get());
}

OH_Drawing_ErrorCode OH_Drawing_SurfaceFlush(OH_Drawing_Surface* cSurface)
{
#ifdef RS_ENABLE_GPU
    if (cSurface == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }

    DrawingSurfaceUtils::FlushSurface(CastToSurface(cSurface));
    return OH_DRAWING_SUCCESS;
#else
    return OH_DRAWING_ERROR_INVALID_PARAMETER;
#endif
}

void OH_Drawing_SurfaceDestroy(OH_Drawing_Surface* cSurface)
{
    if (cSurface == nullptr) {
        return;
    }
    
    DrawingSurfaceUtils::RemoveSurface(CastToSurface(cSurface));
}
