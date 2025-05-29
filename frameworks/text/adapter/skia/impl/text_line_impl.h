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

#ifndef ROSEN_MODULES_SPTEXT_TEXTLINE_IMPL_H
#define ROSEN_MODULES_SPTEXT_TEXTLINE_IMPL_H

#include <cstdint>
#include <memory>
#include <utility>

#include "modules/skparagraph/include/TextLineBase.h"
#include "txt/paint_record.h"
#include "txt/text_line_base.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
class TextLineImpl : public TextLineBase {
public:
    TextLineImpl(std::unique_ptr<skia::textlayout::TextLineBase> textLineBase, const std::vector<PaintRecord>& paints);

    virtual ~TextLineImpl() = default;

    size_t GetGlyphCount() const override;

    std::vector<std::unique_ptr<Run>> GetGlyphRuns() const override;

    Range<size_t> GetTextRange() const override;

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

private:
    std::unique_ptr<skia::textlayout::TextLineBase> textLineBase_;
    const std::vector<PaintRecord>& paints_;
};
} // namepsace SPText
} // namepsace Rosen
} // namepsace OHOS

#endif // ROSEN_MODULES_SPTEXT_TEXTLINE_IMPL_H