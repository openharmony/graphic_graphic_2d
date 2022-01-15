/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "skia_impl_factory.h"

#include "skia_adapter/skia_bitmap.h"
#include "skia_adapter/skia_camera.h"
#include "skia_adapter/skia_canvas.h"
#include "skia_adapter/skia_color_filter.h"
#include "skia_adapter/skia_color_space.h"
#include "skia_adapter/skia_image.h"
#include "skia_adapter/skia_image_filter.h"
#include "skia_adapter/skia_mask_filter.h"
#include "skia_adapter/skia_matrix.h"
#include "skia_adapter/skia_path.h"
#include "skia_adapter/skia_path_effect.h"
#include "skia_adapter/skia_shader_effect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::unique_ptr<CoreCanvasImpl> SkiaImplFactory::CreateCoreCanvas()
{
    return std::make_unique<SkiaCanvas>();
}

std::unique_ptr<BitmapImpl> SkiaImplFactory::CreateBitmap()
{
    return std::make_unique<SkiaBitmap>();
}

std::unique_ptr<ImageImpl> SkiaImplFactory::CreateImage()
{
    return std::make_unique<SkiaImage>();
}

std::unique_ptr<PathImpl> SkiaImplFactory::CreatePath()
{
    return std::make_unique<SkiaPath>();
}

std::unique_ptr<ColorFilterImpl> SkiaImplFactory::CreateColorFilter()
{
    return std::make_unique<SkiaColorFilter>();
}

std::unique_ptr<MaskFilterImpl> SkiaImplFactory::CreateMaskFilter()
{
    return std::make_unique<SkiaMaskFilter>();
}

std::unique_ptr<ImageFilterImpl> SkiaImplFactory::CreateImageFilter()
{
    return std::make_unique<SkiaImageFilter>();
}

std::unique_ptr<ShaderEffectImpl> SkiaImplFactory::CreateShaderEffect()
{
    return std::make_unique<SkiaShaderEffect>();
}

std::unique_ptr<PathEffectImpl> SkiaImplFactory::CreatePathEffect()
{
    return std::make_unique<SkiaPathEffect>();
}

std::unique_ptr<ColorSpaceImpl> SkiaImplFactory::CreateColorSpace()
{
    return std::make_unique<SkiaColorSpace>();
}

std::unique_ptr<MatrixImpl> SkiaImplFactory::CreateMatrix()
{
    return std::make_unique<SkiaMatrix>();
}

std::unique_ptr<CameraImpl> SkiaImplFactory::CreateCamera()
{
    return std::make_unique<SkiaCamera>();
}
}
}
}