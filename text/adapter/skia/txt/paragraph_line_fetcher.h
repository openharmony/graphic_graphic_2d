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

#ifndef ROSEN_MODULES_SPTEXT_PARAGRAPH_LINE_FETCHER_H
#define ROSEN_MODULES_SPTEXT_PARAGRAPH_LINE_FETCHER_H

#include "paragraph.h"
#include "text_line_base.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
class ParagraphLineFetcher {
public:
    virtual ~ParagraphLineFetcher() = default;
    virtual size_t GetLineBreak(size_t startIndex, SkScalar width) const = 0;
    virtual std::unique_ptr<TextLineBase> CreateLine(size_t startIndex, size_t count) = 0;
    virtual std::unique_ptr<Paragraph> GetTempParagraph() = 0;
    virtual size_t GetUnicodeSize() const = 0;
};
} // namespace SPText
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_MODULES_SPTEXT_PARAGRAPH_LINE_FETCHER_H
