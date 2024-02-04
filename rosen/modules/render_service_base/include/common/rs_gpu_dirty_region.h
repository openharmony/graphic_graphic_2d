/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_GPU_DIRTY_REGION_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_GPU_DIRTY_REGION_H

#include <unordered_map>

#include "common/rs_common_def.h"
#include "common/rs_rect.h"

namespace OHOS {
namespace Rosen {

struct GpuDirtyRegionInfo {
    int64_t activeGpuDirtyRegionAreas;
    int64_t globalGpuDirtyRegionAreas;
    int32_t skipProcessFramesNumber;
    int32_t activeFramesNumber;
    int32_t globalFramesNumber;
    std::string windowName;
    GpuDirtyRegionInfo()
        : activeGpuDirtyRegionAreas(), globalGpuDirtyRegionAreas(), skipProcessFramesNumber(), activeFramesNumber(),
          globalFramesNumber(), windowName() {}
    GpuDirtyRegionInfo(int64_t activeGpuDirtyRegionAreas_, int64_t globalGpuDirtyRegionAreas_,
        int32_t skipProcessFramesNumber_, int32_t activeFramesNumber_, int32_t globalFramesNumber_,
        std::string windowName_)
        : activeGpuDirtyRegionAreas(activeGpuDirtyRegionAreas_), globalGpuDirtyRegionAreas(globalGpuDirtyRegionAreas_),
          skipProcessFramesNumber(skipProcessFramesNumber_), activeFramesNumber(activeFramesNumber_),
          globalFramesNumber(globalFramesNumber_), windowName(windowName_) {}
};

class RSB_EXPORT GpuDirtyRegion {
public:
    static GpuDirtyRegion& GetInstance();

    void UpdateActiveDirtyRegionAreasAndFrameNumberForXpower(NodeId id, std::vector<RectI> rects);
    void UpdateGlobalDirtyRegionAreasAndFrameNumberForXpower(NodeId id, RectI rect);
    void AddSkipProcessFramesNumberForXpower(NodeId id);
    void SetWindowNameForXpower(NodeId id, std::string& windowName);
    GpuDirtyRegionInfo GetGpuDirtyRegionInfo(NodeId id);
    void ResetDirtyRegionInfo();

private:
    GpuDirtyRegion();
    ~GpuDirtyRegion() noexcept;
    GpuDirtyRegion(const GpuDirtyRegion&) = delete;
    GpuDirtyRegion(const GpuDirtyRegion&&) = delete;
    GpuDirtyRegion& operator=(const GpuDirtyRegion&) = delete;
    GpuDirtyRegion& operator=(const GpuDirtyRegion&&) = delete;

    std::unordered_map<NodeId, GpuDirtyRegionInfo> gpuDirtyRegionInfoMap_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_GPU_DIRTY_REGION_H