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
#include "text_span.h"

#include <iomanip>
#include <stack>
#include <utility>

#include <hb-icu.h>
#include <unicode/ubidi.h>

#include "font_collection.h"
#include "font_styles.h"
#include "measurer.h"
#include "texgine_dash_path_effect.h"
#include "texgine_exception.h"
#include "texgine_mask_filter.h"
#include "texgine_path.h"
#include "texgine_path_1d_path_effect.h"
#include "texgine/utils/exlog.h"
#ifdef LOGGER_ENABLE_SCOPE
#include "texgine/utils/trace.h"
#endif
#include "text_converter.h"
#include "word_breaker.h"
#include "symbol_engine/hm_symbol_run.h"
#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define MAXRGB 255
#define MAXALPHA 255
#define OFFSETY 3
#define HALF 0.5f
#define DEFAULT_FONT_SIZE 14.0f
#define SCALE 0.7f
#define WIDTH_SCALAR 5.0f
#define HEIGHT_SCALAR 5.0f
#define DOTTED_ADVANCE 10.0f
#define WAVY_ADVANCE 6.0f
#define PHASE 0.0f
#define COUNT 2
#define MAX_BLURRADIUS 64u
#define POINTX0 0
#define POINTX1 1
#define POINTX2 2
#define POINTX3 3
#define POINTX4 4
#define POINTX5 5
#define POINTX6 6
#define POINTY0 0
#define POINTY2 2
#define POINTY4 4
#define POINTY6 6

#ifdef BUILD_NON_SDK_VER
const bool G_IS_HMSYMBOL_ENABLE = Drawing::SystemProperties::GetHMSymbolEnable();
#else
const bool G_IS_HMSYMBOL_ENABLE = true;
#endif

std::shared_ptr<TextSpan> TextSpan::MakeEmpty()
{
    return std::make_shared<TextSpan>();
}

std::shared_ptr<TextSpan> TextSpan::MakeFromText(const std::string &text)
{
    auto span = MakeEmpty();
    auto decodeText = TextConverter::ToUTF16(text);
    span->AddUTF16Text(decodeText);
    return span;
}

std::shared_ptr<TextSpan> TextSpan::MakeFromText(const std::u16string &text)
{
    auto span = MakeEmpty();
    std::vector<uint16_t> u16;
    for (const auto &t : text) {
        u16.push_back(t);
    }
    span->AddUTF16Text(u16);
    return span;
}

std::shared_ptr<TextSpan> TextSpan::MakeFromText(const std::u32string &text)
{
    auto span = MakeEmpty();
    std::vector<uint32_t> u32;
    for (const auto &t : text) {
        u32.push_back(t);
    }
    auto decodeText = TextConverter::ToUTF16(u32);
    span->AddUTF16Text(decodeText);
    return span;
}

std::shared_ptr<TextSpan> TextSpan::MakeFromText(const std::vector<uint16_t> &text)
{
    auto span = MakeEmpty();
    span->AddUTF16Text(text);
    return span;
}

std::shared_ptr<TextSpan> TextSpan::MakeFromText(const std::vector<uint32_t> &text)
{
    auto span = MakeEmpty();
    auto decodeText = TextConverter::ToUTF16(text);
    span->AddUTF16Text(decodeText);
    return span;
}

std::shared_ptr<TextSpan> TextSpan::MakeFromCharGroups(const CharGroups &cgs)
{
    auto span = MakeEmpty();
    span->cgs_= cgs;
    return span;
}

void TextSpan::AddUTF16Text(const std::vector<uint16_t> &text)
{
    u16vect_.insert(u16vect_.end(), text.begin(), text.end());
}

std::shared_ptr<TextSpan> TextSpan::CloneWithCharGroups(const CharGroups &cgs)
{
    auto span = MakeEmpty();
    *span = *this;
    span->cgs_ = cgs;
    return span;
}

double TextSpan::GetHeight() const
{
    return *tmetrics_->fDescent_ - *tmetrics_->fAscent_;
}

double TextSpan::GetWidth() const
{
    return width_;
}

double TextSpan::GetPreBreak() const
{
    return preBreak_;
}

