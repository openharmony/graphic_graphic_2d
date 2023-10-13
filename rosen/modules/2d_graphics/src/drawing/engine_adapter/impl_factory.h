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

#ifndef IMPLFACTORY_H
#define IMPLFACTORY_H

#include "impl_interface/bitmap_impl.h"
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
#include "impl_interface/font_style_set_impl.h"
#include "impl_interface/image_filter_impl.h"
#include "impl_interface/image_impl.h"
#include "impl_interface/vertices_impl.h"
#include "impl_interface/mask_filter_impl.h"
#include "impl_interface/matrix_impl.h"
#include "impl_interface/matrix44_impl.h"
#include "impl_interface/path_effect_impl.h"
#include "impl_interface/path_impl.h"
#include "impl_interface/picture_impl.h"
#include "impl_interface/region_impl.h"
#include "impl_interface/shader_effect_impl.h"
#include "impl_interface/surface_impl.h"
#include "impl_interface/text_blob_builder_impl.h"
#include "impl_interface/typeface_impl.h"

struct FontInfo;
class FontConfig_OHOS;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ImplFactory {
public:
    static std::unique_ptr<CoreCanvasImpl> CreateCoreCanvasImpl();
    static std::unique_ptr<CoreCanvasImpl> CreateCoreCanvasImpl(void* rawCanvas);
    static std::unique_ptr<CoreCanvasImpl> CreateCoreCanvasImpl(int32_t width, int32_t height);
    static std::unique_ptr<DataImpl> CreateDataImpl();
#ifdef ACE_ENABLE_GPU
    static std::unique_ptr<GPUContextImpl> CreateGPUContextImpl();
    static std::unique_ptr<TraceMemoryDumpImpl> CreateTraceMemoryDumpImpl(const char* categoryKey, bool itemizeType);
#endif
    static std::unique_ptr<BitmapImpl> CreateBitmapImpl();
    static std::unique_ptr<ImageImpl> CreateImageImpl();
    static std::unique_ptr<ImageImpl> CreateImageImpl(void* rawImage);
    static std::unique_ptr<PathImpl> CreatePathImpl();
    static std::unique_ptr<ColorFilterImpl> CreateColorFilterImpl();
    static std::unique_ptr<MaskFilterImpl> CreateMaskFilterImpl();
    static std::unique_ptr<ImageFilterImpl> CreateImageFilterImpl();
    static std::unique_ptr<PictureImpl> CreatePictureImpl();
    static std::unique_ptr<ShaderEffectImpl> CreateShaderEffectImpl();
    static std::unique_ptr<SurfaceImpl> CreateSurfaceImpl();
    static std::unique_ptr<PathEffectImpl> CreatePathEffectImpl();
    static std::unique_ptr<ColorSpaceImpl> CreateColorSpaceImpl();
    static std::unique_ptr<MatrixImpl> CreateMatrixImpl();
    static std::unique_ptr<Matrix44Impl> CreateMatrix44Impl();
    static std::unique_ptr<CameraImpl> CreateCameraImpl();
    static std::unique_ptr<RegionImpl> CreateRegionImpl();
    static std::unique_ptr<VerticesImpl> CreateVerticesImpl();
    static std::unique_ptr<VerticesImpl::BuilderImpl> CreateVerticesBuilderImpl();
    static std::unique_ptr<FontImpl> CreateFontImpl();
    static std::unique_ptr<TextBlobBuilderImpl> CreateTextBlobBuilderImpl();
    static std::shared_ptr<FontMgrImpl> CreateDefaultFontMgrImpl();
    static std::unique_ptr<FontMgrImpl> CreateDynamicFontMgrImpl();
    static std::unique_ptr<TypefaceImpl> CreateTypefaceImpl(const std::string& specifiedName, FontInfo& info);
    static std::unique_ptr<FontStyleSetImpl> CreateTypefaceFontStyleSetImpl();
    static std::unique_ptr<FontStyleSetImpl> CreateFontStyleSetOhosImpl(
        const std::shared_ptr<FontConfig_OHOS>& fontConfig, int index, bool isFallback);
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
