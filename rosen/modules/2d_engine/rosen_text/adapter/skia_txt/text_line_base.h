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

#ifndef ROSEN_TEXT_ADAPTER_TXT_TEXT_LINE_BASE_H
#define ROSEN_TEXT_ADAPTER_TXT_TEXT_LINE_BASE_H

#include <memory>
#include <utility>

#include "draw/canvas.h" // Drawing
#include "rosen_text/text_line_base.h"
#include "rosen_text/typography.h"
#include "txt/text_line_base.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTxt {
class TextLineBaseImpl : public ::OHOS::Rosen::TextLineBase {
public:
    explicit TextLineBaseImpl(std::unique_ptr<SPText::TextLineBase> textlinebase);
    size_t GetGlyphCount() const override;
    std::vector<std::unique_ptr<Run>> GetGlyphRuns() const override;
    Boundary GetTextRange() const override;
    void Paint(Drawing::Canvas *canvas, double x, double y) override;

    std::unique_ptr<TextLineBase> CreateTruncatedLine(double width, EllipsisModal ellipsisMode,
        const std::string& ellipsisStr) const override;
    double GetTypographicBounds(double* ascent, double* descent, double* leading) const override;
    Drawing::Rect GetImageBounds() const override;
    double GetTrailingSpaceWidth() const override;
    int32_t GetStringIndexForPosition(SkPoint point) const override;
    double GetOffsetForStringIndex(int32_t index) const override;
    std::map<int32_t, double> GetIndexAndOffsets(bool& isHardBreak) const override;
    double GetAlignmentOffset(double alignmentFactor, double alignmentWidth) const override;
    void* GetSpTextLineBase() { return reinterpret_cast<void*>(textlinebase_.get()); }

private:
    std::unique_ptr<SPText::TextLineBase> textlinebase_ = nullptr;
};
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_ADAPTER_TXT_TEXT_LINE_BASE_H