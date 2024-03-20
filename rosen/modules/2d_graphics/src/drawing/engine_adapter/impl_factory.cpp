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
#include "utils/system_properties.h"
#ifdef ENABLE_DDGR_OPTIMIZE
#include "ddgr_impl_factory.h"
#endif
namespace OHOS {
namespace Rosen {
namespace Drawing {
using EngineImplFactory = SkiaImplFactory;

std::unique_ptr<CoreCanvasImpl> ImplFactory::CreateCoreCanvasImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateCoreCanvas();
    }
#endif
    return EngineImplFactory::CreateCoreCanvas();
}

std::unique_ptr<CoreCanvasImpl> ImplFactory::CreateCoreCanvasImpl(void* rawCanvas)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateCoreCanvas(rawCanvas);
    }
#endif
    return EngineImplFactory::CreateCoreCanvas(rawCanvas);
}

std::unique_ptr<CoreCanvasImpl> ImplFactory::CreateCoreCanvasImpl(int32_t width, int32_t height)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateCoreCanvas(width, height);
    }
#endif
    return EngineImplFactory::CreateCoreCanvas(width, height);
}

std::unique_ptr<DataImpl> ImplFactory::CreateDataImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateData();
    }
#endif
    return EngineImplFactory::CreateData();
}

#ifdef ACE_ENABLE_GPU
std::unique_ptr<GPUContextImpl> ImplFactory::CreateGPUContextImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateGPUContext();
    }
#endif
    return EngineImplFactory::CreateGPUContext();
}
#endif

std::unique_ptr<TraceMemoryDumpImpl> ImplFactory::CreateTraceMemoryDumpImpl(const char* categoryKey, bool itemizeType)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateTraceMemoryDump(categoryKey, itemizeType);
    }
#endif
    return EngineImplFactory::CreateTraceMemoryDump(categoryKey, itemizeType);
}

std::unique_ptr<BitmapImpl> ImplFactory::CreateBitmapImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateBitmap();
    }
#endif
    return EngineImplFactory::CreateBitmap();
}

std::unique_ptr<PixmapImpl> ImplFactory::CreatePixmapImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreatePixmap();
    }
#endif
    return EngineImplFactory::CreatePixmap();
}

std::unique_ptr<PixmapImpl> ImplFactory::CreatePixmapImpl(const ImageInfo& imageInfo, const void* addr, size_t rowBytes)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreatePixmap(imageInfo, addr, rowBytes);
    }
#endif
    return EngineImplFactory::CreatePixmap(imageInfo, addr, rowBytes);
}

std::unique_ptr<ImageImpl> ImplFactory::CreateImageImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateImage();
    }
#endif
    return EngineImplFactory::CreateImage();
}

std::unique_ptr<ImageImpl> ImplFactory::CreateImageImpl(void* rawImage)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateImage(rawImage);
    }
#endif
    return EngineImplFactory::CreateImage(rawImage);
}

std::unique_ptr<PictureImpl> ImplFactory::CreatePictureImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreatePicture();
    }
#endif
    return EngineImplFactory::CreatePicture();
}

std::unique_ptr<PathImpl> ImplFactory::CreatePathImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreatePath();
    }
#endif
    return EngineImplFactory::CreatePath();
}

std::unique_ptr<ColorFilterImpl> ImplFactory::CreateColorFilterImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateColorFilter();
    }
#endif
    return EngineImplFactory::CreateColorFilter();
}
std::unique_ptr<MaskFilterImpl> ImplFactory::CreateMaskFilterImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateMaskFilter();
    }
#endif
    return EngineImplFactory::CreateMaskFilter();
}

std::unique_ptr<ImageFilterImpl> ImplFactory::CreateImageFilterImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateImageFilter();
    }
#endif
    return EngineImplFactory::CreateImageFilter();
}

std::unique_ptr<ShaderEffectImpl> ImplFactory::CreateShaderEffectImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateShaderEffect();
    }
#endif
    return EngineImplFactory::CreateShaderEffect();
}

std::unique_ptr<BlenderImpl> ImplFactory::CreateBlenderImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateBlender();
    }
#endif
    return EngineImplFactory::CreateBlender();
}

std::unique_ptr<RuntimeEffectImpl> ImplFactory::CreateRuntimeEffectImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateRuntimeEffect();
    }
#endif
    return EngineImplFactory::CreateRuntimeEffect();
}

std::unique_ptr<RuntimeShaderBuilderImpl> ImplFactory::CreateRuntimeShaderBuilderImpl(
    std::shared_ptr<RuntimeEffect> runtimeEffect)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateRuntimeShaderBuilder(runtimeEffect);
    }
#endif
    return EngineImplFactory::CreateRuntimeShaderBuilder(runtimeEffect);
}

