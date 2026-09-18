
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

#include "drawing_text_line.h"

#include <string>

#include "array_mgr.h"
#include "drawing_rect.h"
#include "rosen_text/run.h"
#include "rosen_text/text_line_base.h"
#include "rosen_text/typography.h"
#include "text_line_entry.h"

#include "utils/text_log.h"

using namespace OHOS::Rosen;

namespace {
TextLineBase* GetTextLineImpl(OH_Drawing_TextLine* line)
{
    if (line == nullptr) {
        return nullptr;
    }
    auto lineEntry = static_cast<TextLineEntry*>(reinterpret_cast<LineObject*>(line)->line);
    return (lineEntry == nullptr) ? nullptr : lineEntry->line.get();
}
} // namespace

OH_Drawing_Array* OH_Drawing_TypographyGetTextLines(OH_Drawing_Typography* typography)
{
    if (typography == nullptr) {
        TEXT_LOGE("Null typography");
        return nullptr;
    }

    auto textLines = reinterpret_cast<Typography*>(typography)->GetTextLines();
    if (textLines.size() == 0) {
        TEXT_LOGE("Failed to get text lines");
        return nullptr;
    }

    LineObject* lineObjectArr = new LineObject[textLines.size()];
    for (size_t i = 0; i < textLines.size(); ++i) {
        lineObjectArr[i].line = new TextLineEntry { std::move(textLines[i]) };
        lineObjectArr[i].isArray = true;
    }

    ObjectArray* array = new ObjectArray();
    array->addr = lineObjectArr;
    array->num = textLines.size();
    array->type = TEXT_LINE;

    return reinterpret_cast<OH_Drawing_Array*>(array);
}

void OH_Drawing_DestroyTextLines(OH_Drawing_Array* lines)
{
    if (lines == nullptr) {
        TEXT_LOGE("Null lines");
        return;
    }

    auto arrayLines = reinterpret_cast<ObjectArray*>(lines);
    if (arrayLines != nullptr && arrayLines->type == TEXT_LINE && arrayLines->num > 0) {
        LineObject* lineObjectArr = reinterpret_cast<LineObject*>(arrayLines->addr);
        if (lineObjectArr != nullptr) {
            for (size_t i = 0; i < arrayLines->num; ++i) {
                delete static_cast<TextLineEntry*>(lineObjectArr[i].line);
                lineObjectArr[i].line = nullptr;
            }
            delete[] lineObjectArr;
            arrayLines->addr = nullptr;
        }
        arrayLines->num = 0;
        arrayLines->type = INVALID;
        delete arrayLines;
    }
}

void OH_Drawing_DestroyTextLine(OH_Drawing_TextLine* line)
{
    if (line == nullptr) {
        TEXT_LOGE("Null line");
        return;
    }

    LineObject* lineObject = reinterpret_cast<LineObject*>(line);
    if (!lineObject->isArray) {
        delete static_cast<TextLineEntry*>(lineObject->line);
        lineObject->line = nullptr;
        delete lineObject;
    }
}

OH_Drawing_TextLine* OH_Drawing_GetTextLineByIndex(OH_Drawing_Array* lines, size_t index)
{
    if (lines == nullptr) {
        TEXT_LOGE("Null lines");
        return nullptr;
    }

    auto arrayLines = reinterpret_cast<ObjectArray*>(lines);
    if (arrayLines != nullptr &&  arrayLines->addr != nullptr &&
        arrayLines->type == TEXT_LINE && index < arrayLines->num) {
        LineObject* lineObjectArr = reinterpret_cast<LineObject*>(arrayLines->addr);
        return reinterpret_cast<OH_Drawing_TextLine*>(&lineObjectArr[index]);
    }

    return nullptr;
}

double OH_Drawing_TextLineGetGlyphCount(OH_Drawing_TextLine* line)
{
    if (line == nullptr) {
        TEXT_LOGE("Null line");
        return 0.0;
    }

    auto lineImpl = GetTextLineImpl(line);
    if (lineImpl == nullptr) {
        TEXT_LOGE("Failed to get line");
        return 0.0;
    }

    return lineImpl->GetGlyphCount();
}

void OH_Drawing_TextLineGetTextRange(OH_Drawing_TextLine* line, size_t* start, size_t* end)
{
    if (line == nullptr || start == nullptr || end == nullptr) {
        TEXT_LOGE("Param is nullptr");
        return;
    }

    auto lineImpl = GetTextLineImpl(line);
    if (lineImpl == nullptr) {
        TEXT_LOGE("Failed to get line");
        return;
    }

    Boundary range = lineImpl->GetTextRange();
    *start = range.leftIndex;
    *end = range.rightIndex;
}

OH_Drawing_Array* OH_Drawing_TextLineGetGlyphRuns(OH_Drawing_TextLine* line)
{
    if (line == nullptr) {
        TEXT_LOGE("Null line");
        return nullptr;
    }

    auto lineImpl = GetTextLineImpl(line);
    if (lineImpl == nullptr) {
        TEXT_LOGE("Failed to get line");
        return nullptr;
    }

    auto runs = lineImpl->GetGlyphRuns();
    if (runs.size() == 0) {
        TEXT_LOGE("Failed to get glyph runs");
        return nullptr;
    }

    Run** runsArr = new Run*[runs.size()];
    for (size_t i = 0; i < runs.size(); ++i) {
        runsArr[i] = runs[i].release();
    }

    ObjectArray* array = new ObjectArray();
    array->addr = runsArr;
    array->num = runs.size();
    array->type = TEXT_RUN;

    return reinterpret_cast<OH_Drawing_Array*>(array);
}

void OH_Drawing_DestroyRuns(OH_Drawing_Array* runs)
{
    if (runs == nullptr) {
        TEXT_LOGE("Null runs");
        return;
    }

    auto arrayRuns = reinterpret_cast<ObjectArray*>(runs);
    if (arrayRuns != nullptr && arrayRuns->type == TEXT_RUN && arrayRuns->num > 0) {
        Run** runsArr = reinterpret_cast<Run**>(arrayRuns->addr);
        if (runsArr != nullptr) {
            for (size_t i = 0; i < arrayRuns->num; ++i) {
                delete runsArr[i];
            }
            delete[] runsArr;
            arrayRuns->addr = nullptr;
        }
        arrayRuns->num = 0;
        arrayRuns->type = INVALID;
        delete arrayRuns;
    }
}

OH_Drawing_Run* OH_Drawing_GetRunByIndex(OH_Drawing_Array* runs, size_t index)
{
    if (runs == nullptr) {
        TEXT_LOGE("Null runs");
        return nullptr;
    }

    auto arrayRuns = reinterpret_cast<ObjectArray*>(runs);
    if (arrayRuns != nullptr && arrayRuns->addr != nullptr && arrayRuns->type == TEXT_RUN && index < arrayRuns->num) {
        Run** run = reinterpret_cast<Run**>(arrayRuns->addr);
        return reinterpret_cast<OH_Drawing_Run*>(run[index]);
    }

    return nullptr;
}

void OH_Drawing_TextLinePaint(OH_Drawing_TextLine* line, OH_Drawing_Canvas* canvas, double x, double y)
{
    if (line == nullptr || canvas == nullptr) {
        TEXT_LOGE("Invalid parameter");
        return;
    }

    auto lineImpl = GetTextLineImpl(line);
    if (lineImpl == nullptr) {
        TEXT_LOGE("Failed to get line");
        return;
    }

    lineImpl->Paint(reinterpret_cast<OHOS::Rosen::Drawing::Canvas*>(canvas), x, y);
}

OH_Drawing_TextLine* OH_Drawing_TextLineCreateTruncatedLine(OH_Drawing_TextLine* line, double width, int mode,
    const char* ellipsis)
{
    if (line == nullptr || ellipsis == nullptr) {
        TEXT_LOGE("Invalid parameter");
        return nullptr;
    }

    auto lineImpl = GetTextLineImpl(line);
    if (lineImpl == nullptr) {
        TEXT_LOGE("Failed to get line");
        return nullptr;
    }

    std::string ellipsisStr(ellipsis);
    auto truncatedTextLine = lineImpl->CreateTruncatedLine(width, static_cast<EllipsisModal>(mode), ellipsisStr);
    if (truncatedTextLine == nullptr) {
        TEXT_LOGE("Failed to create truncated line");
        return nullptr;
    }

    LineObject* lineObject = new LineObject();
    lineObject->line = new TextLineEntry { std::move(truncatedTextLine) };
    lineObject->isArray = false;

    return reinterpret_cast<OH_Drawing_TextLine*>(lineObject);
}

double OH_Drawing_TextLineGetTypographicBounds(OH_Drawing_TextLine* line, double* ascent, double* descent,
    double* leading)
{
    if (line == nullptr || ascent == nullptr || descent == nullptr || leading == nullptr) {
        TEXT_LOGE("Invalid parameter");
        return 0.0;
    }

    auto lineImpl = GetTextLineImpl(line);
    if (lineImpl == nullptr) {
        TEXT_LOGE("Failed to get line");
        return 0.0;
    }

    return lineImpl->GetTypographicBounds(ascent, descent, leading);
}

OH_Drawing_Rect* OH_Drawing_TextLineGetImageBounds(OH_Drawing_TextLine* line)
{
    if (line == nullptr) {
        TEXT_LOGE("Null line");
        return nullptr;
    }

    auto lineImpl = GetTextLineImpl(line);
    if (lineImpl == nullptr) {
        TEXT_LOGE("Failed to get line");
        return nullptr;
    }

    auto skRect = lineImpl->GetImageBounds();
    return OH_Drawing_RectCreate(skRect.GetLeft(), skRect.GetTop(), skRect.GetRight(), skRect.GetBottom());
}

double OH_Drawing_TextLineGetTrailingSpaceWidth(OH_Drawing_TextLine* line)
{
    if (line == nullptr) {
        TEXT_LOGE("Null line");
        return 0.0;
    }

    auto lineImpl = GetTextLineImpl(line);
    if (lineImpl == nullptr) {
        TEXT_LOGE("Failed to get line");
        return 0.0;
    }

    return lineImpl->GetTrailingSpaceWidth();
}

int32_t OH_Drawing_TextLineGetStringIndexForPosition(OH_Drawing_TextLine* line, OH_Drawing_Point* point)
{
    if (line == nullptr || point == nullptr) {
        TEXT_LOGE("Invalid parameter");
        return 0;
    }

    auto lineImpl = GetTextLineImpl(line);
    if (lineImpl == nullptr) {
        TEXT_LOGE("Failed to get line");
        return 0;
    }

    return lineImpl->GetStringIndexForPosition(*reinterpret_cast<SkPoint*>(point));
}

double OH_Drawing_TextLineGetOffsetForStringIndex(OH_Drawing_TextLine* line, int32_t index)
{
    if (line == nullptr) {
        TEXT_LOGE("Null line");
        return 0.0;
    }

    auto lineImpl = GetTextLineImpl(line);
    if (lineImpl == nullptr) {
        TEXT_LOGE("Failed to get line");
        return 0.0;
    }

    return lineImpl->GetOffsetForStringIndex(index);
}

void OH_Drawing_TextLineEnumerateCaretOffsets(OH_Drawing_TextLine* line, Drawing_CaretOffsetsCallback callback)
{
    if (line == nullptr || callback == nullptr) {
        TEXT_LOGE("Invalid parameter");
        return;
    }

    auto lineImpl = GetTextLineImpl(line);
    if (lineImpl == nullptr) {
        TEXT_LOGE("Failed to get line");
        return;
    }

    bool isHardBreak = false;
    std::map<int32_t, double> offsetMap = lineImpl->GetIndexAndOffsets(isHardBreak);
    double leftOffset = 0.0;
    const size_t twoNum = 2;
    for (auto it = offsetMap.begin(); it != offsetMap.end(); ++it) {
        for (size_t i = 0; i < twoNum; i++) {
            double offset = (i == 0) ? leftOffset: it->second;
            bool leadingEdge = (i == 0) ? true : false;
            if (callback(offset, it->first, leadingEdge)) {
                return;
            }
        }
        leftOffset = it->second;
    }
    if (isHardBreak && offsetMap.size() > 0) {
        if (!callback(leftOffset, offsetMap.rbegin()->first + 1, true)) {
            callback(leftOffset, offsetMap.rbegin()->first + 1, false);
        }
    }
}

double OH_Drawing_TextLineGetAlignmentOffset(OH_Drawing_TextLine* line, double alignmentFactor, double alignmentWidth)
{
    if (line == nullptr) {
        TEXT_LOGE("Null line");
        return 0.0;
    }

    auto lineImpl = GetTextLineImpl(line);
    if (lineImpl == nullptr) {
        TEXT_LOGE("Failed to get line");
        return 0.0;
    }

    return lineImpl->GetAlignmentOffset(alignmentFactor, alignmentWidth);
}