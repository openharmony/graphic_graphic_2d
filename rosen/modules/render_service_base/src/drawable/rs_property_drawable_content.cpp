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

#include "rs_trace.h"

#include "drawable/rs_property_drawable_content.h"
#include "drawable/rs_drawable_utils.h"
#include "common/rs_obj_abs_geometry.h"
#include "effect/runtime_blender_builder.h"

#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"

namespace OHOS::Rosen {
namespace {
bool g_forceBgAntiAlias = true;
constexpr int PARAM_TWO = 2;
} // namespace

std::shared_ptr<Drawing::RuntimeEffect> RSDynamicLightUpContent::dynamicLightUpBlenderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSBinarizationShaderContent::binarizationShaderEffect_ = nullptr;

void RSPropertyDrawableContent::OnSync()
{
    if (!needSync_) {
        return;
    }
    drawCmdList_ = std::move(stagingDrawCmdList_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSPropertyDrawableContent::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSPropertyDrawableContent>(shared_from_this());
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
    explicit RSPropertyDrawCmdListUpdater(int width, int height, RSPropertyDrawableContent* target)
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
    RSPropertyDrawableContent* target_;
};

RSDrawableContent::Ptr RSBackgroundColorContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBackgroundColorContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSBackgroundColorContent::OnUpdate(const RSRenderNode& node)
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
    canvas.DrawRoundRect(RSDrawableUtils::RRect2DrawingRRect(properties.GetRRect()));
    canvas.DetachBrush();
    return true;
}

RSDrawableContent::Ptr RSBackgroundShaderContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBackgroundShaderContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSBackgroundShaderContent::OnUpdate(const RSRenderNode& node)
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

RSDrawableContent::Ptr RSBackgroundImageContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBackgroundImageContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSBackgroundImageContent::OnUpdate(const RSRenderNode& node)
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
    auto boundsRect = RSDrawableUtils::Rect2DrawingRect(properties.GetBoundsRect());
    bgImage->SetDstRect(properties.GetBgImageRect());
    canvas.AttachBrush(brush);
    bgImage->CanvasDrawImage(canvas, boundsRect, Drawing::SamplingOptions(), true);
    canvas.DetachBrush();
    return true;
}

RSDrawableContent::Ptr RSBackgroundFilterContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBackgroundFilterContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSBackgroundFilterContent::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto& rsFilter = properties.GetBackgroundFilter();
    if (rsFilter == nullptr) {
        return false;
    }
    RS_TRACE_NAME("DrawBackgroundFilter " + rsFilter->GetDescription());
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSBackgroundFilterContent::OnUpdate surface null");
        return false;
    }

    auto bounds = canvas.GetDeviceClipBounds();
    auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    // Optional use cacheManager to draw filter
    // TODO canvas.GetDisableFilterCache(), canvas.SetEffectData, cacheManager->GeneratedCachedEffectData
    // if (auto& cacheManager = properties.GetFilterCacheManager(false);
    //     cacheManager != nullptr && !canvas.GetDisableFilterCache()) {
    //     auto node = properties.backref_.lock();
    //     if (node == nullptr) {
    //         ROSEN_LOGE("RSBackgroundFilterContent::OnUpdate node is null");
    //         return false;
    //     }
    //     auto effectNode = node->ReinterpretCastTo<RSEffectRenderNode>();
    //     if (effectNode == nullptr) {
    //         ROSEN_LOGE("RSBackgroundFilterContent::OnUpdate node reinterpret cast failed.");
    //         return false;
    //     }
    //     // node is freeze or screen rotating, force cache filterred snapshot.
    //     auto forceCacheFlags = std::make_tuple(effectNode->IsStaticCached(), effectNode->GetRotationChanged());
    //     auto&& data = cacheManager->GeneratedCachedEffectData(canvas, filter, bounds, bounds, forceCacheFlags);
    //     canvas.SetEffectData(data);
    //     return true;
    // }
#endif

    auto imageRect = bounds;
    auto imageSnapshot = surface->GetImageSnapshot(imageRect);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSBackgroundFilterContent::OnUpdate image snapshot null");
        return false;
    }

    filter->PreProcess(imageSnapshot);
    // create a offscreen skSurface
    std::shared_ptr<Drawing::Surface> offscreenSurface =
        surface->MakeSurface(imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
    if (offscreenSurface == nullptr) {
        ROSEN_LOGE("RSBackgroundFilterContent::OnUpdate offscreenSurface null");
        return false;
    }
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());
    auto clipBounds = Drawing::Rect(0, 0, imageRect.GetWidth(), imageRect.GetHeight());
    auto imageSnapshotBounds = Drawing::Rect(0, 0, imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
    filter->DrawImageRect(offscreenCanvas, imageSnapshot, imageSnapshotBounds, clipBounds);
    filter->PostProcess(offscreenCanvas);

    auto imageCache = offscreenSurface->GetImageSnapshot();
    if (imageCache == nullptr) {
        ROSEN_LOGE("RSBackgroundFilterContent::OnUpdate imageCache snapshot null");
        return false;
    }
    // TODO canvas.SetEffectData
    // auto data = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(imageCache),
    //     std::move(imageRect));
    // canvas.SetEffectData(std::move(data));
    return true;
}

RSDrawableContent::Ptr RSBorderContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBorderContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSBorderContent::OnUpdate(const RSRenderNode& node)
{
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    const RSProperties& properties = node.GetRenderProperties();
    DrawBorder(properties, *updater.GetRecordingCanvas(), properties.GetBorder(), false);
    return true;
}

void RSBorderContent::DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas,
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
        auto roundRect = RSDrawableUtils::RRect2DrawingRRect(RSDrawableUtils::GetRRectForDrawingBorder(properties,
            border, isOutline));
        auto innerRoundRect = RSDrawableUtils::RRect2DrawingRRect(RSDrawableUtils::GetInnerRRectForDrawingBorder(
            properties, border, isOutline));
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
            RRect rrect = RSDrawableUtils::GetRRectForDrawingBorder(properties, border, isOutline);
            rrect.rect_.width_ -= borderWidth;
            rrect.rect_.height_ -= borderWidth;
            rrect.rect_.Move(borderWidth / PARAM_TWO, borderWidth / PARAM_TWO);
            Drawing::Path borderPath;
            borderPath.AddRoundRect(RSDrawableUtils::RRect2DrawingRRect(rrect));
            canvas.AttachPen(pen);
            canvas.DrawPath(borderPath);
            canvas.DetachPen();
        } else {
            Drawing::AutoCanvasRestore acr(canvas, true);
            auto rrect = RSDrawableUtils::RRect2DrawingRRect(RSDrawableUtils::GetRRectForDrawingBorder(properties,
                border, isOutline));
            canvas.ClipRoundRect(rrect, Drawing::ClipOp::INTERSECT, true);
            auto innerRoundRect = RSDrawableUtils::RRect2DrawingRRect(RSDrawableUtils::GetInnerRRectForDrawingBorder(
                properties, border, isOutline));
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

RSDrawableContent::Ptr RSOutlineContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSOutlineContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSOutlineContent::OnUpdate(const RSRenderNode& node)
{
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    const RSProperties& properties = node.GetRenderProperties();
    RSBorderContent::DrawBorder(properties, *updater.GetRecordingCanvas(), properties.GetOutline(), true);
    return true;
}

RSDrawableContent::Ptr RSShadowContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSShadowContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSShadowContent::OnUpdate(const RSRenderNode& node)
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
        path.AddRoundRect(RSDrawableUtils::RRect2DrawingRRect(rrect));
        if (!properties.GetShadowIsFilled()) {
            canvas.ClipRoundRect(RSDrawableUtils::RRect2DrawingRRect(rrect), Drawing::ClipOp::DIFFERENCE, true);
        }
    }
    if (properties.GetShadowMask()) {
        DrawColorfulShadowInner(properties, canvas, path);
    } else {
        DrawShadowInner(properties, canvas, path);
    }
    return true;
}

