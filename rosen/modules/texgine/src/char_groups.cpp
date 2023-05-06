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
    os << "[" << range.start_ << ", " << range.end_ << ")";
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
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    size_t sum = 0;
    for (size_t i = range_.start_; i < range_.end_; i++) {
        sum += pcgs_->at(i).glyphs_.size();
    }
    return sum;
}

size_t CharGroups::GetNumberOfCharGroup() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    return range_.end_ - range_.start_;
}

const IndexRange &CharGroups::GetRange() const
{
    return range_;
}

CharGroup &CharGroups::GetBack() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    if (GetSize() == 0) {
        throw TEXGINE_EXCEPTION(OutOfRange);
    }

    return *(pcgs_->begin() + range_.end_ - 1);
}

size_t CharGroups::GetSize() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    return pcgs_->size();
}

bool CharGroups::IsValid() const
{
    if (pcgs_ == nullptr) {
        return false;
    }

    if (range_.start_ > range_.end_ || range_.start_ < 0 ||  range_.end_ < 0 ||
        range_.end_ > pcgs_->size() || range_.start_ > pcgs_->size()) {
        throw TEXGINE_EXCEPTION(ErrorStatus);
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
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    try {
        if (!right.IsValid()) {
            throw TEXGINE_EXCEPTION(InvalidArgument);
        }
    } catch(const struct TexgineException &err) {
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    if (pcgs_ != right.pcgs_) {
        return false;
    }

    return range_.start_ < right.range_.end_ && right.range_.start_ < range_.end_;
}

CharGroupsPair CharGroups::GetSplit(const int &index) const
{
    if (index < 0) {
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    return GetSplitAll(index + range_.start_);
}

CharGroupsPair CharGroups::GetSplitAll(const int &index) const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    if (index <= range_.start_ || index >= range_.end_) {
        throw TEXGINE_EXCEPTION(OutOfRange);
    }
    CharGroupsPair retval;
    retval[0] = *this;
    retval[0].range_.end_ = index;
    retval[1] = *this;
    retval[1].range_.start_ = index;
    return retval;
}

CharGroups CharGroups::GetSub(const int &start, const int &end) const
{
    if (!(0 <= start && start <= end)) {
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    return GetSubAll(start + range_.start_, end + range_.start_);
}

CharGroups CharGroups::GetSubAll(const int &start, const int &end) const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    if (!(0 <= start && start <= end && end <= GetSize())) {
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    CharGroups cgs = *this;
    cgs.range_ = {start, end};
    return cgs;
}

CharGroups CharGroups::GetSubFromU16RangeAll(const int &u16start, const int &u16end) const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    if (!(0 <= u16start && u16start <= u16end)) {
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    size_t sum = 0;
    int start = 0;
    int end = 1e9;
    for (int i = -1; i <= static_cast<int>(pcgs_->size()); i++) {
        if (sum <= u16start) {
            start = std::max(start, i);
        }

        if (0 <= i && i < pcgs_->size()) {
            sum += pcgs_->at(i).chars_.size();
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
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    try {
        if (!right.IsValid()) {
            throw TEXGINE_EXCEPTION(InvalidArgument);
        }
    } catch(const TexgineException &err) {
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    if (!IsIntersect(right)) {
        throw CustomException("these two cgs is not intersect");
    }

    CharGroups retval = *this;
    retval.range_.start_ = std::max(range_.start_, right.range_.start_);
    retval.range_.end_ = std::min(range_.end_, right.range_.end_);
    return retval;
}

struct CharGroup &CharGroups::Get(const int32_t &index) const
{
    if (index < 0) {
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    return GetAll(index + range_.start_);
}

struct CharGroup &CharGroups::GetAll(const int32_t &index) const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    if (!(0 <= index && index < GetSize())) {
        throw TEXGINE_EXCEPTION(OutOfRange);
    }

    return pcgs_->at(index);
}

std::vector<uint16_t> CharGroups::ToUTF16() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    std::vector<uint16_t> u16;
    for (auto i = range_.start_; i < range_.end_; i++) {
        u16.insert(u16.end(), pcgs_->at(i).chars_.begin(), pcgs_->at(i).chars_.end());
    }

    return u16;
}

std::vector<uint16_t> CharGroups::ToUTF16All() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    std::vector<uint16_t> u16;
    for (const auto &cg : *pcgs_) {
        u16.insert(u16.end(), cg.chars_.begin(), cg.chars_.end());
    }

    return u16;
}

std::vector<struct CharGroup>::iterator CharGroups::Begin() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    return pcgs_->begin() + range_.start_;
}

std::vector<struct CharGroup>::iterator CharGroups::End() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    return pcgs_->begin() + range_.end_;
}

CharGroups CharGroups::Clone() const
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    auto ret = CharGroups::CreateEmpty();
    *ret.pcgs_ = *pcgs_;
    ret.range_ = range_;
    return ret;
}

void CharGroups::Merge(const CharGroups &right)
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    try {
        if (!right.IsValid()) {
            throw TEXGINE_EXCEPTION(InvalidArgument);
        }
    } catch(const struct TexgineException &err) {
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    if (range_.end_ != right.range_.start_) {
        throw CustomException("the right start not equal this end");
    }

    range_.end_ += right.range_.end_ - right.range_.start_;
}

void CharGroups::PushBack(const struct CharGroup &cg)
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    if (!(0 == range_.start_ && range_.end_ == pcgs_->size())) {
        throw CustomException("incomplete CharGroups cannot revert");
    }

    pcgs_->push_back(cg);
    range_.end_++;
}

void CharGroups::ReverseAll()
{
    if (!IsValid()) {
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    if (!(0 == range_.start_ && range_.end_ == pcgs_->size())) {
        throw CustomException("incomplete CharGroups cannot revert");
    }

    std::reverse(pcgs_->begin(), pcgs_->end());
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
