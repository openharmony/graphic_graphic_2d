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

#include "drawable/rs_property_drawable_content.h"
#include "drawable/rs_drawable_utils.h"

#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"

namespace OHOS::Rosen {
namespace {
bool g_forceBgAntiAlias = true;
constexpr int PARAM_TWO = 2;
} // namespace

void RSPropertyDrawableContent::OnSync()
{
    if (!needSync_) {
        return;
    }
    drawCmdList_ = std::move(stagingDrawCmdList_);
    needSync_ = false;
}

RSDrawable::Ptr RSPropertyDrawableContent::CreateDrawable() const
{
    auto ptr = std::static_pointer_cast<const RSPropertyDrawableContent>(shared_from_this());
    return std::make_shared<RSPropertyDrawableNG>(ptr);
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

RSDrawableContent::Ptr RSBackgroundContent::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSBackgroundContent>(); ret->OnUpdate(node)) {
        return ret;
    }
    return nullptr;
};

bool RSBackgroundContent::OnUpdate(const RSRenderNode& node)
{
    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    const RSProperties& properties = node.GetRenderProperties();
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    // only disable antialias when background is rect and g_forceBgAntiAlias is false
    bool antiAlias = g_forceBgAntiAlias || !properties.GetCornerRadius().IsZero();
    // paint backgroundColor
    Drawing::Brush brush;
    brush.SetAntiAlias(antiAlias);
    auto bgColor = properties.GetBackgroundColor();
    if (bgColor != RgbPalette::Transparent()) {
        brush.SetColor(Drawing::Color(bgColor.AsArgbInt()));
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(RSDrawableUtils::RRect2DrawingRRect(properties.GetRRect()));
        canvas.DetachBrush();
    }
    // paint backgroundShader
    if (const auto& bgShader = properties.GetBackgroundShader()) {
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipRoundRect(RSDrawableUtils::RRect2DrawingRRect(properties.GetRRect()),
            Drawing::ClipOp::INTERSECT, antiAlias);
        auto shaderEffect = bgShader->GetDrawingShader();
        brush.SetShaderEffect(shaderEffect);
        canvas.DrawBackground(brush);
    }
    // paint bgImage
    if (const auto& bgImage = properties.GetBgImage()) {
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipRoundRect(RSDrawableUtils::RRect2DrawingRRect(properties.GetRRect()),
            Drawing::ClipOp::INTERSECT, antiAlias);
        auto boundsRect = RSDrawableUtils::Rect2DrawingRect(properties.GetBoundsRect());
        bgImage->SetDstRect(properties.GetBgImageRect());
        canvas.AttachBrush(brush);
        bgImage->CanvasDrawImage(canvas, boundsRect, Drawing::SamplingOptions(), true);
        canvas.DetachBrush();
    }
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
} // namespace OHOS::Rosen
