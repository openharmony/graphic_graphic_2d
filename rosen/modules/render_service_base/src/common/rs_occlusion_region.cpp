/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "common/rs_occlusion_region.h"
#include "common/rs_occlusion_region_helper.h"

#include <map>
#include <set>
#include "platform/common/rs_innovation.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace Occlusion {

std::ostream& operator<<(std::ostream& os, const Rect& r)
{
    os << "{" << r.left_ << "," << r.top_ << "," << r.right_ << "," << r.bottom_ << "}";
    return os;
}

bool EventSortByY(const Event& e1, const Event& e2)
{
    if (e1.y_ == e2.y_) {
        return e1.type_ < e2.type_;
    }
    return e1.y_ < e2.y_;
}

Rect::Rect(int l, int t, int r, int b, bool checkValue)
    : left_(l), top_(t), right_(r), bottom_(b)
{
    if (checkValue) {
        CheckAndCorrectValue();
        if (left_ != l || top_ != t || right_ != r || bottom_ != b) {
            RS_LOGE("Occlusion::Rect initialized with invalid value, ltrb[%{public}d, %{public}d, %{public}d, "
                "%{public}d], should in range [%{public}d, %{public}d]",
                l, t, r, b, MIN_REGION_VALUE, MAX_REGION_VALUE);
        }
    }
}

Rect::Rect(const RectI& r, bool checkValue)
    : left_(r.left_), top_(r.top_), right_(r.GetRight()), bottom_(r.GetBottom())
{
    if (checkValue) {
        CheckAndCorrectValue();
        if (left_ != r.left_ || top_ != r.top_ || right_ != r.GetRight() || bottom_ != r.GetBottom()) {
            RS_LOGE("Occlusion::Rect initialized with invalid value, ltrb[%{public}d, %{public}d, %{public}d, "
                "%{public}d], should in range [%{public}d, %{public}d]",
                r.left_, r.top_, r.GetRight(), r.GetBottom(), MIN_REGION_VALUE, MAX_REGION_VALUE);
        }
    }
}

void Node::Update(int updateStart, int updateEnd, Event::Type type)
{
    if (updateStart >= updateEnd) {
        return;
    }
    if (updateStart == start_ && updateEnd == end_) {
        if (type == Event::Type::OPEN || type == Event::Type::CLOSE) {
            positive_count_ += type;
        } else {
            negative_count_ += type;
        }
    } else {
        if (left_ == nullptr) {
            left_ = new Node { start_, mid_ };
        }
        if (right_ == nullptr) {
            right_ = new Node { mid_, end_ };
        }
        // In case the heap memory allocation fails
        if (left_ != nullptr) {
            left_->Update(updateStart, mid_ < updateEnd ? mid_ : updateEnd, type);
        }
        if (right_ != nullptr) {
            right_->Update(mid_ > updateStart ? mid_ : updateStart, updateEnd, type);
        }
    }
}

void Node::GetAndRange(std::vector<Range>& res, bool isParentNodePos = false, bool isParentNodeNeg = false)
{
    bool isPos = isParentNodePos || (positive_count_ > 0);
    bool isNeg = isParentNodeNeg || (negative_count_ > 0);
    if (isPos && isNeg) {
        PushRange(res);
    } else {
        if (left_ != nullptr) {
            left_->GetAndRange(res, isPos, isNeg);
        }
        if (right_ != nullptr) {
            right_->GetAndRange(res, isPos, isNeg);
        }
    }
}

void Node::GetOrRange(std::vector<Range>& res, bool isParentNodePos = false, bool isParentNodeNeg = false)
{
    bool isPos = isParentNodePos || (positive_count_ > 0);
    bool isNeg = isParentNodeNeg || (negative_count_ > 0);
    if (isPos || isNeg) {
        PushRange(res);
    } else {
        if (left_ != nullptr) {
            left_->GetOrRange(res, isPos, isNeg);
        }
        if (right_ != nullptr) {
            right_->GetOrRange(res, isPos, isNeg);
        }
    }
}

void Node::GetXOrRange(std::vector<Range>& res, bool isParentNodePos = false, bool isParentNodeNeg = false)
{
    bool isPos = isParentNodePos || (positive_count_ > 0);
    bool isNeg = isParentNodeNeg || (negative_count_ > 0);
    if (((isPos && !isNeg) || (!isPos && isNeg)) && IsLeaf()) {
        PushRange(res);
    } else if (isPos && isNeg) {
        return;
    } else {
        if (left_ != nullptr) {
            left_->GetXOrRange(res, isPos, isNeg);
        }
        if (right_ != nullptr) {
            right_->GetXOrRange(res, isPos, isNeg);
        }
    }
}

