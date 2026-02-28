/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_property_drawable_background.h"

#include "common/rs_background_thread.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#ifdef ROSEN_OHOS
#include "common/rs_common_tools.h"
#endif
#include "drawable/rs_property_drawable_utils.h"
#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "effect/runtime_blender_builder.h"
#include "gfx/performance/rs_perfmonitor_reporter.h"
#include "memory/rs_tag_tracker.h"
#ifdef ROSEN_OHOS
#include "native_buffer_inner.h"
#include "native_window.h"
#endif
#include "modifier_ng/rs_render_modifier_ng.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_task_dispatcher.h"
#include "platform/common/rs_log.h"
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#include "render/rs_drawing_filter.h"
#include "ge_render.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/vk/GrVkBackendSemaphore.h"
#else
#include "include/gpu/GrBackendSemaphore.h"
#endif
#endif
#include "common/rs_rect.h"

namespace OHOS::Rosen {
class RSSDFEffectFilter;
namespace DrawableV2 {
namespace {
constexpr int TRACE_LEVEL_TWO = 2;
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_VK) || defined(RS_ENABLE_GL))
constexpr uint8_t ASTC_HEADER_SIZE = 16;
#endif
}

RSDrawable::Ptr RSShadowDrawable::OnGenerate(const RSRenderNode& node)
{
    // skip shadow if not valid. ShadowMask is processed by foreground
    if (!node.GetRenderProperties().IsShadowValid() || node.GetRenderProperties().GetShadowMask()) {
        return nullptr;
    }
    RSDrawable::Ptr ret = std::make_shared<RSShadowDrawable>();
    if (ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSShadowDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    // skip shadow if not valid. ShadowMask is processed by foreground
    if (!properties.IsShadowValid() || properties.GetShadowMask()) {
        return false;
    }

    stagingPath_ = RSPropertyDrawableUtils::CreateShadowPath(properties.GetShadowPath(),
        properties.GetClipBounds(), properties.GetRRect());
    stagingOffsetX_ = properties.GetShadowOffsetX();
    stagingOffsetY_ = properties.GetShadowOffsetY();
    stagingElevation_ = properties.GetShadowElevation();
    stagingColor_ = properties.GetShadowColor();
    stagingIsFilled_ = properties.GetShadowIsFilled();
    stagingColorStrategy_ = properties.GetShadowColorStrategy();
    stagingRadius_ = properties.GetShadowRadius();
    stagingDisableSDFBlur_ = properties.GetShadowDisableSDFBlur();
    needSync_ = true;

    stagingGeContainer_ = nullptr;
    if (auto sdfShape = properties.GetSDFShape()) {
        std::shared_ptr<Drawing::GEVisualEffect> geVisualEffect = sdfShape->GenerateGEVisualEffect();
        std::shared_ptr<Drawing::GEShaderShape> geShape =
            geVisualEffect ? geVisualEffect->GenerateShaderShape() : nullptr;
        auto geFilter = std::make_shared<Drawing::GEVisualEffect>(
            Drawing::GE_SHADER_SDF_SHADOW, Drawing::DrawingPaintType::BRUSH);
        geFilter->SetParam(Drawing::GE_SHADER_SDF_SHADOW_SHAPE, geShape);

        Drawing::Color color(stagingColor_.GetRed(), stagingColor_.GetGreen(),
            stagingColor_.GetBlue(), stagingColor_.GetAlpha());
        Drawing::GESDFShadowParams shadow {color, stagingOffsetX_, stagingOffsetY_,
            stagingRadius_, stagingPath_, stagingIsFilled_};
        geFilter->SetParam(Drawing::GE_SHADER_SDF_SHADOW_SHADOW, shadow);
        stagingGeContainer_ = std::make_shared<Drawing::GEVisualEffectContainer>();
        stagingGeContainer_->AddToChainedFilter(geFilter);
    }
    return true;
}

void RSShadowDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    path_ = std::move(stagingPath_);
    color_ = std::move(stagingColor_);
    offsetX_ = stagingOffsetX_;
    offsetY_ = stagingOffsetY_;
    elevation_ = stagingElevation_;
    isFilled_ = stagingIsFilled_;
    radius_ = stagingRadius_;
    colorStrategy_ = stagingColorStrategy_;
    disableSDFBlur_ = stagingDisableSDFBlur_;
    geContainer_ = std::move(stagingGeContainer_);
    needSync_ = false;
}

void RSShadowDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (geContainer_) {
        if (canvas && rect) {
            auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
            geRender->DrawShaderEffect(*canvas, *geContainer_, *rect);
        }
        return;
    }

    // skip shadow if cache is enabled
    if (canvas->GetCacheType() == Drawing::CacheType::ENABLED) {
        ROSEN_LOGD("RSShadowDrawable::OnDraw cache type enabled.");
        return;
    }
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
        RSTagTracker::SOURCETYPE::SOURCE_RSSHADOWDRAWABLE);
#endif
    Drawing::Path path = path_;
    Color shadowColor = color_;
    if (colorStrategy_ != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE) {
        shadowColor = RSPropertyDrawableUtils::GetColorForShadowSyn(canvas, path,
            color_, colorStrategy_);
    }
    if (ROSEN_GNE(elevation_, 0.f)) {
        RSPropertyDrawableUtils::DrawShadow(canvas, path, offsetX_, offsetY_,
            elevation_, isFilled_, shadowColor);
        return;
    }
    if (ROSEN_GNE(radius_, 0.f)) {
        RSPropertyDrawableUtils::DrawShadowMaskFilter(canvas, path, offsetX_, offsetY_,
            radius_, isFilled_, shadowColor, disableSDFBlur_);
        return;
    }
}

