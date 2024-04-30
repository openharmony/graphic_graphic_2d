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

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "drawable/rs_property_drawable_utils.h"
#include "effect/runtime_blender_builder.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
namespace {
bool g_forceBgAntiAlias = true;
constexpr int TRACE_LEVEL_TWO = 2;
}

RSDrawable::Ptr RSShadowDrawable::OnGenerate(const RSRenderNode& node)
{
    // skip shadow if not valid
    if (!node.GetRenderProperties().IsShadowValid()) {
        return nullptr;
    }
    RSDrawable::Ptr ret = nullptr;
    if (node.GetRenderProperties().GetShadowMask()) {
        ret = std::make_shared<RSColorfulShadowDrawable>();
    } else if (node.GetRenderProperties().GetShadowElevation() > 0.f) {
        ret = std::make_shared<RSShadowDrawable>();
    } else {
        ret = std::make_shared<RSMaskShadowDrawable>();
    }
    if (ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSShadowDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    // skip shadow if not valid
    if (!properties.IsShadowValid()) {
        return false;
    }

    stagingPath_ = RSPropertyDrawableUtils::CreateShadowPath(properties.GetShadowPath(),
        properties.GetClipBounds(), properties.GetRRect());
    stagingOffsetX_ = properties.GetShadowOffsetX();
    stagingOffsetY_ = properties.GetShadowOffsetY();
    stagingElevation_ = properties.GetShadowElevation();
    stagingColor_ = properties.GetShadowColor();
    stagingIsFilled_ = properties.GetShadowIsFilled();
    needSync_ = true;
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
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSShadowDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSShadowDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        // skip shadow if cache is enabled
        if (canvas->GetCacheType() == Drawing::CacheType::ENABLED) {
            ROSEN_LOGD("RSShadowDrawable::CreateDrawFunc cache type enabled.");
            return;
        }
        Drawing::Path path = ptr->path_;
        RSPropertyDrawableUtils::DrawShadow(canvas, path, ptr->offsetX_, ptr->offsetY_,
            ptr->elevation_, ptr->isFilled_, ptr->color_);
    };
}

bool RSMaskShadowDrawable::OnUpdate(const RSRenderNode& node)
{
    // skip shadow if not valid
    if (!node.GetRenderProperties().IsShadowValid()) {
        return false;
    }
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    // skip shadow if cache is enabled
    if (canvas.GetCacheType() == Drawing::CacheType::ENABLED) {
        ROSEN_LOGD("RSPropertyDrawableUtils::Canvas cache type enabled.");
        return false;
    }

    const RSProperties& properties = node.GetRenderProperties();
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path path = RSPropertyDrawableUtils::CreateShadowPath(properties.GetShadowPath(),
        properties.GetClipBounds(), properties.GetRRect());
    if (!properties.GetShadowIsFilled()) {
        canvas.ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
    }
    path.Offset(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());
    Color spotColor = properties.GetShadowColor();
    // shadow alpha follow setting
    auto shadowAlpha = spotColor.GetAlpha();
    RSColor colorPicked;
    if (properties.GetColorPickerCacheTaskShadow() != nullptr &&
        properties.GetShadowColorStrategy() != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE) {
        if (!properties.GetColorPickerCacheTaskShadow()->GetFirstGetColorFinished()) {
            shadowAlpha = 0;
        }
    } else {
        shadowAlpha = spotColor.GetAlpha();
        colorPicked = spotColor;
    }

    Drawing::Brush brush;
    brush.SetColor(Drawing::Color::ColorQuadSetARGB(
        shadowAlpha, colorPicked.GetRed(), colorPicked.GetGreen(), colorPicked.GetBlue()));
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetMaskFilter(
        Drawing::MaskFilter::CreateBlurMaskFilter(Drawing::BlurType::NORMAL, properties.GetShadowRadius()));
    brush.SetFilter(filter);
    canvas.AttachBrush(brush);
    canvas.DrawPath(path);
    canvas.DetachBrush();
    return true;
}

