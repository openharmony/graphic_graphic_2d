/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "common/rs_optional_trace.h"
#include "display_engine/rs_luminance_control.h"
#ifdef RS_ENABLE_GPU
#include "drawable/rs_screen_render_node_drawable.h"
#endif
#include <memory>
#include "memory/rs_tag_tracker.h"
#include "metadata_helper.h"
#include "pipeline/render_thread/rs_divided_render_util.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#if (defined(RS_ENABLE_GPU) && defined(RS_ENABLE_GL))
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "feature/gpuComposition/rs_image_manager.h"
#endif
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "render/rs_colorspace_convert.h"
#endif
#include "render/rs_drawing_filter.h"
#include "render/rs_skia_filter.h"
#include "rs_base_render_engine.h"
#include "feature/hdr/rs_hdr_util.h"
#ifdef RS_ENABLE_EGLIMAGE
#ifdef USE_M133_SKIA
#include "src/gpu/ganesh/gl/GrGLDefines.h"
#else
#include "src/gpu/gl/GrGLDefines.h"
#endif
#endif
#include "v2_1/cm_color_space.h"

namespace OHOS {
namespace Rosen {
constexpr float DEFAULT_DISPLAY_NIT = 500.0f;
constexpr float DEGAMMA = 1.0f / 2.2f;

std::vector<RectI> RSRenderFrame::CheckAndVerifyDamageRegion(
    const std::vector<RectI>& rects, const RectI& surfaceRect) const
{
    std::vector<RectI> dstRects;
    for (const auto& rect : rects) {
        dstRects.emplace_back(rect.IntersectRect(surfaceRect));
    }
    return dstRects;
}

RSBaseRenderEngine::RSBaseRenderEngine()
{
}

RSBaseRenderEngine::~RSBaseRenderEngine() noexcept
{
}

void RSBaseRenderEngine::Init()
{
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
    renderContext_ = std::make_shared<RenderContext>();
#ifdef RS_ENABLE_GL
    if (!RSSystemProperties::IsUseVulkan()) {
        renderContext_->InitializeEglContext();
    }
#endif
    if (RSUniRenderJudgement::IsUniRender()) {
        RS_LOGI("RSRenderEngine::RSRenderEngine set new cacheDir");
        renderContext_->SetUniRenderMode(true);
    }
#if defined(RS_ENABLE_VK)
    if (RSSystemProperties::IsUseVulkan()) {
        skContext_ = RsVulkanContext::GetSingleton().CreateDrawingContext();
        renderContext_->SetUpGpuContext(skContext_);
    } else {
        renderContext_->SetUpGpuContext();
    }
#else
    renderContext_->SetUpGpuContext();
#endif
#endif // RS_ENABLE_GL || RS_ENABLE_VK
#if (defined(RS_ENABLE_EGLIMAGE) && defined(RS_ENABLE_GPU)) || defined(RS_ENABLE_VK)
    imageManager_ = RSImageManager::Create(renderContext_);
#endif // RS_ENABLE_EGLIMAGE
#ifdef USE_VIDEO_PROCESSING_ENGINE
    colorSpaceConverterDisplay_ = Media::VideoProcessingEngine::ColorSpaceConverterDisplay::Create();
#endif
}

void RSBaseRenderEngine::ResetCurrentContext()
{
#ifdef RS_ENABLE_GPU
    if (renderContext_ == nullptr) {
        RS_LOGE("This render context is nullptr.");
        return;
    }
#endif
#if (defined RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        renderContext_->ShareMakeCurrentNoSurface(EGL_NO_CONTEXT);
    }
#endif

#if defined(RS_ENABLE_VK) // end RS_ENABLE_GL and enter RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        renderContext_->AbandonContext();
    }
#endif // end RS_ENABLE_GL and RS_ENABLE_VK
}

bool RSBaseRenderEngine::NeedForceCPU(const std::vector<LayerInfoPtr>& layers)
{
    bool forceCPU = false;
    for (const auto& layer: layers) {
        if (layer == nullptr) {
            continue;
        }

        auto buffer = layer->GetBuffer();
        if (buffer == nullptr) {
            continue;
        }

#ifndef RS_ENABLE_EGLIMAGE
        const auto bufferFormat = buffer->GetFormat();
        if (bufferFormat == GRAPHIC_PIXEL_FMT_YCRCB_420_SP || bufferFormat == GRAPHIC_PIXEL_FMT_YCBCR_420_SP) {
            forceCPU = true;
            break;
        }
#endif
    }

    return forceCPU;
}

std::unique_ptr<RSRenderFrame> RSBaseRenderEngine::RequestFrame(
    const std::shared_ptr<RSSurfaceOhos>& rsSurface,
    const BufferRequestConfig& config, bool forceCPU, bool useAFBC,
    const FrameContextConfig& frameContextConfig)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        skContext_ = RsVulkanContext::GetSingleton().CreateDrawingContext();
        if (renderContext_ == nullptr) {
            return nullptr;
        }
        renderContext_->SetUpGpuContext(skContext_);
    }
#endif
    if (rsSurface == nullptr) {
        RS_LOGE("RSBaseRenderEngine::RequestFrame: surface is null!");
        return nullptr;
    }
    RS_OPTIONAL_TRACE_BEGIN("RSBaseRenderEngine::RequestFrame(RSSurface)");