RSDrawable::Ptr RSMaskDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSMaskDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSMaskDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    std::shared_ptr<RSMask> mask = properties.GetMask();
    if (mask == nullptr) {
        ROSEN_LOGE("RSMaskDrawable::OnUpdate null mask");
        return false;
    }
    if (mask->IsSvgMask() && !mask->GetSvgDom() && !mask->GetSvgPicture()) {
        ROSEN_LOGE("RSMaskDrawable::OnUpdate not has Svg Mask property");
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    Drawing::Rect maskBounds = RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetBoundsRect());
    canvas.Save();
    Drawing::SaveLayerOps slr(&maskBounds, nullptr);
    canvas.SaveLayer(slr);
    uint32_t tmpLayer = canvas.GetSaveCount();

    Drawing::Brush maskfilter;
    Drawing::Filter filter;
    filter.SetColorFilter(Drawing::ColorFilter::CreateComposeColorFilter(
        *(Drawing::ColorFilter::CreateLumaColorFilter()), *(Drawing::ColorFilter::CreateSrgbGammaToLinear())));
    maskfilter.SetFilter(filter);
    Drawing::SaveLayerOps slrMask(&maskBounds, &maskfilter);
    canvas.SaveLayer(slrMask);
    if (mask->IsSvgMask()) {
        Drawing::AutoCanvasRestore maskSave(canvas, true);
        canvas.Translate(maskBounds.GetLeft() + mask->GetSvgX(), maskBounds.GetTop() + mask->GetSvgY());
        canvas.Scale(mask->GetScaleX(), mask->GetScaleY());
        if (mask->GetSvgDom()) {
            canvas.DrawSVGDOM(mask->GetSvgDom());
        } else if (mask->GetSvgPicture()) {
            canvas.DrawPicture(*mask->GetSvgPicture());
        }
    } else if (mask->IsGradientMask()) {
        Drawing::AutoCanvasRestore maskSave(canvas, true);
        canvas.Translate(maskBounds.GetLeft(), maskBounds.GetTop());
        Drawing::Rect rect = Drawing::Rect(0, 0, maskBounds.GetWidth(), maskBounds.GetHeight());
        canvas.AttachBrush(mask->GetMaskBrush());
        canvas.DrawRect(rect);
        canvas.DetachBrush();
    } else if (mask->IsPathMask()) {
        Drawing::AutoCanvasRestore maskSave(canvas, true);
        canvas.Translate(maskBounds.GetLeft(), maskBounds.GetTop());
        canvas.AttachBrush(mask->GetMaskBrush());
        canvas.AttachPen(mask->GetMaskPen());
        canvas.DrawPath(*mask->GetMaskPath());
        canvas.DetachBrush();
        canvas.DetachPen();
    } else if (mask->IsPixelMapMask() && mask->GetImage()) {
        Drawing::AutoCanvasRestore maskSave(canvas, true);
        canvas.DrawImage(*mask->GetImage(), 0.f, 0.f, Drawing::SamplingOptions());
    }

    // back to mask layer
    canvas.RestoreToCount(tmpLayer);
    // create content layer
    Drawing::Brush maskPaint;
    maskPaint.SetBlendMode(Drawing::BlendMode::SRC_IN);
    Drawing::SaveLayerOps slrContent(&maskBounds, &maskPaint);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(maskBounds, Drawing::ClipOp::INTERSECT, true);
    return true;
}

// ============================================================================
// Background
RSDrawable::Ptr RSBackgroundColorDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBackgroundColorDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSBackgroundColorDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto bgColor = properties.GetBackgroundColor();
    if (bgColor == RgbPalette::Transparent()) {
        return false;
    }

    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    Drawing::Brush brush;
    if (bgColor.GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB) {
        brush.SetColor(bgColor.ConvertToDrawingColor());
    } else {
        // Currently, only P3 wide color space is supported, and it will be expanded soon.
        brush.SetColor(bgColor.GetColor4f(),
            Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3));
    }
    if (properties.IsBgBrightnessValid()) {
        if (Rosen::RSSystemProperties::GetDebugTraceLevel() >= TRACE_LEVEL_TWO) {
            RSPropertyDrawable::stagingPropertyDescription_ = properties.GetBgBrightnessDescription();
        }
        auto blender = RSPropertyDrawableUtils::MakeDynamicBrightnessBlender(
            properties.GetBgBrightnessParams().value());
        brush.SetBlender(blender);
    }
    canvas.AttachBrush(brush);
    canvas.DrawRect(RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect()));
    canvas.DetachBrush();
    return true;
}

RSDrawable::Ptr RSBackgroundShaderDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBackgroundShaderDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSBackgroundShaderDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    const auto& bgShader = properties.GetBackgroundShader();
    if (!bgShader) {
        return false;
    }

    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    Drawing::Brush brush;
    auto shaderEffect = bgShader->GetDrawingShader();
    // do not draw if shaderEffect is nullptr and keep RSShader behavior consistent
    if (shaderEffect == nullptr && bgShader->GetShaderType() != RSShader::ShaderType::DRAWING) {
        return true;
    }
    brush.SetShaderEffect(shaderEffect);
    canvas.AttachBrush(brush);
    canvas.DrawRect(RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect()));
    canvas.DetachBrush();
    return true;
}

RSDrawable::Ptr RSBackgroundNGShaderDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBackgroundNGShaderDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSBackgroundNGShaderDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    const auto& shader = properties.GetBackgroundNGShader();
    if (!shader) {
        return false;
    }
    needSync_ = true;
    stagingShader_ = shader;
    stagingCornerRadius_ = node.GetRenderProperties().GetCornerRadius().x_;
    return true;
}

void RSBackgroundNGShaderDrawable::OnSync()
{
    if (needSync_ && stagingShader_) {
        auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
        stagingShader_->AppendToGEContainer(visualEffectContainer);
        visualEffectContainer->UpdateCacheDataFrom(visualEffectContainer_);
        visualEffectContainer_ = visualEffectContainer;
        needSync_ = false;
    }
    cornerRadius_ = stagingCornerRadius_;
}

void RSBackgroundNGShaderDrawable::OnDraw(Drawing::Canvas *canvas, const Drawing::Rect *rect) const
{
    auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
    if (canvas == nullptr || visualEffectContainer_ == nullptr || rect == nullptr) {
        return;
    }
    auto effectData = RSNGRenderShaderHelper::GetCachedBlurImage(canvas);
    if (effectData != nullptr) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
        if (paintFilterCanvas->GetEffectIntersectWithDRM()) {
            RSPropertyDrawableUtils::DrawColorUsingSDFWithDRM(canvas, rect, paintFilterCanvas->GetDarkColorMode(),
                visualEffectContainer_, Drawing::GE_SHADER_FROSTED_GLASS_EFFECT,
                Drawing::GE_SHADER_FROSTED_GLASS_EFFECT_SHAPE);
            return;
        }
        visualEffectContainer_->UpdateCachedBlurImage(canvas, effectData->cachedImage_,
            effectData->cachedRect_.GetLeft(), effectData->cachedRect_.GetTop());
        visualEffectContainer_->UpdateTotalMatrix(effectData->cachedMatrix_);
    } else {
        visualEffectContainer_->UpdateCachedBlurImage(canvas, nullptr, 0, 0);
    }
    visualEffectContainer_->UpdateCornerRadius(cornerRadius_);
    geRender->DrawShaderEffect(*canvas, *visualEffectContainer_, *rect);
}

RSBackgroundImageDrawable::~RSBackgroundImageDrawable()
{
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    ReleaseNativeWindowBuffer();
#endif
}

RSDrawable::Ptr RSBackgroundImageDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBackgroundImageDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
Drawing::ColorType RSBackgroundImageDrawable::GetColorTypeFromVKFormat(VkFormat vkFormat)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return Drawing::COLORTYPE_RGBA_8888;
    }
    switch (vkFormat) {
        case VK_FORMAT_R8G8B8A8_UNORM:
            return Drawing::COLORTYPE_RGBA_8888;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return Drawing::COLORTYPE_RGBA_F16;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            return Drawing::COLORTYPE_RGB_565;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
            return Drawing::COLORTYPE_RGBA_1010102;
        default:
            return Drawing::COLORTYPE_RGBA_8888;
    }
}
#endif

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
void RSBackgroundImageDrawable::ReleaseNativeWindowBuffer()
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        if (nativeWindowBuffer_ == nullptr && cleanUpHelper_ == nullptr) {
            return;
        }
        RSTaskDispatcher::GetInstance().PostTask(
            tid_, [nativeWindowBuffer = nativeWindowBuffer_, cleanUpHelper = cleanUpHelper_]() {
                if (nativeWindowBuffer != nullptr) {
                    DestroyNativeWindowBuffer(nativeWindowBuffer);
                }
                if (cleanUpHelper != nullptr) {
                    NativeBufferUtils::DeleteVkImage(cleanUpHelper);
                }
            });
        nativeWindowBuffer_ = nullptr;
        cleanUpHelper_ = nullptr;
    }
}

