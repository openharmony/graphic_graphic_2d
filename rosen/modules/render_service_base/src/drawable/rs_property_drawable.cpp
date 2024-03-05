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

#include "drawable/rs_property_drawable.h"

#include "rs_trace.h"
#include "platform/common/rs_log.h"

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "drawable/rs_property_drawable_utils.h"
#include "effect/runtime_blender_builder.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_properties_painter.h"
#include "property/rs_point_light_manager.h"
#include "render/rs_material_filter.h"

namespace OHOS::Rosen {
namespace {
bool g_forceBgAntiAlias = true;
constexpr int PARAM_TWO = 2;
constexpr int MAX_LIGHT_SOUCES = 4;
} // namespace

std::shared_ptr<Drawing::RuntimeEffect> RSDynamicLightUpDrawable::dynamicLightUpBlenderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeShaderBuilder> RSPointLightDrawable::phongShaderBuilder_ = nullptr;

void RSPropertyDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    drawCmdList_ = std::move(stagingDrawCmdList_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSPropertyDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSPropertyDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        if (const auto& drawCmdList = ptr->drawCmdList_) {
            drawCmdList->Playback(*canvas);
        }
    };
}

class RSPropertyDrawCmdListRecorder {
public:
    // not copyable and moveable
    RSPropertyDrawCmdListRecorder(const RSPropertyDrawCmdListRecorder&) = delete;
    RSPropertyDrawCmdListRecorder(const RSPropertyDrawCmdListRecorder&&) = delete;
    RSPropertyDrawCmdListRecorder& operator=(const RSPropertyDrawCmdListRecorder&) = delete;
    RSPropertyDrawCmdListRecorder& operator=(const RSPropertyDrawCmdListRecorder&&) = delete;

    explicit RSPropertyDrawCmdListRecorder(int width, int height)
    {
        // PLANNING: use RSRenderNode to determine the correct recording canvas size
        recordingCanvas_ = std::make_unique<ExtendRecordingCanvas>(width, height, true);
    }

    virtual ~RSPropertyDrawCmdListRecorder()
    {
        if (recordingCanvas_) {
            ROSEN_LOGE("Object destroyed without calling EndRecording.");
        }
    }

    const std::unique_ptr<ExtendRecordingCanvas>& GetRecordingCanvas() const
    {
        return recordingCanvas_;
    }

    std::shared_ptr<Drawing::DrawCmdList>&& EndRecordingAndReturnRecordingList()
    {
        auto displayList = recordingCanvas_->GetDrawCmdList();
        recordingCanvas_.reset();
        return std::move(displayList);
    }

protected:
    std::unique_ptr<ExtendRecordingCanvas> recordingCanvas_;
};

class RSPropertyDrawCmdListUpdater : public RSPropertyDrawCmdListRecorder {
public:
    explicit RSPropertyDrawCmdListUpdater(int width, int height, RSPropertyDrawable* target)
        : RSPropertyDrawCmdListRecorder(width, height), target_(target)
    {}
    ~RSPropertyDrawCmdListUpdater() override
    {
        if (recordingCanvas_) {
            target_->stagingDrawCmdList_ = EndRecordingAndReturnRecordingList();
            target_->needSync_ = true;
        } else {
            ROSEN_LOGE("Update failed, recording canvas is null!");
        }
    }

private:
    RSPropertyDrawable* target_;
};

// ============================================================================
RSDrawable::Ptr RSFrameOffsetDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSFrameOffsetDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSFrameOffsetDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto frameOffsetX = properties.GetFrameOffsetX();
    auto frameOffsetY = properties.GetFrameOffsetY();
    if (frameOffsetX == 0 && frameOffsetY == 0) {
        return false;
    }

    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    updater.GetRecordingCanvas()->Translate(frameOffsetX, frameOffsetY);
    return true;
}

// ============================================================================
RSDrawable::Ptr RSClipToBoundsDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSClipToBoundsDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSClipToBoundsDrawable::OnUpdate(const RSRenderNode &node)
{
    const RSProperties& properties = node.GetRenderProperties();
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    auto& canvas = *updater.GetRecordingCanvas();
    if (properties.GetClipBounds() != nullptr) {
        canvas.ClipPath(properties.GetClipBounds()->GetDrawingPath(), Drawing::ClipOp::INTERSECT, true);
    } else if (properties.GetClipToRRect()) {
        canvas.ClipRoundRect(
            RSPropertiesPainter::RRect2DrawingRRect(properties.GetClipRRect()), Drawing::ClipOp::INTERSECT, false);
    } else if (!properties.GetCornerRadius().IsZero()) {
        canvas.ClipRoundRect(
            RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas.ClipRect(
            RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect()), Drawing::ClipOp::INTERSECT, true);
    }
    return true;
}

RSDrawable::Ptr RSClipToFrameDrawable::OnGenerate(const RSRenderNode &node)
{
    if (auto ret = std::make_shared<RSClipToFrameDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
}

bool RSClipToFrameDrawable::OnUpdate(const RSRenderNode &node)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (!properties.GetClipToFrame()) {
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    updater.GetRecordingCanvas()->ClipRect(
        RSPropertiesPainter::Rect2DrawingRect(properties.GetFrameRect()), Drawing::ClipOp::INTERSECT, false);
    return true;
}

// ============================================================================
// Background
RSDrawable::Ptr RSBackgroundColorDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBackgroundColorDrawable>(); ret->OnUpdate(node)) {
        return ret;
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
        return ret;
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
        return ret;
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
        return ret;
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
        if (ptr->filter_) {
            RSPropertyDrawableUtils::DrawFilter(canvas, ptr->filter_, false);
        }
    };
}

RSDrawable::Ptr RSBorderDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBorderDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSBorderDrawable::OnUpdate(const RSRenderNode& node)
{
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    const RSProperties& properties = node.GetRenderProperties();
    DrawBorder(properties, *updater.GetRecordingCanvas(), properties.GetBorder(), false);
    return true;
}

void RSBorderDrawable::DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas,
    const std::shared_ptr<RSBorder>& border, const bool& isOutline)
{
    if (!border || !border->HasBorder()) {
        return;
    }

    Drawing::Brush brush;
    Drawing::Pen pen;
    brush.SetAntiAlias(true);
    pen.SetAntiAlias(true);
    if (border->ApplyFillStyle(brush)) {
        auto roundRect = RSPropertyDrawableUtils::RRect2DrawingRRect(
            RSPropertyDrawableUtils::GetRRectForDrawingBorder(properties, border, isOutline));
        auto innerRoundRect = RSPropertyDrawableUtils::RRect2DrawingRRect(
            RSPropertyDrawableUtils::GetInnerRRectForDrawingBorder(properties, border, isOutline));
        canvas.AttachBrush(brush);
        canvas.DrawNestedRoundRect(roundRect, innerRoundRect);
        canvas.DetachBrush();
    } else {
        bool isZero = isOutline ? properties.GetCornerRadius().IsZero() : border->GetRadiusFour().IsZero();
        if (isZero && border->ApplyFourLine(pen)) {
            RectF rectf = isOutline ?
                properties.GetBoundsRect().MakeOutset(border->GetWidthFour()) : properties.GetBoundsRect();
            border->PaintFourLine(canvas, pen, rectf);
        } else if (border->ApplyPathStyle(pen)) {
            auto borderWidth = border->GetWidth();
            RRect rrect = RSPropertyDrawableUtils::GetRRectForDrawingBorder(properties, border, isOutline);
            rrect.rect_.width_ -= borderWidth;
            rrect.rect_.height_ -= borderWidth;
            rrect.rect_.Move(borderWidth / PARAM_TWO, borderWidth / PARAM_TWO);
            Drawing::Path borderPath;
            borderPath.AddRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(rrect));
            canvas.AttachPen(pen);
            canvas.DrawPath(borderPath);
            canvas.DetachPen();
        } else {
            Drawing::AutoCanvasRestore acr(canvas, true);
            auto rrect = RSPropertyDrawableUtils::RRect2DrawingRRect(
                RSPropertyDrawableUtils::GetRRectForDrawingBorder(properties, border, isOutline));
            canvas.ClipRoundRect(rrect, Drawing::ClipOp::INTERSECT, true);
            auto innerRoundRect = RSPropertyDrawableUtils::RRect2DrawingRRect(
                RSPropertyDrawableUtils::GetInnerRRectForDrawingBorder(properties, border, isOutline));
            canvas.ClipRoundRect(innerRoundRect, Drawing::ClipOp::DIFFERENCE, true);
            Drawing::scalar centerX = innerRoundRect.GetRect().GetLeft() + innerRoundRect.GetRect().GetWidth() / 2;
            Drawing::scalar centerY = innerRoundRect.GetRect().GetTop() + innerRoundRect.GetRect().GetHeight() / 2;
            Drawing::Point center = { centerX, centerY };
            auto rect = rrect.GetRect();
            Drawing::SaveLayerOps slr(&rect, nullptr);
            canvas.SaveLayer(slr);
            border->PaintTopPath(canvas, pen, rrect, center);
            border->PaintRightPath(canvas, pen, rrect, center);
            border->PaintBottomPath(canvas, pen, rrect, center);
            border->PaintLeftPath(canvas, pen, rrect, center);
        }
    }
}

RSDrawable::Ptr RSOutlineDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSOutlineDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSOutlineDrawable::OnUpdate(const RSRenderNode& node)
{
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    const RSProperties& properties = node.GetRenderProperties();
    RSBorderDrawable::DrawBorder(properties, *updater.GetRecordingCanvas(), properties.GetOutline(), true);
    return true;
}

RSDrawable::Ptr RSShadowDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSShadowDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSShadowDrawable::OnUpdate(const RSRenderNode& node)
{
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    const RSProperties& properties = node.GetRenderProperties();
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    const RectF absBounds = {0, 0, properties.GetBoundsWidth(), properties.GetBoundsHeight()};
    RRect rrect = RRect(absBounds, properties.GetCornerRadius());
    // RRect rrect = properties.GetRRect();
    // skip shadow if not valid or cache is enabled
    if (properties.IsSpherizeValid() || !properties.IsShadowValid() ||
        canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::ENABLED) {
        return false;
    }
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path path;
    if (properties.GetShadowPath() && properties.GetShadowPath()->GetDrawingPath().IsValid()) {
        path = properties.GetShadowPath()->GetDrawingPath();
        if (!properties.GetShadowIsFilled()) {
            canvas.ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
        }
    } else if (properties.GetClipBounds()) {
        path = properties.GetClipBounds()->GetDrawingPath();
        if (!properties.GetShadowIsFilled()) {
            canvas.ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
        }
    } else {
        path.AddRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(rrect));
        if (!properties.GetShadowIsFilled()) {
            canvas.ClipRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(rrect), Drawing::ClipOp::DIFFERENCE,
                true);
        }
    }
    if (properties.GetShadowMask()) {
        DrawColorfulShadowInner(properties, canvas, path);
    } else {
        DrawShadowInner(properties, canvas, path);
    }
    return true;
}

void RSShadowDrawable::DrawColorfulShadowInner(const RSProperties& properties, Drawing::Canvas& canvas,
    Drawing::Path& path)
{
    // blurRadius calculation is based on the formula in Canvas::DrawShadow, 0.25f and 128.0f are constants
    const Drawing::scalar blurRadius =
        properties.GetShadowElevation() > 0.f
            ? 0.25f * properties.GetShadowElevation() * (1 + properties.GetShadowElevation() / 128.0f)
            : properties.GetShadowRadius();

    // save layer, draw image with clipPath, blur and draw back
    Drawing::Brush blurBrush;
    Drawing::Filter filter;
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        blurRadius, blurRadius, Drawing::TileMode::DECAL, nullptr));
    blurBrush.SetFilter(filter);
    canvas.SaveLayer({nullptr, &blurBrush});
    canvas.Translate(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());
    canvas.ClipPath(path, Drawing::ClipOp::INTERSECT, false);
    // draw node content as shadow
    // [PLANNING]: maybe we should also draw background color / image here, and we should cache the shadow image
    // TODO
    // if (auto node = RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(properties.backref_.lock())) {
    //     node->InternalDrawContent(canvas);
    // }
}

void RSShadowDrawable::DrawShadowInner(const RSProperties& properties, Drawing::Canvas& canvas, Drawing::Path& path)
{
    path.Offset(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());
    Color spotColor = properties.GetShadowColor();
    // shadow alpha follow setting
    auto shadowAlpha = spotColor.GetAlpha();
    auto deviceClipBounds = canvas.GetDeviceClipBounds();

    // The translation of the matrix is rounded to improve the hit ratio of skia blurfilter cache,
    // the function <compute_key_and_clip_bounds> in <skia/src/gpu/GrBlurUtil.cpp> for more details.
    Drawing::AutoCanvasRestore rst(canvas, true);
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::TRANS_X, std::ceil(matrix.Get(Drawing::Matrix::TRANS_X)));
    matrix.Set(Drawing::Matrix::TRANS_Y, std::ceil(matrix.Get(Drawing::Matrix::TRANS_Y)));
    canvas.SetMatrix(matrix);

    RSColor colorPicked;
    auto& colorPickerTask = properties.GetColorPickerCacheTaskShadow();
    if (colorPickerTask != nullptr &&
        properties.GetShadowColorStrategy() != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE) {
        if (RSPropertyDrawableUtils::PickColor(properties, canvas, path, matrix, deviceClipBounds, colorPicked)) {
            RSPropertyDrawableUtils::GetDarkColor(colorPicked);
        } else {
            shadowAlpha = 0;
        }
        if (!colorPickerTask->GetFirstGetColorFinished()) {
            shadowAlpha = 0;
        }
    } else {
        shadowAlpha = spotColor.GetAlpha();
        colorPicked = spotColor;
    }

    if (properties.GetShadowElevation() > 0.f) {
        Drawing::Point3 planeParams = { 0.0f, 0.0f, properties.GetShadowElevation() };
        std::vector<Drawing::Point> pt{{path.GetBounds().GetLeft() + path.GetBounds().GetWidth() / 2,
            path.GetBounds().GetTop() + path.GetBounds().GetHeight() / 2}};
        canvas.GetTotalMatrix().MapPoints(pt, pt, 1);
        Drawing::Point3 lightPos = {pt[0].GetX(), pt[0].GetY(), DEFAULT_LIGHT_HEIGHT};
        Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
        ambientColor.MultiplyAlpha(canvas.GetAlpha());
        spotColor.MultiplyAlpha(canvas.GetAlpha());
        canvas.DrawShadow(path, planeParams, lightPos, DEFAULT_LIGHT_RADIUS,
            Drawing::Color(ambientColor.AsArgbInt()), Drawing::Color(spotColor.AsArgbInt()),
            Drawing::ShadowFlags::TRANSPARENT_OCCLUDER);
    } else {
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
    }
}

RSDrawable::Ptr RSForegroundColorDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSForegroundColorDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSForegroundColorDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto fgColor = properties.GetForegroundColor();
    if (fgColor == RgbPalette::Transparent()) {
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color(fgColor.AsArgbInt()));
    brush.SetAntiAlias(true);
    canvas.AttachBrush(brush);
    canvas.DrawRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetRRect()));
    canvas.DetachBrush();
    return true;
}

RSDrawable::Ptr RSForegroundFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSForegroundFilterDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
}

bool RSForegroundFilterDrawable::OnUpdate(const RSRenderNode& node)
{
    auto& rsFilter = node.GetRenderProperties().GetFilter();
    if (rsFilter == nullptr) {
        return false;
    }
    needSync_ = true;
    stagingFilter_ = rsFilter;
    return true;
}

void RSForegroundFilterDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    filter_ = std::move(stagingFilter_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSForegroundFilterDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSForegroundFilterDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        if (ptr->filter_) {
            RSPropertyDrawableUtils::DrawFilter(canvas, ptr->filter_, true);
        }
    };
}

RSDrawable::Ptr RSPixelStretchDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSPixelStretchDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
}

bool RSPixelStretchDrawable::OnUpdate(const RSRenderNode& node)
{
    auto& pixelStretch = node.GetRenderProperties().GetPixelStretch();
    if (!pixelStretch.has_value()) {
        return false;
    }
    needSync_ = true;
    stagingPixelStretch_ = pixelStretch;
    const auto& boundsGeo = node.GetRenderProperties().GetBoundsGeometry();
    stagingBoundsGeoValid_ = boundsGeo && !boundsGeo->IsEmpty();
    stagingBoundsRect_ = node.GetRenderProperties().GetBoundsRect();
    return true;
}

void RSPixelStretchDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    pixelStretch_ = std::move(stagingPixelStretch_);
    boundsGeoValid_ = stagingBoundsGeoValid_;
    stagingBoundsGeoValid_ = false;
    boundsRect_ = stagingBoundsRect_;
    stagingBoundsRect_.Clear();
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSPixelStretchDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSPixelStretchDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        if (ptr->pixelStretch_.has_value()) {
            RSPropertyDrawableUtils::DrawPixelStretch(canvas, ptr->pixelStretch_, ptr->boundsRect_,
                ptr->boundsGeoValid_);
        }
    };
}

RSDrawable::Ptr RSDynamicLightUpDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSDynamicLightUpDrawable>(); ret->OnUpdate(node)) {
        return ret;
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

std::shared_ptr<Drawing::Blender> RSDynamicLightUpDrawable::MakeDynamicLightUpBlender(float dynamicLightUpRate,
    float dynamicLightUpDeg)
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

RSDrawable::Ptr RSLightUpEffectDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSLightUpEffectDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
}

bool RSLightUpEffectDrawable::OnUpdate(const RSRenderNode& node)
{
    if (!node.GetRenderProperties().IsLightUpEffectValid()) {
        return false;
    }
    needSync_ = true;
    stagingLightUpEffectDegree_ = node.GetRenderProperties().GetLightUpEffect();
    return true;
}

void RSLightUpEffectDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    lightUpEffectDegree_ = stagingLightUpEffectDegree_;
    stagingLightUpEffectDegree_ = 1.0f;
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSLightUpEffectDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSLightUpEffectDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        RSPropertyDrawableUtils::DrawLightUpEffect(canvas, ptr->lightUpEffectDegree_);
    };
}

RSDrawable::Ptr RSColorFilterDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSColorFilterDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
}

bool RSColorFilterDrawable::OnUpdate(const RSRenderNode& node)
{
    auto& colorFilter = node.GetRenderProperties().GetColorFilter();
    if (colorFilter == nullptr) {
        return false;
    }
    needSync_ = true;
    stagingFilter_ = colorFilter;
    return true;
}

void RSColorFilterDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    filter_ = std::move(stagingFilter_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSColorFilterDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSColorFilterDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        if (ptr->filter_) {
            RSPropertyDrawableUtils::DrawColorFilter(canvas, ptr->filter_);
        }
    };
}

RSDrawable::Ptr RSMaskDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSMaskDrawable>(); ret->OnUpdate(node)) {
        return ret;
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
        Drawing::Rect rect = Drawing::Rect(
            0, 0, maskBounds.GetWidth(), maskBounds.GetHeight());
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

RSDrawable::Ptr RSBinarizationDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBinarizationDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
}

bool RSBinarizationDrawable::OnUpdate(const RSRenderNode& node)
{
    auto& aiInvert = node.GetRenderProperties().GetAiInvert();
    if (!aiInvert.has_value()) {
        return false;
    }
    needSync_ = true;
    stagingAiInvert_ = aiInvert;
    return true;
}

void RSBinarizationDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    aiInvert_ = std::move(stagingAiInvert_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSBinarizationDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSBinarizationDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        if (ptr->aiInvert_.has_value()) {
            RSPropertyDrawableUtils::DrawBinarization(canvas, ptr->aiInvert_);
        }
    };
}

RSDrawable::Ptr RSParticleDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSParticleDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSParticleDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    const auto& particleVector = properties.GetParticles();
    if (particleVector.GetParticleSize() == 0) {
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    const auto& particles = particleVector.GetParticleVector();
    auto bounds = properties.GetDrawRegion();
    for (const auto& particle : particles) {
        if (particle != nullptr && particle->IsAlive()) {
            // Get particle properties
            auto position = particle->GetPosition();
            float opacity = particle->GetOpacity();
            float scale = particle->GetScale();
            if (!(bounds->Intersect(position.x_, position.y_)) || opacity <= 0.f || scale <= 0.f) {
                continue;
            }
            auto particleType = particle->GetParticleType();
            Drawing::Brush brush;
            brush.SetAntiAlias(true);
            brush.SetAlphaF(opacity);
            auto clipBounds = Drawing::Rect(
                bounds->left_, bounds->top_, bounds->left_ + bounds->width_, bounds->top_ + bounds->height_);
            canvas.ClipRect(clipBounds, Drawing::ClipOp::INTERSECT, true);

            if (particleType == ParticleType::POINTS) {
                auto radius = particle->GetRadius();
                Color color = particle->GetColor();
                auto alpha = color.GetAlpha();
                color.SetAlpha(alpha * opacity);
                brush.SetColor(color.AsArgbInt());
                canvas.AttachBrush(brush);
                canvas.DrawCircle(Drawing::Point(position.x_, position.y_), radius * scale);
                canvas.DetachBrush();
            } else {
                auto imageSize = particle->GetImageSize();
                auto image = particle->GetImage();
                float left = position.x_;
                float top = position.y_;
                float right = position.x_ + imageSize.x_ * scale;
                float bottom = position.y_ + imageSize.y_ * scale;
                canvas.Save();
                canvas.Translate(position.x_, position.y_);
                canvas.Rotate(particle->GetSpin(), imageSize.x_ * scale / 2.f, imageSize.y_ * scale / 2.f);
                RectF destRect(left, top, right, bottom);
                image->SetDstRect(destRect);
                image->SetScale(scale);
                image->SetImageRepeat(0);
                Drawing::Rect rect { left, top, right, bottom };
                canvas.AttachBrush(brush);
                image->CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), false);
                canvas.DetachBrush();
                canvas.Restore();
            }
        }
    }
    return true;
}

RSDrawable::Ptr RSBeginBlendModeDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBeginBlendModeDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSBeginBlendModeDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto blendMode = properties.GetColorBlendMode();
    if (blendMode == 0) {
        // no blend
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    // fast blend mode
    if (properties.GetColorBlendApplyType() == static_cast<int>(RSColorBlendApplyType::FAST)) {
        // TODO canvas.SaveBlendMode, canvas.SetBlendMode
        // canvas.SaveBlendMode();
        // canvas.SetBlendMode({ blendMode - 1 }); // map blendMode to SkBlendMode
        return true;
    }

    // save layer mode
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::TRANS_X, std::ceil(matrix.Get(Drawing::Matrix::TRANS_X)));
    matrix.Set(Drawing::Matrix::TRANS_Y, std::ceil(matrix.Get(Drawing::Matrix::TRANS_Y)));
    canvas.SetMatrix(matrix);
    Drawing::Brush blendBrush_;
    blendBrush_.SetAlphaF(canvas.GetAlpha());
    blendBrush_.SetBlendMode(static_cast<Drawing::BlendMode>(blendMode - 1)); // map blendMode to Drawing::BlendMode
    Drawing::SaveLayerOps maskLayerRec(nullptr, &blendBrush_, 0);
    canvas.SaveLayer(maskLayerRec);
    // TODO canvas.SaveBlendMode, canvas.SetBlendMode, canvas.SaveAlpha, canvas.SetAlpha
    // canvas.SaveBlendMode();
    // canvas.SetBlendMode(std::nullopt);
    // canvas.SaveAlpha();
    // canvas.SetAlpha(1.0f);
    return true;
}

RSDrawable::Ptr RSEndBlendModeDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSEndBlendModeDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSEndBlendModeDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (properties.GetColorBlendMode() == 0) {
        // no blend
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    // TODO canvas.RestoreBlendMode
    // canvas.RestoreBlendMode();
    if (properties.GetColorBlendApplyType() != static_cast<int>(RSColorBlendApplyType::FAST)) {
        // TODO canvas.RestoreAlpha
        // canvas.RestoreAlpha();
    }
    canvas.Restore();
    return true;
}

RSDrawable::Ptr RSPointLightDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSPointLightDrawable>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSPointLightDrawable::OnUpdate(const RSRenderNode& node)
{
    auto lightBuilder = GetPhongShaderBuilder();
    if (!lightBuilder) {
        ROSEN_LOGE("RSPointLightDrawable::OnUpdate lightBuilder is null.");
        return false;
    }
    const RSProperties& properties = node.GetRenderProperties();
    const auto& lightSources = properties.GetIlluminated()->GetLightSources();
    if (lightSources.empty()) {
        ROSEN_LOGE("RSPointLightDrawable::OnUpdate lightSourceList is empty.");
        return false;
    }
    const auto& geoPtr = properties.GetBoundsGeometry();
    if (!geoPtr || geoPtr->IsEmpty()) {
        ROSEN_LOGE("RSPointLightDrawable::OnUpdate geoPtr is null or empty.");
        return false;
    }

    auto iter = lightSources.begin();
    auto cnt = 0;
    Drawing::Matrix44 lightPositionMatrix;
    Drawing::Matrix44 viewPosMatrix;
    Vector4f lightIntensityV4;
    while (iter != lightSources.end() && cnt < MAX_LIGHT_SOUCES) {
        auto lightPos = RSPointLightManager::Instance()->CalculateLightPosForIlluminated((*iter), geoPtr);
        lightIntensityV4[cnt] = (*iter)->GetLightIntensity();
        lightPositionMatrix.SetCol(cnt, lightPos.x_, lightPos.y_, lightPos.z_, lightPos.w_);
        viewPosMatrix.SetCol(cnt, lightPos.x_, lightPos.y_, lightPos.z_, lightPos.w_);
        iter++;
        cnt++;
    }
    lightBuilder->SetUniform("lightPos", lightPositionMatrix);
    lightBuilder->SetUniform("viewPos", viewPosMatrix);
    Drawing::Pen pen;
    Drawing::Brush brush;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    auto illuminatedType = properties.GetIlluminated()->GetIlluminatedType();
    ROSEN_LOGD("RSPointLightDrawable::OnUpdate illuminatedType:%{public}d", illuminatedType);
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    if (illuminatedType == IlluminatedType::CONTENT || illuminatedType == IlluminatedType::BORDER_CONTENT) {
        DrawContentLight(properties, canvas, lightBuilder, brush, lightIntensityV4);
    }
    if (illuminatedType == IlluminatedType::BORDER || illuminatedType == IlluminatedType::BORDER_CONTENT) {
        DrawBorderLight(properties, canvas, lightBuilder, pen, lightIntensityV4);
    }
    return true;
}