void RSShadowContent::DrawColorfulShadowInner(const RSProperties& properties, Drawing::Canvas& canvas,
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

void RSShadowContent::DrawShadowInner(const RSProperties& properties, Drawing::Canvas& canvas, Drawing::Path& path)
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
        if (RSDrawableUtils::PickColor(properties, canvas, path, matrix, deviceClipBounds, colorPicked)) {
            RSDrawableUtils::GetDarkColor(colorPicked);
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

RSDrawableContent::Ptr RSForegroundColorContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSForegroundColorContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSForegroundColorContent::OnUpdate(const RSRenderNode& node)
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
    canvas.DrawRoundRect(RSDrawableUtils::RRect2DrawingRRect(properties.GetRRect()));
    canvas.DetachBrush();
    return true;
}

RSDrawableContent::Ptr RSPixelStretchContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSPixelStretchContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSPixelStretchContent::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto& pixelStretch = properties.GetPixelStretch();
    if (!pixelStretch.has_value()) {
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPixelStretchContent::OnUpdate surface null");
        return false;
    }

    /*  Calculates the relative coordinates of the clipbounds
        with respect to the origin of the current canvas coordinates */
    Drawing::Matrix worldToLocalMat;
    if (!canvas.GetTotalMatrix().Invert(worldToLocalMat)) {
        ROSEN_LOGE("RSPixelStretchContent::OnUpdate get invert matrix failed.");
    }
    Drawing::Rect localClipBounds;
    auto tmpBounds = canvas.GetDeviceClipBounds();
    Drawing::Rect clipBounds(
        tmpBounds.GetLeft(), tmpBounds.GetTop(), tmpBounds.GetRight() - 1, tmpBounds.GetBottom() - 1);
    Drawing::Rect fClipBounds(clipBounds.GetLeft(), clipBounds.GetTop(), clipBounds.GetRight(),
        clipBounds.GetBottom());
    if (!worldToLocalMat.MapRect(localClipBounds, fClipBounds)) {
        ROSEN_LOGE("RSPixelStretchContent::OnUpdate map rect failed.");
    }

    auto bounds = RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect());
    if (!bounds.Intersect(localClipBounds)) {
        ROSEN_LOGE("RSPixelStretchContent::OnUpdate intersect clipbounds failed");
    }

    auto scaledBounds = Drawing::Rect(bounds.GetLeft() - pixelStretch->x_, bounds.GetTop() - pixelStretch->y_,
        bounds.GetRight() + pixelStretch->z_, bounds.GetBottom() + pixelStretch->w_);
    if (!scaledBounds.IsValid() || !bounds.IsValid() || !clipBounds.IsValid()) {
        ROSEN_LOGE("RSPixelStretchContent::OnUpdate invalid scaled bounds");
        return false;
    }

    Drawing::RectI rectI(static_cast<int>(fClipBounds.GetLeft()), static_cast<int>(fClipBounds.GetTop()),
        static_cast<int>(fClipBounds.GetRight()), static_cast<int>(fClipBounds.GetBottom()));
    auto image = surface->GetImageSnapshot(rectI);
    if (image == nullptr) {
        ROSEN_LOGE("RSPixelStretchContent::OnUpdate image null");
        return false;
    }

    Drawing::Brush brush;
    Drawing::Matrix inverseMat;
    Drawing::Matrix rotateMat;
    auto boundsGeo = properties.GetBoundsGeometry();
    if (boundsGeo && !boundsGeo->IsEmpty()) {
        auto transMat = canvas.GetTotalMatrix();
        /* transMat.getSkewY() is the sin of the rotation angle(sin0 = 0,sin90 =1 sin180 = 0,sin270 = -1),
            if transMat.getSkewY() is not 0 or -1 or 1,the rotation angle is not a multiple of 90,not Stretch*/
        auto skewY = transMat.Get(Drawing::Matrix::SKEW_Y);
        if (ROSEN_EQ(skewY, 0.f) || ROSEN_EQ(skewY, 1.f) ||
            ROSEN_EQ(skewY, -1.f)) {
        } else {
            ROSEN_LOGD("rotate degree is not 0 or 90 or 180 or 270,return.");
            return false;
        }
        rotateMat.SetScale(transMat.Get(Drawing::Matrix::SCALE_X), transMat.Get(Drawing::Matrix::SCALE_Y));
        rotateMat.Set(Drawing::Matrix::SKEW_X, transMat.Get(Drawing::Matrix::SKEW_X));
        rotateMat.Set(Drawing::Matrix::SKEW_Y, transMat.Get(Drawing::Matrix::SKEW_Y));
        rotateMat.PreTranslate(-bounds.GetLeft(), -bounds.GetTop());
        rotateMat.PostTranslate(bounds.GetLeft(), bounds.GetTop());

        Drawing::Rect transBounds;
        rotateMat.MapRect(transBounds, bounds);
        rotateMat.Set(Drawing::Matrix::TRANS_X, bounds.GetLeft() - transBounds.GetLeft());
        rotateMat.Set(Drawing::Matrix::TRANS_Y, bounds.GetTop() - transBounds.GetTop());
        if (!rotateMat.Invert(inverseMat)) {
            ROSEN_LOGE("RSPixelStretchContent::OnUpdate get invert matrix failed.");
        }
    }

    canvas.Save();
    canvas.Translate(bounds.GetLeft(), bounds.GetTop());
    Drawing::SamplingOptions samplingOptions;
    constexpr static float EPS = 1e-5f;
    if (pixelStretch->x_ > EPS || pixelStretch->y_ > EPS || pixelStretch->z_ > EPS || pixelStretch->w_ > EPS) {
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, inverseMat));
        canvas.AttachBrush(brush);
        canvas.DrawRect(Drawing::Rect(-pixelStretch->x_, -pixelStretch->y_,
            -pixelStretch->x_ + scaledBounds.GetWidth(), -pixelStretch->y_ + scaledBounds.GetHeight()));
        canvas.DetachBrush();
    } else {
        inverseMat.PostScale(scaledBounds.GetWidth() / bounds.GetWidth(),
            scaledBounds.GetHeight() / bounds.GetHeight());
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, inverseMat));

        canvas.Translate(-pixelStretch->x_, -pixelStretch->y_);
        canvas.AttachBrush(brush);
        canvas.DrawRect(Drawing::Rect(pixelStretch->x_, pixelStretch->y_,
            pixelStretch->x_ + bounds.GetWidth(), pixelStretch->y_ + bounds.GetHeight()));
        canvas.DetachBrush();
    }
    canvas.Restore();
    return true;
}

RSDrawableContent::Ptr RSDynamicLightUpContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSDynamicLightUpContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSDynamicLightUpContent::OnUpdate(const RSRenderNode& node)
{
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    Drawing::Surface* surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSDynamicLightUpContent::OnUpdate surface is null");
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

std::shared_ptr<Drawing::Blender> RSDynamicLightUpContent::MakeDynamicLightUpBlender(float dynamicLightUpRate,
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
            ROSEN_LOGE("RSDynamicLightUpContent::MakeDynamicLightUpBlender effect error!");
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::RuntimeBlenderBuilder> builder =
        std::make_shared<Drawing::RuntimeBlenderBuilder>(dynamicLightUpBlenderEffect_);
    builder->SetUniform("dynamicLightUpRate", dynamicLightUpRate);
    builder->SetUniform("dynamicLightUpDeg", dynamicLightUpDeg);
    return builder->MakeBlender();
}

RSDrawableContent::Ptr RSLightUpEffectContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSLightUpEffectContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSLightUpEffectContent::OnUpdate(const RSRenderNode& node)
{
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    Drawing::Surface* surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSLightUpEffectContent::OnUpdate surface is null");
        return false;
    }

    auto clipBounds = canvas.GetDeviceClipBounds();
    auto image = surface->GetImageSnapshot(clipBounds);
    if (image == nullptr) {
        ROSEN_LOGE("RSLightUpEffectContent::OnUpdate image is null");
        return false;
    }

    Drawing::Matrix scaleMat;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), scaleMat);
    const RSProperties& properties = node.GetRenderProperties();
    auto shader = Drawing::ShaderEffect::CreateLightUp(properties.GetLightUpEffect(), *imageShader);
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.ResetMatrix();
    canvas.Translate(clipBounds.GetLeft(), clipBounds.GetTop());
    canvas.DrawBackground(brush);
    return true;
}