Drawing::RecordingCanvas::DrawFunc RSMaskShadowDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSMaskShadowDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        Drawing::AutoCanvasRestore rst(*canvas, true);
        RSPropertyDrawableUtils::CeilMatrixTrans(canvas);
        ptr->drawCmdList_->Playback(*canvas);
    };
}

bool RSColorfulShadowDrawable::OnUpdate(const RSRenderNode& node)
{
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    const RSProperties& properties = node.GetRenderProperties();
    // skip shadow if not valid or cache is enabled
    if (!properties.IsShadowValid() || canvas.GetCacheType() == Drawing::CacheType::ENABLED) {
        return false;
    }
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path path = RSPropertyDrawableUtils::CreateShadowPath(properties.GetShadowPath(),
        properties.GetClipBounds(), properties.GetRRect());
    if (!properties.GetShadowIsFilled()) {
        canvas.ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
    }
    // blurRadius calculation is based on the formula in Canvas::DrawShadow, 0.25f and 128.0f are constants
    const Drawing::scalar blurRadius =
        properties.GetShadowElevation() > 0.f
            ? 0.25f * properties.GetShadowElevation() * (1 + properties.GetShadowElevation() / 128.0f)
            : properties.GetShadowRadius();
    // save layer, draw image with clipPath, blur and draw back
    Drawing::Brush blurBrush;
    Drawing::Filter filter;
    filter.SetImageFilter(
        Drawing::ImageFilter::CreateBlurImageFilter(blurRadius, blurRadius, Drawing::TileMode::DECAL, nullptr));
    blurBrush.SetFilter(filter);
    canvas.SaveLayer({ nullptr, &blurBrush });
    canvas.Translate(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());
    canvas.ClipPath(path, Drawing::ClipOp::INTERSECT, false);
    // draw node content as shadow
    // [PLANNING]: maybe we should also draw background color / image here, and we should cache the shadow image
    return true;
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
    int tmpLayer = canvas.GetSaveCount();

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
    // only disable antialias when background is rect and g_forceBgAntiAlias is false
    bool antiAlias = g_forceBgAntiAlias || !properties.GetCornerRadius().IsZero();
    Drawing::Brush brush;
    brush.SetAntiAlias(antiAlias);
    brush.SetColor(Drawing::Color(bgColor.AsArgbInt()));
    if (properties.IsBgBrightnessValid()) {
        auto blender = RSPropertyDrawableUtils::MakeDynamicBrightnessBlender(
            properties.GetBgBrightnessParams().value(), properties.GetBgBrightnessFract());
        brush.SetBlender(blender);
    }

    canvas.AttachBrush(brush);
    // use drawrrect to avoid texture update in phone screen rotation scene
    if (RSSystemProperties::IsPhoneType()) {
        if (properties.GetBorderColorIsTransparent()) {
            canvas.DrawRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetRRect()));
        } else {
            canvas.DrawRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetInnerRRect()));
        }
    } else {
        canvas.DrawRect(RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetBoundsRect()));
    }
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
    // only disable antialias when background is rect and g_forceBgAntiAlias is false
    bool antiAlias = g_forceBgAntiAlias || !properties.GetCornerRadius().IsZero();
    Drawing::Brush brush;
    brush.SetAntiAlias(antiAlias);
    auto shaderEffect = bgShader->GetDrawingShader();
    brush.SetShaderEffect(shaderEffect);
    canvas.AttachBrush(brush);
    // use drawrrect to avoid texture update in phone screen rotation scene
    if (RSSystemProperties::IsPhoneType()) {
        if (properties.GetBorderColorIsTransparent()) {
            canvas.DrawRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetRRect()));
        } else {
            canvas.DrawRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetInnerRRect()));
        }
    } else {
        canvas.DrawRect(RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetBoundsRect()));
    }
    canvas.DetachBrush();
    return true;
}

RSDrawable::Ptr RSBackgroundImageDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBackgroundImageDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSBackgroundImageDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    const auto& bgImage = properties.GetBgImage();
    if (!bgImage) {
        return false;
    }

    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    // only disable antialias when background is rect and g_forceBgAntiAlias is false
    bool antiAlias = g_forceBgAntiAlias || !properties.GetCornerRadius().IsZero();
    Drawing::Brush brush;
    brush.SetAntiAlias(antiAlias);
    auto boundsRect = RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetBoundsRect());
    auto innerRect = properties.GetBgImageInnerRect();
    bgImage->SetDstRect(properties.GetBgImageRect());
    bgImage->SetInnerRect(std::make_optional<Drawing::RectI>(
        innerRect.x_, innerRect.y_, innerRect.x_ + innerRect.z_, innerRect.y_ + innerRect.w_));
    canvas.AttachBrush(brush);
    bgImage->CanvasDrawImage(canvas, boundsRect, Drawing::SamplingOptions(), true);
    canvas.DetachBrush();
    return true;
}

RSDrawable::Ptr RSBackgroundFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    auto& rsFilter = node.GetRenderProperties().GetBackgroundFilter();
    if (rsFilter == nullptr) {
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
    nodeId_ = node.GetId();
    auto& rsFilter = node.GetRenderProperties().GetBackgroundFilter();
    if (rsFilter == nullptr) {
        return false;
    }
    RecordFilterInfos(rsFilter);
    needSync_ = true;
    stagingFilter_ = rsFilter;
    return true;
}

bool RSBackgroundEffectDrawable::OnUpdate(const RSRenderNode& node)
{
    nodeId_ = node.GetId();
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
    hasEffectChildren_ = stagingHasEffectChildren_;
    RSFilterDrawable::OnSync();
    // clear both cache image when it has no effect children and will not draw in this frame
    if (!hasEffectChildren_) {
        lastCacheType_ = FilterCacheType::NONE;
    }
    stagingHasEffectChildren_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSBackgroundEffectDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSBackgroundEffectDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        if (canvas && ptr && ptr->filter_ && ptr->hasEffectChildren_) {
            auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
            RS_TRACE_NAME_FMT("RSBackgroundEffectDrawable::DrawBackgroundEffect nodeId[%lld]", ptr->nodeId_);
            RSPropertyDrawableUtils::DrawBackgroundEffect(
                paintFilterCanvas, ptr->filter_, ptr->cacheManager_, ptr->clearFilteredCacheAfterDrawing_);
        }
    };
}

RSDrawable::Ptr RSUseEffectDrawable::OnGenerate(const RSRenderNode& node)
{
    if (!node.GetRenderProperties().GetUseEffect()) {
        return nullptr;
    }
    return std::make_shared<RSUseEffectDrawable>();
}

bool RSUseEffectDrawable::OnUpdate(const RSRenderNode& node)
{
    if (!node.GetRenderProperties().GetUseEffect()) {
        return false;
    }
    return true;
}

Drawing::RecordingCanvas::DrawFunc RSUseEffectDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSUseEffectDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
        RSPropertyDrawableUtils::DrawUseEffect(paintFilterCanvas);
    };
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

Drawing::RecordingCanvas::DrawFunc RSDynamicLightUpDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSDynamicLightUpDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
        auto alpha = paintFilterCanvas->GetAlpha();
        auto blender = RSDynamicLightUpDrawable::MakeDynamicLightUpBlender(
            ptr->dynamicLightUpRate_, ptr->dynamicLightUpDeg_, alpha);
        RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
            "RSDynamicLightUpDrawable::DrawDynamicLightUp, rate: %f, degree: %f, bounds: %s", ptr->dynamicLightUpRate_,
            ptr->dynamicLightUpDeg_, rect->ToString().c_str());
        Drawing::Brush brush;
        brush.SetBlender(blender);
        paintFilterCanvas->DrawBackground(brush);
    };
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
} // namespace DrawableV2
} // namespace OHOS::Rosen
