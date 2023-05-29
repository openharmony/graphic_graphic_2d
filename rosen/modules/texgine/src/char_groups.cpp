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

#include "char_groups.h"

#include <algorithm>
#include <cassert>

#include "texgine_exception.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
std::ostream &operator<<(std::ostream &os, const struct IndexRange &range)
{
    os << "[" << range.start << ", " << range.end << ")";
    return os;
}

CharGroups CharGroups::CreateEmpty()
{
    CharGroups cgs;
    cgs.pcgs_ = std::make_shared<std::vector<struct CharGroup>>();
    cgs.range_ = {0, 0};
    return cgs;
}

CharGroups CharGroups::CreateWithInvalidRange(IndexRange range)
{
    CharGroups cgs = CreateEmpty();
    cgs.range_ = range;
    return cgs;
}

size_t CharGroups::GetNumberOfGlyph() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    size_t sum = 0;
    for (size_t i = range_.start; i < range_.end; i++) {
        sum += pcgs_->at(i).glyphs.size();
    }
    return sum;
}

size_t CharGroups::GetNumberOfCharGroup() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    return range_.end - range_.start;
}

const IndexRange &CharGroups::GetRange() const
{
    return range_;
}

CharGroup &CharGroups::GetBack() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    if (GetSize() == 0) {
        throw TEXGINE_EXCEPTION(OUT_OF_RANGE);
    }

    return *(pcgs_->begin() + range_.end - 1);
}

size_t CharGroups::GetSize() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    return pcgs_->size();
}

bool CharGroups::IsValid() const
{
    if (pcgs_ == nullptr) {
        return false;
    }

    if (range_.start > range_.end || range_.start < 0 ||  range_.end < 0 ||
        range_.end > pcgs_->size() || range_.start > pcgs_->size()) {
        throw TEXGINE_EXCEPTION(ERROR_STATUS);
    }

    return true;
}

bool CharGroups::IsSameCharGroups(const CharGroups &right) const
{
    return pcgs_ == right.pcgs_;
}

bool CharGroups::IsIntersect(const CharGroups &right) const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    try {
        if (!right.IsValid()) {
            throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
        }
    } catch(const struct TexgineException &err) {
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    if (pcgs_ != right.pcgs_) {
        return false;
    }

    return range_.start < right.range_.end && right.range_.start < range_.end;
}

CharGroupsPair CharGroups::GetSplit(const int &index) const
{
    if (index < 0) {
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    return GetSplitAll(index + range_.start);
}

CharGroupsPair CharGroups::GetSplitAll(const int &index) const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    if (index <= range_.start || index >= range_.end) {
        throw TEXGINE_EXCEPTION(OUT_OF_RANGE);
    }
    CharGroupsPair retval;
    retval[0] = *this;
    retval[0].range_.end = index;
    retval[1] = *this;
    retval[1].range_.start = index;
    return retval;
}

CharGroups CharGroups::GetSub(const int &start, const int &end) const
{
    if (!(0 <= start && start <= end)) {
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    return GetSubAll(start + range_.start, end + range_.start);
}

CharGroups CharGroups::GetSubAll(const int &start, const int &end) const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    if (!(0 <= start && start <= end && end <= GetSize())) {
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    CharGroups cgs = *this;
    cgs.range_ = {start, end};
    return cgs;
}

CharGroups CharGroups::GetSubFromU16RangeAll(const int &u16start, const int &u16end) const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    if (!(0 <= u16start && u16start <= u16end)) {
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    size_t sum = 0;
    int start = 0;
    int end = 1e9;
    for (int i = -1; i <= static_cast<int>(pcgs_->size()); i++) {
        if (sum <= u16start) {
            start = std::max(start, i);
        }

        if (0 <= i && i < pcgs_->size()) {
            sum += pcgs_->at(i).chars.size();
        }

        if (sum >= u16end) {
            end = std::min(end, i);
        }
    }

    return GetSubAll(start, end + 1);
}

CharGroups CharGroups::GetIntersect(const CharGroups &right) const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    try {
        if (!right.IsValid()) {
            throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
        }
    } catch(const TexgineException &err) {
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    if (!IsIntersect(right)) {
        throw CustomException("these two cgs is not intersect");
    }

    CharGroups retval = *this;
    retval.range_.start = std::max(range_.start, right.range_.start);
    retval.range_.end = std::min(range_.end, right.range_.end);
    return retval;
}

struct CharGroup &CharGroups::Get(const int32_t &index) const
{
    if (index < 0) {
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    return GetAll(index + range_.start);
}

struct CharGroup &CharGroups::GetAll(const int32_t &index) const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    if (!(0 <= index && index < GetSize())) {
        throw TEXGINE_EXCEPTION(OUT_OF_RANGE);
    }

    return pcgs_->at(index);
}

std::vector<uint16_t> CharGroups::ToUTF16() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    std::vector<uint16_t> u16;
    for (auto i = range_.start; i < range_.end; i++) {
        u16.insert(u16.end(), pcgs_->at(i).chars.begin(), pcgs_->at(i).chars.end());
    }

    return u16;
}

std::vector<uint16_t> CharGroups::ToUTF16All() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    std::vector<uint16_t> u16;
    for (const auto &cg : *pcgs_) {
        u16.insert(u16.end(), cg.chars.begin(), cg.chars.end());
    }

    return u16;
}

std::vector<struct CharGroup>::iterator CharGroups::begin() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    return pcgs_->begin() + range_.start;
}

std::vector<struct CharGroup>::iterator CharGroups::end() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    return pcgs_->begin() + range_.end;
}

CharGroups CharGroups::Clone() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    auto ret = CharGroups::CreateEmpty();
    *ret.pcgs_ = *pcgs_;
    ret.range_ = range_;
    return ret;
}

void CharGroups::Merge(const CharGroups &right)
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    try {
        if (!right.IsValid()) {
            throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
        }
    } catch(const struct TexgineException &err) {
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    if (range_.end != right.range_.start) {
        throw CustomException("the right start not equal this end");
    }

    range_.end += right.range_.end - right.range_.start;
}

void CharGroups::PushBack(const struct CharGroup &cg)
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    if (!(0 == range_.start && range_.end == pcgs_->size())) {
        throw CustomException("incomplete CharGroups cannot revert");
    }

    pcgs_->push_back(cg);
    range_.end++;
}

void CharGroups::ReverseAll()
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(INVALID_CHAR_GROUPS);
    }

    if (!(0 == range_.start && range_.end == pcgs_->size())) {
        throw CustomException("incomplete CharGroups cannot revert");
    }

    std::reverse(pcgs_->begin(), pcgs_->end());
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
