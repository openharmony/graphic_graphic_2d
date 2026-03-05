/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_MODULES_SPTEXT_CONVERT_H
#define ROSEN_MODULES_SPTEXT_CONVERT_H

#include <vector>

#include "txt/paint_record.h"
#include "txt/paragraph_style.h"
#include "txt/text_style.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/include/TextStyle.h"

namespace OHOS {
namespace Rosen {
namespace SPText {

void GetExtraTextStyleAttributes(const skia::textlayout::TextStyle& skStyle, TextStyle& textStyle);

TextStyle SkStyleToSPTextStyle(const skia::textlayout::TextStyle& skStyle,
    const std::vector<PaintRecord>& paints);

ParagraphStyle SkParagraphStyleToParagraphStyle(const skia::textlayout::ParagraphStyle& skStyle);

} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_CONVERT_H