#ifdef RS_ENABLE_VK
    RSTagTracker tagTracker(skContext_, RSTagTracker::TAGTYPE::TAG_ACQUIRE_SURFACE);
#endif
    rsSurface->SetColorSpace(config.colorGamut);
    rsSurface->SetSurfacePixelFormat(config.format);
    if (frameContextConfig.isVirtual) {
        RS_LOGD("RSBaseRenderEngine::RequestFrame: Mirror Screen Set Timeout to 0.");
        rsSurface->SetTimeOut(frameContextConfig.timeOut);
    }
    auto bufferUsage = config.usage;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
    if (forceCPU) {
        bufferUsage |= BUFFER_USAGE_CPU_WRITE;
    }
#else
    bufferUsage |= BUFFER_USAGE_CPU_WRITE;
#endif
    if (frameContextConfig.isProtected) {
        bufferUsage |= BUFFER_USAGE_PROTECTED;
    }
    rsSurface->SetSurfaceBufferUsage(bufferUsage);

    // check if we can use GPU context
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL &&
        renderContext_ != nullptr) {
        rsSurface->SetRenderContext(renderContext_.get());
    }
#endif
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan() && skContext_ != nullptr) {
        std::static_pointer_cast<RSSurfaceOhosVulkan>(rsSurface)->SetSkContext(skContext_);
    }
#endif
    auto surfaceFrame = rsSurface->RequestFrame(config.width, config.height, 0, useAFBC,
        frameContextConfig.isProtected);
    RS_OPTIONAL_TRACE_END();
    if (surfaceFrame == nullptr) {
        RS_LOGE("RSBaseRenderEngine::RequestFrame: request SurfaceFrame failed!");
        return nullptr;
    }
    return std::make_unique<RSRenderFrame>(rsSurface, std::move(surfaceFrame));
}

std::unique_ptr<RSRenderFrame> RSBaseRenderEngine::RequestFrame(const sptr<Surface>& targetSurface,
    const BufferRequestConfig& config, bool forceCPU, bool useAFBC,
    const FrameContextConfig& frameContextConfig)
{
    RS_OPTIONAL_TRACE_BEGIN("RSBaseRenderEngine::RequestFrame(targetSurface)");
    if (targetSurface == nullptr) {
        RS_LOGE("RSBaseRenderEngine::RequestFrame: surface is null!");
        RS_OPTIONAL_TRACE_END();
        return nullptr;
    }

    std::shared_ptr<RSSurfaceOhos> rsSurface = nullptr;
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        if (forceCPU) {
            rsSurface = std::make_shared<RSSurfaceOhosRaster>(targetSurface);
        } else {
            rsSurface = std::make_shared<RSSurfaceOhosGl>(targetSurface);
        }
    }
#endif
#if (defined RS_ENABLE_VK)
    if (RSSystemProperties::IsUseVulkan()) {
        rsSurface = std::make_shared<RSSurfaceOhosVulkan>(targetSurface);
    }
#endif
    if (rsSurface == nullptr) {
        rsSurface = std::make_shared<RSSurfaceOhosRaster>(targetSurface);
    }
    RS_OPTIONAL_TRACE_END();
    return RequestFrame(rsSurface, config, forceCPU, useAFBC, frameContextConfig);
}

std::shared_ptr<RSSurfaceOhos> RSBaseRenderEngine::MakeRSSurface(const sptr<Surface>& targetSurface, bool forceCPU)
{
    RS_TRACE_FUNC();
    if (targetSurface == nullptr) {
        RS_LOGE("RSBaseRenderEngine::MakeRSSurface: surface is null!");
        RS_OPTIONAL_TRACE_END();
        return nullptr;
    }

    std::shared_ptr<RSSurfaceOhos> rsSurface = nullptr;
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        if (forceCPU) {
            rsSurface = std::make_shared<RSSurfaceOhosRaster>(targetSurface);
        } else {
            rsSurface = std::make_shared<RSSurfaceOhosGl>(targetSurface);
        }
    }
#endif
#if (defined RS_ENABLE_VK)
    if (RSSystemProperties::IsUseVulkan()) {
        rsSurface = std::make_shared<RSSurfaceOhosVulkan>(targetSurface);
    }
#endif
    if (rsSurface == nullptr) {
        rsSurface = std::make_shared<RSSurfaceOhosRaster>(targetSurface);
    }
    return rsSurface;
}

void RSBaseRenderEngine::SetUiTimeStamp(const std::unique_ptr<RSRenderFrame>& renderFrame,
    std::shared_ptr<RSSurfaceOhos> surfaceOhos)
{
    if (surfaceOhos == nullptr) {
        RS_LOGE("RSBaseRenderEngine::SetUiTimeStamp: surfaceOhos is null!");
        return;
    }

    if (renderFrame == nullptr) {
        RS_LOGE("RSBaseRenderEngine::SetUiTimeStamp: renderFrame is null!.");
        return;
    }

    auto& frame = renderFrame->GetFrame();
    surfaceOhos->SetUiTimeStamp(frame);
}

