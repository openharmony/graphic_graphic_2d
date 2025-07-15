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

#include "drawsurface_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "draw/surface.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t ACCESS_SIZE = 3;
constexpr size_t ALPHATYPE_SIZE = 4;
constexpr size_t COLORTYPE_SIZE = 10;
constexpr size_t MAX_SIZE = 5000;
constexpr size_t ORIGION_SIZE = 2;
} // namespace
namespace Drawing {
/*
 * 测试以下 Surface 接口：
 * 1. Bind(...)
 * 2. MakeFromBackendRenderTarget(...)
 * 3. MakeFromBackendTexture(...)
 */
bool SurfaceFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Surface surface;
    Bitmap bitmap;
    int width = GetObject<int>() % MAX_SIZE;
    int height = GetObject<int>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    surface.Bind(bitmap);
    Image image;
    bool buildImage = image.BuildFromBitmap(bitmap);
    if (!buildImage) {
        return false;
    }
    surface.Bind(image);
    FrameBuffer frameBuffer;
    surface.Bind(frameBuffer);
    GPUContext gpuContext;
    TextureInfo info;
    uint32_t origin = GetObject<uint32_t>();
    uint32_t colorType = GetObject<uint32_t>();
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
#ifdef RS_ENABLE_VK
    surface.MakeFromBackendRenderTarget(&gpuContext, info, static_cast<TextureOrigin>(origin % ORIGION_SIZE),
        static_cast<ColorType>(colorType % COLORTYPE_SIZE), colorSpace, nullptr, nullptr);
#endif
    int sampleCnt = GetObject<int>();
    surface.MakeFromBackendTexture(&gpuContext, info, static_cast<TextureOrigin>(origin % ORIGION_SIZE), sampleCnt,
        static_cast<ColorType>(colorType % COLORTYPE_SIZE), colorSpace, nullptr, nullptr);
    return true;
}

/*
 * 测试以下 Surface 接口：
 * 1. MakeRenderTarget(...)
 * 2. MakeRaster(...)
 * 3. MakeRasterN32Premul(...)
 * 4. GetCanvas()
 * 5. GetImageSnapshot()
 * 6. GetImageSnapshot(RectI)
 * 7. GetImageInfo()
 * 8. GetBackendTexture(...)
 * 9. FlushAndSubmit(...)
 * 10. Flush(...)
 * 11. Width()
 * 12. Height()
 * 13. GetHeadroom()
 * 14. SetHeadroom(float)
 */
bool SurfaceFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Surface surface;
    GPUContext gpuContext;
    bool budgeted = GetObject<bool>();
    int width = GetObject<int>() % MAX_SIZE;
    int height = GetObject<int>() % MAX_SIZE;
    uint32_t colorType = GetObject<uint32_t>();
    uint32_t alphaType = GetObject<uint32_t>();
    ImageInfo imageInfo = ImageInfo(width, height, static_cast<ColorType>(colorType % COLORTYPE_SIZE),
        static_cast<AlphaType>(alphaType % ALPHATYPE_SIZE));
    surface.MakeRenderTarget(&gpuContext, budgeted, imageInfo);
    surface.MakeRaster(imageInfo);
    int32_t widthT = GetObject<int32_t>() % MAX_SIZE;
    int32_t heightT = GetObject<int32_t>() % MAX_SIZE;
    surface.MakeRasterN32Premul(widthT, heightT);
    surface.GetCanvas();
    surface.GetImageSnapshot();
    int32_t left = GetObject<int32_t>();
    int32_t top = GetObject<int32_t>();
    int32_t right = GetObject<int32_t>();
    int32_t bottom = GetObject<int32_t>();
    RectI bounds { left, top, right, bottom };
    surface.GetImageSnapshot(bounds);
    int surWidth = GetObject<int>() % MAX_SIZE;
    int surHeight = GetObject<int>() % MAX_SIZE;
    surface.MakeRasterN32Premul(surWidth, surHeight);
    surface.GetImageInfo();
    uint32_t access = GetObject<uint32_t>();
    surface.GetBackendTexture(static_cast<BackendAccess>(access % ACCESS_SIZE));
    bool syncCpu = GetObject<bool>();
    surface.FlushAndSubmit(syncCpu);
    FlushInfo drawingflushInfo;
    surface.Flush(&drawingflushInfo);
    surface.Width();
    surface.Height();
    float headroom = GetObject<float>();
    surface.SetHeadroom(headroom);
    surface.GetHeadroom();
    return true;
}

/*
 * 测试以下 Surface 接口：
 * 1. Surface()
 * 2. MakeRasterDirect(...)
 */
bool SurfaceFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Surface surface;
    int width = GetObject<int>() % MAX_SIZE;
    int height = GetObject<int>() % MAX_SIZE;
    uint32_t colorType = GetObject<uint32_t>();
    uint32_t alphaType = GetObject<uint32_t>();
    ImageInfo imageInfo = ImageInfo(width, height, static_cast<ColorType>(colorType % COLORTYPE_SIZE),
        static_cast<AlphaType>(alphaType % ALPHATYPE_SIZE));
    int32_t widthT = GetObject<int32_t>() % MAX_SIZE;
    int32_t heightT = GetObject<int32_t>() % MAX_SIZE;
    uint32_t size_pix = (widthT * heightT) % MAX_SIZE;
    uint32_t* pixels = new uint32_t[size_pix];
    if (pixels == nullptr) {
        return false;
    }
    for (size_t i = 0; i < size_pix; i++) {
        pixels[i] = GetObject<uint32_t>();
    }
    size_t rowBytes = GetObject<size_t>();
    std::shared_ptr<Surface> surfaceOne = Surface::MakeRasterDirect(imageInfo, pixels, rowBytes);
    if (pixels != nullptr) {
        delete[]  pixels;
        pixels = nullptr;
    }
    return true;
}

/*
 * 测试以下 Surface 接口：
 * 1. Surface()
 * 2. Wait(...)
 */
bool SurfaceFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

#ifdef RS_ENABLE_GL
    Surface surface;
    std::vector<GrGLsync> syncs;
    surface.Wait(syncs);
#endif
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::SurfaceFuzzTest001(data, size);
    OHOS::Rosen::Drawing::SurfaceFuzzTest002(data, size);
    OHOS::Rosen::Drawing::SurfaceFuzzTest003(data, size);
    OHOS::Rosen::Drawing::SurfaceFuzzTest004(data, size);
    return 0;
}