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

#ifndef IOS_PLATFORM
#ifdef RS_ENABLE_GPU
#include "egl_manager.h"
#endif
#endif
 
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/gpu/GpuTypes.h"
#include "include/core/SkSurface.h"
#else
#include <include/gpu/GrDirectContext.h>
#include "include/gpu/gl/GrGLInterface.h"
#endif

#if (!defined(ANDROID_PLATFORM)) && (!defined(IOS_PLATFORM))
#include "rs_trace.h"
#endif

#include "sk_image_chain.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
SKImageChain::SKImageChain(SkCanvas* canvas, sk_sp<SkImage> image) : canvas_(canvas), image_(image)
{}

SKImageChain::SKImageChain(std::shared_ptr<Media::PixelMap> srcPixelMap) : srcPixelMap_(srcPixelMap)
{}

SKImageChain::~SKImageChain()
{
    canvas_ = nullptr;
    gpuSurface_ = nullptr;
    dstPixmap_ = nullptr;
    srcPixelMap_ = nullptr;
    dstPixelMap_ = nullptr;
    filters_ = nullptr;
    cpuSurface_ = nullptr;
    image_ = nullptr;
}

DrawError SKImageChain::Render(const std::vector<sk_sp<SkImageFilter>>& skFilters, const bool& forceCPU,
    std::shared_ptr<Media::PixelMap>& dstPixelMap)
{
    for (auto filter : skFilters) {
        SetFilters(filter);
    }

    ForceCPU(forceCPU);
    DrawError ret = Draw();
    if (ret == DrawError::ERR_OK) {
        dstPixelMap = GetPixelMap();
    } else {
        LOGE("skImage.Draw() = %{public}d", ret);
    }

    return ret;
}

DrawError SKImageChain::InitWithoutCanvas()
{
    if (srcPixelMap_ == nullptr) {
        LOGE("The srcPixelMap_ is nullptr.");
        return DrawError::ERR_IMAGE_NULL;
    }
    imageInfo_ = SkImageInfo::Make(srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight(),
    PixelFormatConvert(srcPixelMap_->GetPixelFormat()), static_cast<SkAlphaType>(srcPixelMap_->GetAlphaType()));
    SkPixmap srcPixmap(imageInfo_, srcPixelMap_->GetPixels(), srcPixelMap_->GetRowStride());
    SkBitmap srcBitmap;
    srcBitmap.installPixels(srcPixmap);
#ifdef USE_M133_SKIA
    image_ = SkImages::RasterFromBitmap(srcBitmap);
#else
    image_ = SkImage::MakeFromBitmap(srcBitmap);
#endif
    Media::InitializationOptions opts;
    opts.size.width = srcPixelMap_->GetWidth();
    opts.size.height = srcPixelMap_->GetHeight();
    opts.editable = true;
    auto dstPixelMap = Media::PixelMap::Create(opts);
    if (dstPixelMap != nullptr) {
        dstPixmap_ = std::make_shared<SkPixmap>(imageInfo_, dstPixelMap->GetPixels(), dstPixelMap->GetRowStride());
        dstPixelMap_ = std::shared_ptr<Media::PixelMap>(dstPixelMap.release());
    } else {
        LOGE("Failed to create the dstPixelMap.");
        return DrawError::ERR_IMAGE_NULL;
    }

    return DrawError::ERR_OK;
}

bool SKImageChain::CreateCPUCanvas()
{
    if (dstPixmap_ == nullptr) {
        LOGE("The dstPixmap_ is nullptr.");
        return false;
    }
#ifdef USE_M133_SKIA
    cpuSurface_ = SkSurfaces::WrapPixels(imageInfo_, const_cast<void*>(dstPixmap_->addr()),
    dstPixelMap_->GetRowStride());
#else
    cpuSurface_ = SkSurface::MakeRasterDirect(imageInfo_, const_cast<void*>(dstPixmap_->addr()),
    dstPixelMap_->GetRowStride());
#endif
    if (!cpuSurface_) {
        LOGE("Failed to create surface for CPU.");
        return false;
    }
    canvas_ = cpuSurface_->getCanvas();
    if (canvas_ == nullptr) {
        LOGE("Failed to getCanvas for CPU.");
        return false;
    }

    return true;
}

bool SKImageChain::CreateGPUCanvas()
{
#if defined(RS_ENABLE_GL) && (!defined(IOS_PLATFORM))
    if (!EglManager::GetInstance().Init()) {
        LOGE("Failed to init for GPU.");
        return false;
    }
#ifdef USE_M133_SKIA
    sk_sp<const GrGLInterface> glInterface(GrGLMakeNativeInterface());
    sk_sp<GrDirectContext> grContext(GrDirectContexts::MakeGL(std::move(glInterface)));
    gpuSurface_ = SkSurfaces::RenderTarget(grContext.get(), skgpu::Budgeted::kNo, imageInfo_);
#else
    sk_sp<const GrGLInterface> glInterface(GrGLCreateNativeInterface());
    sk_sp<GrDirectContext> grContext(GrDirectContext::MakeGL(std::move(glInterface)));
    gpuSurface_ = SkSurface::MakeRenderTarget(grContext.get(), SkBudgeted::kNo, imageInfo_);
#endif
    if (!gpuSurface_) {
        LOGE("Failed to create surface for GPU.");
        return false;
    }
    canvas_ = gpuSurface_->getCanvas();
    if (canvas_ == nullptr) {
        LOGE("Failed to getCanvas for GPU.");
        return false;
    }

    return true;
#else
    LOGI("GPU rendering is not supported.");
    return false;
#endif
}

void SKImageChain::ForceCPU(bool forceCPU)
{
    if (forceCPU_ == forceCPU) {
        LOGI("Same result without change.");
        return;
    }
    forceCPU_ = forceCPU;
    if (canvas_ == nullptr) {
        LOGI("The canvas_ is nullptr.");
        return;
    }
    if (forceCPU) {
        if (cpuSurface_ != nullptr) {
            canvas_ = cpuSurface_->getCanvas();
        } else {
            canvas_ = nullptr;
        }
    } else {
        if (gpuSurface_ != nullptr) {
            canvas_ = gpuSurface_->getCanvas();
        } else {
            canvas_ = nullptr;
        }
    }
}

void SKImageChain::SetFilters(sk_sp<SkImageFilter> filter)
{
    if (filters_ == nullptr) {
        filters_ = filter;
    } else {
        filters_ = SkImageFilters::Compose(filter, filters_);
    }
}

void SKImageChain::SetClipRect(SkRect* rect)
{
    rect_ = rect;
}

void SKImageChain::SetClipPath(SkPath* path)
{
    path_ = path;
}

void SKImageChain::SetClipRRect(SkRRect* rRect)
{
    rRect_ = rRect;
}

std::shared_ptr<Media::PixelMap> SKImageChain::GetPixelMap()
{
    return dstPixelMap_;
}

bool SKImageChain::InitializeCanvas()
{
    DrawError ret = InitWithoutCanvas();
    if (ret != DrawError::ERR_OK) {
        LOGE("Failed to init.");
        return false;
    }

    if (forceCPU_) {
        if (!CreateCPUCanvas()) {
            LOGE("Failed to create canvas for CPU.");
            return false;
        }
    } else {
        if (!CreateGPUCanvas()) {
            LOGE("Failed to create canvas for GPU.");
            return false;
        }
    }
    return canvas_ != nullptr;
}

DrawError SKImageChain::CheckForErrors()
{
    if (canvas_ == nullptr) {
        LOGE("Failed to create canvas");
        return DrawError::ERR_CANVAS_NULL;
    }
    if (image_ == nullptr) {
        LOGE("The image_ is nullptr, nothing to draw.");
        return DrawError::ERR_IMAGE_NULL;
    }
    return DrawError::ERR_OK;
}

void SKImageChain::SetupPaint(SkPaint& paint)
{
    paint.setAntiAlias(true);
    paint.setBlendMode(SkBlendMode::kSrc);
    paint.setImageFilter(filters_);
}

void SKImageChain::ApplyClipping()
{
    if (rect_ != nullptr) {
        canvas_->clipRect(*rect_, true);
    } else if (path_ != nullptr) {
        canvas_->clipPath(*path_, true);
    } else if (rRect_ != nullptr) {
        canvas_->clipRRect(*rRect_, true);
    }
}

bool SKImageChain::DrawImage(SkPaint& paint)
{
    canvas_->save();
    canvas_->resetMatrix();
    canvas_->drawImage(image_.get(), 0, 0, SkSamplingOptions(), &paint);
    if (!forceCPU_ && dstPixmap_ != nullptr) {
        if (!canvas_->readPixels(*dstPixmap_.get(), 0, 0)) {
            LOGE("Failed to readPixels to target Pixmap.");
            canvas_->restore();
            return false;
        }
    }
    canvas_->restore();
    return true;
}

DrawError SKImageChain::Draw()
{
    if (!InitializeCanvas()) {
        return DrawError::ERR_CPU_CANVAS;
    }

    DrawError error = CheckForErrors();
    if (error != DrawError::ERR_OK) {
        return error;
    }
#if (!defined(ANDROID_PLATFORM)) && (!defined(IOS_PLATFORM))
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "SKImageChain::Draw");
#endif
    SkPaint paint;
    SetupPaint(paint);
    ApplyClipping();

    if (!DrawImage(paint)) {
        return DrawError::ERR_PIXEL_READ;
    }
#if (!defined(ANDROID_PLATFORM)) && (!defined(IOS_PLATFORM))
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
#endif
    return DrawError::ERR_OK;
}

SkColorType SKImageChain::PixelFormatConvert(const Media::PixelFormat& pixelFormat)
{
    SkColorType colorType;
    switch (pixelFormat) {
        case Media::PixelFormat::BGRA_8888:
            colorType = SkColorType::kBGRA_8888_SkColorType;
            break;
        case Media::PixelFormat::RGBA_8888:
            colorType = SkColorType::kRGBA_8888_SkColorType;
            break;
        case Media::PixelFormat::RGB_565:
            colorType = SkColorType::kRGB_565_SkColorType;
            break;
        case Media::PixelFormat::ALPHA_8:
            colorType = SkColorType::kAlpha_8_SkColorType;
            break;
        default:
            colorType = SkColorType::kUnknown_SkColorType;
            break;
    }
    return colorType;
}
} // namespcae Rosen
} // namespace OHOS
