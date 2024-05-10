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

#ifndef RS_GPU_DIRTY_REGION_COLLECTION_H
#define RS_GPU_DIRTY_REGION_COLLECTION_H

#include <unordered_map>

#include "common/rs_common_def.h"
#include "common/rs_occlusion_region.h"
#include "surface_type.h"

namespace OHOS {
namespace Rosen {

struct ActiveDirtyRegionInfo {
    int64_t activeDirtyRegionArea = 0;
    int32_t activeFramesNumber = 0;
    int32_t pidOfBelongsApp = 0;
    std::string windowName;
    ActiveDirtyRegionInfo()
        : activeDirtyRegionArea(), activeFramesNumber(), pidOfBelongsApp(), windowName() {}
    ActiveDirtyRegionInfo(int64_t activeDirtyRegionArea_, int32_t activeFramesNumber_, int32_t pidOfBelongsApp_,
        std::string windowName_)
        : activeDirtyRegionArea(activeDirtyRegionArea_), activeFramesNumber(activeFramesNumber_),
          pidOfBelongsApp(pidOfBelongsApp_), windowName(windowName_) {}
};

struct GlobalDirtyRegionInfo {
    int64_t globalDirtyRegionAreas = 0;
    int32_t globalFramesNumber = 0;
    int32_t skipProcessFramesNumber = 0;
    GlobalDirtyRegionInfo()
        : globalDirtyRegionAreas(), globalFramesNumber(), skipProcessFramesNumber() {}
    GlobalDirtyRegionInfo(int64_t globalDirtyRegionAreas_, int32_t globalFramesNumber_,
        int32_t skipProcessFramesNumber_)
        : globalDirtyRegionAreas(globalDirtyRegionAreas_), globalFramesNumber(globalFramesNumber_),
          skipProcessFramesNumber(skipProcessFramesNumber_) {}
};

class RSB_EXPORT GpuDirtyRegionCollection {
public:
    static GpuDirtyRegionCollection& GetInstance();

    void UpdateActiveDirtyInfoForDFX(NodeId id, const std::string& windowName, Occlusion::Region region);
    void UpdateActiveDirtyInfoForDFX(NodeId id, const std::string& windowName, Rect damage);
    void UpdateGlobalDirtyInfoForDFX(RectI rect);
    void AddSkipProcessFramesNumberForDFX();
    std::vector<ActiveDirtyRegionInfo> GetActiveDirtyRegionInfo() const;
    GlobalDirtyRegionInfo GetGlobalDirtyRegionInfo();
    void ResetActiveDirtyRegionInfo();
    void ResetGlobalDirtyRegionInfo();

private:
    GpuDirtyRegionCollection();
    ~GpuDirtyRegionCollection() noexcept;
    GpuDirtyRegionCollection(const GpuDirtyRegionCollection&) = delete;
    GpuDirtyRegionCollection(const GpuDirtyRegionCollection&&) = delete;
    GpuDirtyRegionCollection& operator=(const GpuDirtyRegionCollection&) = delete;
    GpuDirtyRegionCollection& operator=(const GpuDirtyRegionCollection&&) = delete;

    std::unordered_map<NodeId, ActiveDirtyRegionInfo> activeDirtyRegionInfoMap_;
    GlobalDirtyRegionInfo globalDirtyRegionInfo_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_GPU_DIRTY_REGION_COLLECTION_H