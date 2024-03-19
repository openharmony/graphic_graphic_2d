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

#include "skia_impl_factory.h"

#include "skia_adapter/skia_bitmap.h"
#include "skia_adapter/skia_blender.h"
#include "skia_adapter/skia_pixmap.h"
#include "skia_adapter/skia_camera.h"
#include "skia_adapter/skia_canvas.h"
#include "skia_adapter/skia_color_filter.h"
#include "skia_adapter/skia_color_space.h"
#include "skia_adapter/skia_data.h"
#ifdef ACE_ENABLE_GPU
#include "skia_adapter/skia_gpu_context.h"
#endif
#include "skia_adapter/skia_font.h"
#include "skia_adapter/skia_font_mgr.h"
#include "skia_adapter/skia_hm_symbol_config_ohos.h"
#include "skia_adapter/skia_image.h"
#include "skia_adapter/skia_image_filter.h"
#include "skia_adapter/skia_kawase_blur.h"
#include "skia_adapter/skia_mask_filter.h"
#include "skia_adapter/skia_matrix.h"
#include "skia_adapter/skia_matrix44.h"
#include "skia_adapter/skia_path.h"
#include "skia_adapter/skia_path_effect.h"
#include "skia_adapter/skia_picture.h"
#include "skia_adapter/skia_region.h"
#include "skia_adapter/skia_resourece_holder.h"
#include "skia_adapter/skia_shader_effect.h"
#include "skia_adapter/skia_runtime_blender_builder.h"
#include "skia_adapter/skia_runtime_effect.h"
#include "skia_adapter/skia_runtime_shader_builder.h"
#include "skia_adapter/skia_surface.h"
#include "skia_adapter/skia_text_blob_builder.h"
#include "skia_adapter/skia_trace_memory_dump.h"
#include "skia_adapter/skia_memory_stream.h"
#include "skia_adapter/skia_gradient_blur.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::unique_ptr<CoreCanvasImpl> SkiaImplFactory::CreateCoreCanvas()
{
    return std::make_unique<SkiaCanvas>();
}

std::unique_ptr<CoreCanvasImpl> SkiaImplFactory::CreateCoreCanvas(void* rawCanvas)
{
    auto skCanvasPtr = reinterpret_cast<std::shared_ptr<SkCanvas>*>(rawCanvas);
    return std::make_unique<SkiaCanvas>(*skCanvasPtr);
}

std::unique_ptr<CoreCanvasImpl> SkiaImplFactory::CreateCoreCanvas(int32_t width, int32_t height)
{
    return std::make_unique<SkiaCanvas>(width, height);
}

std::unique_ptr<DataImpl> SkiaImplFactory::CreateData()
{
    return std::make_unique<SkiaData>();
}

#ifdef ACE_ENABLE_GPU
std::unique_ptr<GPUContextImpl> SkiaImplFactory::CreateGPUContext()
{
    return std::make_unique<SkiaGPUContext>();
}
#endif

std::unique_ptr<TraceMemoryDumpImpl> SkiaImplFactory::CreateTraceMemoryDump(const char* categoryKey, bool itemizeType)
{
    return std::make_unique<SkiaTraceMemoryDump>(categoryKey, itemizeType);
}

std::unique_ptr<BitmapImpl> SkiaImplFactory::CreateBitmap()
{
    return std::make_unique<SkiaBitmap>();
}

std::unique_ptr<PixmapImpl> SkiaImplFactory::CreatePixmap()
{
    return std::make_unique<SkiaPixmap>();
}

std::unique_ptr<PixmapImpl> SkiaImplFactory::CreatePixmap(const ImageInfo& imageInfo, const void* addr, size_t rowBytes)
{
    return std::make_unique<SkiaPixmap>(imageInfo, addr, rowBytes);
}

std::unique_ptr<ImageImpl> SkiaImplFactory::CreateImage()
{
    return std::make_unique<SkiaImage>();
}

std::unique_ptr<ImageImpl> SkiaImplFactory::CreateImage(void* rawImg)
{
    auto skImg = reinterpret_cast<sk_sp<SkImage>*>(rawImg);
    return std::make_unique<SkiaImage>(*skImg);
}

std::unique_ptr<PictureImpl> SkiaImplFactory::CreatePicture()
{
    return std::make_unique<SkiaPicture>();
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

std::unique_ptr<BlenderImpl> SkiaImplFactory::CreateBlender()
{
    return std::make_unique<SkiaBlender>();
}

std::unique_ptr<RuntimeEffectImpl> SkiaImplFactory::CreateRuntimeEffect()
{
    return std::make_unique<SkiaRuntimeEffect>();
}

std::unique_ptr<RuntimeShaderBuilderImpl> SkiaImplFactory::CreateRuntimeShaderBuilder(
    std::shared_ptr<RuntimeEffect> runtimeEffect)
{
    return std::make_unique<SkiaRuntimeShaderBuilder>(runtimeEffect);
}

std::unique_ptr<RuntimeBlenderBuilderImpl> SkiaImplFactory::CreateRuntimeBlenderBuilder(
    std::shared_ptr<RuntimeEffect> runtimeEffect)
{
    return std::make_unique<SkiaRuntimeBlenderBuilder>(runtimeEffect);
}

std::unique_ptr<SurfaceImpl> SkiaImplFactory::CreateSurface()
{
    return std::make_unique<SkiaSurface>();
}

// opinc_begin
std::unique_ptr<OpListHandleImpl> SkiaImplFactory::CreateOplistHandle()
{
    return nullptr;
}
// opinc_end

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

std::unique_ptr<MatrixImpl> SkiaImplFactory::CreateMatrix(const Matrix& other)
{
    return std::make_unique<SkiaMatrix>(other);
}

std::unique_ptr<Matrix44Impl> SkiaImplFactory::CreateMatrix44()
{
    return std::make_unique<SkiaMatrix44>();
}

std::unique_ptr<CameraImpl> SkiaImplFactory::CreateCamera()
{
    return std::make_unique<SkiaCamera>();
}

std::unique_ptr<RegionImpl> SkiaImplFactory::CreateRegion()
{
    return std::make_unique<SkiaRegion>();
}

std::unique_ptr<VerticesImpl> SkiaImplFactory::CreateVertices()
{
    return std::make_unique<SkiaVertices>();
}

std::unique_ptr<VerticesImpl::BuilderImpl> SkiaImplFactory::CreateVerticesBuilder()
{
    return std::make_unique<SkiaVertices::SkiaBuilder>();
}

std::unique_ptr<FontImpl> SkiaImplFactory::CreateFont()
{
    return std::make_unique<SkiaFont>();
}

std::unique_ptr<FontImpl> SkiaImplFactory::CreateFont(std::shared_ptr<Typeface> typeface,
    scalar size, scalar scaleX, scalar skewX)
{
    return std::make_unique<SkiaFont>(typeface, size, scaleX, skewX);
}

std::unique_ptr<TextBlobBuilderImpl> SkiaImplFactory::CreateTextBlobBuilder()
{
    return std::make_unique<SkiaTextBlobBuilder>();
}

std::shared_ptr<FontMgrImpl> SkiaImplFactory::CreateDefaultFontMgr()
{
    return SkiaFontMgr::CreateDefaultFontMgr();
}

#ifndef USE_TEXGINE
std::shared_ptr<FontMgrImpl> SkiaImplFactory::CreateDynamicFontMgr()
{
    return SkiaFontMgr::CreateDynamicFontMgr();
}
#endif

std::shared_ptr<MemoryStreamImpl> SkiaImplFactory::CreateMemoryStream()
{
    return std::make_shared<SkiaMemoryStream>();
}

std::shared_ptr<MemoryStreamImpl> SkiaImplFactory::CreateMemoryStream(const void* data, size_t length, bool copyData)
{
    return std::make_shared<SkiaMemoryStream>(data, length, copyData);
}

std::shared_ptr<ResourceHolderImpl> SkiaImplFactory::CreateResourceHolder()
{
    return std::make_shared<SkiaResourceHolder>();
}

std::unique_ptr<KawaseBlurImpl> SkiaImplFactory::CreateKawaseBlur()
{
    return std::make_unique<SkiaKawaseBlur>();
}

std::unique_ptr<GradientBlurImpl> SkiaImplFactory::CreateGradientBlur()
{
    return std::make_unique<SkiaGradientBlur>();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS