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

#include "rspixelmapshader_fuzzer.h"

#include <securec.h>
#include <cstddef>
#include <cstdint>

#include "draw/surface.h"
#include "render/rs_pixel_map_shader.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

/*
 * get a pixelmap from g_data
 */
std::shared_ptr<Media::PixelMap> GetPixelMapFromData()
{
    int32_t width = GetData<uint8_t>();
    int32_t height = GetData<uint8_t>();
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>() % 14); // Max Media::PixelFormat is 14
    opts.pixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>() % 14);    // Max Media::PixelFormat is 14
    opts.alphaType = static_cast<Media::AlphaType>(GetData<int32_t>() % 4);         // Max Media::AlphaType is 4
    opts.scaleMode = static_cast<Media::ScaleMode>(GetData<int32_t>() % 2);         // Max Media::ScaleMode is 2
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
    Drawing::ColorType colorType =
        static_cast<Drawing::ColorType>(GetData<int32_t>() % 10); // Max Drawing::ColorType is 10
    Drawing::AlphaType alphaType =
        static_cast<Drawing::AlphaType>(GetData<int32_t>() % 4);  // Max Drawing::AlphaType is 4
    Drawing::ImageInfo info =
        Drawing::ImageInfo(pixelmap->GetWidth(), pixelmap->GetHeight(), colorType, alphaType);
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

bool RSMainFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // getdata
    std::shared_ptr<Media::PixelMap> pixelMap = GetPixelMapFromData();
    Drawing::TileMode tileX = static_cast<Drawing::TileMode>(GetData<int>() % 4);   // Max Drawing::TileMode is 4
    Drawing::TileMode tileY = static_cast<Drawing::TileMode>(GetData<int>() % 4);   // Max Drawing::TileMode is 4
    Drawing::SamplingOptions sampling;
    if (GetData<int>() % 2) {   // use cubic sampling
        sampling = Drawing::SamplingOptions({GetData<float>(), GetData<float>()});
    } else {
        Drawing::FilterMode filterMode =
            static_cast<Drawing::FilterMode>(GetData<int>() % 2);  // Max Drawing::FilterMode is 2
        Drawing::MipmapMode mipmapMode =
            static_cast<Drawing::MipmapMode>(GetData<int>() % 3);  // Max Drawing::MipmapMode is 3
        sampling = Drawing::SamplingOptions(filterMode, mipmapMode);
    }
    Drawing::Matrix matrix;
    matrix.SetMatrix(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>(),
        GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());

    // Test
    std::shared_ptr<RSPixelMapShader> pixelMapShader1 =
        std::make_shared<RSPixelMapShader>(pixelMap, tileX, tileY, sampling, matrix);
    std::shared_ptr<RSPixelMapShader> pixelMapShader2 = std::make_shared<RSPixelMapShader>();
    Parcel parcel;
    pixelMapShader1->Marshalling(parcel);
    pixelMapShader2->Unmarshalling(parcel);
    std::shared_ptr<Drawing::ShaderEffect> shaderEffect;
    shaderEffect.reset(reinterpret_cast<Drawing::ShaderEffect*>(pixelMapShader2->GenerateBaseObject()));

    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSMainFuzzTest(data, size);
    return 0;
}
