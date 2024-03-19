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

#ifndef SKIA_IMPLFACTORY_H
#define SKIA_IMPLFACTORY_H

#include "impl_interface/bitmap_impl.h"
#include "impl_interface/blender_impl.h"
#include "impl_interface/pixmap_impl.h"
#include "impl_interface/camera_impl.h"
#include "impl_interface/color_filter_impl.h"
#include "impl_interface/color_space_impl.h"
#include "impl_interface/core_canvas_impl.h"
#include "impl_interface/data_impl.h"
#ifdef ACE_ENABLE_GPU
#include "impl_interface/gpu_context_impl.h"
#endif
#include "impl_interface/font_impl.h"
#include "impl_interface/font_mgr_impl.h"
#include "impl_interface/gradient_blur_impl.h"
#include "impl_interface/image_filter_impl.h"
#include "impl_interface/image_impl.h"
#include "impl_interface/kawase_blur_impl.h"
#include "impl_interface/mask_filter_impl.h"
#include "impl_interface/matrix_impl.h"
#include "impl_interface/matrix44_impl.h"
#include "impl_interface/path_effect_impl.h"
#include "impl_interface/path_impl.h"
#include "impl_interface/picture_impl.h"
#include "impl_interface/region_impl.h"
#include "impl_interface/resource_holder_impl.h"
#include "impl_interface/shader_effect_impl.h"
#include "impl_interface/runtime_blender_builder_impl.h"
#include "impl_interface/runtime_effect_impl.h"
#include "impl_interface/runtime_shader_builder_impl.h"
#include "impl_interface/surface_impl.h"
#include "impl_interface/text_blob_builder_impl.h"
#include "impl_interface/vertices_impl.h"
#include "impl_interface/trace_memory_dump_impl.h"
#include "impl_interface/memory_stream_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaImplFactory {
public:
    static std::unique_ptr<CoreCanvasImpl> CreateCoreCanvas();
    static std::unique_ptr<CoreCanvasImpl> CreateCoreCanvas(void* rawCanvas);
    static std::unique_ptr<CoreCanvasImpl> CreateCoreCanvas(int32_t width, int32_t height);
    static std::unique_ptr<DataImpl> CreateData();
#ifdef ACE_ENABLE_GPU
    static std::unique_ptr<GPUContextImpl> CreateGPUContext();
#endif
    static std::unique_ptr<TraceMemoryDumpImpl> CreateTraceMemoryDump(const char* categoryKey, bool itemizeType);
    static std::unique_ptr<BitmapImpl> CreateBitmap();
    static std::unique_ptr<PixmapImpl> CreatePixmap();
    static std::unique_ptr<PixmapImpl> CreatePixmap(const ImageInfo& imageInfo, const void* addr, size_t rowBytes);
    static std::unique_ptr<ImageImpl> CreateImage();
    static std::unique_ptr<ImageImpl> CreateImage(void* rawImg);
    static std::unique_ptr<PictureImpl> CreatePicture();
    static std::unique_ptr<PathImpl> CreatePath();
    static std::unique_ptr<ColorFilterImpl> CreateColorFilter();
    static std::unique_ptr<MaskFilterImpl> CreateMaskFilter();
    static std::unique_ptr<ImageFilterImpl> CreateImageFilter();
    static std::unique_ptr<ShaderEffectImpl> CreateShaderEffect();
    static std::unique_ptr<BlenderImpl> CreateBlender();
    static std::unique_ptr<RuntimeEffectImpl> CreateRuntimeEffect();
    static std::unique_ptr<RuntimeShaderBuilderImpl> CreateRuntimeShaderBuilder(
        std::shared_ptr<RuntimeEffect> runtimeEffect);
    static std::unique_ptr<RuntimeBlenderBuilderImpl> CreateRuntimeBlenderBuilder(
        std::shared_ptr<RuntimeEffect> runtimeEffect);
    static std::unique_ptr<SurfaceImpl> CreateSurface();
    // opinc_begin
    static std::unique_ptr<OpListHandleImpl> CreateOplistHandle();
    // opinc_end
    static std::unique_ptr<PathEffectImpl> CreatePathEffect();
    static std::unique_ptr<ColorSpaceImpl> CreateColorSpace();
    static std::unique_ptr<MatrixImpl> CreateMatrix();
    static std::unique_ptr<MatrixImpl> CreateMatrix(const Matrix& other);
    static std::unique_ptr<Matrix44Impl> CreateMatrix44();
    static std::unique_ptr<CameraImpl> CreateCamera();
    static std::unique_ptr<RegionImpl> CreateRegion();
    static std::unique_ptr<VerticesImpl> CreateVertices();
    static std::unique_ptr<VerticesImpl::BuilderImpl> CreateVerticesBuilder();
    static std::unique_ptr<FontImpl> CreateFont();
    static std::unique_ptr<FontImpl> CreateFont(std::shared_ptr<Typeface> typeface,
        scalar size, scalar scaleX, scalar skewX);
    static std::unique_ptr<TextBlobBuilderImpl> CreateTextBlobBuilder();
    static std::shared_ptr<FontMgrImpl> CreateDefaultFontMgr();
#ifndef USE_TEXGINE
    static std::shared_ptr<FontMgrImpl> CreateDynamicFontMgr();
#endif
    static std::shared_ptr<MemoryStreamImpl> CreateMemoryStream();
    static std::shared_ptr<MemoryStreamImpl> CreateMemoryStream(const void* data, size_t length, bool copyData);
    static std::shared_ptr<ResourceHolderImpl> CreateResourceHolder();
    static std::unique_ptr<KawaseBlurImpl> CreateKawaseBlur();
    static std::unique_ptr<GradientBlurImpl> CreateGradientBlur();
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif