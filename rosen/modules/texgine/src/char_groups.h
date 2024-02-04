/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_MODULES_TEXGINE_SRC_CHAR_GROUPS_H
#define ROSEN_MODULES_TEXGINE_SRC_CHAR_GROUPS_H

#include "typeface.h"

#include <array>
#include <cstdint>
#include <memory>
#include <ostream>
#include <vector>
#include <unicode/uchar.h>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct Glyph {
    uint32_t codepoint;
    double advanceX;
    double advanceY;
    double offsetX;
    double offsetY;
};

enum SpacesModel : uint8_t {
    NORMAL = 0, // no handle of spaces
    LEFT = 1, // handle the whitespace at the end of the string on the left
    RIGHT = 2, // handle whitespace at the begin of the string on the right
};

struct CharGroup {
    std::vector<uint16_t> chars;
    std::vector<struct Glyph> glyphs;
    std::shared_ptr<Typeface> typeface;
    double visibleWidth = 0;
    double invisibleWidth = 0;
    bool isWordEnd = false;

    double GetWidth() const
    {
        return visibleWidth + invisibleWidth;
    }

    double GetHeight() const
    {
        double maxAdvanceY = 0;
        for (const auto &glyph : glyphs) {
            maxAdvanceY = std::max(maxAdvanceY, glyph.advanceY);
        }
        return maxAdvanceY;
    }

    bool CheckCodePoint() const
    {
        if (!glyphs.size()) {
            return false;
        }
        for (const auto& glyph : glyphs) {
            if (!glyph.codepoint) {
                return false;
            }
        }
        return true;
    };

    bool IsHardBreak()
    {
        ULineBreak lineBreak = static_cast<ULineBreak>(
            u_getIntPropertyValue(chars[0], UCHAR_LINE_BREAK));
        return (lineBreak == U_LB_LINE_FEED || lineBreak == U_LB_MANDATORY_BREAK);
    }

    bool IsEmoji() const
    {
        bool isEmoji = false;
        for (size_t i = 0; i < chars.size(); i++) {
            isEmoji = (u_hasBinaryProperty(chars[i], UCHAR_EMOJI) ||
                u_hasBinaryProperty(chars[i], UCHAR_EMOJI_PRESENTATION) ||
                u_hasBinaryProperty(chars[i], UCHAR_EMOJI_MODIFIER) ||
                u_hasBinaryProperty(chars[i], UCHAR_EMOJI_MODIFIER_BASE));
            if (isEmoji) {
                return isEmoji;
            }
        }
        return isEmoji;
    }

    bool HasWhitesSpace() const
    {
        for (const auto &ch : chars) {
            if (u_isWhitespace(ch)) {
                return true;
            }
        }
        return false;
    }

    bool JudgeOnlyHardBreak() const
    {
        if (chars.empty()) {
            return false;
        }
        bool onlyHardBreak = true;
        for (size_t i = 0; i < chars.size(); i++) {
            ULineBreak lineBreak = static_cast<ULineBreak>(
                u_getIntPropertyValue(chars[i], UCHAR_LINE_BREAK));
            onlyHardBreak = (lineBreak == U_LB_LINE_FEED || lineBreak == U_LB_MANDATORY_BREAK);
            if (!onlyHardBreak) {
                break;
            }
        }
        return onlyHardBreak;
    }

    bool HasHardBreak() const
    {
        if (chars.empty()) {
            return false;
        }
        bool isHardBreak = false;
        for (size_t i = 0; i < chars.size(); i++) {
            ULineBreak lineBreak = static_cast<ULineBreak>(
                u_getIntPropertyValue(chars[i], UCHAR_LINE_BREAK));
            isHardBreak = (lineBreak == U_LB_LINE_FEED || lineBreak == U_LB_MANDATORY_BREAK);
            if (isHardBreak) {
                break;
            }
        }
        return isHardBreak;
    }
};

struct IndexRange {
    int start = 0;
    int end = 0;

    bool operator==(IndexRange const &range) const
    {
        return range.start == start && range.end == end;
    }
};

std::ostream &operator<<(std::ostream &os, const struct IndexRange &range);
class CharGroups;
// 2 is the count of CharGroups in the return value
using CharGroupsPair = std::array<CharGroups, 2>;
class CharGroups {
public:
    static CharGroups CreateEmpty();
    static CharGroups CreateWithInvalidRange(IndexRange range);

    size_t GetNumberOfGlyph() const;
    size_t GetNumberOfCharGroup() const;
    const IndexRange &GetRange() const;
    CharGroup &GetBack() const;
    size_t GetSize() const;

    bool IsValid() const;
    bool IsSameCharGroups(const CharGroups &right) const;
    bool IsIntersect(const CharGroups &right) const;

    CharGroupsPair GetSplit(const int &index) const;
    CharGroupsPair GetSplitAll(const int &index) const;
    CharGroups GetSub(const int &start, const int &end) const;
    CharGroups GetSubAll(const int &start, const int &end) const;
    CharGroups GetSubFromU16RangeAll(const int &u16start, const int &u16end) const;
    CharGroups GetIntersect(const CharGroups &right) const;
    struct CharGroup &Get(const int32_t &index) const;
    struct CharGroup &GetAll(const int32_t &index) const;

    std::vector<uint16_t> ToUTF16() const;
    std::vector<uint16_t> ToUTF16All() const;

    std::vector<struct CharGroup>::iterator begin() const;
    std::vector<struct CharGroup>::iterator end() const;

    CharGroups Clone() const;

    void Merge(const CharGroups &right);
    void PushBack(const struct CharGroup &cg);
    void ReverseAll();
    bool operator==(CharGroups const &cgs) const
    {
        return IsSameCharGroups(cgs) && range_ == cgs.range_;
    }

    bool CheckCodePoint() const;
    std::string GetTypefaceName();
    double GetAllCharWidth() const;
    double GetCharWidth(const size_t index) const;
    std::vector<uint16_t> GetCharsToU16(size_t start, size_t end, const SpacesModel &spacesModel);
    bool IsSingleWord() const;
    bool JudgeOnlyHardBreak() const;
    int FindHardBreakPos() const;
    std::vector<uint16_t> GetSubCharsToU16(const int start, const int end);
private:
    friend void ReportMemoryUsage(const std::string &member, const CharGroups &that, bool needThis);

    std::shared_ptr<std::vector<struct CharGroup>> pcgs_ = nullptr;
    struct IndexRange range_ = {0, 0};
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_CHAR_GROUPS_H