void Node::GetSubRange(std::vector<Range>& res, bool isParentNodePos = false, bool isParentNodeNeg = false)
{
    bool isPos = isParentNodePos || (positive_count_ > 0);
    bool isNeg = isParentNodeNeg || (negative_count_ > 0);
    if (IsLeaf() && isPos && !isNeg) {
        PushRange(res);
    } else if (isNeg) {
        return;
    } else {
        if (left_ != nullptr) {
            left_->GetSubRange(res, isPos, isNeg);
        }
        if (right_ != nullptr) {
            right_->GetSubRange(res, isPos, isNeg);
        }
    }
}

void MakeEnumerate(std::set<int>& ys, std::map<int, int>& indexOf, std::vector<int>& indexAt)
{
    auto it = ys.begin();
    int index = 0;
    while (it != ys.end()) {
        indexOf[*it] = index++;
        indexAt.push_back(*it);
        ++it;
    }
    return;
}

void Region::getRange(std::vector<Range>& ranges, Node& node, Region::OP op)
{
    switch (op) {
        case Region::OP::AND:
            node.GetAndRange(ranges);
            break;
        case Region::OP::OR:
            node.GetOrRange(ranges);
            break;
        case Region::OP::XOR:
            node.GetXOrRange(ranges);
            break;
        case Region::OP::SUB:
            node.GetSubRange(ranges);
            break;
        default:
            break;
    }
    return;
}

void Region::UpdateRects(Rects& r, std::vector<Range>& ranges, std::vector<int>& indexAt, Region& res)
{
    uint32_t i = 0;
    uint32_t j = 0;
    while (i < r.preRects.size() && j < ranges.size()) {
        if (r.preRects[i].left_ == indexAt[ranges[j].start_] && r.preRects[i].right_ == indexAt[ranges[j].end_]) {
            r.curRects.emplace_back(Rect { r.preRects[i].left_, r.preRects[i].top_, r.preRects[i].right_, r.curY });
            i++;
            j++;
        } else if (r.preRects[i].right_ < indexAt[ranges[j].end_]) {
            res.GetRegionRectsRef().push_back(r.preRects[i]);
            i++;
        } else {
            r.curRects.emplace_back(Rect { indexAt[ranges[j].start_], r.preY, indexAt[ranges[j].end_], r.curY });
            j++;
        }
    }
    for (; j < ranges.size(); j++) {
        r.curRects.emplace_back(Rect { indexAt[ranges[j].start_], r.preY, indexAt[ranges[j].end_], r.curY });
    }
    for (; i < r.preRects.size(); i++) {
        res.GetRegionRectsRef().push_back(r.preRects[i]);
    }
    r.preRects.clear();
    r.preRects.swap(r.curRects);
    return;
}

void Region::MakeBound()
{
    if (rects_.size()) {
        bound_ = rects_[0];
        for (const auto& r : rects_) {
            bound_.left_ = std::min(r.left_, bound_.left_);
            bound_.top_ = std::min(r.top_, bound_.top_);
            bound_.right_ = std::max(r.right_, bound_.right_);
            bound_.bottom_ = std::max(r.bottom_, bound_.bottom_);
        }
    }
}

void Region::RegionOp(Region& r1, Region& r2, Region& res, Region::OP op)
{
    if (r1.IsEmpty()) {
        if (op == Region::OP::AND || op == Region::OP::SUB) {
            res = Region();
        } else {
            res = r2;
        }
        return;
    }
    if (r2.IsEmpty()) {
        if (op == Region::OP::AND) {
            res = Region();
        } else {
            res = r1;
        }
        return;
    }
    RegionOpAccelate(r1, r2, res, op);
}

