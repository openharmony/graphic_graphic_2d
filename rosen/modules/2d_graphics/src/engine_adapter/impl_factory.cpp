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

#include "impl_factory.h"

#include "skia_adapter/skia_impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

using EngineImplFactory = SkiaImplFactory;

std::unique_ptr<CoreCanvasImpl> ImplFactory::CreateCoreCanvasImpl()
{
    return EngineImplFactory::CreateCoreCanvas();
}

std::unique_ptr<BitmapImpl> ImplFactory::CreateBitmapImpl()
{
    return EngineImplFactory::CreateBitmap();
}

std::unique_ptr<ImageImpl> ImplFactory::CreateImageImpl()
{
    return EngineImplFactory::CreateImage();
}

std::unique_ptr<PathImpl> ImplFactory::CreatePathImpl()
{
    return EngineImplFactory::CreatePath();
}

std::unique_ptr<ColorFilterImpl> ImplFactory::CreateColorFilterImpl()
{
    return EngineImplFactory::CreateColorFilter();
}
std::unique_ptr<MaskFilterImpl> ImplFactory::CreateMaskFilterImpl()
{
    return EngineImplFactory::CreateMaskFilter();
}

std::unique_ptr<ImageFilterImpl> ImplFactory::CreateImageFilterImpl()
{
    return EngineImplFactory::CreateImageFilter();
}

std::unique_ptr<ShaderEffectImpl> ImplFactory::CreateShaderEffectImpl()
{
    return EngineImplFactory::CreateShaderEffect();
}

std::unique_ptr<PathEffectImpl> ImplFactory::CreatePathEffectImpl()
{
    return EngineImplFactory::CreatePathEffect();
}

std::unique_ptr<ColorSpaceImpl> ImplFactory::CreateColorSpaceImpl()
{
    return EngineImplFactory::CreateColorSpace();
}

std::unique_ptr<MatrixImpl> ImplFactory::CreateMatrixImpl()
{
    return EngineImplFactory::CreateMatrix();
}

std::unique_ptr<CameraImpl> ImplFactory::CreateCameraImpl()
{
    return EngineImplFactory::CreateCamera();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS