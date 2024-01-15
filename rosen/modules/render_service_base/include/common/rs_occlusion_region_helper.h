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
#ifndef RENDER_SERVICE_BASE_CORE_COMMON_RS_OCCLUSION_REGION_HELPER_H
#define RENDER_SERVICE_BASE_CORE_COMMON_RS_OCCLUSION_REGION_HELPER_H

#include <limits.h>
#include "common/rs_occlusion_region.h"
namespace OHOS {
namespace Rosen {
namespace Occlusion {

enum RectType {
    LHS_ONLY = 1,       // 001
    LHS_RHS_BOTH = 2,   // 010
    RHS_ONLY = 4        // 100
};

class RectsPtr {
public:
    std::vector<Rect>::const_iterator rects_;
    size_t count_ = 0;
    inline RectsPtr(const RectsPtr &rhs) : rects_(rhs.rects_), count_(rhs.count_) {}
    inline RectsPtr(const std::vector<Rect>::const_iterator &r, size_t c) : rects_(r), count_(c) {}
    ~RectsPtr() {}
};
class Assembler {
public:
    explicit Assembler(Region &r)
        : storage_(r.GetRegionRectsRef()), bound_(r.GetBoundRef()), lastRectRowBegin_(), end_(), cur_()
    {
        storage_.clear();
        bound_ = Rect{INT_MAX, INT_MAX, INT_MIN, INT_MIN, false};
    }
    void Insert(const Rect &r); // insert a Rect into span
    void FlushVerticalSpan();
    bool CurrentSpanCanMerge() const;
    void MergeSpanVertically();
    ~Assembler();
private:
    std::vector<Rect> &storage_;            // current all rects storage
    Rect &bound_;                           // current bound
    std::vector<Rect> rectsRow_;            // current span rects; will be dumped into storage every otter loop
    std::vector<Rect>::iterator lastRectRowBegin_;
    std::vector<Rect>::iterator end_;
    std::vector<Rect>::iterator cur_;
};
class Looper {
public:
    int lhsStart_ = INT_MAX;
    int lhsEnd_ = INT_MAX;
    int rhsStart_ = INT_MAX;
    int rhsEnd_ = INT_MAX;

    Looper() : lhsStart_(INT_MAX), lhsEnd_(INT_MAX), rhsStart_(INT_MAX), rhsEnd_(INT_MAX) {}
    virtual ~Looper() {}
    inline RectType GetEdgeRelation(int &head, int &tail, bool &more_lhs, bool &more_rhs);
};
class OuterLooper : protected Looper {
public:
    RectsPtr lhs_;
    RectsPtr rhs_;
    inline OuterLooper(const RectsPtr &lhs, const RectsPtr &rhs) : lhs_(lhs), rhs_(rhs)
    {
        if (lhs_.count_) {
            Looper::lhsStart_ = lhs_.rects_->top_;
            Looper::lhsEnd_ = lhs_.rects_->bottom_;
        }
        if (rhs_.count_) {
            Looper::rhsStart_ = rhs_.rects_->top_;
            Looper::rhsEnd_ = rhs_.rects_->bottom_;
        }
    }
    ~OuterLooper() {}
    RectType NextScanline(int &curTop, int &curBottom);
    inline void MoveScanline(bool moreLhs, bool moreRhs);
    void MoveScanline(RectsPtr &r, int &top, int &bottom);
    inline bool isDone() const
    {
        return lhs_.count_ == 0 && rhs_.count_ == 0;
    }
};
class InnerLooper : protected Looper {
public:
    RectsPtr lhs_;
    RectsPtr rhs_;
    inline InnerLooper(const RectsPtr &lhs, const RectsPtr &rhs)
        : lhs_(lhs), rhs_(rhs) {}
    InnerLooper(const OuterLooper &outer) : lhs_(outer.lhs_), rhs_(outer.rhs_) {}
    ~InnerLooper() {}
    void Init(RectType relationship);
    RectType NextRect(int &curLeft, int &curRight);
    inline void MoveRect(bool moreLhs, bool moreRhs);
    void MoveRect(RectsPtr &r, int &left, int &right);
    inline bool IsDone() const
    {
        return lhsStart_ == INT_MAX && rhsStart_ == INT_MAX;
    }
};
}
}
}
#endif //RENDER_SERVICE_BASE_CORE_COMMON_RS_OCCLUSION_REGION_HELPER_H