void RSBaseRenderEngine::DrawScreenNodeWithParams(RSPaintFilterCanvas& canvas, RSScreenRenderNode& node,
    BufferDrawParam& params)
{
    if (params.useCPU) {
        DrawBuffer(canvas, params);
    }
#ifdef RS_ENABLE_GPU
    else {
        auto drawable = node.GetRenderDrawable();
        if (!drawable) {
            return;
        }
        auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(drawable);
        RegisterDeleteBufferListener(screenDrawable->GetRSSurfaceHandlerOnDraw()->GetConsumer());
        DrawImage(canvas, params);
    }
#endif
}

void RSBaseRenderEngine::DrawScreenNodeWithParams(RSPaintFilterCanvas& canvas, RSSurfaceHandler& surfaceHandler,
    BufferDrawParam& drawParam)
{
    if (drawParam.useCPU) {
        DrawBuffer(canvas, drawParam);
    } else {
        RegisterDeleteBufferListener(surfaceHandler.GetConsumer());
        DrawImage(canvas, drawParam);
    }
}

void RSBaseRenderEngine::SetColorFilterMode(ColorFilterMode mode)
{
    std::lock_guard<std::mutex> lock(colorFilterMutex_);
    uint32_t uMode = static_cast<uint32_t>(mode);
    uint32_t uInvertMode = static_cast<uint32_t>(ColorFilterMode::INVERT_COLOR_ENABLE_MODE);
    uint32_t ucolorFilterMode = static_cast<uint32_t>(colorFilterMode_);
    bool isInvertModeEnabled = ((ucolorFilterMode & uInvertMode) == uInvertMode);

    switch (mode) {
        case ColorFilterMode::INVERT_COLOR_DISABLE_MODE: {
            if (colorFilterMode_ != ColorFilterMode::COLOR_FILTER_END) {
                colorFilterMode_ = static_cast<ColorFilterMode>(ucolorFilterMode & ~uInvertMode);
            }
            break;
        }
        case ColorFilterMode::INVERT_COLOR_ENABLE_MODE: {
            if (colorFilterMode_ != ColorFilterMode::COLOR_FILTER_END) {
                colorFilterMode_ = static_cast<ColorFilterMode>(ucolorFilterMode | uInvertMode);
            } else {
                colorFilterMode_ = mode;
            }
            break;
        }
        case ColorFilterMode::DALTONIZATION_PROTANOMALY_MODE:
        case ColorFilterMode::DALTONIZATION_DEUTERANOMALY_MODE:
        case ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE: {
            if (isInvertModeEnabled) {
                colorFilterMode_ = static_cast<ColorFilterMode>(uMode | uInvertMode);
            } else {
                colorFilterMode_ = mode;
            }
            break;
        }
        case ColorFilterMode::DALTONIZATION_NORMAL_MODE: {
            if (isInvertModeEnabled) {
                colorFilterMode_ = ColorFilterMode::INVERT_COLOR_ENABLE_MODE;
            } else {
                colorFilterMode_ = ColorFilterMode::COLOR_FILTER_END;
            }
            break;
        }

        // INVERT_DALTONIZATION_PROTANOMALY_MODE, INVERT_DALTONIZATION_DEUTERANOMALY_MODE
        // INVERT_DALTONIZATION_TRITANOMALY_MODE and COLOR_FILTER_END can not be set directly
        case ColorFilterMode::INVERT_DALTONIZATION_PROTANOMALY_MODE: // fall-through
        case ColorFilterMode::INVERT_DALTONIZATION_DEUTERANOMALY_MODE: // fall-through
        case ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE: // fall-through
        case ColorFilterMode::COLOR_FILTER_END: // fall-through
        default: {
            colorFilterMode_ = ColorFilterMode::COLOR_FILTER_END;
            break;
        }
    }
}

ColorFilterMode RSBaseRenderEngine::GetColorFilterMode()
{
    std::lock_guard<std::mutex> lock(colorFilterMutex_);
    return colorFilterMode_;
}

void RSBaseRenderEngine::SetHighContrast(bool enabled)
{
    isHighContrastEnabled_ = enabled;
}

bool RSBaseRenderEngine::IsHighContrastEnabled()
{
    return isHighContrastEnabled_;
}

void RSBaseRenderEngine::DrawBuffer(RSPaintFilterCanvas& canvas, BufferDrawParam& params)
{
    RS_TRACE_NAME("RSBaseRenderEngine::DrawBuffer(CPU)");
    Drawing::Bitmap bitmap;
    std::vector<uint8_t> newBuffer;
    if (!RSBaseRenderUtil::ConvertBufferToBitmap(params.buffer, newBuffer, params.targetColorGamut, bitmap,
        params.metaDatas)) {
        RS_LOGE("RSDividedRenderUtil::DrawBuffer: create bitmap failed.");
        return;
    }
    Drawing::Image drImage;
    drImage.BuildFromBitmap(bitmap);
    canvas.DrawImageRect(drImage, params.srcRect, params.dstRect, Drawing::SamplingOptions(),
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
}

std::shared_ptr<Drawing::ColorSpace> RSBaseRenderEngine::GetCanvasColorSpace(const RSPaintFilterCanvas& canvas)
{
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        return nullptr;
    }

    return surface->GetImageInfo().GetColorSpace();
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
bool RSBaseRenderEngine::ConvertDrawingColorSpaceToSpaceInfo(const std::shared_ptr<Drawing::ColorSpace>& colorSpace,
    HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceInfo& colorSpaceInfo)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    CM_ColorSpaceType colorSpaceType = CM_COLORSPACE_NONE;
    if (colorSpace == nullptr || colorSpace->Equals(nullptr) || colorSpace->IsSRGB()) {
        colorSpaceType = CM_SRGB_FULL;
    } else if (colorSpace->Equals(Drawing::ColorSpace::CreateRGB(
        Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3))) {
        colorSpaceType = CM_P3_FULL;
    } else if (colorSpace->Equals(Drawing::ColorSpace::CreateRGB(
        Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::ADOBE_RGB))) {
        colorSpaceType = CM_ADOBERGB_FULL;
    } else if (colorSpace->Equals(Drawing::ColorSpace::CreateRGB(
        Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::REC2020))) {
        colorSpaceType = CM_DISPLAY_BT2020_SRGB;
    } else {
        RS_LOGD("RSBaseRenderEngine::ConvertDrawingColorSpaceToSpaceInfo color space not supported");
        return false;
    }

    GSError ret = MetadataHelper::ConvertColorSpaceTypeToInfo(colorSpaceType, colorSpaceInfo);
    if (ret != GSERROR_OK) {
        RS_LOGE("RSBaseRenderEngine::ConvertDrawingColorSpaceToSpaceInfo ConvertColorSpaceTypeToInfo failed with \
            %{public}u.", ret);
        return false;
    }

    return true;
}

bool RSBaseRenderEngine::SetColorSpaceConverterDisplayParameter(
    const BufferDrawParam& params, Media::VideoProcessingEngine::ColorSpaceConverterDisplayParameter& parameter)
{
    using namespace HDI::Display::Graphic::Common::V1_0;

    CM_HDR_Metadata_Type hdrMetadataType = CM_METADATA_NONE;
    GSError ret = MetadataHelper::GetHDRMetadataType(params.buffer, hdrMetadataType);
    if (ret != GSERROR_OK) {
        RS_LOGD("RSBaseRenderEngine::ColorSpaceConvertor GetHDRMetadataType failed with %{public}u.", ret);
    }

    parameter.inputColorSpace.metadataType = hdrMetadataType;
    parameter.outputColorSpace.metadataType = hdrMetadataType;

#ifdef USE_VIDEO_PROCESSING_ENGINE
    RSColorSpaceConvert::Instance().GetHDRStaticMetadata(params.buffer, parameter.staticMetadata, ret);
    RSColorSpaceConvert::Instance().GetHDRDynamicMetadata(params.buffer, parameter.dynamicMetadata, ret);
    RSColorSpaceConvert::Instance().GetFOVMetadata(params.buffer, parameter.adaptiveFOVMetadata);
#endif

    parameter.width = params.buffer->GetWidth();
    parameter.height = params.buffer->GetHeight();
    parameter.tmoNits = params.tmoNits;
    parameter.currentDisplayNits = params.displayNits;
    parameter.sdrNits = params.sdrNits;
    // color temperature
    parameter.layerLinearMatrix = params.layerLinearMatrix;

    RS_LOGD("RSBaseRenderEngine::ColorSpaceConvertor parameter inPrimaries = %{public}u, inMetadataType = %{public}u, "
            "outPrimaries = %{public}u, outMetadataType = %{public}u, "
            "tmoNits = %{public}.2f, currentDisplayNits = %{public}.2f, sdrNits = %{public}.2f",
            parameter.inputColorSpace.colorSpaceInfo.primaries, parameter.inputColorSpace.metadataType,
            parameter.outputColorSpace.colorSpaceInfo.primaries, parameter.outputColorSpace.metadataType,
            parameter.tmoNits, parameter.currentDisplayNits, parameter.sdrNits);

    return true;
}

void RSBaseRenderEngine::ColorSpaceConvertor(std::shared_ptr<Drawing::ShaderEffect>& inputShader,
    BufferDrawParam& params, Media::VideoProcessingEngine::ColorSpaceConverterDisplayParameter& parameter,
    const RSPaintFilterCanvas::HDRProperties& hdrProperties)
{
    RS_OPTIONAL_TRACE_BEGIN("RSBaseRenderEngine::ColorSpaceConvertor");

    if (!SetColorSpaceConverterDisplayParameter(params, parameter)) {
        RS_OPTIONAL_TRACE_END();
        return;
    }

    if(!inputShader) {
        RS_LOGE("RSBaseRenderEngine::ColorSpaceConvertor inputShader is null");
        RS_OPTIONAL_TRACE_END();
        return;
    }
    if (params.isHdrRedraw) {
        parameter.disableHdrFloatHeadRoom = true;
    } else if (params.isTmoNitsFixed) {
        parameter.sdrNits = DEFAULT_DISPLAY_NIT;
        parameter.tmoNits = DEFAULT_DISPLAY_NIT;
        parameter.currentDisplayNits = DEFAULT_DISPLAY_NIT;
        parameter.layerLinearMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    } else if (hdrProperties.isHDREnabledVirtualScreen) {
        parameter.sdrNits = RSLuminanceConst::DEFAULT_CAST_SDR_NITS;
        parameter.currentDisplayNits = RSLuminanceConst::DEFAULT_CAST_HDR_NITS;
        parameter.tmoNits = RSLuminanceConst::DEFAULT_CAST_HDR_NITS;
        parameter.layerLinearMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    }

    std::shared_ptr<Drawing::ShaderEffect> outputShader;
    auto convRet = colorSpaceConverterDisplay_->Process(inputShader, outputShader, parameter);
    if (convRet != Media::VideoProcessingEngine::VPE_ALGO_ERR_OK) {
        RS_LOGE("RSBaseRenderEngine::ColorSpaceConvertor colorSpaceConverterDisplay failed with %{public}u.", convRet);
        RS_OPTIONAL_TRACE_END();
        return;
    }
    if (outputShader == nullptr) {
        RS_LOGE("RSBaseRenderEngine::ColorSpaceConvertor outputShader is nullptr.");
        RS_OPTIONAL_TRACE_END();
        return;
    }
    params.paint.SetShaderEffect(outputShader);
    RS_OPTIONAL_TRACE_END();
}
#endif

