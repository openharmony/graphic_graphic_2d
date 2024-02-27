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

#include "skia_font.h"
#include <memory>

#include "include/core/SkFontTypes.h"

#include "skia_adapter/skia_convert_utils.h"
#include "skia_adapter/skia_typeface.h"
#include "skia_typeface.h"
#include "text/font.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaFont::SkiaFont(std::shared_ptr<Typeface> typeface, scalar size, scalar scaleX, scalar skewX) noexcept
{
    if (!typeface) {
        skFont_ = SkFont(nullptr, size, scaleX, skewX);
        LOGD("typeface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return;
    }
    auto skiaTypeface = typeface->GetImpl<SkiaTypeface>();
    if (!skiaTypeface) {
        skFont_ = SkFont(nullptr, size, scaleX, skewX);
        LOGD("skiaTypeface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return;
    }
    skFont_ = SkFont(skiaTypeface->GetTypeface(), size, scaleX, skewX);
}

void SkiaFont::SetEdging(FontEdging edging)
{
    skFont_.setEdging(static_cast<SkFont::Edging>(edging));
}

void SkiaFont::SetSubpixel(bool isSubpixel)
{
    skFont_.setSubpixel(isSubpixel);
}

void SkiaFont::SetHinting(FontHinting hintingLevel)
{
    skFont_.setHinting(static_cast<SkFontHinting>(hintingLevel));
}

void SkiaFont::SetTypeface(std::shared_ptr<Typeface> typeface)
{
    if (!typeface) {
        LOGD("typeface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return;
    }
    auto skiaTypeface = typeface->GetImpl<SkiaTypeface>();
    if (!skiaTypeface) {
        LOGD("skiaTypeface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return;
    }
    sk_sp<SkTypeface> skTypeface = skiaTypeface->GetTypeface();
    skFont_.setTypeface(skTypeface);
}

void SkiaFont::SetSize(scalar textSize)
{
    skFont_.setSize(textSize);
}

void SkiaFont::SetEmbolden(bool isEmbolden)
{
    skFont_.setEmbolden(isEmbolden);
}

void SkiaFont::SetScaleX(scalar scaleX)
{
    skFont_.setScaleX(scaleX);
}

void SkiaFont::SetSkewX(scalar skewX)
{
    skFont_.setSkewX(skewX);
}

void SkiaFont::SetLinearMetrics(bool isLinearMetrics)
{
    skFont_.setLinearMetrics(isLinearMetrics);
}

scalar SkiaFont::GetMetrics(FontMetrics* metrics) const
{
    if (!metrics) {
        return skFont_.getMetrics(nullptr);
    }
    SkFontMetrics skFontMetrics;
    SkiaConvertUtils::DrawingFontMetricsCastToSkFontMetrics(*metrics, skFontMetrics);
    scalar res = skFont_.getMetrics(&skFontMetrics);
    SkiaConvertUtils::SkFontMetricsCastToDrawingFontMetrics(skFontMetrics, *metrics);
    return res;
}

void SkiaFont::GetWidths(const uint16_t glyphs[], int count, scalar widths[]) const
{
    skFont_.getWidths(glyphs, count, widths);
}

void SkiaFont::GetWidths(const uint16_t glyphs[], int count, scalar widths[], Rect bounds[]) const
{
    if (!bounds) {
        skFont_.getWidths(glyphs, count, widths, nullptr);
        return;
    }
    SkRect skBounds[count];
    for (int idx = 0; idx < count; ++idx) {
        SkiaConvertUtils::DrawingRectCastToSkRect(bounds[idx], skBounds[idx]);
    }
    skFont_.getWidths(glyphs, count, widths, skBounds);
    for (int idx = 0; idx < count; ++idx) {
        SkiaConvertUtils::SkRectCastToDrawingRect(skBounds[idx], bounds[idx]);
    }
}

scalar SkiaFont::GetSize() const
{
    return skFont_.getSize();
}

std::shared_ptr<Typeface> SkiaFont::GetTypeface()
{
    sk_sp<SkTypeface> skTypeface = sk_ref_sp(skFont_.getTypeface());
    auto skiaTypeface = std::make_shared<SkiaTypeface>(skTypeface);
    typeface_ = std::make_shared<Typeface>(skiaTypeface);
    return typeface_;
}

FontEdging SkiaFont::GetEdging() const
{
    return static_cast<FontEdging>(skFont_.getEdging());
}

FontHinting SkiaFont::GetHinting() const
{
    return static_cast<FontHinting>(skFont_.getHinting());
}

scalar SkiaFont::GetScaleX() const
{
    return skFont_.getScaleX();
}

scalar SkiaFont::GetSkewX() const
{
    return skFont_.getSkewX();
}

bool SkiaFont::IsSubpixel() const
{
    return skFont_.isSubpixel();
}

uint16_t SkiaFont::UnicharToGlyph(int32_t uni) const
{
    return skFont_.unicharToGlyph(uni);
}

int SkiaFont::TextToGlyphs(const void* text, size_t byteLength, TextEncoding encoding,
    uint16_t glyphs[], int maxGlyphCount) const
{
    SkTextEncoding skEncoding = static_cast<SkTextEncoding>(encoding);
    return skFont_.textToGlyphs(text, byteLength, skEncoding, glyphs, maxGlyphCount);
}

scalar SkiaFont::MeasureText(const void* text, size_t byteLength, TextEncoding encoding, Rect* bounds)
{
    SkTextEncoding skEncoding = static_cast<SkTextEncoding>(encoding);
    if (bounds) {
        SkRect rect;
        scalar width = skFont_.measureText(text, byteLength, skEncoding, &rect);
        bounds->SetLeft(rect.fLeft);
        bounds->SetTop(rect.fTop);
        bounds->SetRight(rect.fRight);
        bounds->SetBottom(rect.fBottom);
        return width;
    } else {
        return skFont_.measureText(text, byteLength, skEncoding);
    }
}

int SkiaFont::CountText(const void* text, size_t byteLength, TextEncoding encoding) const
{
    SkTextEncoding skEncoding = static_cast<SkTextEncoding>(encoding);
    return skFont_.countText(text, byteLength, skEncoding);
}

const SkFont& SkiaFont::GetFont() const
{
    return skFont_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS