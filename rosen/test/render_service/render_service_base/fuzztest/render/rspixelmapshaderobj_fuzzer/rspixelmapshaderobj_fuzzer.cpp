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

#include "rspixelmapshaderobj_fuzzer.h"

#include <securec.h>
#include <cstddef>
#include <cstdint>

#include "draw/surface.h"
#include "effect/shader_effect_lazy.h"
#include "message_parcel.h"
#include "render/rs_pixel_map_shader_obj.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
constexpr int32_t TILE_MODE_MAX = 4;
constexpr int32_t PIXEL_FORMAT_MAX = 14;
constexpr int32_t ALPHA_TYPE_MAX = 4;
constexpr int32_t SCALE_MODE_MAX = 2;
constexpr int32_t COLOR_TYPE_MAX = 10;
constexpr int32_t FILTER_MODE_MAX = 2;
constexpr int32_t MIPMAP_MODE_MAX = 3;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

std::shared_ptr<Media::PixelMap> GetPixelMapFromData()
{
    int32_t width = GetData<uint8_t>();
    int32_t height = GetData<uint8_t>();
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>() % PIXEL_FORMAT_MAX);
    opts.pixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>() % PIXEL_FORMAT_MAX);
    opts.alphaType = static_cast<Media::AlphaType>(GetData<int32_t>() % ALPHA_TYPE_MAX);
    opts.scaleMode = static_cast<Media::ScaleMode>(GetData<int32_t>() % SCALE_MODE_MAX);
    opts.editable = GetData<bool>();
    opts.useSourceIfMatch = GetData<bool>();
    auto pixelmap = Media::PixelMap::Create(opts);
    if (pixelmap == nullptr) {
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        return nullptr;
    }
    Drawing::ColorType colorType = static_cast<Drawing::ColorType>(GetData<int32_t>() % COLOR_TYPE_MAX);
    Drawing::AlphaType alphaType = static_cast<Drawing::AlphaType>(GetData<int32_t>() % ALPHA_TYPE_MAX);
    Drawing::ImageInfo info = Drawing::ImageInfo(pixelmap->GetWidth(), pixelmap->GetHeight(), colorType, alphaType);
    auto surface = Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    if (surface == nullptr) {
        return nullptr;
    }
    auto canvas = surface->GetCanvas();
    if (canvas == nullptr) {
        return nullptr;
    }
    canvas->Clear(GetData<uint32_t>());
    Drawing::Brush brush;
    brush.SetColor(GetData<uint32_t>());
    canvas->AttachBrush(brush);
    canvas->DrawRect(Drawing::Rect(0, 0, width, height));
    canvas->DetachBrush();
    return pixelmap;
}

bool DoCreatePixelMapShader(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    DATA = data;
    g_size = size;
    g_pos = 0;

    constexpr int sampingTypeChoiceCount = 2;
    std::shared_ptr<Media::PixelMap> pixelMap = GetPixelMapFromData();
    Drawing::TileMode tileX = static_cast<Drawing::TileMode>(GetData<int>() % TILE_MODE_MAX);
    Drawing::TileMode tileY = static_cast<Drawing::TileMode>(GetData<int>() % TILE_MODE_MAX);
    Drawing::SamplingOptions sampling;
    if (GetData<int>() % sampingTypeChoiceCount) {
        sampling = Drawing::SamplingOptions({GetData<float>(), GetData<float>()});
    } else {
        Drawing::FilterMode filterMode = static_cast<Drawing::FilterMode>(GetData<int>() % FILTER_MODE_MAX);
        Drawing::MipmapMode mipmapMode = static_cast<Drawing::MipmapMode>(GetData<int>() % MIPMAP_MODE_MAX);
        sampling = Drawing::SamplingOptions(filterMode, mipmapMode);
    }
    Drawing::Matrix matrix;
    matrix.SetMatrix(GetData<float>(), GetData<float>(), GetData<float>(),
        GetData<float>(), GetData<float>(), GetData<float>(),
        GetData<float>(), GetData<float>(), GetData<float>());

    auto shaderEffectLazy = RSPixelMapShaderObj::CreatePixelMapShader(pixelMap, tileX, tileY, sampling, matrix);
    if (shaderEffectLazy == nullptr) {
        return true;
    }

    shaderEffectLazy->Materialize();
    return true;
}

bool DoCreateForUnmarshalling(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    auto shaderObj = RSPixelMapShaderObj::CreateForUnmarshalling();
    if (shaderObj == nullptr) {
        return false;
    }
    return true;
}

bool RSMainFuzzTest(const uint8_t* data, size_t size)
{
    DoCreatePixelMapShader(data, size);
    DoCreateForUnmarshalling(data, size);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::Rosen::RSMainFuzzTest(data, size);
    return 0;
}