void Region::RegionOpLocal(Region& r1, Region& r2, Region& res, Region::OP op)
{
    r1.MakeBound();
    r2.MakeBound();
    res.GetRegionRectsRef().clear();
    std::vector<Event> events;
    std::set<int> xs;

    for (auto& r : r1.GetRegionRects()) {
        events.emplace_back(Event { r.top_, Event::Type::OPEN, r.left_, r.right_ });
        events.emplace_back(Event { r.bottom_, Event::Type::CLOSE, r.left_, r.right_ });
        xs.insert(r.left_);
        xs.insert(r.right_);
    }
    for (auto& r : r2.GetRegionRects()) {
        events.emplace_back(Event { r.top_, Event::Type::VOID_OPEN, r.left_, r.right_ });
        events.emplace_back(Event { r.bottom_, Event::Type::VOID_CLOSE, r.left_, r.right_ });
        xs.insert(r.left_);
        xs.insert(r.right_);
    }

    if (events.size() == 0) {
        return;
    }

    std::map<int, int> indexOf;
    std::vector<int> indexAt;
    MakeEnumerate(xs, indexOf, indexAt);
    sort(events.begin(), events.end(), EventSortByY);

    Node rootNode { 0, static_cast<int>(indexOf.size() - 1) };

    std::vector<Range> ranges;
    Rects r;
    r.preY = events[0].y_;
    r.curY = events[0].y_;
    for (auto& e : events) {
        r.curY = e.y_;
        ranges.clear();
        getRange(ranges, rootNode, op);
        if (r.curY > r.preY) {
            UpdateRects(r, ranges, indexAt, res);
        }
        rootNode.Update(indexOf[e.left_], indexOf[e.right_], e.type_);
        r.preY = r.curY;
    }
    copy(r.preRects.begin(), r.preRects.end(), back_inserter(res.GetRegionRectsRef()));
    res.MakeBound();
}

void Region::RegionOpAccelate(Region& r1, Region& r2, Region& res, Region::OP op)
{
    RectsPtr lhs(r1.CBegin(), r1.Size());
    RectsPtr rhs(r2.CBegin(), r2.Size());
    Assembler assembler(res);
    OuterLooper outer(lhs, rhs);
    Rect current(0, 0, 0, 0); // init value is irrelevant
    do {
        InnerLooper inner(outer);
        RectType relationship = outer.NextScanline(current.top_, current.bottom_);
        if (op == Region::OP::AND && relationship != RectType::LHS_RHS_BOTH) {
            continue;
        }
        if (op == Region::OP::SUB && relationship == RectType::RHS_ONLY) {
            continue;
        }
        inner.Init(relationship);
        do {
            RectType rectType = inner.NextRect(current.left_, current.right_);
            if (((static_cast<uint32_t>(op) & static_cast<uint32_t>(rectType)) != 0) && !current.IsEmpty()) {
                assembler.Insert(current);
            }
        } while (!inner.IsDone());
    } while (!outer.isDone());
    assembler.FlushVerticalSpan();
    return;
}

Region& Region::OperationSelf(Region& r, Region::OP op)
{
    Region r1(*this);
    RegionOp(r1, r, *this, op);
    return *this;
}

Region& Region::AndSelf(Region& r)
{
    return OperationSelf(r, Region::OP::AND);
}

Region& Region::OrSelf(Region& r)
{
    return OperationSelf(r, Region::OP::OR);
}

Region& Region::XOrSelf(Region& r)
{
    return OperationSelf(r, Region::OP::XOR);
}

Region& Region::SubSelf(Region& r)
{
    return OperationSelf(r, Region::OP::SUB);
}

Region Region::And(Region& r)
{
    Region res;
    RegionOp(*this, r, res, Region::OP::AND);
    return res;
}

Region Region::Or(Region& r)
{
    Region res;
    RegionOp(*this, r, res, Region::OP::OR);
    return res;
}

Region Region::Xor(Region& r)
{
    Region res;
    RegionOp(*this, r, res, Region::OP::XOR);
    return res;
}

Region Region::Sub(Region& r)
{
    Region res;
    RegionOp(*this, r, res, Region::OP::SUB);
    return res;
}

std::ostream& operator<<(std::ostream& os, const Region& r)
{
    os << "{";
    os << r.GetSize() << ": ";
    for (const Rect& rect : r.GetRegionRects()) {
        os << rect;
    }
    os << "}" << std::endl;
    return os;
}

int Region::Area() const
{
    int areaSum = 0;
    for (const auto& rect : GetRegionRects()) {
        areaSum += rect.Area();
    }
    return areaSum;
}

int Region::IntersectArea(const Rect& r) const
{
    if (r.IsEmpty()) {
        return 0;
    }
    int areaSum = 0;
    for (const auto& rect : GetRegionRects()) {
        areaSum += rect.IntersectArea(r);
    }
    return areaSum;
}

} // namespace Occlusion
} // namespace Rosen
} // namespace OHOS
