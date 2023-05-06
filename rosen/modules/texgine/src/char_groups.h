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

#include <array>
#include <cstdint>
#include <memory>
#include <ostream>
#include <vector>

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

class Typeface;
struct CharGroup {
    std::vector<uint16_t> chars;
    std::vector<struct Glyph> glyphs;
    std::shared_ptr<Typeface> typeface;
    double visibleWidth = 0;
    double invisibleWidth = 0;

    double GetWidth() const
    {
        return visibleWidth + invisibleWidth;
    }

    double GetHeight() const
    {
        double maxAdvanceY = 0;
        for (const auto &glyph : glyphs_) {
            maxAdvanceY = std::max(maxAdvanceY, glyph.advanceY);
        }
        return maxAdvanceY;
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

    std::vector<struct CharGroup>::iterator Begin() const;
    std::vector<struct CharGroup>::iterator End() const;

    CharGroups Clone() const;

    void Merge(const CharGroups &right);
    void PushBack(const struct CharGroup &cg);
    void ReverseAll();
    bool operator==(CharGroups const &cgs) const
    {
        return IsSameCharGroups(cgs) && range_ == cgs.range_;
    }

private:
    friend void ReportMemoryUsage(const std::string &member, const CharGroups &that, bool needThis);

    std::shared_ptr<std::vector<struct CharGroup>> pcgs_ = nullptr;
    struct IndexRange range_ = {0, 0};
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_CHAR_GROUPS_H
