/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_BASE_CORE_COMMON_RS_RECTANGLES_MERGER_H
#define RENDER_SERVICE_BASE_CORE_COMMON_RS_RECTANGLES_MERGER_H

#include <algorithm>
#include <utility>
#include "common/rs_occlusion_region.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RectsMerger {
public:
    RectsMerger() = default;
    ~RectsMerger() = default;
    // set parameters
    RectsMerger(int thresholdMergingAll, int thresholdMergingByLevel)
    {
        thresholdMergingAll_ = std::max(thresholdMergingAll, 1);
        thresholdMergingByLevel_ = std::max(thresholdMergingByLevel, 1);
    }

    std::vector<Occlusion::Rect> MergeAllRects(const std::vector<Occlusion::Rect>& rectangles,
        int expectedOutputNum, int maxTolerableCost);

private:
    Occlusion::Rect MergeSomeRects(const std::vector<Occlusion::Rect>& rectangles,
        int idx1, int idx2);

    // return the rectangle with minimal area, which encloses two rectangles
    Occlusion::Rect MergeTwoRects(const Occlusion::Rect& a, const Occlusion::Rect& b);


    // check if one rectangles is enclosed by another rectangle
    bool IsEnclosedBy(const Occlusion::Rect& a, const Occlusion::Rect& b) const;

    // calculate the explicit rectangles
    // sort by area, and then check
    void CalcExplicitRects(std::vector<Occlusion::Rect> rectangles);

    // all rectangles are merged into one
    std::vector<Occlusion::Rect> NaiveMerging(const std::vector<Occlusion::Rect>& rects);

    // sort the rectangles by height, equally divide them into some groups and then merge
    std::vector<Occlusion::Rect> MergeRectsByLevel(int inputNumOfRects, int outputNumOfRects);
    // two thresholds for different cases
    int thresholdMergingAll_;
    int thresholdMergingByLevel_;
    std::vector<Occlusion::Rect> explicitRects;
};

} // namespace Rosen
} // namespace OHOS
#endif //RENDER_SERVICE_BASE_CORE_COMMON_RS_RECTANGLES_MERGER_H