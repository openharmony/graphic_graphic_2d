/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ROSEN_TEXT_ADAPTER_TEXGINE_CONVERT_H
#define ROSEN_TEXT_ADAPTER_TEXGINE_CONVERT_H

#include "font_collection.h"
#include "rosen_text/typography_create.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_style.h"
#include "texgine/any_span.h"
#include "texgine/text_style.h"
#include "texgine/typography.h"
#include "texgine/typography_style.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTextEngine {
#define DEFINE_CONVERT_FUNC(from, to) to Convert(const from &)

#define FONT_COLLECTION_RET std::shared_ptr<OHOS::Rosen::AdapterTextEngine::FontCollection>
// from interface to adapter txt
DEFINE_CONVERT_FUNC(std::shared_ptr<OHOS::Rosen::FontCollection>, FONT_COLLECTION_RET);
#undef FONT_COLLECTION_RET

// from texgine to rosen_text
DEFINE_CONVERT_FUNC(TextEngine::IndexAndAffinity, IndexAndAffinity);
DEFINE_CONVERT_FUNC(TextEngine::Boundary,         Boundary);
DEFINE_CONVERT_FUNC(TextEngine::TextRect,         TextRect);
DEFINE_CONVERT_FUNC(TextEngine::Affinity,         Affinity);
DEFINE_CONVERT_FUNC(TextEngine::TextDirection,    TextDirection);

// from rosen_text to texgine
DEFINE_CONVERT_FUNC(TextRectHeightStyle,          TextEngine::TextRectHeightStyle);
DEFINE_CONVERT_FUNC(TextRectWidthStyle,           TextEngine::TextRectWidthStyle);
DEFINE_CONVERT_FUNC(WordBreakType,                TextEngine::WordBreakType);
DEFINE_CONVERT_FUNC(BreakStrategy,                TextEngine::BreakStrategy);
DEFINE_CONVERT_FUNC(TypographyStyle,              TextEngine::TypographyStyle);
DEFINE_CONVERT_FUNC(TextStyle,                    TextEngine::TextStyle);
DEFINE_CONVERT_FUNC(FontWeight,                   TextEngine::FontWeight);
DEFINE_CONVERT_FUNC(FontStyle,                    TextEngine::FontStyle);
DEFINE_CONVERT_FUNC(TextAlign,                    TextEngine::TextAlign);
DEFINE_CONVERT_FUNC(TextBaseline,                 TextEngine::TextBaseline);
DEFINE_CONVERT_FUNC(TextDirection,                TextEngine::TextDirection);
DEFINE_CONVERT_FUNC(TextDecorationStyle,          TextEngine::TextDecorationStyle);
DEFINE_CONVERT_FUNC(TextDecoration,               TextEngine::TextDecoration);
DEFINE_CONVERT_FUNC(PlaceholderVerticalAlignment, TextEngine::AnySpanAlignment);
DEFINE_CONVERT_FUNC(EllipsisModal,                TextEngine::EllipsisModal);
DEFINE_CONVERT_FUNC(RectStyle,                    TextEngine::RectStyle);

#undef DEFINE_CONVERT_FUNC
} // namespace AdapterTextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_ADAPTER_TEXGINE_CONVERT_H