std::shared_ptr<Drawing::ColorSpace> RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(
    GraphicColorGamut colorGamut)
{
    std::shared_ptr<Drawing::ColorSpace>  colorSpace = nullptr;
    switch (colorGamut) {
        case GRAPHIC_COLOR_GAMUT_DISPLAY_P3:
        case GRAPHIC_COLOR_GAMUT_DCI_P3:
            colorSpace = Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
            break;
        case GRAPHIC_COLOR_GAMUT_ADOBE_RGB:
            colorSpace = Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::ADOBE_RGB);
            break;
        case GRAPHIC_COLOR_GAMUT_BT2020:
            colorSpace = Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::REC2020);
            break;
        default:
            colorSpace = Drawing::ColorSpace::CreateSRGB();
            break;
    }

    return colorSpace;
}

std::shared_ptr<Drawing::ColorSpace> RSBaseRenderEngine::ConvertColorSpaceNameToDrawingColorSpace(
    OHOS::ColorManager::ColorSpaceName colorSpaceName)
{
    std::shared_ptr<Drawing::ColorSpace>  colorSpace = nullptr;
    switch (colorSpaceName) {
        case OHOS::ColorManager::ColorSpaceName::DISPLAY_P3:
        case OHOS::ColorManager::ColorSpaceName::DCI_P3:
            colorSpace = Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
            break;
        case OHOS::ColorManager::ColorSpaceName::ADOBE_RGB:
            colorSpace = Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::ADOBE_RGB);
            break;
        default:
            colorSpace = Drawing::ColorSpace::CreateSRGB();
            break;
    }
    return colorSpace;
}

void RSBaseRenderEngine::DumpVkImageInfo(std::string &dumpString)
{
    if (imageManager_ != nullptr) {
        imageManager_->DumpVkImageInfo(dumpString);
    }
}

std::shared_ptr<Drawing::Image> RSBaseRenderEngine::CreateImageFromBuffer(RSPaintFilterCanvas& canvas,
    BufferDrawParam& params, VideoInfo& videoInfo)
{
    std::shared_ptr<Drawing::AutoCanvasRestore> acr = nullptr;
    if (params.preRotation) {
        RS_LOGD_IF(DEBUG_COMPOSER, "  - Applying pre-rotation, resetting canvas matrix");
        acr = std::make_shared<Drawing::AutoCanvasRestore>(canvas, true);
        canvas.ResetMatrix();
    }

    std::shared_ptr<Drawing::Image> image;
    if (!RSBaseRenderUtil::IsBufferValid(params.buffer)) {
        RS_LOGE("RSBaseRenderEngine::CreateImageFromBuffer invalid buffer!");
        return nullptr;
    }
    RS_LOGD_IF(DEBUG_COMPOSER,
        "  - Buffer info: width=%{public}u, height=%{public}u, format=%{public}d, seqNum=%{public}u",
        params.buffer->GetWidth(), params.buffer->GetHeight(),
        params.buffer->GetFormat(), params.buffer->GetSeqNum());
    videoInfo.drawingColorSpace_ = Drawing::ColorSpace::CreateSRGB();
#ifdef USE_VIDEO_PROCESSING_ENGINE
    videoInfo.parameter_ = {};
    videoInfo.retGetColorSpaceInfo_ = MetadataHelper::GetColorSpaceInfo(params.buffer,
        videoInfo.parameter_.inputColorSpace.colorSpaceInfo);
    if (videoInfo.retGetColorSpaceInfo_ == GSERROR_OK || params.colorFollow) {
        videoInfo.drawingColorSpace_ = GetCanvasColorSpace(canvas);
    }
#endif
#if defined(RS_ENABLE_VK) || defined(RS_ENABLE_GL)
    if (imageManager_ == nullptr) {
        RS_LOGE("RSBaseRenderEngine::CreateImageFromBuffer: imageManager is nullptr!");
        return nullptr;
    }
    image = imageManager_->CreateImageFromBuffer(canvas,
        params.buffer, params.acquireFence, params.threadIndex,
        videoInfo.drawingColorSpace_);
    if (image == nullptr) {
        RS_LOGE("RSBaseRenderEngine::CreateImageFromBuffer: vk image is nullptr!");
        return nullptr;
    }
#endif
    RS_LOGD_IF(DEBUG_COMPOSER,
        "RSBaseRenderEngine::CreateImageFromBuffer: Image creation successful, size: %{public}d x %{public}d",
        image->GetWidth(), image->GetHeight());
    return image;
}

