/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef PIPELINE_RS_VIRTUAL_SCREEN_THREAD_ID_ADAPT_H
#define PIPELINE_RS_VIRTUAL_SCREEN_THREAD_ID_ADAPT_H

namespace OHOS {
namespace Rosen {

class RSVirtualScreenThreadIdAdapt {
public:
    static constexpr std::pair<int32_t, int32_t> VIRTUAL_SCREEN_THREAD_INDEX = {200, 300};
    static inline void AdaptVirtualScreenFfrtThreadId(Drawing::Canvas& rsCanvas, pid_t& threadId)
    {
        if (rsCanvas.GetDrawingType() == Drawing::DrawingType::PAINT_FILTER) {
            auto curCanvas = static_cast<RSPaintFilterCanvas*>(&rsCanvas);
            auto threadIndex = curCanvas->GetParallelThreadIdx();
            if (threadIndex >= VIRTUAL_SCREEN_THREAD_INDEX.first &&
                threadIndex <= VIRTUAL_SCREEN_THREAD_INDEX.second) {
                    threadId = -threadIndex;
                }
        }
    }
};
} // namespace Rosen
} // namespace OHOS

#endif // PIPELINE_RS_VIRTUAL_SCREEN_THREAD_ID_ADAPT_H