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

#include "modifier_ng/background/rs_outline_render_modifier.h"

#include "drawable/rs_property_drawable_utils.h"
#include "modifier_ng/rs_render_modifier_utils.h"

namespace OHOS::Rosen::ModifierNG {
constexpr int PARAM_TWO = 2;

const RSOutlineRenderModifier::LegacyPropertyApplierMap RSOutlineRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::OUTLINE_COLOR,
        RSRenderModifier::PropertyApplyHelper<Vector4<Color>, &RSProperties::SetOutlineColor> },
    { RSPropertyType::OUTLINE_WIDTH, RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetOutlineWidth> },
    { RSPropertyType::OUTLINE_STYLE,
        RSRenderModifier::PropertyApplyHelper<Vector4<uint32_t>, &RSProperties::SetOutlineStyle> },
    { RSPropertyType::OUTLINE_DASH_WIDTH,
        RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetOutlineDashWidth> },
    { RSPropertyType::OUTLINE_DASH_GAP,
        RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetOutlineDashGap> },
    { RSPropertyType::OUTLINE_RADIUS, RSRenderModifier::PropertyApplyHelperAdd<Vector4f,
                                          &RSProperties::SetOutlineRadius, &RSProperties::GetOutlineRadius> },
};

void RSOutlineRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetOutlineColor(RSColor());
    properties.SetOutlineWidth(0.f);
    properties.SetOutlineStyle((uint32_t)BorderStyle::NONE);
    properties.SetOutlineDashWidth({ -1.f });
    properties.SetOutlineDashGap({ -1.f });
    properties.SetOutlineRadius(0.f);
}

bool RSOutlineRenderModifier::OnApply(RSModifierContext& context)
{
    if (!(HasProperty(RSPropertyType::OUTLINE_COLOR) || HasProperty(RSPropertyType::OUTLINE_WIDTH) ||
            HasProperty(RSPropertyType::OUTLINE_STYLE) || HasProperty(RSPropertyType::OUTLINE_DASH_WIDTH) ||
            HasProperty(RSPropertyType::OUTLINE_DASH_GAP) || HasProperty(RSPropertyType::OUTLINE_RADIUS))) {
        return false;
    }
    auto outline = GetOutline();
    // regenerate stagingDrawCmdList_
    RSDisplayListModifierUpdater updater(this);
    RSOutlineRenderModifier::DrawBorder(context, *updater.GetRecordingCanvas(), outline, true);
    return true;
}

std::shared_ptr<RSBorder> RSOutlineRenderModifier::GetOutline()
{
    auto outline = std::make_shared<RSBorder>(true);
    outline->SetColorFour(Getter<Vector4<Color>>(RSPropertyType::OUTLINE_COLOR, Vector4<Color>()));
    outline->SetWidthFour(Getter<Vector4f>(RSPropertyType::OUTLINE_WIDTH, Vector4f(0.f)));
    outline->SetStyleFour(Getter<Vector4<uint32_t>>(RSPropertyType::OUTLINE_STYLE, Vector4<uint32_t>()));
    outline->SetDashWidthFour(Getter<Vector4f>(RSPropertyType::OUTLINE_DASH_WIDTH, Vector4f(0.f)));
    outline->SetDashGapFour(Getter<Vector4f>(RSPropertyType::OUTLINE_DASH_GAP, Vector4f(0.f)));
    outline->SetRadiusFour(Getter<Vector4f>(RSPropertyType::OUTLINE_RADIUS, Vector4f(0.f)));
    return outline;
}

void RSOutlineRenderModifier::DrawBorder(
    const RSModifierContext& context, Drawing::Canvas& canvas, const std::shared_ptr<RSBorder>& border, bool isOutline)
{
    auto GetOutRRect = [](const RSModifierContext& context, const std::shared_ptr<RSBorder>& border, bool isOutline) {
        auto outRRect =
            isOutline ? RRect(context.GetRRect().rect_.MakeOutset(border->GetWidthFour()), border->GetRadiusFour())
                      : context.GetRRect();
        return outRRect;
    };
    Drawing::Brush brush;
    Drawing::Pen pen;
    brush.SetAntiAlias(true);
    pen.SetAntiAlias(true);
    if (border->ApplyFillStyle(brush)) {
        auto roundRect = RSPropertyDrawableUtils::RRect2DrawingRRect(GetOutRRect(context, border, isOutline));
        auto innerRect = isOutline ? context.GetRRect() : GetInnerRRect(context, border);
        auto innerRoundRect = RSPropertyDrawableUtils::RRect2DrawingRRect(innerRect);
        canvas.AttachBrush(brush);
        canvas.DrawNestedRoundRect(roundRect, innerRoundRect);
        canvas.DetachBrush();
        return;
    }
    bool isZero = isOutline ? border->GetRadiusFour().IsZero() : context.clipBoundsParams_.borderRadius_.IsZero();
    if (isZero && border->ApplyFourLine(pen)) {
        RectF rectf = isOutline ? context.GetBoundsRect().MakeOutset(border->GetWidthFour()) : context.GetBoundsRect();
        border->PaintFourLine(canvas, pen, rectf);
        return;
    }
    if (border->ApplyPathStyle(pen)) {
        auto borderWidth = border->GetWidth();
        RRect rrect = GetOutRRect(context, border, isOutline);
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
    borderGeo.rrect = RSPropertyDrawableUtils::RRect2DrawingRRect(GetOutRRect(context, border, isOutline));
    auto geoInnerRect = isOutline ? context.GetRRect() : GetInnerRRect(context, border);
    borderGeo.innerRRect = RSPropertyDrawableUtils::RRect2DrawingRRect(geoInnerRect);
    auto centerX = borderGeo.innerRRect.GetRect().GetLeft() + borderGeo.innerRRect.GetRect().GetWidth() / 2;
    auto centerY = borderGeo.innerRRect.GetRect().GetTop() + borderGeo.innerRRect.GetRect().GetHeight() / 2;
    borderGeo.center = { centerX, centerY };
    auto rect = borderGeo.rrect.GetRect();
    Drawing::AutoCanvasRestore acr(canvas, false);
    Drawing::SaveLayerOps slr(&rect, nullptr);
    canvas.SaveLayer(slr);
    border->DrawBorders(canvas, pen, borderGeo);
}

RRect RSOutlineRenderModifier::GetInnerRRect(const RSModifierContext& context, const std::shared_ptr<RSBorder>& border)
{
    auto rect = context.GetBoundsRect();
    if (border) {
        rect.left_ += border->GetWidth(RSBorder::LEFT);
        rect.top_ += border->GetWidth(RSBorder::TOP);
        rect.width_ -= border->GetWidth(RSBorder::LEFT) + border->GetWidth(RSBorder::RIGHT);
        rect.height_ -= border->GetWidth(RSBorder::TOP) + border->GetWidth(RSBorder::BOTTOM);
    }
    RRect rrect = RRect(rect, context.clipBoundsParams_.borderRadius_);
    if (border) {
        rrect.radius_[0] -= { border->GetWidth(RSBorder::LEFT), border->GetWidth(RSBorder::TOP) };
        rrect.radius_[1] -= { border->GetWidth(RSBorder::RIGHT), border->GetWidth(RSBorder::TOP) };
        rrect.radius_[2] -= { border->GetWidth(RSBorder::RIGHT), border->GetWidth(RSBorder::BOTTOM) };
        rrect.radius_[3] -= { border->GetWidth(RSBorder::LEFT), border->GetWidth(RSBorder::BOTTOM) };
    }
    return rrect;
}
} // namespace OHOS::Rosen::ModifierNG