std::shared_ptr<Drawing::Image> RSBackgroundImageDrawable::MakeFromTextureForVK(
    Drawing::Canvas& canvas, SurfaceBuffer* surfaceBuffer)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return nullptr;
    }
    if (surfaceBuffer == nullptr || surfaceBuffer->GetBufferHandle() == nullptr) {
        RS_LOGE("MakeFromTextureForVK surfaceBuffer is nullptr or buffer handle is nullptr");
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> pixelMap = bgImage_->GetPixelMap();
    if (pixelMapId_ != pixelMap->GetUniqueId() || !backendTexture_.IsValid()) {
        backendTexture_ = {};
        ReleaseNativeWindowBuffer();
        sptr<SurfaceBuffer> sfBuffer(surfaceBuffer);
        nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&sfBuffer);
        if (!nativeWindowBuffer_) {
            RS_LOGE("MakeFromTextureForVK create native window buffer fail");
            return nullptr;
        }
        backendTexture_ = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(
            nativeWindowBuffer_, surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight(), false);
        if (backendTexture_.IsValid()) {
            auto vkTextureInfo = backendTexture_.GetTextureInfo().GetVKTextureInfo();
            cleanUpHelper_ = new NativeBufferUtils::VulkanCleanupHelper(
                RsVulkanContext::GetSingleton(), vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
        } else {
            return nullptr;
        }
        pixelMapId_ = pixelMap->GetUniqueId();
        tid_ = gettid();
    }

    if (canvas.GetGPUContext() == nullptr) {
        RS_LOGE("RSBackgroundImageDrawable::MakeFromTextureForVK canvas.GetGPUContext is nullptr");
        ReleaseNativeWindowBuffer();
        return nullptr;
    }
    std::shared_ptr<Drawing::Image> dmaImage = std::make_shared<Drawing::Image>();
    auto vkTextureInfo = backendTexture_.GetTextureInfo().GetVKTextureInfo();
    Drawing::ColorType colorType = GetColorTypeFromVKFormat(vkTextureInfo->format);
    Drawing::BitmapFormat bitmapFormat = { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };
    if (!dmaImage->BuildFromTexture(*canvas.GetGPUContext(), backendTexture_.GetTextureInfo(),
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr, NativeBufferUtils::DeleteVkImage,
        cleanUpHelper_->Ref())) {
        RS_LOGE_LIMIT(__func__, __line__, "MakeFromTextureForVK build image failed");
        return nullptr;
    }
    return dmaImage;
}
#endif

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_VK) || defined(RS_ENABLE_GL))
void RSBackgroundImageDrawable::SetCompressedDataForASTC()
{
    std::shared_ptr<Media::PixelMap> pixelMap = bgImage_->GetPixelMap();
    if (!pixelMap || !pixelMap->GetFd()) {
        RS_LOGE("SetCompressedDataForASTC fail, data is null");
        return;
    }
    std::shared_ptr<Drawing::Data> fileData = std::make_shared<Drawing::Data>();
    // After RS is switched to Vulkan, the judgment of GpuApiType can be deleted.
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    if (pixelMap->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC &&
        (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR)) {
        if (pixelMapId_ != pixelMap->GetUniqueId()) {
            if (nativeWindowBuffer_) {
                DestroyNativeWindowBuffer(nativeWindowBuffer_);
            }
            sptr<SurfaceBuffer> surfaceBuf(reinterpret_cast<SurfaceBuffer *>(pixelMap->GetFd()));
            nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBuf);
            pixelMapId_ = pixelMap->GetUniqueId();
        }
        OH_NativeBuffer* nativeBuffer = OH_NativeBufferFromNativeWindowBuffer(nativeWindowBuffer_);
        if (nativeBuffer == nullptr || !fileData->BuildFromOHNativeBuffer(nativeBuffer, pixelMap->GetCapacity())) {
            RS_LOGE("SetCompressedDataForASTC data BuildFromOHNativeBuffer fail");
            return;
        }
    } else
#endif
    {
        const void* data = pixelMap->GetPixels();
        if (pixelMap->GetCapacity() > ASTC_HEADER_SIZE &&
            (data == nullptr || !fileData->BuildWithoutCopy(
                reinterpret_cast<const void *>(reinterpret_cast<const char *>(data) + ASTC_HEADER_SIZE),
                pixelMap->GetCapacity() - ASTC_HEADER_SIZE))) {
            RS_LOGE("SetCompressedDataForASTC data BuildWithoutCopy fail");
            return;
        }
    }
    bgImage_->SetCompressData(fileData);
}
#endif

bool RSBackgroundImageDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    stagingBgImage_ = properties.GetBgImage();
    if (!stagingBgImage_) {
        return false;
    }

    stagingBoundsRect_ = RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetBoundsRect());
    auto innerRect = properties.GetBgImageInnerRect();
    stagingBgImage_->SetDstRect(properties.GetBgImageRect());
    stagingBgImage_->SetInnerRect(std::make_optional<Drawing::RectI>(
        innerRect.x_, innerRect.y_, innerRect.x_ + innerRect.z_, innerRect.y_ + innerRect.w_));
    needSync_ = true;
    return true;
}

void RSBackgroundImageDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    bgImage_ = std::move(stagingBgImage_);
    boundsRect_ = stagingBoundsRect_;
    needSync_ = false;
}

void RSBackgroundImageDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    auto ptr = std::const_pointer_cast<RSBackgroundImageDrawable>(
        std::static_pointer_cast<const RSBackgroundImageDrawable>(shared_from_this()));
    Drawing::Brush brush;
    canvas->AttachBrush(brush);
    auto bgImage = ptr->bgImage_;
    if (!bgImage) {
        return;
    }
    RSTagTracker tagTracker(canvas->GetGPUContext(),
        RSTagTracker::SOURCETYPE::SOURCE_RSBACKGROUNDIMAGEDRAWABLE);
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    if (bgImage->GetPixelMap() && !bgImage->GetPixelMap()->IsAstc() &&
        bgImage->GetPixelMap()->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC) {
        if (!bgImage->GetPixelMap()->GetFd()) {
            return;
        }
        auto dmaImage =
            ptr->MakeFromTextureForVK(*canvas, reinterpret_cast<SurfaceBuffer*>(bgImage->GetPixelMap()->GetFd()));
        bgImage->SetDmaImage(dmaImage);
    }
#endif
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_VK) || defined(RS_ENABLE_GL))
    if (bgImage->GetPixelMap() && bgImage->GetPixelMap()->IsAstc()) {
        ptr->SetCompressedDataForASTC();
    }
#endif
    bgImage->CanvasDrawImage(*canvas, ptr->boundsRect_, Drawing::SamplingOptions(), true);
    canvas->DetachBrush();
}

