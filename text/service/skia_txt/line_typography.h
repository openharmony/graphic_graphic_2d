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

#ifndef LINE_TYPOGRAPHY_H
#define LINE_TYPOGRAPHY_H

#include "rosen_text/line_typography.h"
#include "txt/paragraph_line_fetcher.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTxt {
class LineTypography : public ::OHOS::Rosen::LineTypography {
public:
    explicit LineTypography(std::unique_ptr<SPText::ParagraphLineFetcher> lineFetcher);
    size_t GetLineBreak(size_t startIndex, double width) const override;
    std::unique_ptr<TextLineBase> CreateLine(size_t startIndex, size_t count) override;
    std::unique_ptr<OHOS::Rosen::Typography> GetTempTypography() override;
    void* GetLineFetcher() const override { return reinterpret_cast<void*>(lineFetcher_.get()); };
    size_t GetUnicodeSize() const override { return lineFetcher_->GetUnicodeSize(); };
private:
    std::unique_ptr<SPText::ParagraphLineFetcher> lineFetcher_ = nullptr;
};

} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS

#endif // LINE_TYPOGRAPHY_H
