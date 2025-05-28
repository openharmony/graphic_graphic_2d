/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
#include "include/core/SkPath.h"
#include "include/utils/SkTextUtils.h"

#include "skia_adapter/skia_convert_utils.h"
#include "skia_adapter/skia_typeface.h"
#include "skia_adapter/skia_paint.h"
#include "skia_path.h"
#include "skia_typeface.h"
#include "src/core/SkFontPriv.h"
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
    typeface_ = typeface;
    skFont_ = SkFont(skiaTypeface->GetTypeface(), size, scaleX, skewX);
}

SkiaFont::SkiaFont(const Font& font) noexcept
{
    auto skiaFont = font.GetImpl<SkiaFont>();
    if (!skiaFont) {
        skFont_ = SkFont();
        LOGD("skiaFont nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return;
    }
    typeface_ = skiaFont->typeface_;
    skFont_ = skiaFont->skFont_;
}

void SkiaFont::SetEdging(FontEdging edging)
{
    skFont_.setEdging(static_cast<SkFont::Edging>(edging));
}

void SkiaFont::SetBaselineSnap(bool baselineSnap)
{
    skFont_.setBaselineSnap(baselineSnap);
}

void SkiaFont::SetForceAutoHinting(bool isForceAutoHinting)
{
    skFont_.setForceAutoHinting(isForceAutoHinting);
}

void SkiaFont::SetSubpixel(bool isSubpixel)
{
    skFont_.setSubpixel(isSubpixel);
}

void SkiaFont::SetHinting(FontHinting hintingLevel)
{
    skFont_.setHinting(static_cast<SkFontHinting>(hintingLevel));
}

void SkiaFont::SetEmbeddedBitmaps(bool embeddedBitmaps)
{
    skFont_.setEmbeddedBitmaps(embeddedBitmaps);
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
    typeface_ = typeface;
    skFont_.setTypeface(skiaTypeface->GetTypeface());
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
    if (count <= 0) {
        LOGE("SkiaFont GetWidths error param");
        return;
    }
    SkRect *skBounds = new(std::nothrow) SkRect[count];
    if (!skBounds) {
        LOGE("SkiaFont GetWidths no memory or count is too big %{public}d", count);
        return;
    }
    for (int idx = 0; idx < count; ++idx) {
        SkiaConvertUtils::DrawingRectCastToSkRect(bounds[idx], skBounds[idx]);
    }
    skFont_.getWidths(glyphs, count, widths, skBounds);
    for (int idx = 0; idx < count; ++idx) {
        SkiaConvertUtils::SkRectCastToDrawingRect(skBounds[idx], bounds[idx]);
    }
    delete[] skBounds;
}

scalar SkiaFont::GetSize() const
{
    return skFont_.getSize();
}

std::shared_ptr<Typeface> SkiaFont::GetTypeface() const
{
    return typeface_;
}

FontEdging SkiaFont::GetEdging() const
{
    return static_cast<FontEdging>(skFont_.getEdging());
}

bool SkiaFont::IsEmbeddedBitmaps() const
{
    return skFont_.isEmbeddedBitmaps();
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

bool SkiaFont::IsBaselineSnap() const
{
    return skFont_.isBaselineSnap();
}

bool SkiaFont::IsForceAutoHinting() const
{
    return skFont_.isForceAutoHinting();
}

bool SkiaFont::IsSubpixel() const
{
    return skFont_.isSubpixel();
}

bool SkiaFont::IsLinearMetrics() const
{
    return skFont_.isLinearMetrics();
}

bool SkiaFont::IsEmbolden() const
{
    return skFont_.isEmbolden();
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

scalar SkiaFont::MeasureText(const void* text, size_t byteLength, TextEncoding encoding, Rect* bounds) const
{
    SkTextEncoding skEncoding = static_cast<SkTextEncoding>(encoding);
    return skFont_.measureText(text, byteLength, skEncoding, reinterpret_cast<SkRect*>(bounds));
}

scalar SkiaFont::MeasureText(
    const void* text, size_t byteLength, TextEncoding encoding, Rect* bounds, const Brush* brush, const Pen* pen) const
{
    if (!text || byteLength == 0) {
        LOGE("SkiaFont MeasureText error param");
        return 0.0f;
    }
    SkTextEncoding skEncoding = static_cast<SkTextEncoding>(encoding);
    SkRect* skBounds = reinterpret_cast<SkRect*>(bounds);
    std::unique_ptr<SkPaint> paint = nullptr;
    if (brush != nullptr) {
        paint = std::make_unique<SkPaint>();
        SkiaPaint::BrushToSkPaint(*brush, *paint);
    } else if (pen != nullptr) {
        paint = std::make_unique<SkPaint>();
        SkiaPaint::PenToSkPaint(*pen, *paint);
    }
    return skFont_.measureText(text, byteLength, skEncoding, skBounds, paint.get());
}

void SkiaFont::GetWidthsBounds(const uint16_t glyphs[], int count, float widths[], Rect bounds[],
    const Brush* brush, const Pen* pen) const
{
    if (count <= 0) {
        LOGE("SkiaFont GetWidthsBounds error param");
        return;
    }
    SkRect* skRect = bounds ? reinterpret_cast<SkRect*>(bounds) : nullptr;
    std::unique_ptr<SkPaint> paint = nullptr;
    if (brush != nullptr) {
        paint = std::make_unique<SkPaint>();
        SkiaPaint::BrushToSkPaint(*brush, *paint);
    } else if (pen != nullptr) {
        paint = std::make_unique<SkPaint>();
        SkiaPaint::PenToSkPaint(*pen, *paint);
    }
    skFont_.getWidthsBounds(glyphs, count, widths, skRect, paint.get());
}

void SkiaFont::GetPos(const uint16_t glyphs[], int count, Point points[], Point origin) const
{
    if (count <= 0) {
        LOGE("SkiaFont GetPos error param");
        return;
    }
    SkPoint* skPts = reinterpret_cast<SkPoint*>(points);
    SkPoint skOrigin = SkPoint::Make(origin.GetX(), origin.GetY());
    skFont_.getPos(glyphs, count, skPts, skOrigin);
}

float SkiaFont::GetSpacing() const
{
    return skFont_.getSpacing();
}

int SkiaFont::CountText(const void* text, size_t byteLength, TextEncoding encoding) const
{
    SkTextEncoding skEncoding = static_cast<SkTextEncoding>(encoding);
    return skFont_.countText(text, byteLength, skEncoding);
}

bool SkiaFont::GetPathForGlyph(uint16_t glyph, Path* path) const
{
    if (!path) {
        LOGE("Path param is nullptr, fatal error");
        return false;
    }
    auto skPathImpl = path->GetImpl<SkiaPath>();
    if (skPathImpl != nullptr) {
        SkPath& skpath = skPathImpl->GetMutablePath();
        bool ret = skFont_.getPath(glyph, &skpath);
        if (!ret) {
            LOGE("No path found for glyph:%{public}hu", glyph);
            return false;
        }
    }
    return true;
}

void SkiaFont::GetTextPath(const void* text, size_t byteLength, TextEncoding encoding,
    float x, float y, Path* path) const
{
    if (path == nullptr || text == nullptr) {
        LOGE("Param is invalid, fatal error");
        return;
    }
    auto skPathImpl = path->GetImpl<SkiaPath>();
    if (skPathImpl == nullptr) {
        LOGE("skPathImpl is nullptr");
        return;
    }
    SkPath& skpath = skPathImpl->GetMutablePath();

    size_t strLength = 0;
    switch (encoding) {
        case TextEncoding::UTF8: {
            strLength = std::char_traits<char>::length(static_cast<const char*>(text)) * sizeof(char);
            break;
        }
        case TextEncoding::UTF16: {
            strLength = std::char_traits<char16_t>::length(static_cast<const char16_t*>(text)) * sizeof(char16_t);
            break;
        }
        case TextEncoding::UTF32: {
            strLength = std::char_traits<char32_t>::length(static_cast<const char32_t*>(text)) * sizeof(char32_t);
            break;
        }
        case TextEncoding::GLYPH_ID: {
            strLength = byteLength;
            break;
        }
        default:
            break;
    }
    SkTextUtils::GetPath(text, std::min(strLength, byteLength), static_cast<SkTextEncoding>(encoding), x, y, skFont_,
        &skpath);
}

const SkFont& SkiaFont::GetFont() const
{
    return skFont_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS