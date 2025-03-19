/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "utils.h"
#include "foundation/graphic/graphic_2d/rosen/modules/2d_graphics/include/draw/surface.h"
#include <memory>

namespace OHOS {
namespace Rosen {
namespace Drawing {

static const int IMAGE_SIZE = 200;

Bitmap BuildRandomBitmap()
{
    int seed = 2333;
    srand(seed);
    Bitmap bitmap;
    bitmap.TryAllocPixels(ImageInfo::MakeN32Premul(IMAGE_SIZE, IMAGE_SIZE));
    uint32_t* pixel = static_cast<uint32_t*>(bitmap.GetPixels());
    
    int mask = 255;
    for (int i = 0; i < IMAGE_SIZE; i++) {
        int r = rand() & mask;
        int g = rand() & mask;
        int b = rand() & mask;
        int a = mask;
        int x = r; // r: 0~7bit
        x |= g << 8; // g: 8~15bit
        x |= b << 16; // b: 16~23bit
        x |= a << 24; // a: 24~31bit
        for (int j = 0; j < IMAGE_SIZE; ++j) {
            pixel[i * IMAGE_SIZE + j] = x;
        }
    }
    return bitmap;
}

std::shared_ptr<Image> MakeImageRaster()
{
    std::shared_ptr<Image> image = std::make_shared<Image>();
    image->BuildFromBitmap(BuildRandomBitmap());
    return image;
}

std::shared_ptr<Image> MakeImageEncoded()
{
    std::shared_ptr<Image> image = std::make_shared<Image>();
    image->MakeFromEncoded(Data::MakeFromFileName("panda.png"));
    return image;
}

std::shared_ptr<Image> MakeImageGpu(std::shared_ptr<GPUContext> gpuContext)
{
    auto surface = Surface::MakeRenderTarget(gpuContext.get(), true, ImageInfo::MakeN32Premul(200, 200)); // 200 * 200
    Image image;
    image.BuildFromBitmap(*gpuContext, BuildRandomBitmap());
    auto canvas = surface->GetCanvas();
    canvas->DrawImageRect(image, Rect(0, 0, 200, 200), SamplingOptions()); // 200 * 200
    surface->FlushAndSubmit();
    auto colorspace = ColorSpace::CreateSRGB();
    std::shared_ptr<Image> dst = std::make_shared<Image>();
    dst->BuildFromSurface(*gpuContext, *surface, TextureOrigin::TOP_LEFT,
                          { ColorType::COLORTYPE_RGBA_8888, AlphaType::ALPHATYPE_PREMUL }, colorspace);
    return dst;
}

std::shared_ptr<Image> MakeImageYUVA(std::shared_ptr<GPUContext> gpuContext)
{
    Bitmap bitmap = BuildRandomBitmap();
    YUVInfo yuvInfo = {
        200,
        200,
        Drawing::YUVInfo::PlaneConfig::Y_UV,
        Drawing::YUVInfo::SubSampling::K420,
        Drawing::YUVInfo::YUVColorSpace::JPEG_FULL_YUVCOLORSPACE,
        Drawing::YUVInfo::YUVDataType::UNORM_8,
    };
    auto image = Image::MakeFromYUVAPixmaps(*gpuContext, yuvInfo, bitmap.GetPixels());
    return image;
}

}
}
}