RSDrawable::Ptr RSBackgroundFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    auto& rsFilter = node.GetRenderProperties().GetBackgroundFilter();
    if (rsFilter == nullptr && !node.NeedDrawBehindWindow()) {
        return nullptr;
    }

    RSDrawable::Ptr filterDrawable = nullptr;
    if (node.IsInstanceOf<RSEffectRenderNode>()) {
        filterDrawable = std::make_shared<RSBackgroundEffectDrawable>();
    } else {
        filterDrawable = std::make_shared<RSBackgroundFilterDrawable>();
    }
    if (filterDrawable->OnUpdate(node)) {
        return filterDrawable;
    }
    return nullptr;
}

bool RSBackgroundFilterDrawable::OnUpdate(const RSRenderNode& node)
{
    stagingNodeId_ = node.GetId();
    stagingNodeName_ = node.GetNodeName();
    auto& rsFilter = node.GetRenderProperties().GetBackgroundFilter();
    if (rsFilter != nullptr) {
        const auto& drawingFilter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
        RSPropertyDrawableUtils::ApplySDFShapeToFilter(node.GetRenderProperties(),
            drawingFilter, node.GetId());
        RecordFilterInfos(rsFilter);
        needSync_ = true;
        stagingFilter_ = rsFilter;
        stagingNeedDrawBehindWindow_ = false;
        PostUpdate(node);
        return true;
    }
    if (node.NeedDrawBehindWindow()) {
        auto behindWindowFilter = GetBehindWindowFilter(node);
        if (behindWindowFilter == nullptr) {
            return false;
        }
        RecordFilterInfos(behindWindowFilter);
        needSync_ = true;
        stagingFilter_ = behindWindowFilter;
        stagingNeedDrawBehindWindow_ = true;
        stagingDrawBehindWindowRegion_ = node.GetBehindWindowRegion();
        PostUpdate(node);
        return true;
    }
    return false;
}

std::shared_ptr<RSFilter> RSBackgroundFilterDrawable::GetBehindWindowFilter(const RSRenderNode& node)
{
    float radius = 0.f;
    float saturation = 1.f;
    float brightness = 1.f;
    RSColor maskColor = {};
    if (GetBehindWindowFilterProperty(node, ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_RADIUS, radius) &&
        GetBehindWindowFilterProperty(node, ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_SATURATION, saturation) &&
        GetBehindWindowFilterProperty(node, ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_BRIGHTNESS, brightness) &&
        GetBehindWindowFilterProperty(node, ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_MASK_COLOR, maskColor)) {
        return RSPropertyDrawableUtils::GenerateBehindWindowFilter(radius, saturation, brightness, maskColor);
    }
    return nullptr;
}

template<typename T>
bool RSBackgroundFilterDrawable::GetBehindWindowFilterProperty(
    const RSRenderNode& node, ModifierNG::RSPropertyType type, T& property)
{
    const auto modifier = node.GetModifierNG(ModifierNG::RSModifierType::BEHIND_WINDOW_FILTER);
    if (!modifier) {
        return false;
    }
    property = modifier->Getter<T>(type, T());
    return true;
}

void RSBackgroundFilterDrawable::OnSync()
{
    if (needSync_) {
        needDrawBehindWindow_ = stagingNeedDrawBehindWindow_;
    }
    drawBehindWindowRegion_ = stagingDrawBehindWindowRegion_;
    RSFilterDrawable::OnSync();
}

bool RSBackgroundFilterDrawable::FuzePixelStretch(const RSRenderNode& node)
{
    return RSPropertyDrawableUtils::RSFilterSetPixelStretch(node.GetRenderProperties(), stagingFilter_);
}

void RSBackgroundFilterDrawable::RemovePixelStretch()
{
    RSPropertyDrawableUtils::RSFilterRemovePixelStretch(stagingFilter_);
}

bool RSBackgroundEffectDrawable::OnUpdate(const RSRenderNode& node)
{
    stagingNodeId_ = node.GetId();
    stagingNodeName_ = node.GetNodeName();
    auto& rsFilter = node.GetRenderProperties().GetBackgroundFilter();
    if (rsFilter == nullptr) {
        return false;
    }
    RecordFilterInfos(rsFilter);
    needSync_ = true;
    stagingFilter_ = rsFilter;
    return true;
}

void RSBackgroundEffectDrawable::OnSync()
{
    RSFilterDrawable::OnSync();
}

void RSBackgroundEffectDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    Drawing::AutoCanvasRestore acr(*canvas, true);
    if (rect) {
        paintFilterCanvas->ClipRect(*rect);
    }
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas->GetGPUContext(),
        RSTagTracker::SOURCETYPE::SOURCE_RSBACKGROUNDEFFECTDRAWABLE);
#endif
    RectF bound = rect != nullptr ?
        RectF(rect->GetLeft(), rect->GetTop(), rect->GetWidth(), rect->GetHeight()) : RectF();
    Drawing::RectI boundsRect = GetAbsRenderEffectRect(*canvas, EffectRectType::SNAPSHOT, bound);
    if (boundsRect.IsEmpty()) {
        RS_TRACE_NAME_FMT("RSBackgroundEffectDrawable::DrawBackgroundEffect boundsRect is empty");
        return;
    }
    if (rect) {
        Drawing::Rect snapshotRect = renderRelativeRectInfo_ != nullptr ?
            GetAbsRenderEffectRect(*canvas, EffectRectType::SNAPSHOT, bound) : *rect;
        Drawing::Rect drawRect = renderRelativeRectInfo_ != nullptr ?
            GetAbsRenderEffectRect(*canvas, EffectRectType::DRAW, bound) : *rect;
        RectF snapshotRelativeRect = GetRenderRelativeRect(EffectRectType::SNAPSHOT, bound);
        auto filter = std::static_pointer_cast<RSDrawingFilter>(filter_);
        if (filter) {
            filter->SetGeometry(canvas->GetTotalMatrix(), Drawing::Rect(snapshotRect), Drawing::Rect(drawRect),
                snapshotRelativeRect.GetWidth(), snapshotRelativeRect.GetHeight());
        }
    }
    RS_TRACE_NAME_FMT("RSBackgroundEffectDrawable::DrawBackgroundEffect nodeId[%lld]", renderNodeId_);
    RSPropertyDrawableUtils::DrawBackgroundEffect(
        paintFilterCanvas, filter_, cacheManager_, boundsRect);
}

Drawing::RectI RSBackgroundEffectDrawable::GetAbsRenderEffectRect(const Drawing::Canvas& canvas,
    EffectRectType type, const RectF& bound) const
{
    auto surface = canvas.GetSurface();
    if (!surface) {
        ROSEN_LOGE("RSBackgroundEffectDrawable::GetAbsRenderEffectRect surface is nullptr.");
        return Drawing::RectI();
    }
    RectF rect = GetRenderRelativeRect(type, bound);
    auto drawingRect = RSPropertyDrawableUtils::Rect2DrawingRect(rect);

    Drawing::Rect absRect;
    canvas.GetTotalMatrix().MapRect(absRect, drawingRect);
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas const*>(&canvas);
    auto filterClipBounds = RSPropertyDrawableUtils::DrawingRectI2RectI(paintFilterCanvas->GetFilterClipBounds());
    RectI clipBound = renderRelativeRectInfo_ == nullptr || filterClipBounds.IsEmpty() ?
        RectI(0, 0, surface->Width(), surface->Height()) : filterClipBounds;
    RectI bounds(std::ceil(absRect.GetLeft()), std::ceil(absRect.GetTop()), std::ceil(absRect.GetWidth()),
        std::ceil(absRect.GetHeight()));
    bounds = bounds.IntersectRect(clipBound);
    Drawing::RectI boundsRect(bounds.GetLeft(), bounds.GetTop(), bounds.GetRight(), bounds.GetBottom());
    // When the drawing area of a useEffect node is empty in the current frame,
    // it won't be collected by the effect node, effect data is null(RSRenderNode::UpdateVisibleEffectChild).
    // if useEffect is within a node group, it will draw with fallback branch,
    // causing an increase in RSPropertyDrawableUtils::DrawBackgroundEffect g_blurCnt and
    // triggering frequency boosting. so add check for boundsRect: skip DrawBackgroundEffect to reduce blur count.
    return boundsRect;
}

RSDrawable::Ptr RSUseEffectDrawable::OnGenerate(const RSRenderNode& node)
{
    if (!node.GetRenderProperties().GetUseEffect()) {
        return nullptr;
    }
    if (node.GetRenderProperties().GetUseEffectType() == static_cast<int>(UseEffectType::BEHIND_WINDOW)) {
        return std::make_shared<RSUseEffectDrawable>(UseEffectType::BEHIND_WINDOW);
    }
    // Find effect render node
    auto parentNode = node.GetParent().lock();
    while (parentNode && !parentNode->IsInstanceOf<RSEffectRenderNode>()) {
        parentNode = parentNode->GetParent().lock();
    }
    DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr effectRenderNodeDrawable = nullptr;
    if (parentNode) {
        effectRenderNodeDrawable = parentNode->GetRenderDrawable();
    } else {
        ROSEN_LOGD("RSUseEffectDrawable::OnGenerate: find EffectRenderNode failed.");
    }
    return std::make_shared<RSUseEffectDrawable>(effectRenderNodeDrawable);
}

bool RSUseEffectDrawable::OnUpdate(const RSRenderNode& node)
{
    if (!node.GetRenderProperties().GetUseEffect()) {
        return false;
    }
    needSync_ = true;
    stagingUseEffectType_ = static_cast<UseEffectType>(node.GetRenderProperties().GetUseEffectType());
    return true;
}

void RSUseEffectDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    useEffectType_ = stagingUseEffectType_;
    needSync_ = false;
}

void RSUseEffectDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (!(RSSystemProperties::GetEffectMergeEnabled() && RSFilterCacheManager::isCCMEffectMergeEnable_)) {
        return;
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    if (paintFilterCanvas == nullptr) {
        return;
    }
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(paintFilterCanvas->GetGPUContext(),
        RSTagTracker::SOURCETYPE::SOURCE_RSUSEEFFECTDRAWABLE);
#endif
    if (useEffectType_ == UseEffectType::BEHIND_WINDOW &&
        (paintFilterCanvas->GetIsWindowFreezeCapture() || paintFilterCanvas->GetIsDrawingCache())) {
        RS_TRACE_NAME_FMT("RSUseEffectDrawable::OnDraw drawBehindWindow WindowFreezeCapture:%d, "
            "DrawingCache:%d, CacheData_valid:%d, bounds:%s", paintFilterCanvas->GetIsWindowFreezeCapture(),
            paintFilterCanvas->GetIsDrawingCache(), paintFilterCanvas->GetCacheBehindWindowData() != nullptr,
            paintFilterCanvas->GetDeviceClipBounds().ToString().c_str());
        if (paintFilterCanvas->GetIsDrawingCache() && !paintFilterCanvas->GetCacheBehindWindowData()) {
            return;
        }
        paintFilterCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
        return;
    }
    const auto& effectData = paintFilterCanvas->GetEffectData();
    if (useEffectType_ != UseEffectType::BEHIND_WINDOW &&
        (effectData == nullptr || effectData->cachedImage_ == nullptr)) {
        ROSEN_LOGD("RSPropertyDrawableUtils::DrawUseEffect effectData null, try to generate.");
        if (paintFilterCanvas->GetIsParallelCanvas()) {
            ROSEN_LOGE("RSPropertyDrawableUtils::DrawUseEffect is parallel canvas, disable fallback");
            return;
        }
        auto drawable = effectRenderNodeDrawableWeakRef_.lock();
        if (!drawable) {
            return;
        }
        RS_TRACE_NAME_FMT("RSPropertyDrawableUtils::DrawUseEffect Fallback");
        RSAutoCanvasRestore arc(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kEnv);
        bool disableFilterCache = paintFilterCanvas->GetDisableFilterCache();
        paintFilterCanvas->SetDisableFilterCache(true);
        int8_t index = drawable->drawCmdIndex_.backgroundFilterIndex_;
        drawable->DrawImpl(*paintFilterCanvas, *rect, index);
        paintFilterCanvas->SetDisableFilterCache(disableFilterCache);
        if (paintFilterCanvas->GetEffectIntersectWithDRM()) {
            RSPropertyDrawableUtils::DrawFilterWithDRM(canvas, paintFilterCanvas->GetDarkColorMode());
            return;
        }
        RSPropertyDrawableUtils::DrawUseEffect(paintFilterCanvas, useEffectType_);
        return;
    }
    if (paintFilterCanvas->GetEffectIntersectWithDRM()) {
        RSPropertyDrawableUtils::DrawFilterWithDRM(canvas, paintFilterCanvas->GetDarkColorMode());
        return;
    }
    RSPropertyDrawableUtils::DrawUseEffect(paintFilterCanvas, useEffectType_);
}

RSDrawable::Ptr RSDynamicLightUpDrawable::OnGenerate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (!properties.IsDynamicLightUpValid()) {
        return nullptr;
    }

    return std::make_shared<RSDynamicLightUpDrawable>(
        properties.GetDynamicLightUpRate().value(), properties.GetDynamicLightUpDegree().value());
};

bool RSDynamicLightUpDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (!properties.IsDynamicLightUpValid()) {
        return false;
    }

    stagingDynamicLightUpRate_ = properties.GetDynamicLightUpRate().value();
    stagingDynamicLightUpDeg_ = properties.GetDynamicLightUpDegree().value();
    needSync_ = true;

    return true;
}

void RSDynamicLightUpDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    dynamicLightUpRate_ = stagingDynamicLightUpRate_;
    dynamicLightUpDeg_ = stagingDynamicLightUpDeg_;
    needSync_ = false;
}

void RSDynamicLightUpDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (canvas->GetUICapture()) {
        return;
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(paintFilterCanvas->GetGPUContext(),
        RSTagTracker::SOURCETYPE::SOURCE_RSDYNAMICLIGHTUPDRAWABLE);
#endif
    auto alpha = paintFilterCanvas->GetAlpha();
    auto blender = RSDynamicLightUpDrawable::MakeDynamicLightUpBlender(
        dynamicLightUpRate_, dynamicLightUpDeg_, alpha);
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
        "RSDynamicLightUpDrawable::DrawDynamicLightUp, rate: %f, degree: %f, bounds: %s", dynamicLightUpRate_,
        dynamicLightUpDeg_, rect->ToString().c_str());
    Drawing::Brush brush;
    brush.SetBlender(blender);
    paintFilterCanvas->DrawBackground(brush);
}

std::shared_ptr<Drawing::Blender> RSDynamicLightUpDrawable::MakeDynamicLightUpBlender(
    float rate, float degree, float alpha)
{
    static constexpr char prog[] = R"(
        uniform float dynamicLightUpRate;
        uniform float dynamicLightUpDeg;

        vec4 main(vec4 drawing_src, vec4 drawing_dst) {
            float x = 0.299 * drawing_dst.r + 0.587 * drawing_dst.g + 0.114 * drawing_dst.b;
            float y = (0 - dynamicLightUpRate) * x + dynamicLightUpDeg;
            float R = clamp((drawing_dst.r + y), 0.0, 1.0);
            float G = clamp((drawing_dst.g + y), 0.0, 1.0);
            float B = clamp((drawing_dst.b + y), 0.0, 1.0);
            return vec4(R, G, B, 1.0);
        }
    )";
    static std::shared_ptr<Drawing::RuntimeEffect> dynamicLightUpBlenderEffect_ = nullptr;

    if (dynamicLightUpBlenderEffect_ == nullptr) {
        dynamicLightUpBlenderEffect_ = Drawing::RuntimeEffect::CreateForBlender(prog);
        if (dynamicLightUpBlenderEffect_ == nullptr) {
            ROSEN_LOGE("RSDynamicLightUpDrawable::MakeDynamicLightUpBlender effect error!");
            return nullptr;
        }
    }
    auto builder = std::make_shared<Drawing::RuntimeBlenderBuilder>(dynamicLightUpBlenderEffect_);
    builder->SetUniform("dynamicLightUpRate", rate * alpha);
    builder->SetUniform("dynamicLightUpDeg", degree * alpha);
    return builder->MakeBlender();
}

