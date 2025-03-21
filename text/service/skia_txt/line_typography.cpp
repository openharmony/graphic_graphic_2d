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

#include "line_typography.h"
#include "text_line_base.h"
#include "typography.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTxt {
LineTypography::LineTypography(std::unique_ptr<SPText::ParagraphLineFetcher> lineFetcher)
    : lineFetcher_(std::move(lineFetcher))
{
}

size_t LineTypography::GetLineBreak(size_t startIndex, double width) const
{
    return lineFetcher_->GetLineBreak(startIndex, width);
}

std::unique_ptr<TextLineBase> LineTypography::CreateLine(size_t startIndex, size_t count)
{
    return std::make_unique<TextLineBaseImpl>(lineFetcher_->CreateLine(startIndex, count));
}

std::unique_ptr<OHOS::Rosen::Typography> LineTypography::GetTempTypography()
{
    return std::make_unique<AdapterTxt::Typography>(lineFetcher_->GetTempParagraph());
}
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
