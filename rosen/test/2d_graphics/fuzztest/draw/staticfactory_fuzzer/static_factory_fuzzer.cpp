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

#include "static_factory_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "text/font_arguments.h"
#include "text/typeface.h"
#include "image/gpu_context.h"
#include "impl_interface/gpu_context_impl.h"
#include "static_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
constexpr size_t MAX_SIZE = 1000;
} // namespace

bool StaticFactoryFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    std::shared_ptr<Typeface> typeface = Drawing::StaticFactory::DeserializeTypeface(data, size);
    GPUContext gpuContext;
#ifdef RS_ENABLE_VK
    skgpu::VulkanBackendContext grVkBackendContext {};
    gpuContext.BuildFromVK(grVkBackendContext);
#endif
    TextureInfo info;
    info.SetWidth(GetObject<int>() % MAX_SIZE);
    auto colorSpace = Drawing::ColorSpace::CreateSRGB();
#ifdef RS_ENABLE_VK
    std::shared_ptr<Surface> surface1 = Drawing::StaticFactory::MakeFromBackendRenderTarget(&gpuContext, info,
        GetObject<TextureOrigin>(), GetObject<ColorType>(), colorSpace, nullptr, nullptr);
#endif
    std::shared_ptr<Surface> surface2 = Drawing::StaticFactory::MakeFromBackendTexture(&gpuContext, info,
        GetObject<TextureOrigin>(), GetObject<int>(), GetObject<ColorType>(), colorSpace, nullptr, nullptr);
    YUVInfo yuvInfo;
    std::shared_ptr<Image> image1 = Drawing::StaticFactory::MakeFromYUVAPixmaps(gpuContext, yuvInfo, nullptr);
    Drawing::ImageInfo imageInfo = { GetObject<int>() % MAX_SIZE, GetObject<int>() % MAX_SIZE,
        GetObject<ColorType>(), GetObject<AlphaType>() };
    std::shared_ptr<Surface> surface3 = Drawing::StaticFactory::MakeRaster(imageInfo);
    std::shared_ptr<Data> pixels = std::make_shared<Data>();
    pixels->BuildWithCopy(data, size);
    std::shared_ptr<Image> image2 = Drawing::StaticFactory::MakeRasterData(imageInfo, pixels, GetObject<size_t>());
    std::shared_ptr<Surface> surface4 =
        Drawing::StaticFactory::MakeRasterDirect(imageInfo, nullptr, GetObject<size_t>());
    std::shared_ptr<Surface> surface5 =
        Drawing::StaticFactory::MakeRasterN32Premul(GetObject<int32_t>() % MAX_SIZE, GetObject<int32_t>() % MAX_SIZE);
    std::shared_ptr<Surface> surface6 =
        Drawing::StaticFactory::MakeRenderTarget(&gpuContext, GetObject<bool>(), imageInfo);
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
    OHOS::Rosen::Drawing::StaticFactoryFuzzTest001(data, size);
    return 0;
}
