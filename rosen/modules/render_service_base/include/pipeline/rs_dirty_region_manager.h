/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DIRTY_REGION_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DIRTY_REGION_MANAGER_H

#include <vector>

#include "common/rs_rect.h"

namespace OHOS {
namespace Rosen {
class RSDirtyRegionManager final {
public:
    RSDirtyRegionManager();
    ~RSDirtyRegionManager() = default;
    void MergeDirtyRect(const RectI& rect);
    void IntersectDirtyRect(const RectI& rect);
    void Clear();
    const RectI& GetDirtyRegion() const;
    bool IsDirty() const;
    void UpdateDirty();

private:
    RectI MergeHistory(int age, RectI rect) const;
    void PushHistory(RectI rect);
    RectI GetHistory(unsigned i) const;

    RectI dirtyRegion_;
    std::vector<RectI> dirtyHistory_;
    int historyHead_ = -1;
    unsigned historySize_ = 0;
    const unsigned HISTORY_QUEUE_MAX_SIZE = 4;

    int surfaceWidth_ = 0;
    int surfaceHeight_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DIRTY_REGION_MANAGER_H