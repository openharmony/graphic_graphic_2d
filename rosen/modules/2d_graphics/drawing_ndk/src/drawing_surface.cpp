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
#include "drawing_canvas.h"
#include "draw/surface.h"
#include <mutex>
#include <unordered_map>
#include "image/gpu_context.h"
#include "image/image_info.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static std::mutex g_surfaceMutex;
static std::unordered_map<void*, std::shared_ptr<Surface>> g_surfaceMap;

static Surface* CastToSurface(OH_Drawing_Surface* cSurface)
{
    return reinterpret_cast<Surface*>(cSurface);
}

static GPUContext* CastToGpuContext(OH_Drawing_GpuContext* cGpuContext)
{
    return reinterpret_cast<GPUContext*>(cGpuContext);
}

OH_Drawing_Surface* OH_Drawing_SurfaceCreateFromGpuContext(OH_Drawing_GpuContext* cGpuContext,
    bool budgeted, OH_Drawing_Image_Info cImageInfo)
{
    if (cGpuContext == nullptr) {
        return nullptr;
    }
    ImageInfo imageInfo(cImageInfo.width, cImageInfo.height,
        static_cast<ColorType>(cImageInfo.colorType), static_cast<AlphaType>(cImageInfo.alphaType));
    std::shared_ptr<Surface> surface = Surface::MakeRenderTarget(CastToGpuContext(cGpuContext),
        budgeted, imageInfo);
    if (surface == nullptr) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_surfaceMutex);
    g_surfaceMap.insert({surface.get(), surface});
    return (OH_Drawing_Surface*)(surface.get());
}

OH_Drawing_Canvas* OH_Drawing_SurfaceGetCanvas(OH_Drawing_Surface* cSurface)
{
    Surface* surface = CastToSurface(cSurface);
    if (surface == nullptr) {
        return nullptr;
    }
    return (OH_Drawing_Canvas*)(surface->GetCanvas().get());
}

void OH_Drawing_SurfaceDestroy(OH_Drawing_Surface* cSurface)
{
    if (cSurface == nullptr) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_surfaceMutex);
    auto it = g_surfaceMap.find(cSurface);
    if (it == g_surfaceMap.end()) {
        return;
    }
    g_surfaceMap.erase(it);
}
