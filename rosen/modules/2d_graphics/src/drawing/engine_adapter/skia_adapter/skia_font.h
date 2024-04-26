/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef SKIA_FONT_H
#define SKIA_FONT_H

#include <memory>

#include "include/core/SkFont.h"

#include "impl_interface/font_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaFont : public FontImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaFont() noexcept = default;
    SkiaFont(std::shared_ptr<Typeface> typeface, scalar size, scalar scaleX, scalar skewX) noexcept;
    ~SkiaFont() override = default;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void SetEdging(FontEdging edging) override;
    void SetBaselineSnap(bool baselineSnap) override;
    void SetForceAutoHinting(bool isForceAutoHinting) override;
    void SetSubpixel(bool isSubpixel) override;
    void SetHinting(FontHinting hintingLevel) override;
    void SetEmbeddedBitmaps(bool embeddedBitmaps) override;
    void SetTypeface(std::shared_ptr<Typeface> typeface) override;
    void SetSize(scalar textSize) override;
    void SetEmbolden(bool isEmbolden) override;
    void SetScaleX(scalar scaleX) override;
    void SetSkewX(scalar skewX) override;
    void SetLinearMetrics(bool isLinearMetrics) override;

    scalar GetMetrics(FontMetrics* metrics) const override;
    void GetWidths(const uint16_t glyphs[], int count, scalar widths[]) const override;
    void GetWidths(const uint16_t glyphs[], int count, scalar widths[], Rect bounds[]) const override;
    scalar GetSize() const override;
    std::shared_ptr<Typeface> GetTypeface() override;

    FontEdging GetEdging() const override;
    FontHinting GetHinting() const override;
    bool IsEmbeddedBitmaps() const override;
    scalar GetScaleX() const override;
    scalar GetSkewX() const override;
    bool IsBaselineSnap() const override;
    bool IsForceAutoHinting() const override;
    bool IsSubpixel() const override;
    bool IsLinearMetrics() const override;
    bool IsEmbolden() const override;

    uint16_t UnicharToGlyph(int32_t uni) const override;
    int TextToGlyphs(const void* text, size_t byteLength, TextEncoding encoding,
        uint16_t glyphs[], int maxGlyphCount) const override;

    scalar MeasureText(const void* text, size_t byteLength, TextEncoding encoding, Rect* bounds) override;
    int CountText(const void* text, size_t byteLength, TextEncoding encoding) const override;

    const SkFont& GetFont() const;

private:
    SkFont skFont_;
    std::shared_ptr<Typeface> typeface_ = nullptr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif