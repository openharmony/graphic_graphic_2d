/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "surface_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "image/gpu_context.h"
#include "skia_adapter/skia_gpu_context.h"
#include "utils/rect.h"
#include "draw/surface.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t SURFACE_WIDTH = 500;
constexpr int32_t SURFACE_HEIGHT = 500;
constexpr uint32_t BACKEND_ACCESS_TYPE_COUNT = 3;
} // namespace

namespace Drawing {
/*
 * 测试以下 Surface 接口：
 * 1. MakeSurface(int width, int height) const
 * 2. MakeSurface(const ImageInfo& imageInfo) const
 * 3. Flush(FlushInfo *drawingflushInfo)
 * 4. ImportDDGRSurface(std::shared_ptr<DDGR::Surface> surface) 确保调用MakeSurface时调用到
 * 5. SetGrContext(const std::shared_ptr<DDGR::GrContext>& grContext) 确保调用MakeSurface时调用到
 * 6. GetBackendTexture(BackendAccess access) const
 * 7. Wait(int32_t time, const VkSemaphore& semaphore)
 * 8. SetDrawingArea(const std::vector<RectI>& rects)
 */
bool SurfaceFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    auto surface = Surface::MakeRaster(Drawing::ImageInfo::MakeN32Premul(SURFACE_WIDTH, SURFACE_HEIGHT));

    int width = GetObject<int>();
    int height = GetObject<int>();
    surface->MakeSurface(width, height);
    Drawing::ImageInfo imageInfo = Drawing::ImageInfo { width, height, Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    surface->MakeSurface(imageInfo);

    bool backendSurfaceAccess = GetObject<bool>();
    size_t numSemaphores = GetObject<size_t>();
    FlushInfo flushInfo;
    flushInfo.backendSurfaceAccess = backendSurfaceAccess;
    flushInfo.numSemaphores = numSemaphores;
    surface->Flush(&flushInfo);

    // Simultaneously test both ImportDDGRSurface and SetGrContext
    surface->MakeSurface(SURFACE_WIDTH, SURFACE_HEIGHT);

    Surface emptySurface;
    BackendAccess backendAccess = static_cast<BackendAccess>(GetObject<uint32_t>() % BACKEND_ACCESS_TYPE_COUNT);
    emptySurface.GetBackendTexture(backendAccess);

#ifdef RS_ENABLE_VK
    int32_t time = GetObject<int32_t>();
    VkSemaphore semaphore { VK_NULL_HANDLE };
    emptySurface.Wait(time, semaphore);

    const std::vector<RectI> rects = {
        RectI(GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>()),
        RectI(GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>()),
        RectI(GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>())
    };
    emptySurface.SetDrawingArea(rects);
#endif
    return true;
}

/*
 * 测试以下 Surface 接口：
 * 1. GetImageSnapshot() const
 * 2. GetImageSnapshot(const RectI& bounds, bool allowRefCache) const
 */
bool SurfaceFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    auto surface1 = Surface::MakeRaster(Drawing::ImageInfo::MakeN32Premul(SURFACE_WIDTH, SURFACE_HEIGHT));
    auto surface2 = Surface::MakeRaster(Drawing::ImageInfo::MakeN32Premul(SURFACE_WIDTH, SURFACE_HEIGHT));

    surface1->GetImageSnapshot();

    const RectI bounds = RectI(GetObject<int32_t>(), GetObject<int32_t>(),
        GetObject<int32_t>(), GetObject<int32_t>());
    bool allowRefCache = GetObject<bool>();
    surface2->GetImageSnapshot(bounds, allowRefCache);
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // initialize
    OHOS::Rosen::Drawing::g_data = data;
    OHOS::Rosen::Drawing::g_size = size;
    OHOS::Rosen::Drawing::g_pos = 0;

    /* Run your code on data */
    OHOS::Rosen::Drawing::SurfaceFuzzTest001(data, size);
    OHOS::Rosen::Drawing::SurfaceFuzzTest002(data, size);
    return 0;
}