RSDrawable::Ptr RSMaterialFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    auto& rsFilter = node.GetRenderProperties().GetMaterialFilter();
    if (!rsFilter) {
        return nullptr;
    }

    if (auto ret = std::make_shared<RSMaterialFilterDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSMaterialFilterDrawable::OnUpdate(const RSRenderNode& node)
{
    stagingNodeId_ = node.GetId();
    stagingNodeName_ = node.GetNodeName();
    auto& rsFilter = node.GetRenderProperties().GetMaterialFilter();
    if (!rsFilter) {
        return false;
    }
    const auto& drawingFilter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
    RSPropertyDrawableUtils::ApplySDFShapeToFilter(node.GetRenderProperties(),
        drawingFilter, node.GetId());
    stagingEmptyShape_ = node.GetRenderProperties().GetSDFShape() &&
        node.GetRenderProperties().GetSDFShape()->GetType() == RSNGEffectType::SDF_EMPTY_SHAPE;
    RecordFilterInfos(rsFilter);
    needSync_ = true;
    stagingFilter_ = rsFilter;
    PostUpdate(node);
    return true;
}

void RSMaterialFilterDrawable::OnSync()
{
    if (needSync_) {
        emptyShape_ = stagingEmptyShape_;
    }
    RSFilterDrawable::OnSync();
}

void RSMaterialFilterDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    if (emptyShape_) {
        return;
    }
    auto filter = std::static_pointer_cast<RSDrawingFilter>(filter_);
    if (renderIntersectWithDRM_) {
        RSPropertyDrawableUtils::DrawColorUsingSDFWithDRM(canvas, rect, renderIsDarkColorMode_,
            filter->GetGEContainer(), Drawing::GE_FILTER_FROSTED_GLASS, Drawing::GE_FILTER_FROSTED_GLASS_SHAPE);
            return;
    }
    RSPropertyDrawableUtils::ApplyAdaptiveFrostedGlassParams(canvas, filter);
    RectF bound = (rect != nullptr ?
        RectF(rect->GetLeft(), rect->GetTop(), rect->GetWidth(), rect->GetHeight()) : RectF());
    Drawing::RectI snapshotRect = GetAbsRenderEffectRect(*canvas, EffectRectType::SNAPSHOT, bound);
    Drawing::RectI drawRect = GetAbsRenderEffectRect(*canvas, EffectRectType::DRAW, bound);
    RectF snapshotRelativeRect = GetRenderRelativeRect(EffectRectType::SNAPSHOT, bound);
    RS_TRACE_NAME_FMT("RSMaterialFilterDrawable::OnDraw node[%llu] ", renderNodeId_);
    if (rect) {
        filter->SetGeometry(canvas->GetTotalMatrix(), Drawing::Rect(snapshotRect), Drawing::Rect(drawRect),
            snapshotRelativeRect.GetWidth(), snapshotRelativeRect.GetHeight());
    }
    int64_t startBlurTime = Drawing::PerfmonitorReporter::GetCurrentTime();
    RSPropertyDrawableUtils::DrawFilter(canvas, filter_,
        cacheManager_, renderNodeId_, IsForeground(), snapshotRect, drawRect);
    int64_t blurDuration = Drawing::PerfmonitorReporter::GetCurrentTime() - startBlurTime;
    auto filterType = filter_->GetFilterType();
    RSPerfMonitorReporter::GetInstance().RecordBlurNode(renderNodeName_, blurDuration,
        RSPropertyDrawableUtils::IsBlurFilterType(filterType));
    if (rect != nullptr) {
        RSPerfMonitorReporter::GetInstance().RecordBlurPerfEvent(renderNodeId_, renderNodeName_,
            static_cast<uint16_t>(filterType), RSPropertyDrawableUtils::GetBlurFilterRadius(filter_),
            rect->GetWidth(), rect->GetHeight(), blurDuration,
            RSPropertyDrawableUtils::IsBlurFilterType(filterType));
    }
}

Drawing::RectI RSMaterialFilterDrawable::GetAbsRenderEffectRect(const Drawing::Canvas& canvas,
    EffectRectType type, const RectF& bound) const
{
    RectF rect = GetRenderRelativeRect(type, bound);
    auto drawingRect = RSPropertyDrawableUtils::Rect2DrawingRect(rect);

    Drawing::Rect absRect;
    canvas.GetTotalMatrix().MapRect(absRect, drawingRect);
    auto surface = canvas.GetSurface();
    if (!surface) {
        return Drawing::RectI();
    }

    Drawing::RectI absRectI = absRect.RoundOut();
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas const*>(&canvas);
    auto filterClipBounds = paintFilterCanvas->GetFilterClipBounds();
    Drawing::RectI clipBound =
        filterClipBounds.IsEmpty() ? Drawing::RectI(0, 0, surface->Width(), surface->Height()) : filterClipBounds;
    // if absRectI.Intersect(deviceRect) is true,
    // it means that absRectI intersects with deviceRect, and absRectI has been set to their intersection.
    return absRectI.Intersect(clipBound) ? absRectI : Drawing::RectI();
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