void RSBaseRenderEngine::DrawImage(RSPaintFilterCanvas& canvas, BufferDrawParam& params)
{
    RS_TRACE_NAME_FMT("RSBaseRenderEngine::DrawImage(GPU) targetColorGamut=%d", params.targetColorGamut);

    RS_LOGD_IF(DEBUG_COMPOSER, "RSBaseRenderEngine::DrawImage: Starting to draw image with gamut:%{public}d, "
        "src:[%{public}.2f,%{public}.2f,%{public}.2f,%{public}.2f],"
        "dst:[%{public}.2f,%{public}.2f,%{public}.2f,%{public}.2f], "
        "useCPU:%{public}d, isMirror:%{public}d, useBilinear:%{public}d",
        params.targetColorGamut, params.srcRect.GetLeft(), params.srcRect.GetTop(), params.srcRect.GetRight(),
        params.srcRect.GetBottom(), params.dstRect.GetLeft(), params.dstRect.GetTop(), params.dstRect.GetRight(),
        params.dstRect.GetBottom(), params.useCPU, params.isMirror, params.useBilinearInterpolation);
#ifdef USE_VIDEO_PROCESSING_ENGINE
    RS_LOGD_IF(DEBUG_COMPOSER, "  - Video processing: brightness:%{public}.2f, hasMetadata:%{public}d",
        params.brightnessRatio, params.hasMetadata);
#endif

    RSMainThread::GPUCompositonCacheGuard guard;
    VideoInfo videoInfo;
    auto image = CreateImageFromBuffer(canvas, params, videoInfo);
    if (image == nullptr) {
        return;
    }

    // add for tv metadata
    Drawing::SamplingOptions samplingOptions;
    if (!RSSystemProperties::GetUniRenderEnabled()) {
        samplingOptions = Drawing::SamplingOptions();
        RS_LOGD_IF(DEBUG_COMPOSER, "  - Sampling options: Default sampling options (UniRender not enabled)");
    } else {
        if (params.isMirror) {
            samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NEAREST);
            RS_LOGD_IF(DEBUG_COMPOSER, "  - Sampling options: Mirror mode (LINEAR, NEAREST)");
        } else {
            bool needBilinear = NeedBilinearInterpolation(params, canvas.GetTotalMatrix());
            samplingOptions = needBilinear
                ? Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE)
                : Drawing::SamplingOptions();
            RS_LOGD_IF(DEBUG_COMPOSER, "  - Sampling options: %{public}s",
                needBilinear ? "Bilinear interpolation (LINEAR, NONE)" : "Default sampling options");
        }
    }

    {
        // Record canvas transformation matrix information
        auto matrix = canvas.GetTotalMatrix();
        RS_LOGD_IF(DEBUG_COMPOSER, "  - Canvas transformation matrix:"
            "[%{public}.2f, %{public}.2f, %{public}.2f, %{public}.2f, %{public}.2f, %{public}.2f]",
            matrix.Get(Drawing::Matrix::SCALE_X), matrix.Get(Drawing::Matrix::SKEW_X),
            matrix.Get(Drawing::Matrix::TRANS_X), matrix.Get(Drawing::Matrix::SKEW_Y),
            matrix.Get(Drawing::Matrix::SCALE_Y), matrix.Get(Drawing::Matrix::TRANS_Y));
    }

    if (params.targetColorGamut == GRAPHIC_COLOR_GAMUT_SRGB) {
        RS_LOGD_IF(DEBUG_COMPOSER, "  - Using SRGB color gamut for drawing");
        canvas.AttachBrush(params.paint);
        canvas.DrawImageRect(*image, params.srcRect, params.dstRect, samplingOptions,
            Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
        canvas.DetachBrush();
        RS_LOGD_IF(DEBUG_COMPOSER, "RSBaseRenderEngine::DrawImage: SRGB color gamut drawing completed");
    } else {
#ifdef USE_VIDEO_PROCESSING_ENGINE

    if (params.isHeterog) {
        RS_LOGD("hdr video comin heterog");
        float hrRatio = std::pow((params.displayNits / params.sdrNits), DEGAMMA);
        Drawing::Matrix scaleMat;
        auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
            Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), scaleMat);
        
        RSHdrUtil util;
        auto shader = util.MakeHdrHeadroomShader(hrRatio, imageShader);
        if (shader == nullptr) {
            RS_LOGE("RSHdrUtil::MakeHdrHeadroomShader shader is null");
            return;
        }
        params.paint.SetShaderEffect(shader);
 
        canvas.AttachBrush(params.paint);
        canvas.DrawRect(params.dstRect);
        canvas.DetachBrush();
        return;
    }

    // For sdr brightness ratio
    if (ROSEN_LNE(params.brightnessRatio, DEFAULT_BRIGHTNESS_RATIO) && !params.isHdrRedraw) {
        RS_LOGD_IF(DEBUG_COMPOSER, "  - Applying brightness ratio: %{public}.2f", params.brightnessRatio);
        Drawing::Filter filter = params.paint.GetFilter();
        Drawing::ColorMatrix luminanceMatrix;
        luminanceMatrix.SetScale(params.brightnessRatio, params.brightnessRatio, params.brightnessRatio, 1.0f);
        auto luminanceColorFilter =
            std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::MATRIX, luminanceMatrix);
        filter.SetColorFilter(luminanceColorFilter);
        params.paint.SetFilter(filter);
    }

    if (videoInfo.retGetColorSpaceInfo_ != GSERROR_OK) {
        RS_LOGD("RSBaseRenderEngine::DrawImage GetColorSpaceInfo failed with %{public}u.",
            videoInfo.retGetColorSpaceInfo_);
        DrawImageRect(canvas, image, params, samplingOptions);
        return;
    }

    auto& inClrInfo = videoInfo.parameter_.inputColorSpace.colorSpaceInfo;
    auto& outClrInfo = videoInfo.parameter_.outputColorSpace.colorSpaceInfo;
    RS_LOGD_IF(DEBUG_COMPOSER, "  - Input color space: primaries=%{public}d, transfunc=%{public}d",
        inClrInfo.primaries, inClrInfo.transfunc);
    if (!ConvertDrawingColorSpaceToSpaceInfo(videoInfo.drawingColorSpace_, outClrInfo)) {
        RS_LOGD("RSBaseRenderEngine::DrawImage ConvertDrawingColorSpaceToSpaceInfo failed");
        DrawImageRect(canvas, image, params, samplingOptions);
        return;
    }

    if (params.colorFollow) {
        // force input and output color spaces to be consistent to avoid color space conversion
        RS_LOGD("RSBaseRenderEngine::DrawImage force to avoid color space conversion");
        DrawImageRect(canvas, image, params, samplingOptions);
        return;
    }

    if (inClrInfo.primaries == outClrInfo.primaries && inClrInfo.transfunc == outClrInfo.transfunc &&
        !params.hasMetadata) {
        RS_LOGD("RSBaseRenderEngine::DrawImage primaries and transfunc equal with no metadata.");
        DrawImageRect(canvas, image, params, samplingOptions);
        return;
    }

    auto& rsLuminance = RSLuminanceControl::Get();
    // Fix tonemapping: all xxxNits reset to 500, layerLinearMatrix reset to 3x3 Identity matrix
    params.isTmoNitsFixed = canvas.IsOnMultipleScreen() ||
        (!canvas.GetHdrOn() &&
        !params.hasMetadata &&
        !rsLuminance.IsScreenNoHeadroom(canvas.GetScreenId())) ||
        !RSSystemProperties::GetHdrVideoEnabled();
    RS_LOGD_IF(DEBUG_COMPOSER, "- Fix tonemapping: %{public}d", params.isTmoNitsFixed);

    Drawing::Matrix matrix;
    auto srcWidth = params.srcRect.GetWidth();
    auto srcHeight = params.srcRect.GetHeight();
    auto sx = params.dstRect.GetWidth() / srcWidth;
    auto sy = params.dstRect.GetHeight() / srcHeight;
    auto tx = params.dstRect.GetLeft() - params.srcRect.GetLeft() * sx;
    auto ty = params.dstRect.GetTop() - params.srcRect.GetTop() * sy;
    if (ROSEN_EQ(srcWidth, 0.0f) || ROSEN_EQ(srcHeight, 0.0f)) {
        RS_LOGE("RSBaseRenderEngine::DrawImage image srcRect params invalid.");
    }
    matrix.SetScaleTranslate(sx, sy, tx, ty);

    RS_LOGD_IF(DEBUG_COMPOSER, "  - Image shader transformation:"
        "sx=%{public}.2f, sy=%{public}.2f, tx=%{public}.2f, ty=%{public}.2f", sx, sy, tx, ty);

    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, matrix);
    if (imageShader == nullptr) {
        RS_LOGW("RSBaseRenderEngine::DrawImage imageShader is nullptr.");
    } else {
        params.paint.SetShaderEffect(imageShader);
        ColorSpaceConvertor(imageShader, params, videoInfo.parameter_, canvas.GetHDRProperties());
    }
    canvas.AttachBrush(params.paint);
    canvas.DrawRect(params.dstRect);
    canvas.DetachBrush();