std::unique_ptr<RuntimeBlenderBuilderImpl> ImplFactory::CreateRuntimeBlenderBuilderImpl(
    std::shared_ptr<RuntimeEffect> runtimeEffect)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateRuntimeBlenderBuilder(runtimeEffect);
    }
#endif
    return EngineImplFactory::CreateRuntimeBlenderBuilder(runtimeEffect);
}

std::unique_ptr<SurfaceImpl> ImplFactory::CreateSurfaceImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateSurface();
    }
#endif
    return EngineImplFactory::CreateSurface();
}

// opinc_begin
std::unique_ptr<OpListHandleImpl> ImplFactory::CreateOplistHandleImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateOplistHandle();
    }
#endif
    return EngineImplFactory::CreateOplistHandle();
}
// opinc_end

std::unique_ptr<PathEffectImpl> ImplFactory::CreatePathEffectImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreatePathEffect();
    }
#endif
    return EngineImplFactory::CreatePathEffect();
}

std::unique_ptr<ColorSpaceImpl> ImplFactory::CreateColorSpaceImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateColorSpace();
    }
#endif
    return EngineImplFactory::CreateColorSpace();
}

std::unique_ptr<MatrixImpl> ImplFactory::CreateMatrixImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateMatrix();
    }
#endif
    return EngineImplFactory::CreateMatrix();
}

std::unique_ptr<MatrixImpl> ImplFactory::CreateMatrixImpl(const Matrix& other)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateMatrix(other);
    }
#endif
    return EngineImplFactory::CreateMatrix(other);
}

std::unique_ptr<Matrix44Impl> ImplFactory::CreateMatrix44Impl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateMatrix44();
    }
#endif
    return EngineImplFactory::CreateMatrix44();
}

std::unique_ptr<CameraImpl> ImplFactory::CreateCameraImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateCamera();
    }
#endif
    return EngineImplFactory::CreateCamera();
}

std::unique_ptr<RegionImpl> ImplFactory::CreateRegionImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateRegion();
    }
#endif
    return EngineImplFactory::CreateRegion();
}

std::unique_ptr<VerticesImpl> ImplFactory::CreateVerticesImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateVertices();
    }
#endif
    return EngineImplFactory::CreateVertices();
}

std::unique_ptr<VerticesImpl::BuilderImpl> ImplFactory::CreateVerticesBuilderImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateVerticesBuilder();
    }
#endif
    return EngineImplFactory::CreateVerticesBuilder();
}

std::unique_ptr<FontImpl> ImplFactory::CreateFontImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateFont();
    }
#endif
    return EngineImplFactory::CreateFont();
}

std::unique_ptr<FontImpl> ImplFactory::CreateFontImpl(std::shared_ptr<Typeface> typeface,
    scalar size, scalar scaleX, scalar skewX)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateFont(typeface, size, scaleX, skewX);
    }
#endif
    return EngineImplFactory::CreateFont(typeface, size, scaleX, skewX);
}

std::unique_ptr<TextBlobBuilderImpl> ImplFactory::CreateTextBlobBuilderImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateTextBlobBuilder();
    }
#endif
    return EngineImplFactory::CreateTextBlobBuilder();
}

std::shared_ptr<FontMgrImpl> ImplFactory::CreateDefaultFontMgrImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateDefaultFontMgr();
    }
#endif
    return EngineImplFactory::CreateDefaultFontMgr();
}

#ifndef USE_TEXGINE
std::shared_ptr<FontMgrImpl> ImplFactory::CreateDynamicFontMgrImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateDynamicFontMgr();
    }
#endif
    return EngineImplFactory::CreateDynamicFontMgr();
}
#endif

std::shared_ptr<MemoryStreamImpl> ImplFactory::CreateMemoryStreamImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateMemoryStream();
    }
#endif
    return EngineImplFactory::CreateMemoryStream();
}

std::shared_ptr<MemoryStreamImpl> ImplFactory::CreateMemoryStreamImpl(const void* data, size_t length, bool copyData)
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateMemoryStream(data, length, copyData);
    }
#endif
    return EngineImplFactory::CreateMemoryStream(data, length, copyData);
}

std::shared_ptr<ResourceHolderImpl> ImplFactory::CreateResourceHolderImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateResourceHolder();
    }
#endif
    return EngineImplFactory::CreateResourceHolder();
}

std::unique_ptr<KawaseBlurImpl> ImplFactory::CreateKawaseBlurImpl()
{
#ifdef ENABLE_DDGR_OPTIMIZE
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return DDGRImplFactory::CreateKawaseBlur();
    }
#endif
    return EngineImplFactory::CreateKawaseBlur();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS