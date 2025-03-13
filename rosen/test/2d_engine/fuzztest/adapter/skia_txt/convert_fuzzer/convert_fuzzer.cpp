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

#include "convert_fuzzer.h"

#include <cstddef>

#include "convert.h"
#include "get_object.h"
#include "rosen_text/text_style.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void OHDrawingConvert1(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::shared_ptr<OHOS::Rosen::AdapterTxt::FontCollection> adapterFontCollection =
        AdapterTxt::Convert(fontCollection);

    const SPText::PositionWithAffinity zeroAndUpstream = { GetObject<int>() % DATA_MAX_ENUM_SIZE1,
        SPText::Affinity(DATA_MAX_ENUM_SIZE1) };
    AdapterTxt::Convert(zeroAndUpstream);

    SPText::Range<size_t> range(GetObject<int>(), GetObject<int>());
    AdapterTxt::Convert(range);

    SkRect skRect = SkRect::MakeEmpty();
    const SPText::TextBox box(skRect, SPText::TextDirection(GetObject<int>() % DATA_MAX_ENUM_SIZE1));
    AdapterTxt::Convert(box);

    TextRectHeightStyle textRectHeightStyle = TextRectHeightStyle(GetObject<int>() % DATA_MAX_ENUM_SIZE3);
    AdapterTxt::Convert(textRectHeightStyle);

    TextRectWidthStyle textRectWidthStyle = TextRectWidthStyle(GetObject<int>() % DATA_MAX_ENUM_SIZE1);
    AdapterTxt::Convert(textRectWidthStyle);

    PlaceholderSpan placeholderSpan;
    placeholderSpan.alignment = PlaceholderVerticalAlignment(GetObject<int>() % DATA_MAX_ENUM_SIZE3);
    AdapterTxt::Convert(placeholderSpan);
}

void OHDrawingConvert2(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint32_t red = GetObject<uint32_t>();
    uint32_t gree = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t alpha = GetObject<uint32_t>();
    TextStyle textStyle;
    textStyle.symbol.SetVisualMode(VisualMode(GetObject<int>() % DATA_MAX_ENUM_SIZE2));
    textStyle.fontFeatures.SetFeature("tag", GetObject<int>());
    textStyle.fontVariations.SetAxisValue("tag", GetObject<float>());
    textStyle.backgroundBrush = Drawing::Brush(Drawing::Color::ColorQuadSetARGB(alpha, red, gree, blue));
    textStyle.foregroundBrush = Drawing::Brush(Drawing::Color::ColorQuadSetARGB(alpha, red, gree, blue));
    textStyle.backgroundPen = Drawing::Pen(Drawing::Color::ColorQuadSetARGB(alpha, red, gree, blue));
    textStyle.foregroundPen = Drawing::Pen(Drawing::Color::ColorQuadSetARGB(alpha, red, gree, blue));
    std::vector<TextShadow> shadows = { {}, {} };
    SPText::TextStyle sptextStyle = AdapterTxt::Convert(textStyle);

    SPText::TextStyle sptextStyle2;
    sptextStyle.fontFeatures.SetFeature("tag", GetObject<float>());
    sptextStyle.fontVariations.SetAxisValue("tag", GetObject<int>());
    sptextStyle.isSymbolGlyph = GetObject<bool>();
    sptextStyle.background =
        SPText::PaintRecord(Drawing::Brush(Drawing::Color::ColorQuadSetARGB(alpha, red, gree, blue)), Drawing::Pen());
    sptextStyle.foreground =
        SPText::PaintRecord(Drawing::Brush(Drawing::Color::ColorQuadSetARGB(alpha, red, gree, blue)), Drawing::Pen());
    std::vector<TextShadow> TextShadow = { {}, {} };
    TextStyle textStyle2 = AdapterTxt::Convert(sptextStyle);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::OHDrawingConvert1(data, size);
    OHOS::Rosen::Drawing::OHDrawingConvert2(data, size);
    return 0;
}
