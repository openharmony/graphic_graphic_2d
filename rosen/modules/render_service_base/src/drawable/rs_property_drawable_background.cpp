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

#include "drawable/rs_property_drawable_utils.h"
#include "effect/runtime_blender_builder.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
namespace {
bool g_forceBgAntiAlias = true;
}

std::shared_ptr<Drawing::RuntimeEffect> RSDynamicLightUpDrawable::dynamicLightUpBlenderEffect_ = nullptr;

RSDrawable::Ptr RSShadowDrawable::OnGenerate(const RSRenderNode& node)
{
    RSDrawable::Ptr ret = nullptr;
    if (node.GetRenderProperties().GetShadowMask()) {
        ret = std::make_shared<RSColorfulShadowDrawable>();
    } else {
        ret = std::make_shared<RSShadowDrawable>();
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
    stagingShadowParam_ = {properties.GetShadowColor(), properties.GetShadowColorStrategy(),
        properties.GetShadowElevation(), properties.GetShadowRadius(), properties.GetShadowOffsetX(),
        properties.GetShadowOffsetY(), properties.GetShadowIsFilled(), properties.GetRRect(),
        properties.GetColorPickerCacheTaskShadow(), properties.GetShadowPath(), properties.GetClipBounds()};
    return true;
}

void RSShadowDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    shadowParam_ = std::move(stagingShadowParam_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSShadowDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSShadowDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        if (canvas) {
            RSPropertyDrawableUtils::DrawShadow(canvas, ptr->shadowParam_);
        }
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
    Drawing::Path path = RSPropertyDrawableUtils::CreateShadowPath(canvas, properties.GetShadowIsFilled(),
        properties.GetShadowPath(), properties.GetClipBounds(), properties.GetRRect());
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
    // TODO
    // if (auto node = RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(properties.backref_.lock())) {
    //     node->InternalDrawContent(canvas);
    // }
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
    canvas.AttachBrush(brush);
    canvas.DrawRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetRRect()));
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
    canvas.DrawBackground(brush);
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
    // paint backgroundColor
    Drawing::Brush brush;
    brush.SetAntiAlias(antiAlias);
    auto boundsRect = RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetBoundsRect());
    bgImage->SetDstRect(properties.GetBgImageRect());
    canvas.AttachBrush(brush);
    bgImage->CanvasDrawImage(canvas, boundsRect, Drawing::SamplingOptions(), true);
    canvas.DetachBrush();
    return true;
}

RSDrawable::Ptr RSBackgroundFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBackgroundFilterDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSBackgroundFilterDrawable::OnUpdate(const RSRenderNode& node)
{
    auto& rsFilter = node.GetRenderProperties().GetBackgroundFilter();
    if (rsFilter == nullptr) {
        return false;
    }
    needSync_ = true;
    stagingFilter_ = rsFilter;
    return true;
}

void RSBackgroundFilterDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    filter_ = std::move(stagingFilter_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSBackgroundFilterDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSBackgroundFilterDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        if (canvas && ptr->filter_) {
            RSPropertyDrawableUtils::DrawFilter(canvas, ptr->filter_, false);
        }
    };
}

RSDrawable::Ptr RSDynamicLightUpDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSDynamicLightUpDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSDynamicLightUpDrawable::OnUpdate(const RSRenderNode& node)
{
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    Drawing::Surface* surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSDynamicLightUpDrawable::OnUpdate surface is null");
        return false;
    }
    const RSProperties& properties = node.GetRenderProperties();
    auto blender = MakeDynamicLightUpBlender(properties.GetDynamicLightUpRate().value() * canvas.GetAlpha(),
        properties.GetDynamicLightUpDegree().value() * canvas.GetAlpha());
    Drawing::Brush brush;
    brush.SetBlender(blender);
    canvas.DrawBackground(brush);
    return true;
}

std::shared_ptr<Drawing::Blender> RSDynamicLightUpDrawable::MakeDynamicLightUpBlender(
    float dynamicLightUpRate, float dynamicLightUpDeg)
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
    if (dynamicLightUpBlenderEffect_ == nullptr) {
        dynamicLightUpBlenderEffect_ = Drawing::RuntimeEffect::CreateForBlender(prog);
        if (dynamicLightUpBlenderEffect_ == nullptr) {
            ROSEN_LOGE("RSDynamicLightUpDrawable::MakeDynamicLightUpBlender effect error!");
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::RuntimeBlenderBuilder> builder =
        std::make_shared<Drawing::RuntimeBlenderBuilder>(dynamicLightUpBlenderEffect_);
    builder->SetUniform("dynamicLightUpRate", dynamicLightUpRate);
    builder->SetUniform("dynamicLightUpDeg", dynamicLightUpDeg);
    return builder->MakeBlender();
}

} // namespace OHOS::Rosen