#else
    DrawImageRect(canvas, image, params, samplingOptions);
#endif // USE_VIDEO_PROCESSING_ENGINE
    }
}

void RSBaseRenderEngine::DrawImageRect(RSPaintFilterCanvas& canvas, std::shared_ptr<Drawing::Image> image,
    BufferDrawParam& params, Drawing::SamplingOptions& samplingOptions)
{
    RS_LOGD_IF(DEBUG_COMPOSER, "RSBaseRenderEngine::DrawImageRect: rect image src:[%{public}.2f,%{public}.2f,"
        "%{public}.2f,%{public}.2f], dst:[%{public}.2f,%{public}.2f,%{public}.2f,%{public}.2f]",
        params.srcRect.GetLeft(), params.srcRect.GetTop(), params.srcRect.GetRight(), params.srcRect.GetBottom(),
        params.dstRect.GetLeft(), params.dstRect.GetTop(), params.dstRect.GetRight(), params.dstRect.GetBottom());
    // Record sampling options information
    RS_LOGD_IF(DEBUG_COMPOSER,
        "  - Sampling filter mode: %{public}d, Sampling mipmap mode: %{public}d",
        static_cast<int>(samplingOptions.GetFilterMode()), static_cast<int>(samplingOptions.GetMipmapMode()));

    canvas.AttachBrush(params.paint);
    canvas.DrawImageRect(*image, params.srcRect, params.dstRect, samplingOptions,
        Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    canvas.DetachBrush();
}

bool RSBaseRenderEngine::NeedBilinearInterpolation(const BufferDrawParam& params, const Drawing::Matrix& matrix)
{
    if (!params.useBilinearInterpolation) {
        return false;
    }
    if (!ROSEN_EQ(params.dstRect.GetWidth(), params.srcRect.GetWidth()) ||
        !ROSEN_EQ(params.dstRect.GetHeight(), params.srcRect.GetHeight())) {
        return true;
    }
    auto scaleX = matrix.Get(Drawing::Matrix::SCALE_X);
    auto scaleY = matrix.Get(Drawing::Matrix::SCALE_Y);
    auto skewX = matrix.Get(Drawing::Matrix::SKEW_X);
    auto skewY = matrix.Get(Drawing::Matrix::SKEW_Y);
    if (ROSEN_EQ(skewX, 0.0f) && ROSEN_EQ(skewY, 0.0f)) {
        if (!ROSEN_EQ(std::abs(scaleX), 1.0f) || !ROSEN_EQ(std::abs(scaleY), 1.0f)) {
            // has scale
            return true;
        }
    } else if (ROSEN_EQ(scaleX, 0.0f) && ROSEN_EQ(scaleY, 0.0f)) {
        if (!ROSEN_EQ(std::abs(skewX), 1.0f) || !ROSEN_EQ(std::abs(skewY), 1.0f)) {
            // has scale
            return true;
        }
    } else {
        // skew and/or non 90 degrees rotation
        return true;
    }
    return false;
}

void RSBaseRenderEngine::RegisterDeleteBufferListener(const sptr<IConsumerSurface>& consumer, bool isForUniRedraw)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        auto regUnMapVkImageFunc = [this, isForUniRedraw](int32_t bufferId) {
            RSMainThread::Instance()->AddToUnmappedCacheSet(bufferId);
        };
        if (consumer == nullptr ||
            (consumer->RegisterDeleteBufferListener(regUnMapVkImageFunc, isForUniRedraw) != GSERROR_OK)) {
            RS_LOGE("RSBaseRenderEngine::RegisterDeleteBufferListener: failed to register UnMapVkImage callback.");
        }
        return;
    }