double TextSpan::GetPostBreak() const
{
    return postBreak_;
}

bool TextSpan::IsRTL() const
{
    return rtl_;
}

void TextSpan::Paint(TexgineCanvas &canvas, double offsetX, double offsetY, const TextStyle &xs,
    const RoundRectType &rType)
{
    TexginePaint paint;
    paint.SetAntiAlias(true);
#ifndef USE_GRAPHIC_TEXT_GINE
    paint.SetARGB(MAXRGB, MAXRGB, 0, 0);
#else
    paint.SetAlpha(MAXALPHA);
#endif
    if (xs.background.has_value()) {
        auto rect = TexgineRect::MakeXYWH(offsetX, offsetY + *tmetrics_->fAscent_, width_,
            *tmetrics_->fDescent_ - *tmetrics_->fAscent_);
#ifdef CROSS_PLATFORM
        canvas.DrawRect(rect, xs.background.__get());
#else
        canvas.DrawRect(rect, xs.background.value());
#endif
    }

    if (xs.backgroundRect.color != 0) {
        paint.SetColor(xs.backgroundRect.color);
        double ltRadius = 0.0;
        double rtRadius = 0.0;
        double rbRadius = 0.0;
        double lbRadius = 0.0;
        if (rType == RoundRectType::ALL || rType == RoundRectType::LEFT_ONLY) {
            ltRadius = std::fmin(xs.backgroundRect.leftTopRadius, maxRoundRectRadius_);
            lbRadius = std::fmin(xs.backgroundRect.leftBottomRadius, maxRoundRectRadius_);
        }
        if (rType == RoundRectType::ALL || rType == RoundRectType::RIGHT_ONLY) {
            rtRadius = std::fmin(xs.backgroundRect.rightTopRadius, maxRoundRectRadius_);
            rbRadius = std::fmin(xs.backgroundRect.rightBottomRadius, maxRoundRectRadius_);
        }
        const SkVector fRadii[4] = {{ltRadius, ltRadius}, {rtRadius, rtRadius}, {rbRadius, rbRadius},
            {lbRadius, lbRadius}};
        auto rect = TexgineRect::MakeRRect(offsetX, offsetY + topInGroup_, width_,
            bottomInGroup_ - topInGroup_, fRadii);
        paint.SetAntiAlias(false);
        canvas.DrawRRect(rect, paint);
    }

    paint.SetAntiAlias(true);
    paint.SetColor(xs.color);
    if (xs.foreground.has_value()) {
#ifdef CROSS_PLATFORM
        paint = xs.foreground.__get();
#else
        paint = xs.foreground.value();
#endif
    }

    PaintShadow(canvas, offsetX, offsetY, xs.shadows);
    if (xs.isSymbolGlyph && G_IS_HMSYMBOL_ENABLE) {
        std::pair<double, double> offset(offsetX, offsetY);
        HMSymbolRun hmSymbolRun = HMSymbolRun();
        hmSymbolRun.SetAnimation(animationFunc_);
        hmSymbolRun.SetSymbolId(symbolId_);
        hmSymbolRun.DrawSymbol(canvas, textBlob_, offset, paint, xs);
    } else {
        canvas.DrawTextBlob(textBlob_, offsetX, offsetY, paint);
    }
    PaintDecoration(canvas, offsetX, offsetY, xs);
}

void TextSpan::PaintDecoration(TexgineCanvas &canvas, double offsetX, double offsetY, const TextStyle &xs)
{
    double left = offsetX;
    double right = left + GetWidth();

    if ((xs.decoration & TextDecoration::UNDERLINE) == TextDecoration::UNDERLINE) {
        double y = offsetY + *tmetrics_->fUnderlinePosition_;
        PaintDecorationStyle(canvas, left, right, y, xs);
    }
    if ((xs.decoration & TextDecoration::OVERLINE) == TextDecoration::OVERLINE) {
        double y = offsetY - abs(*tmetrics_->fAscent_);
        PaintDecorationStyle(canvas, left, right, y, xs);
    }
    if ((xs.decoration & TextDecoration::LINE_THROUGH) == TextDecoration::LINE_THROUGH) {
        double y = offsetY - (*tmetrics_->fCapHeight_ * HALF) +
            (xs.fontSize / DEFAULT_FONT_SIZE * xs.decorationThicknessScale * HALF);
        PaintDecorationStyle(canvas, left, right, y, xs);
    }
    if ((xs.decoration & TextDecoration::BASELINE) == TextDecoration::BASELINE) {
        double y = offsetY;
        PaintDecorationStyle(canvas, left, right, y, xs);
    }
}

