/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "modifier_ng/overlay/rs_border_render_modifier.h"

#include "drawable/rs_property_drawable_utils.h"
#include "modifier_ng/rs_render_modifier_utils.h"
#include "pipeline/rs_recording_canvas.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen::ModifierNG {
namespace {
constexpr int PARAM_TWO = 2;
} // namespace

const RSBorderRenderModifier::LegacyPropertyApplierMap RSBorderRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::BORDER_COLOR,
        RSRenderModifier::PropertyApplyHelper<Vector4<Color>, &RSProperties::SetBorderColor> },
    { RSPropertyType::BORDER_WIDTH, RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetBorderWidth> },
    { RSPropertyType::BORDER_STYLE,
        RSRenderModifier::PropertyApplyHelper<Vector4<uint32_t>, &RSProperties::SetBorderStyle> },
    { RSPropertyType::BORDER_DASH_WIDTH,
        RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetBorderDashWidth> },
    { RSPropertyType::BORDER_DASH_GAP,
        RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetBorderDashGap> },
};

void RSBorderRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetBorderColor(RSColor());
    properties.SetBorderWidth(0.f);
    properties.SetBorderStyle((uint32_t)BorderStyle::NONE);
    properties.SetBorderDashWidth({ -1.f });
    properties.SetBorderDashGap({ -1.f });
}

/*
 * Protected Methods
 */
bool RSBorderRenderModifier::OnApply(RSModifierContext& context)
{
    if (!HasProperty(RSPropertyType::BORDER_COLOR) && !HasProperty(RSPropertyType::BORDER_WIDTH) &&
        !HasProperty(RSPropertyType::BORDER_STYLE) && !HasProperty(RSPropertyType::BORDER_DASH_WIDTH) &&
        !HasProperty(RSPropertyType::BORDER_DASH_GAP)) {
        return false;
    }

    RSBorder border;

    border.SetColorFour(Getter(RSPropertyType::BORDER_COLOR, Vector4<Color>(RgbPalette::Transparent())));
    border.SetWidthFour(Getter(RSPropertyType::BORDER_WIDTH, Vector4f(0.0f)));
    border.SetStyleFour(
        Getter(RSPropertyType::BORDER_STYLE, Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::NONE))));
    border.SetDashWidthFour(Getter(RSPropertyType::BORDER_DASH_WIDTH, Vector4f(0.0f)));
    border.SetDashGapFour(Getter(RSPropertyType::BORDER_DASH_GAP, Vector4f(0.0f)));

    if (!border.HasBorder()) {
        return false;
    }
    // regenerate stagingDrawCmdList_
    RSDisplayListModifierUpdater updater(this); // TODO: this should be a RSPropertyDrawable type, what should we do
    DrawBorder(context, *updater.GetRecordingCanvas(), border, false);
    return true;
}

/*
 * Private Methods
 */
void RSBorderRenderModifier::DrawBorder(
    RSModifierContext& context, Drawing::Canvas& canvas, const RSBorder& border, const bool& isOutline)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    brush.SetAntiAlias(true);
    pen.SetAntiAlias(true);
    if (border.ApplyFillStyle(brush)) {
        auto roundRect = RSPropertyDrawableUtils::RRect2DrawingRRect(GetRRectForDrawingBorder(context, border,
            isOutline)); // Use renderBorder here because we assume the DrawBorder is used at the render thread
        auto innerRoundRect =
            RSPropertyDrawableUtils::RRect2DrawingRRect(GetInnerRRectForDrawingBorder(context, border, isOutline));
        canvas.AttachBrush(brush);
        canvas.DrawNestedRoundRect(roundRect, innerRoundRect);
        canvas.DetachBrush();
        return;
    }
    Vector4f cornerRadius = context.clipBoundsParams_.borderRadius_;
    bool isZero = isOutline ? border.GetRadiusFour().IsZero() : cornerRadius.IsZero();
    if (isZero && border.ApplyFourLine(pen)) {
        RectF boundsRect = context.GetBoundsRect();
        RectF rectf = isOutline ? boundsRect.MakeOutset(border.GetWidthFour()) : boundsRect;
        border.PaintFourLine(canvas, pen, rectf);
        return;
    }
    if (border.ApplyPathStyle(pen)) {
        auto borderWidth = border.GetWidth();
        RRect rrect = GetRRectForDrawingBorder(context, border, isOutline);
        rrect.rect_.width_ -= borderWidth;
        rrect.rect_.height_ -= borderWidth;
        rrect.rect_.Move(borderWidth / PARAM_TWO, borderWidth / PARAM_TWO);
        Drawing::Path borderPath;
        borderPath.AddRoundRect(RSPropertyDrawableUtils::RRect2DrawingRRect(rrect));
        canvas.AttachPen(pen);
        canvas.DrawPath(borderPath);
        canvas.DetachPen();
        return;
    }

    RSBorderGeo borderGeo;
    borderGeo.rrect = RSPropertyDrawableUtils::RRect2DrawingRRect(GetRRectForDrawingBorder(context, border, isOutline));
    borderGeo.innerRRect =
        RSPropertyDrawableUtils::RRect2DrawingRRect(GetInnerRRectForDrawingBorder(context, border, isOutline));
    auto centerX = borderGeo.innerRRect.GetRect().GetLeft() + borderGeo.innerRRect.GetRect().GetWidth() / 2;
    auto centerY = borderGeo.innerRRect.GetRect().GetTop() + borderGeo.innerRRect.GetRect().GetHeight() / 2;
    borderGeo.center = { centerX, centerY };
    auto rect = borderGeo.rrect.GetRect();
    Drawing::AutoCanvasRestore acr(canvas, false);
    Drawing::SaveLayerOps slr(&rect, nullptr);
    canvas.SaveLayer(slr);
    border.DrawBorders(canvas, pen, borderGeo);
}

RRect RSBorderRenderModifier::GetRRectForDrawingBorder(
    RSModifierContext& context, const RSBorder& border, const bool& isOutline)
{
    return isOutline ? RRect(context.GetRRect().rect_.MakeOutset(border.GetWidthFour()), border.GetRadiusFour())
                     : context.GetRRect();
}

RRect RSBorderRenderModifier::GetInnerRRectForDrawingBorder(
    RSModifierContext& context, const RSBorder& border, const bool& isOutline)
{
    return isOutline ? context.GetRRect() : GetInnerRRect(context, border);
}

RRect RSBorderRenderModifier::GetInnerRRect(RSModifierContext& context, const RSBorder& border)
{
    auto rect = context.GetBoundsRect();
    Vector4f cornerRadius = context.clipBoundsParams_.borderRadius_;
    rect.left_ += border.GetWidth(RSBorder::LEFT);
    rect.top_ += border.GetWidth(RSBorder::TOP);
    rect.width_ -= border.GetWidth(RSBorder::LEFT) + border.GetWidth(RSBorder::RIGHT);
    rect.height_ -= border.GetWidth(RSBorder::TOP) + border.GetWidth(RSBorder::BOTTOM);
    RRect rrect = RRect(rect, cornerRadius);
    rrect.radius_[0] -= { border.GetWidth(RSBorder::LEFT), border.GetWidth(RSBorder::TOP) };
    rrect.radius_[1] -= { border.GetWidth(RSBorder::RIGHT), border.GetWidth(RSBorder::TOP) };
    rrect.radius_[2] -= { border.GetWidth(RSBorder::RIGHT), border.GetWidth(RSBorder::BOTTOM) };
    rrect.radius_[3] -= { border.GetWidth(RSBorder::LEFT), border.GetWidth(RSBorder::BOTTOM) };
    return rrect;
}
} // namespace OHOS::Rosen::ModifierNG
