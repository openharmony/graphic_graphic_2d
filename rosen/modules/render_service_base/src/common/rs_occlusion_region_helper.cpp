/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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
#include <common/rs_occlusion_region_helper.h>

namespace OHOS {
namespace Rosen {
namespace Occlusion {

void Assembler::Insert(const Rect&r)
{
    if (rectsRow_.size() > 0) {
        if (cur_->top_ != r.top_) {                 // current rect r is not in same row of current span
            FlushVerticalSpan();                    // current span will be cleared
        } else if (cur_->right_ == r.left_) {      // merge rect in horizon (X) direction
            cur_->right_ = r.right_;
            return;
        }
    }
    rectsRow_.push_back(r);
    cur_ = rectsRow_.begin() + static_cast<long>(rectsRow_.size() - 1);
}

void Assembler::FlushVerticalSpan()
{
    if (rectsRow_.size() == 0) {
        return;
    }
    if (CurrentSpanCanMerge()) {
        MergeSpanVertically();
    } else {
        bound_.left_ = std::min(rectsRow_[0].left_, bound_.left_);
        bound_.right_ = std::max(rectsRow_[rectsRow_.size() - 1].right_, bound_.right_);
        storage_.insert(storage_.end(), rectsRow_.begin(), rectsRow_.end());
        end_ = (storage_.begin() + static_cast<long>(storage_.size()));
        lastRectRowBegin_ = end_ - static_cast<long>(rectsRow_.size());
    }
    rectsRow_.clear();
}

bool Assembler::CurrentSpanCanMerge() const
{
    bool merge = false;
    if (end_ - lastRectRowBegin_ == static_cast<long>(rectsRow_.size())) {
        std::vector<Rect>::const_iterator curRowPtr = rectsRow_.cbegin();
        std::vector<Rect>::const_iterator lastRowPtr = lastRectRowBegin_;
        if (curRowPtr->top_ != lastRowPtr->bottom_) {
            return false;
        }
        merge = true;
        while (lastRowPtr != end_) {
            if ((curRowPtr->left_ != lastRowPtr->left_) ||
                (curRowPtr->right_ != lastRowPtr->right_)) {
                merge = false;
                break;
            }
            ++curRowPtr;
            ++lastRowPtr;
        }
    }
    return merge;
}

void Assembler::MergeSpanVertically()
{
    if (rectsRow_.size() == 0) {
        return;
    }
    const int bottom = rectsRow_[0].bottom_;
    std::vector<Rect>::iterator r = lastRectRowBegin_;
    while (r != end_) {
        r->bottom_ = bottom;
        ++r;
    }
}

Assembler::~Assembler()
{
    if (storage_.size()) {
        bound_.top_ = storage_[0].top_;
        bound_.bottom_ = storage_[storage_.size() - 1].bottom_;
    }
}

inline RectType Looper::GetEdgeRelation(int &head, int &tail,
    bool &moreLhs, bool &moreRhs)
{
    RectType relationship;
    moreLhs = false;
    moreRhs = false;
    if (lhsStart_ < rhsStart_) {
        relationship = RectType::LHS_ONLY;
        head = lhsStart_;
        if (lhsEnd_ <= rhsStart_) {
            tail = lhsEnd_;
            moreLhs = true;
        } else {
            lhsStart_ = rhsStart_;
            tail = rhsStart_;
        }
    } else if (rhsStart_ < lhsStart_) {
        relationship = RectType::RHS_ONLY;
        head = rhsStart_;
        if (rhsEnd_ <= lhsStart_) {
            tail = rhsEnd_;
            moreRhs = true;
        } else {
            rhsStart_ = lhsStart_;
            tail = lhsStart_;
        }
    } else {
        relationship = RectType::LHS_RHS_BOTH;
        head = lhsStart_;
        if (lhsEnd_ <= rhsEnd_) {
            tail = rhsStart_ = lhsEnd_;
            moreLhs = true;
        }
        if (rhsEnd_ <= lhsEnd_) {
            tail = lhsStart_ = rhsEnd_;
            moreRhs = true;
        }
    }
    return relationship;
}

RectType OuterLooper::NextScanline(int &curTop, int &curBottom)
{
    bool moreLhs;
    bool moreRhs;
    RectType relationships = Looper::GetEdgeRelation(curTop, curBottom, moreLhs, moreRhs);
    MoveScanline(moreLhs, moreRhs);
    return relationships;
}

inline void OuterLooper::MoveScanline(bool moreLhs, bool moreRhs)
{
    if (moreLhs) {
        MoveScanline(lhs_, lhsStart_, lhsEnd_);
    }
    if (moreRhs) {
        MoveScanline(rhs_, rhsStart_, rhsEnd_);
    }
    return;
}

void OuterLooper::MoveScanline(RectsPtr &r, int &top, int &bottom)
{
    auto rect = r.rects_;
    auto count = r.count_;
    auto rectEnd = rect + static_cast<long>(count);
    int curTop = rect->top_;
    while (rect != rectEnd && rect->top_ == curTop) {
        ++rect;
        --count;
    }
    if (rect != rectEnd) {
        top = rect->top_;
        bottom = rect->bottom_;
    } else {
        top = INT_MAX;
        bottom = INT_MAX;
    }
    r.rects_ = rect;
    r.count_ = count;
    return;
}

void InnerLooper::Init(RectType relationship)
{
    if (relationship == RectType::LHS_ONLY) {
        if (lhs_.count_) {
            lhsStart_ = lhs_.rects_->left_;
            lhsEnd_ = lhs_.rects_->right_;
        }
        rhsStart_ = INT_MAX;
        rhsEnd_ = INT_MAX;
    } else if (relationship == RectType::RHS_ONLY) {
        if (rhs_.count_) {
            rhsStart_ = rhs_.rects_->left_;
            rhsEnd_ = rhs_.rects_->right_;
        }
        lhsStart_ = INT_MAX;
        lhsEnd_ = INT_MAX;
    } else {
        if (lhs_.count_) {
            lhsStart_ = lhs_.rects_->left_;
            lhsEnd_ = lhs_.rects_->right_;
        }
        if (rhs_.count_) {
            rhsStart_ = rhs_.rects_->left_;
            rhsEnd_ = rhs_.rects_->right_;
        }
    }
}

RectType InnerLooper::NextRect(int &curLeft, int &curRight)
{
    bool moreLhs;
    bool moreRhs;
    RectType rectType = GetEdgeRelation(curLeft, curRight, moreLhs, moreRhs);
    MoveRect(moreLhs, moreRhs);
    return rectType;
}

inline void InnerLooper::MoveRect(bool moreLhs, bool moreRhs)
{
    if (moreLhs) {
        MoveRect(lhs_, lhsStart_, lhsEnd_);
    }
    if (moreRhs) {
        MoveRect(rhs_, rhsStart_, rhsEnd_);
    }
    return;
}

void InnerLooper::MoveRect(RectsPtr &r, int &left, int &right)
{
    if (r.count_) {
        int curTop = r.rects_->top_;
        ++r.rects_;
        r.count_--;
        if (r.count_ > 0 && r.rects_->top_ == curTop) {
            left = r.rects_->left_;
            right = r.rects_->right_;
        } else {
            left = INT_MAX;
            right = INT_MAX;
        }
    }
}

} // namespace Occlusion
} // namespace Rosen
} // namespace OHOS