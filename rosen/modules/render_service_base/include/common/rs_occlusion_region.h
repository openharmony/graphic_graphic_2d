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
#ifndef RENDER_SERVICE_BASE_CORE_COMMON_RS_OCCLUSION_REGION_H
#define RENDER_SERVICE_BASE_CORE_COMMON_RS_OCCLUSION_REGION_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

#include "rs_rect.h"
#include "common/rs_macros.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace Occlusion {

constexpr int MAX_REGION_VALUE = 1000000;       // normal region value should not exceed 1000000
constexpr int MIN_REGION_VALUE = -1000000;      // normal region value should not less than -1000000
class Rect {
public:
    // assumption: left-top is [0,0]
    int left_ = 0;
    int top_ = 0;
    int right_ = 0;
    int bottom_ = 0;
    static Rect _s_empty_rect_;
    static Rect _s_invalid_rect_;

    Rect() : left_(0), top_(0), right_(0), bottom_(0) {}
    Rect(int l, int t, int r, int b, bool checkValue = true)
    {
        left_ = l;
        top_ = t;
        right_ = r;
        bottom_ = b;
        if (checkValue && (IsEmpty() || CheckIfHasAbnormalValue())) {
            SetEmpty();
        }
    }

    Rect(const RectI& r, bool checkValue = true)
    {
        left_ = r.left_;
        top_ = r.top_;
        right_ = r.GetRight();
        bottom_ = r.GetBottom();
        if (checkValue && (IsEmpty() || CheckIfHasAbnormalValue())) {
            SetEmpty();
        }
    }

    void SetEmpty()
    {
        left_ = 0;
        top_ = 0;
        right_ = 0;
        bottom_ = 0;
    }

    bool CheckIfHasAbnormalValue()
    {
        bool hasAbnormalValue = false;
        if (left_ < MIN_REGION_VALUE || left_ > MAX_REGION_VALUE) {
            hasAbnormalValue = true;
        }
        if (top_ < MIN_REGION_VALUE || top_ > MAX_REGION_VALUE) {
            hasAbnormalValue = true;
        }
        if (right_ < MIN_REGION_VALUE || right_ > MAX_REGION_VALUE) {
            hasAbnormalValue = true;
        }
        if (bottom_ < MIN_REGION_VALUE || bottom_ > MAX_REGION_VALUE) {
            hasAbnormalValue = true;
        }
        return hasAbnormalValue;
    }

    bool IsEmpty() const
    {
        return left_ >= right_ || top_ >= bottom_;
    }

    bool operator==(const Rect& r) const
    {
        return left_ == r.left_ && top_ == r.top_ && right_ == r.right_ && bottom_ == r.bottom_;
    }

    bool operator!=(const Rect& r) const
    {
        return !(*this == r);
    }

    Rect Intersect(const Rect& rect) const
    {
        int left = std::max(left_, rect.left_);
        int top = std::max(top_, rect.top_);
        int right = std::min(right_, rect.right_);
        int bottom = std::min(bottom_, rect.bottom_);
        if ((right - left <= 0) || (bottom - top <= 0)) {
            return Rect(0, 0, 0, 0);
        } else {
            return Rect(left, top, right, bottom);
        }
    }

    bool IsIntersect(const Rect& rect) const
    {
        int left = std::max(left_, rect.left_);
        int top = std::max(top_, rect.top_);
        int right = std::min(right_, rect.right_);
        int bottom = std::min(bottom_, rect.bottom_);
        return (right - left > 0) && (bottom - top > 0);
    }

    std::string GetRectInfo() const
    {
        return std::string("[" +
            std::to_string(left_) + ", " +
            std::to_string(top_) + ", " +
            std::to_string(right_ - left_) + ", " +
            std::to_string(bottom_ - top_) + "]");
    }

    RectI ToRectI() const
    {
        return RectI{left_, top_, right_ - left_, bottom_ - top_};
    }

    int Area() const;
    int IntersectArea(const Rect& r) const;
};

std::ostream& operator<<(std::ostream& os, const Rect& r);

/*
    Event: Used for record a rect edge in/out event
    y_: rect edge Y value
    type: OPEN/CLOSE: lhs rect in/out; VOID_OPEN/VOID_CLOSE: rhs rect in/out
*/
class Event {
public:
    // Use different value to differentiate lhs and rhs ranges
    enum Type { OPEN = 1, CLOSE = -1, VOID_OPEN = 2, VOID_CLOSE = -2 };
    int y_ = 0;
    Type type_ = Type::OPEN;
    int left_ = 0;
    int right_ = 0;

    Event(int y, Type type, int l, int r) : y_(y), type_(type), left_(l), right_(r) {}
};
bool EventSortByY(const Event& e1, const Event& e2);

class Range {
public:
    int start_ = 0;
    int end_ = 0;
    Range(int s, int e) : start_(s), end_(e) {}
    bool operator==(const Range& r)
    {
        return start_ == r.start_ && end_ == r.end_;
    }
};

class Node {
public:
    int start_ = 0;
    int end_ = 0;
    int mid_ = 0;
    int positive_count_ = 0; // used for counting current lhs ranges
    int negative_count_ = 0; // used for counting current rhs ranges
    Node* left_ = nullptr;
    Node* right_ = nullptr;

    Node(int s, int e) : start_(s), end_(e), mid_((s + e) >> 1) {}
    ~Node()
    {
        if (left_ != nullptr) {
            delete left_;
            left_ = nullptr;
        }
        if (right_ != nullptr) {
            delete right_;
            right_ = nullptr;
        }
    }

    // push current node [start, end] into range result, merge last range if possible
    inline void PushRange(std::vector<Range>& res)
    {
        if (res.size() > 0 && start_ == res[res.size() - 1].end_) {
            // merge range with previous range if their end and start share same point
            res[res.size() - 1].end_ = end_;
        } else {
            res.emplace_back(Range { start_, end_ });
        }
    }

    inline bool IsLeaf()
    {
        return left_ == nullptr && right_ == nullptr;
    }

    // update segment tree
    void Update(int updateStart, int updateEnd, Event::Type type);
    // get ranges where positive_count_ and negtive_count_ are both positive
    void GetAndRange(std::vector<Range>& res, bool isParentNodePos, bool isParentNodeNeg);
    // get ranges where positive_count_ or negtive_count_ is positive
    void GetOrRange(std::vector<Range>& res, bool isParentNodePos, bool isParentNodeNeg);
    // get ranges where either positive_count_ and negtive_count_ are both positive
    void GetXOrRange(std::vector<Range>& res, bool isParentNodePos, bool isParentNodeNeg);
    // get ranges where positive_count_ is positive and negtive_count_ not
    void GetSubRange(std::vector<Range>& res, bool isParentNodePos, bool isParentNodeNeg);
};

class RSB_EXPORT Region {
public:
    enum OP {
        // bit index 0: lhs
        // bit index 1: lhs & rhs
        // bit index 2: rhs
        AND = 2, // 010
        OR  = 7, // 111
        XOR = 5, // 101
        SUB = 1  // 001
    };

    Region() = default;
    Region(Rect r)
    {
        rects_.push_back(r);
        bound_ = Rect { r };
    }
    
    Region(const Region& reg) : rects_(reg.rects_), bound_(reg.bound_) {}
    ~Region() {}

    void Reset()
    {
        rects_.clear();
        bound_ = Rect {};
    }

    std::vector<Rect>& GetRegionRectsRef()
    {
        return rects_;
    }

    const std::vector<Rect>& GetRegionRects() const
    {
        return rects_;
    }
    int GetSize() const
    {
        return rects_.size();
    }
    Rect GetBound() const
    {
        return bound_;
    }
    Rect& GetBoundRef()
    {
        return bound_;
    }
    bool IsEmpty() const
    {
        return rects_.size() == 0 || bound_.IsEmpty();
    }
    std::string GetRegionInfo() const
    {
        std::string info;
        if (IsEmpty()) {
            info = "Region [Empty]";
        } else {
            info = "Region " + std::to_string(rects_.size()) + ": ";
            for (auto& r : rects_) {
                info.append(r.GetRectInfo());
            }
        }
        return info;
    }

    inline std::vector<Rect>::const_iterator CBegin() const
    {
        return rects_.cbegin();
    }
    inline std::vector<Rect>::const_iterator CEnd() const
    {
        return rects_.cend();
    }
    inline std::vector<Rect>::iterator Begin()
    {
        return rects_.begin();
    }
    inline std::vector<Rect>::const_iterator End()
    {
        return rects_.end();
    }
    inline size_t Size() const
    {
        return rects_.size();
    }

    // bound of all region rects
    void MakeBound();

    bool IsIntersectWith(const Rect& r) const
    {
        for (const Rect& rect : rects_) {
            if (rect.IsIntersect(r)) {
                return true;
            }
        }
        return false;
    }

    /* core Region logic operation function, the return region's rects is guaranteed no-intersection
        (rect in rects_ do not intersect with each other)
    */
    void RegionOp(Region& r1, Region& r2, Region& res, Region::OP op);
    void RegionOpLocal(Region& r1, Region& r2, Region& res, Region::OP op);
    void RegionOpAccelate(Region& r1, Region& r2, Region& res, Region::OP op);

    Region& OperationSelf(Region& r, Region::OP op);
    // replace region with and result
    Region& AndSelf(Region& r);
    // replace region with or result
    Region& OrSelf(Region& r);
    // replace region with xor result
    Region& XOrSelf(Region& r);
    // replace region with sub result
    Region& SubSelf(Region& r);

    // return intersection region
    Region And(Region& r);
    // return merge region
    Region Or(Region& r);
    // return merge region subtract intersection region
    Region Xor(Region& r);
    // return region belongs to Region(lhs) but not Region(rhs)
    Region Sub(Region& r);

    // get current region's area, return the sum of the areas of all rectangles (as they are not intersect each other)
    int Area() const;
    // return the area of the region where the current region intersects the rectangle
    int IntersectArea(const Rect& r) const;

private:
    class Rects {
    public:
        std::vector<Rect> preRects;
        std::vector<Rect> curRects;
        int preY = 0;
        int curY = 0;
    };
    // get ranges from segmentTree node according to logical operation type
    void getRange(std::vector<Range>& ranges, Node& node, OP op);
    // update tmp rects and region according to current ranges
    void UpdateRects(Rects& r, std::vector<Range>& ranges, std::vector<int>& indexAt, Region& res);
    
private:
    std::vector<Rect> rects_;
    Rect bound_;
    static bool _s_so_loaded_;
};
std::ostream& operator<<(std::ostream& os, const Region& r);
} // namespace Occlusion
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_CORE_COMMON_RS_OCCLUSION_REGION_H