RSDrawableContent::Ptr RSColorFilterContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSColorFilterContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSColorFilterContent::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    // if useEffect defined, use color filter from parent EffectView.
    auto& colorFilter = properties.GetColorFilter();
    if (colorFilter == nullptr) {
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetColorFilter(colorFilter);
    brush.SetFilter(filter);
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSColorFilterContent::OnUpdate surface is null");
        return false;
    }
    auto clipBounds = canvas.GetDeviceClipBounds();
    auto imageSnapshot = surface->GetImageSnapshot(clipBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSColorFilterContent::OnUpdate image is null");
        return false;
    }
    imageSnapshot->HintCacheGpuResource();
    canvas.ResetMatrix();
    Drawing::SamplingOptions options(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*imageSnapshot, clipBounds, options);
    canvas.DetachBrush();
    return true;
}

RSDrawableContent::Ptr RSMaskContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSMaskContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSMaskContent::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    std::shared_ptr<RSMask> mask = properties.GetMask();
    if (mask == nullptr) {
        ROSEN_LOGE("RSMaskContent::OnUpdate null mask");
        return false;
    }
    if (mask->IsSvgMask() && !mask->GetSvgDom() && !mask->GetSvgPicture()) {
        ROSEN_LOGE("RSMaskContent::OnUpdate not has Svg Mask property");
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    Drawing::Rect maskBounds = RSDrawableUtils::Rect2DrawingRect(properties.GetBoundsRect());
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

RSDrawableContent::Ptr RSBinarizationShaderContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBinarizationShaderContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSBinarizationShaderContent::OnUpdate(const RSRenderNode& node)
{
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    const RSProperties& properties = node.GetRenderProperties();
    auto drSurface = canvas.GetSurface();
    if (drSurface == nullptr) {
        ROSEN_LOGE("RSBinarizationShaderContent::OnUpdate drSurface is null");
        return false;
    }
    auto clipBounds = canvas.GetDeviceClipBounds();
    auto imageSnapshot = drSurface->GetImageSnapshot(clipBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSBinarizationShaderContent::OnUpdate image is null");
        return false;
    }
    auto& aiInvert = properties.GetAiInvert();
    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*imageSnapshot, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    float thresholdLow = aiInvert->z_ - aiInvert->w_;
    float thresholdHigh = aiInvert->z_ + aiInvert->w_;
    auto shader = MakeBinarizationShader(aiInvert->x_, aiInvert->y_, thresholdLow, thresholdHigh, imageShader);
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    brush.SetAntiAlias(true);
    canvas.ResetMatrix();
    canvas.Translate(clipBounds.GetLeft(), clipBounds.GetTop());
    canvas.DrawBackground(brush);
    return true;
}

std::shared_ptr<Drawing::ShaderEffect> RSBinarizationShaderContent::MakeBinarizationShader(float low, float high,
    float thresholdLow, float thresholdHigh, std::shared_ptr<Drawing::ShaderEffect> imageShader)
{
    static constexpr char prog[] = R"(
        uniform half low;
        uniform half high;
        uniform half thresholdLow;
        uniform half thresholdHigh;
        uniform shader imageShader;

        half4 main(float2 coord) {
            half3 c = imageShader.eval(float2(coord.x, coord.y)).rgb;
            float gray = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
            float lowRes = mix(high, -1.0, step(thresholdLow, gray));
            float highRes = mix(-1.0, low, step(thresholdHigh, gray));
            float midRes = (thresholdHigh - gray) * (high - low) / (thresholdHigh - thresholdLow) + low;
            float invertedGray = mix(midRes, max(lowRes, highRes), step(-0.5, max(lowRes, highRes)));
            half3 invert = half3(invertedGray);
            return half4(invert, 1.0);
        }
    )";
    if (binarizationShaderEffect_ == nullptr) {
        binarizationShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
        if (binarizationShaderEffect_ == nullptr) {
            ROSEN_LOGE("RSBinarizationShaderContent::MakeBinarizationShader effect error\n");
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(binarizationShaderEffect_);
    thresholdHigh = thresholdHigh <= thresholdLow ? thresholdHigh + 1e-6 : thresholdHigh;
    builder->SetChild("imageShader", imageShader);
    builder->SetUniform("low", low);
    builder->SetUniform("high", high);
    builder->SetUniform("thresholdLow", thresholdLow);
    builder->SetUniform("thresholdHigh", thresholdHigh);
    return builder->MakeShader(nullptr, false);
}

RSDrawableContent::Ptr RSParticleContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSParticleContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSParticleContent::OnUpdate(const RSRenderNode& node)
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
} // namespace OHOS::Rosen