void TextSpan::PaintDecorationStyle(TexgineCanvas &canvas, double left, double right, double y, const TextStyle &xs)
{
    TexginePaint paint;
    paint.SetAntiAlias(true);
    paint.SetARGB(MAXRGB, MAXRGB, 0, 0);
    paint.SetColor(xs.decorationColor.value_or(xs.color));
    paint.SetStrokeWidth(xs.fontSize / DEFAULT_FONT_SIZE * xs.decorationThicknessScale * SCALE);

    switch (xs.decorationStyle) {
        case TextDecorationStyle::SOLID:
            break;
        case TextDecorationStyle::DOUBLE:
            canvas.DrawLine(left, y, right, y, paint);
            y += OFFSETY;
            break;
        case TextDecorationStyle::DOTTED: {
            TexginePath circle;
            auto rect = TexgineRect::MakeWH(WIDTH_SCALAR, HEIGHT_SCALAR);
            circle.AddOval(rect);
            paint.SetPathEffect(TexginePath1DPathEffect::Make(circle, DOTTED_ADVANCE, PHASE,
                TexginePath1DPathEffect::K_ROTATE_STYLE));
            break;
        }
        case TextDecorationStyle::DASHED: {
            const float intervals[2] = {WIDTH_SCALAR, HEIGHT_SCALAR};
            paint.SetPathEffect(TexgineDashPathEffect::Make(intervals, COUNT, PHASE));
            paint.SetStyle(TexginePaint::STROKE);
            break;
        }
        case TextDecorationStyle::WAVY: {
            TexginePath wavy;
            float thickness = xs.decorationThicknessScale;
            wavy.MoveTo({POINTX0, POINTY2 - thickness});
            wavy.QuadTo({POINTX1, POINTY0 - thickness}, {POINTX2, POINTY2 - thickness});
            wavy.LineTo({POINTX3, POINTY4 - thickness});
            wavy.QuadTo({POINTX4, POINTY6 - thickness}, {POINTX5, POINTY4 - thickness});
            wavy.LineTo({POINTX6, POINTY2 - thickness});
            wavy.LineTo({POINTX6, POINTY2 + thickness});
            wavy.LineTo({POINTX5, POINTY4 + thickness});
            wavy.QuadTo({POINTX4, POINTY6 + thickness}, {POINTX3, POINTY4 + thickness});
            wavy.LineTo({POINTX2, POINTY2 + thickness});
            wavy.QuadTo({POINTX1, POINTY0 + thickness}, {POINTX0, POINTY2 + thickness});
            wavy.LineTo({POINTX0, POINTY2 - thickness});
            paint.SetPathEffect(TexginePath1DPathEffect::Make(wavy, WAVY_ADVANCE, PHASE,
                TexginePath1DPathEffect::K_ROTATE_STYLE));
            paint.SetStyle(TexginePaint::STROKE);
            break;
        }
    }
    canvas.DrawLine(left, y, right, y, paint);
}

void TextSpan::PaintShadow(TexgineCanvas &canvas, double offsetX, double offsetY,
    const std::vector<TextShadow> &shadows)
{
    for (const auto &shadow : shadows) {
        if (!shadow.HasShadow()) {
            continue;
        }
        auto x = offsetX + shadow.offsetX;
        auto y = offsetY + shadow.offsetY;
        TexginePaint paint;
        paint.SetAntiAlias(true);
        paint.SetColor(shadow.color);
        paint.SetMaskFilter(TexgineMaskFilter::MakeBlur(TexgineMaskFilter::K_NORMAL_SK_BLUR_STYLE,
            shadow.blurLeave, false));

        canvas.DrawTextBlob(textBlob_, x, y, paint);
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
