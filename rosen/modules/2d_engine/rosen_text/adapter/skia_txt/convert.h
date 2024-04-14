/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ROSEN_TEXT_ADAPTER_TXT_CONVERT_H
#define ROSEN_TEXT_ADAPTER_TXT_CONVERT_H

#include "font_collection.h"
#include "rosen_text/typography_create.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_style.h"
#include "rosen_text/typography_types.h"
#include "txt/paragraph.h"
#include "txt/placeholder_run.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTxt {
#define DEFINE_CONVERT_FUNC(from, to) to Convert(const from &)

#define FONT_COLLECTION_RET std::shared_ptr<OHOS::Rosen::AdapterTxt::FontCollection>
// from interface to adapter txt
DEFINE_CONVERT_FUNC(std::shared_ptr<OHOS::Rosen::FontCollection>, FONT_COLLECTION_RET);
#undef FONT_COLLECTION_RET

// from txt to rosen_text
DEFINE_CONVERT_FUNC(SPText::PositionWithAffinity, IndexAndAffinity);
DEFINE_CONVERT_FUNC(SPText::Range<size_t>, Boundary);
DEFINE_CONVERT_FUNC(SPText::TextBox, TextRect);

// from rosen_text to txt
DEFINE_CONVERT_FUNC(TextRectHeightStyle, SPText::RectHeightStyle);
DEFINE_CONVERT_FUNC(TextRectWidthStyle, SPText::RectWidthStyle);
DEFINE_CONVERT_FUNC(TypographyStyle, SPText::ParagraphStyle);
DEFINE_CONVERT_FUNC(PlaceholderSpan, SPText::PlaceholderRun);
DEFINE_CONVERT_FUNC(TextStyle, SPText::TextStyle);

#undef DEFINE_CONVERT_FUNC
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_ADAPTER_TXT_CONVERT_H