const std::shared_ptr<Drawing::RuntimeShaderBuilder>& RSPointLightDrawable::GetPhongShaderBuilder()
{
    if (phongShaderBuilder_) {
        return phongShaderBuilder_;
    }
    std::shared_ptr<Drawing::RuntimeEffect> lightEffect;
    std::string lightString(R"(
        uniform mat4 lightPos;
        uniform mat4 viewPos;
        uniform vec4 specularStrength;

        mediump vec4 main(vec2 drawing_coord) {
            vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
            float ambientStrength = 0.0;
            vec4 diffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
            float diffuseStrength = 0.0;
            vec4 specularColor = vec4(1.0, 1.0, 1.0, 1.0);
            float shininess = 8.0;
            mediump vec4 fragColor;
            vec4 NormalMap = vec4(0.0, 0.0, 1.0, 0.0);
            // ambient
            vec4 ambient = lightColor * ambientStrength;
            vec3 norm = normalize(NormalMap.rgb);

            for (int i = 0; i < 4; i++) {
                if (abs(specularStrength[i]) > 0.01) {
                    vec3 lightDir = normalize(vec3(lightPos[i].xy - drawing_coord, lightPos[i].z));
                    float diff = max(dot(norm, lightDir), 0.0);
                    vec4 diffuse = diff * lightColor;
                    vec3 viewDir = normalize(vec3(viewPos[i].xy - drawing_coord, viewPos[i].z)); // view vector
                    vec3 halfwayDir = normalize(lightDir + viewDir); // half vector
                    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess); // exponential relationship of angle
                    vec4 specular = lightColor * spec; // multiply color of incident light
                    vec4 o = ambient + diffuse * diffuseStrength * diffuseColor; // diffuse reflection
                    fragColor = fragColor + o + specular * specularStrength[i] * specularColor;
                }
            }
            return fragColor;
        }
    )");
    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(lightString);
    if (!effect) {
        ROSEN_LOGE("light effect error");
        return phongShaderBuilder_;
    }
    lightEffect = std::move(effect);
    phongShaderBuilder_ = std::make_shared<Drawing::RuntimeShaderBuilder>(lightEffect);
    return phongShaderBuilder_;
}

void RSPointLightDrawable::DrawContentLight(const RSProperties& properties, Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush, Vector4f& lightIntensity)
{
    auto contentStrength = lightIntensity * 0.3f;
    lightBuilder->SetUniformVec4("specularStrength", contentStrength.x_,
        contentStrength.y_, contentStrength.z_, contentStrength.w_);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    canvas.DrawRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetRRect()));
    canvas.DetachBrush();
}

void RSPointLightDrawable::DrawBorderLight(const RSProperties& properties, Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Pen& pen, Vector4f& lightIntensity)
{
    lightBuilder->SetUniformVec4("specularStrength", lightIntensity.x_,
        lightIntensity.y_, lightIntensity.z_, lightIntensity.w_);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    pen.SetShaderEffect(shader);
    float borderWidth = std::ceil(properties.GetIlluminatedBorderWidth());
    pen.SetWidth(borderWidth);
    auto borderRect = properties.GetRRect().rect_;
    float borderRadius = properties.GetRRect().radius_[0].x_;
    auto borderRRect = RRect(RectF(borderRect.left_ + borderWidth / 2.0f, borderRect.top_ + borderWidth / 2.0f,
        borderRect.width_ - borderWidth, borderRect.height_ - borderWidth),
        borderRadius - borderWidth / 2.0f, borderRadius - borderWidth / 2.0f);
    canvas.AttachPen(pen);
    canvas.DrawRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(borderRRect));
    canvas.DetachPen();
}
} // namespace OHOS::Rosen