#endif // #ifdef RS_ENABLE_VK

#if (defined(RS_ENABLE_EGLIMAGE) && defined(RS_ENABLE_GPU))
    auto regUnMapEglImageFunc = [this, isForUniRedraw](int32_t bufferId) {
        RSMainThread::Instance()->AddToUnmappedCacheSet(bufferId);
    };
    if (consumer == nullptr ||
        (consumer->RegisterDeleteBufferListener(regUnMapEglImageFunc, isForUniRedraw) != GSERROR_OK)) {
        RS_LOGE("RSBaseRenderEngine::RegisterDeleteBufferListener: failed to register UnMapEglImage callback.");
    }
#endif // #ifdef RS_ENABLE_EGLIMAGE
}

void RSBaseRenderEngine::RegisterDeleteBufferListener(RSSurfaceHandler& handler)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        auto regUnMapVkImageFunc = [this](int32_t bufferId) {
            RSMainThread::Instance()->AddToUnmappedCacheSet(bufferId);
        };
        handler.RegisterDeleteBufferListener(regUnMapVkImageFunc);
        return;
    }
#endif // #ifdef RS_ENABLE_VK

#if (defined(RS_ENABLE_EGLIMAGE) && defined(RS_ENABLE_GPU))
    auto regUnMapEglImageFunc = [this](int32_t bufferId) {
        RSMainThread::Instance()->AddToUnmappedCacheSet(bufferId);
    };
    handler.RegisterDeleteBufferListener(regUnMapEglImageFunc);
#endif // #ifdef RS_ENABLE_EGLIMAGE
}

void RSBaseRenderEngine::ShrinkCachesIfNeeded(bool isForUniRedraw)
{
#if (defined(RS_ENABLE_EGLIMAGE) && defined(RS_ENABLE_GPU))
    if (imageManager_ != nullptr) {
        imageManager_->ShrinkCachesIfNeeded(isForUniRedraw);
    }
#endif // RS_ENABLE_EGLIMAGE
}

void RSBaseRenderEngine::ClearCacheSet(const std::set<uint32_t>& unmappedCache)
{
    if (imageManager_ != nullptr) {
        for (auto id : unmappedCache) {
            imageManager_->UnMapImageFromSurfaceBuffer(id);
        }
    }
}
} // namespace Rosen
} // namespace OHOS
