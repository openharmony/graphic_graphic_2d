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
#include "common/rs_rectangles_merger.h"

namespace OHOS {
namespace Rosen {
std::vector<Occlusion::Rect> RectsMerger::MergeAllRects(const std::vector<Occlusion::Rect>& rectangles,
    int expectedOutputNum, int maxTolerableCost)
{
    return rectangles;
}

std::vector<Occlusion::Rect> RectsMerger::MergeRectsByLevel(int inputNumOfRects, int outputNumOfRects)
{
    return std::vector<Occlusion::Rect>{};
}
    
} // namespace Rosen
} // namespace OHOS