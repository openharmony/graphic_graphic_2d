/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "effect/runtime_effect.h"
#include "skia_adapter/skia_impl_factory.h"
#include "utils/matrix.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
using EngineImplFactory = SkiaImplFactory;

std::unique_ptr<CoreCanvasImpl> ImplFactory::CreateCoreCanvasImpl()
{
    return EngineImplFactory::CreateCoreCanvas();
}

std::unique_ptr<CoreCanvasImpl> ImplFactory::CreateCoreCanvasImpl(void* rawCanvas)
{
    return EngineImplFactory::CreateCoreCanvas(rawCanvas);
}

std::unique_ptr<CoreCanvasImpl> ImplFactory::CreateCoreCanvasImpl(int32_t width, int32_t height)
{
    return EngineImplFactory::CreateCoreCanvas(width, height);
}

std::unique_ptr<DataImpl> ImplFactory::CreateDataImpl()
{
    return EngineImplFactory::CreateData();
}

#ifdef ACE_ENABLE_GPU
std::unique_ptr<GPUContextImpl> ImplFactory::CreateGPUContextImpl()
{
    return EngineImplFactory::CreateGPUContext();
}
#endif

std::unique_ptr<TraceMemoryDumpImpl> ImplFactory::CreateTraceMemoryDumpImpl(const char* categoryKey, bool itemizeType)
{
    return EngineImplFactory::CreateTraceMemoryDump(categoryKey, itemizeType);
}

std::unique_ptr<BitmapImpl> ImplFactory::CreateBitmapImpl()
{
    return EngineImplFactory::CreateBitmap();
}

std::unique_ptr<PixmapImpl> ImplFactory::CreatePixmapImpl()
{
    return EngineImplFactory::CreatePixmap();
}

std::unique_ptr<PixmapImpl> ImplFactory::CreatePixmapImpl(const ImageInfo& imageInfo, const void* addr, size_t rowBytes)
{
    return EngineImplFactory::CreatePixmap(imageInfo, addr, rowBytes);
}

std::unique_ptr<ImageImpl> ImplFactory::CreateImageImpl()
{
    return EngineImplFactory::CreateImage();
}

std::unique_ptr<ImageImpl> ImplFactory::CreateImageImpl(void* rawImage)
{
    return EngineImplFactory::CreateImage(rawImage);
}

std::unique_ptr<PictureImpl> ImplFactory::CreatePictureImpl()
{
    return EngineImplFactory::CreatePicture();
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

std::unique_ptr<BlenderImpl> ImplFactory::CreateBlenderImpl()
{
    return EngineImplFactory::CreateBlender();
}

std::unique_ptr<RuntimeEffectImpl> ImplFactory::CreateRuntimeEffectImpl()
{
    return EngineImplFactory::CreateRuntimeEffect();
}

std::unique_ptr<RuntimeShaderBuilderImpl> ImplFactory::CreateRuntimeShaderBuilderImpl(
    std::shared_ptr<RuntimeEffect> runtimeEffect)
{
    return EngineImplFactory::CreateRuntimeShaderBuilder(runtimeEffect);
}

std::unique_ptr<RuntimeBlenderBuilderImpl> ImplFactory::CreateRuntimeBlenderBuilderImpl(
    std::shared_ptr<RuntimeEffect> runtimeEffect)
{
    return EngineImplFactory::CreateRuntimeBlenderBuilder(runtimeEffect);
}

std::unique_ptr<SurfaceImpl> ImplFactory::CreateSurfaceImpl()
{
    return EngineImplFactory::CreateSurface();
}

// opinc_begin
std::unique_ptr<OpListHandleImpl> ImplFactory::CreateOplistHandleImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        return DDGRImplFactory::CreateOplistHandle();
    }
#endif
    return EngineImplFactory::CreateOplistHandle();
}
// opinc_end

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

std::unique_ptr<MatrixImpl> ImplFactory::CreateMatrixImpl(const Matrix& other)
{
    return EngineImplFactory::CreateMatrix(other);
}

std::unique_ptr<Matrix44Impl> ImplFactory::CreateMatrix44Impl()
{
    return EngineImplFactory::CreateMatrix44();
}

std::unique_ptr<CameraImpl> ImplFactory::CreateCameraImpl()
{
    return EngineImplFactory::CreateCamera();
}

std::unique_ptr<RegionImpl> ImplFactory::CreateRegionImpl()
{
    return EngineImplFactory::CreateRegion();
}

std::unique_ptr<VerticesImpl> ImplFactory::CreateVerticesImpl()
{
    return EngineImplFactory::CreateVertices();
}

std::unique_ptr<VerticesImpl::BuilderImpl> ImplFactory::CreateVerticesBuilderImpl()
{
    return EngineImplFactory::CreateVerticesBuilder();
}

std::unique_ptr<FontImpl> ImplFactory::CreateFontImpl()
{
    return EngineImplFactory::CreateFont();
}

std::unique_ptr<FontImpl> ImplFactory::CreateFontImpl(std::shared_ptr<Typeface> typeface,
    scalar size, scalar scaleX, scalar skewX)
{
    return EngineImplFactory::CreateFont(typeface, size, scaleX, skewX);
}

std::unique_ptr<TextBlobBuilderImpl> ImplFactory::CreateTextBlobBuilderImpl()
{
    return EngineImplFactory::CreateTextBlobBuilder();
}

std::shared_ptr<FontMgrImpl> ImplFactory::CreateDefaultFontMgrImpl()
{
    return EngineImplFactory::CreateDefaultFontMgr();
}

#ifndef USE_TEXGINE
std::shared_ptr<FontMgrImpl> ImplFactory::CreateDynamicFontMgrImpl()
{
    return EngineImplFactory::CreateDynamicFontMgr();
}
#endif

std::shared_ptr<MemoryStreamImpl> ImplFactory::CreateMemoryStreamImpl()
{
    return EngineImplFactory::CreateMemoryStream();
}

std::shared_ptr<MemoryStreamImpl> ImplFactory::CreateMemoryStreamImpl(const void* data, size_t length, bool copyData)
{
    return EngineImplFactory::CreateMemoryStream(data, length, copyData);
}

std::shared_ptr<ResourceHolderImpl> ImplFactory::CreateResourceHolderImpl()
{
    return EngineImplFactory::CreateResourceHolder();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS