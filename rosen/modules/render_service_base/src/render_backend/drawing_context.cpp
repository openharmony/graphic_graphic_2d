/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "drawing_context.h"

#include "GLES3/gl32.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/gl/GrGLInterface.h"

#include "render_context/shader_cache.h"
#include "utils/log.h"
#include "render_context_base.h"

namespace OHOS {
namespace Rosen {
const int STENCIL_BUFFER_SIZE = 8;
const std::string UNIRENDER_CACHE_DIR = "/data/service/el0/render_service";
sk_sp<SkSurface> DrawingContext::AcquireSurface(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    if (frame == nullptr) {
        LOGE("Failed to acquire Surface, frame is nullptr");
        return nullptr;
    }
    if (renderType_ == RenderType::GLES) {
        return AcquireSurfaceInGLES(frame);
    } else if (renderType_ == RenderType::RASTER) {
        return AcquireSurfaceInRaster(frame);
    } else {
        return AcquireSurfaceInVulkan(frame);
    }
}
bool DrawingContext::SetUpDrawingContext()
{
#if defined(RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL) {
        return false;
    }
    if (gpuContext_ != nullptr) {
        LOGD("gpuContext_ has already initialized");
        return true;
    }

    auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    auto size = glesVersion ? strlen(glesVersion) : 0;
    bool isUni = false;
    auto &cache = ShaderCache::Instance();
#if defined(RS_ENABLE_UNI_RENDER)
    isUni = true;
    cache.SetFilePath(UNIRENDER_CACHE_DIR);
#endif
    cache.InitShaderCache(glesVersion, size, isUni);

    Drawing::GPUContextOptions options;
    options.SetPersistentCache(&cache);
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    if (!gpuContext->BuildFromGL(options)) {
        LOGE("Failed to create gpuContext, gpuContext is nullptr");
        return false;
    }
    gpuContext_ = std::move(gpuContext);
    return true;
#else
    return false;
#endif
}

GPUContext* DrawingContext::GetDrawingContext() const
{
    return gpuContext_.get();
}

sk_sp<SkSurface> DrawingContext::AcquireSurfaceInGLES(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
#if defined(NEW_SKIA)
    GrDirectContext* grContext = GetDrawingContext();
#else
    GrContext* grContext = GetDrawingContext();
#endif
    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0;
    framebufferInfo.fFormat = GL_RGBA8;

    SkColorType colorType = kRGBA_8888_SkColorType;

    std::shared_ptr<FrameConfig> frameConfig = frame->frameConfig;
    if (frameConfig == nullptr) {
        LOGE("Failed to acquire surface in gles, frameConfig is nullptr");
        return nullptr;
    }
    GrBackendRenderTarget backendRenderTarget(frameConfig->width, frameConfig->height, 0, STENCIL_BUFFER_SIZE,
        framebufferInfo);
#if defined(NEW_SKIA)
    SkSurfaceProps surfaceProps(0, kRGB_H_SkPixelGeometry);
#else
    SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;
#endif

    sk_sp<SkColorSpace> skColorSpace = GetSkColorSpace(frame);
    RSTagTracker tagTracker(grContext, RSTagTracker::TAGTYPE::TAG_ACQUIRE_SURFACE);
    sk_sp<SkSurface> skSurface = SkSurface::MakeFromBackendRenderTarget(
        grContext, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType,
        skColorSpace, &surfaceProps);
    if (skSurface == nullptr) {
        LOGE("Failed to acquire surface in gles, skSurface is nullptr");
        return nullptr;
    }
    LOGD("Acquire Surface In GLES Successfully!");
    return skSurface;
}

sk_sp<SkSurface> DrawingContext::AcquireSurfaceInRaster(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    std::shared_ptr<FrameConfig> frameConfig = frame->frameConfig;
    if (frameConfig == nullptr) {
        LOGE("Failed to acquire surface in raster, frameConfig is nullptr");
        return nullptr;
    }
    sptr<SurfaceBuffer> buffer = frameConfig->buffer;
    if ((buffer == nullptr) || (buffer->GetWidth() <= 0) || (buffer->GetHeight() <= 0)) {
        LOGE("Failed to acquire surface in raster, buffer is invalide");
        return nullptr;
    }

    auto addr = static_cast<uint32_t*>(buffer->GetVirAddr());
    if (addr == nullptr) {
        LOGE("Failed to acquire surface in raster, buffer addr is invalid");
        return nullptr;
    }
    SkImageInfo info =
        SkImageInfo::Make(buffer->GetWidth(), buffer->GetHeight(), kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    sk_sp<SkSurface> skSurface = SkSurface::MakeRasterDirect(info, addr, buffer->GetStride());
    LOGD("Acquire Surface In Raster Successfully!");
    return skSurface;
}

sk_sp<SkSurface> DrawingContext::AcquireSurfaceInVulkan(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return nullptr;
    }
    VulkanState* vulkanState = frame->vulkanState;
    if (vulkanState == nullptr) {
        LOGE("Failed to acquire surface in vulkan, vulkanState is nullptr");
        return nullptr;
    }
    std::shared_ptr<vulkan::VulkanWindow> vulkanWindow = vulkanState->vulkanWindow;
    if (vulkanWindow == nullptr) {
        LOGE("Failed to acquire surface in vulkan, vulkanWindow is nullptr");
        return nullptr;
    }
    LOGD("Acquire Surface In Vulkan Successfully!");
    return vulkanWindow->AcquireSurface();
#else
    return nullptr;
#endif
}

sk_sp<SkColorSpace> DrawingContext::GetSkColorSpace(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    sk_sp<SkColorSpace> skColorSpace = nullptr;
    std::shared_ptr<FrameConfig> frameConfig = frame->frameConfig;
    if (frameConfig == nullptr) {
        LOGE("Failed to get sk color space, frameConfig is nullptr");
        return nullptr;
    }

    GraphicColorGamut colorSpace = frameConfig->colorSpace;
    switch (colorSpace) {
        // [planning] in order to stay consistant with the colorspace used before, we disabled
        // GRAPHIC_COLOR_GAMUT_SRGB to let the branch to default, then skColorSpace is set to nullptr
        case GRAPHIC_COLOR_GAMUT_DISPLAY_P3:
#if defined(NEW_SKIA)
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDisplayP3);
#else
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDCIP3);
#endif
            break;
        case GRAPHIC_COLOR_GAMUT_ADOBE_RGB:
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kAdobeRGB);
            break;
        case GRAPHIC_COLOR_GAMUT_BT2020:
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kRec2020);
            break;
        default:
            break;
    }
    return skColorSpace;
}
}
}