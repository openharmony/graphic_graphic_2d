/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "typography_fuzzer.h"

#include "get_object.h"
#include "texgine/typography.h"
#include "texgine/typography_builder.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
void TypographySetIndentsFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::unique_ptr<TypographyBuilder> typographyBuilder = TypographyBuilder::Create();
    if (typographyBuilder == nullptr) {
        return;
    }
    std::shared_ptr<Typography> typography = typographyBuilder->Build();
    float indent = GetObject<double>();
    std::vector<float> indents;
    indents.push_back(indent);
    typography->SetIndents(indents);
    return;
}

void TypographyLayoutFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::unique_ptr<TypographyBuilder> typographyBuilder = TypographyBuilder::Create();
    if (typographyBuilder == nullptr) {
        return;
    }
    std::string text = GetStringFromData();
    typographyBuilder->AppendSpan(text);
    std::shared_ptr<Typography> typography = typographyBuilder->Build();
    double widthLimit = GetObject<double>();
    typography->Layout(widthLimit);
    return;
}

void TypographyPaintFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::unique_ptr<TypographyBuilder> typographyBuilder = TypographyBuilder::Create();
    if (typographyBuilder == nullptr) {
        return;
    }
    std::shared_ptr<Typography> typography = typographyBuilder->Build();
    std::shared_ptr<TexgineCanvas> canvas = std::make_shared<TexgineCanvas>();
    double offsetx = GetObject<double>();
    double offsety = GetObject<double>();
    typography->Paint(*canvas, offsetx, offsety);
    return;
}

void TypographyGetTextRectsByBoundaryFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::unique_ptr<TypographyBuilder> typographyBuilder = TypographyBuilder::Create();
    if (typographyBuilder == nullptr) {
        return;
    }
    std::shared_ptr<Typography> typography = typographyBuilder->Build();
    size_t left = GetObject<size_t>();
    size_t right = GetObject<size_t>();
    Boundary boundary(left, right);
    TextRectHeightStyle heightStyle = GetObject<TextRectHeightStyle>();
    TextRectWidthStyle widthStyle = GetObject<TextRectWidthStyle>();
    typography->GetTextRectsByBoundary(boundary, heightStyle, widthStyle);
    return;
}

void TypographyGetGlyphIndexByCoordinateFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::unique_ptr<TypographyBuilder> typographyBuilder = TypographyBuilder::Create();
    if (typographyBuilder == nullptr) {
        return;
    }
    std::shared_ptr<Typography> typography = typographyBuilder->Build();
    double x = GetObject<double>();
    double y = GetObject<double>();
    typography->GetGlyphIndexByCoordinate(x, y);
    return;
}

void TypographyGetWordBoundaryByIndexFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::unique_ptr<TypographyBuilder> typographyBuilder = TypographyBuilder::Create();
    if (typographyBuilder == nullptr) {
        return;
    }
    std::shared_ptr<Typography> typography = typographyBuilder->Build();
    size_t index = GetObject<size_t>();
    typography->GetWordBoundaryByIndex(index);
    return;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::TextEngine::TypographySetIndentsFuzzTest(data, size);
    OHOS::Rosen::TextEngine::TypographyLayoutFuzzTest(data, size);
    OHOS::Rosen::TextEngine::TypographyPaintFuzzTest(data, size);
    OHOS::Rosen::TextEngine::TypographyGetTextRectsByBoundaryFuzzTest(data, size);
    OHOS::Rosen::TextEngine::TypographyGetGlyphIndexByCoordinateFuzzTest(data, size);
    OHOS::Rosen::TextEngine::TypographyGetWordBoundaryByIndexFuzzTest(data, size);
    return 0;
}