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

#include "text_line_base.h"
#include "convert.h"
#include "run_impl.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTxt {
TextLineBaseImpl::TextLineBaseImpl(std::unique_ptr<SPText::TextLineBase> textlinebase)
    : textlinebase_(std::move(textlinebase))
{
}

size_t TextLineBaseImpl::GetGlyphCount() const
{
    if (!textlinebase_) {
        return 0;
    }
    return textlinebase_->GetGlyphCount();
}

std::vector<std::unique_ptr<Run>> TextLineBaseImpl::GetGlyphRuns() const
{
    if (!textlinebase_) {
        return {};
    }

    std::vector<std::unique_ptr<SPText::Run>> textRuns = textlinebase_->GetGlyphRuns();
    std::vector<std::unique_ptr<Run>> runs;

    for (std::unique_ptr<SPText::Run>& textRun : textRuns) {
        std::unique_ptr<RunImpl> runPtr = std::make_unique<RunImpl>(std::move(textRun));
        runs.emplace_back(std::move(runPtr));
    }
    return runs;
}

Boundary TextLineBaseImpl::GetTextRange() const
{
    if (!textlinebase_) {
        Boundary boundary(0, 0);
        return boundary;
    }

    return Convert(textlinebase_->GetTextRange());
}

void TextLineBaseImpl::Paint(Drawing::Canvas *canvas, double x, double y)
{
    if (!textlinebase_) {
        return;
    }
    return textlinebase_->Paint(canvas, x, y);
}

std::unique_ptr<TextLineBase> TextLineBaseImpl::CreateTruncatedLine(double width, EllipsisModal ellipsisMode,
    const std::string& ellipsisStr) const
{
    if (!textlinebase_) {
        return nullptr;
    }

    std::unique_ptr<SPText::TextLineBase> textLine = textlinebase_->CreateTruncatedLine(width, ellipsisMode,
        ellipsisStr);
    if (textLine == nullptr) {
        return nullptr;
    }

    return std::make_unique<TextLineBaseImpl>(std::move(textLine));
}

double TextLineBaseImpl::GetTypographicBounds(double* ascent, double* descent, double* leading) const
{
    if (!textlinebase_) {
        return 0.0;
    }
    return textlinebase_->GetTypographicBounds(ascent, descent, leading);
}

Drawing::Rect TextLineBaseImpl::GetImageBounds() const
{
    if (!textlinebase_) {
        return {};
    }
    return textlinebase_->GetImageBounds();
}

double TextLineBaseImpl::GetTrailingSpaceWidth() const
{
    if (!textlinebase_) {
        return 0.0;
    }
    return textlinebase_->GetTrailingSpaceWidth();
}

int32_t TextLineBaseImpl::GetStringIndexForPosition(SkPoint point) const
{
    if (!textlinebase_) {
        return 0;
    }
    return textlinebase_->GetStringIndexForPosition(point);
}

double TextLineBaseImpl::GetOffsetForStringIndex(int32_t index) const
{
    if (!textlinebase_) {
        return 0.0;
    }
    return textlinebase_->GetOffsetForStringIndex(index);
}

std::map<int32_t, double> TextLineBaseImpl::GetIndexAndOffsets(bool& isHardBreak) const
{
    if (!textlinebase_) {
        return {};
    }
    return textlinebase_->GetIndexAndOffsets(isHardBreak);
}

double TextLineBaseImpl::GetAlignmentOffset(double alignmentFactor, double alignmentWidth) const
{
    if (!textlinebase_) {
        return 0.0;
    }
    return textlinebase_->GetAlignmentOffset(alignmentFactor, alignmentWidth);
}
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
