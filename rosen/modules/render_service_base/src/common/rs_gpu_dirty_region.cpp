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

#include "common/rs_gpu_dirty_region.h"

namespace OHOS {
namespace Rosen {
GpuDirtyRegion& GpuDirtyRegion::GetInstance()
{
    static GpuDirtyRegion instance;
    return instance;
}

GpuDirtyRegion::GpuDirtyRegion()
{
}

GpuDirtyRegion::~GpuDirtyRegion() noexcept
{
}

void GpuDirtyRegion::UpdateActiveDirtyRegionAreasAndFrameNumberForXpower(NodeId id, std::vector<RectI> rects)
{
    ++gpuDirtyRegionInfoMap_[id].activeFramesNumber;
    for (auto rect : rects) {
        gpuDirtyRegionInfoMap_[id].activeGpuDirtyRegionAreas += rect.width_ * rect.height_;
    }
}

void GpuDirtyRegion::UpdateGlobalDirtyRegionAreasAndFrameNumberForXpower(NodeId id, RectI rect)
{
    ++gpuDirtyRegionInfoMap_[id].globalFramesNumber;
    gpuDirtyRegionInfoMap_[id].globalGpuDirtyRegionAreas += rect.width_ * rect.height_;
}

void GpuDirtyRegion::AddSkipProcessFramesNumberForXpower(NodeId id)
{
    ++gpuDirtyRegionInfoMap_[id].skipProcessFramesNumber;
}

void GpuDirtyRegion::SetWindowNameForXpower(NodeId id, std::string& windowName)
{
    gpuDirtyRegionInfoMap_[id].windowName = windowName;
}

GpuDirtyRegionInfo GpuDirtyRegion::GetGpuDirtyRegionInfo(NodeId id)
{
    if (gpuDirtyRegionInfoMap_.find(id) == gpuDirtyRegionInfoMap_.end()) {
        return GpuDirtyRegionInfo {};
    } else {
        gpuDirtyRegionInfoMap_[id].activeGpuDirtyRegionAreas /= gpuDirtyRegionInfoMap_[id].activeFramesNumber;
        gpuDirtyRegionInfoMap_[id].globalGpuDirtyRegionAreas /= gpuDirtyRegionInfoMap_[id].globalFramesNumber;
        return gpuDirtyRegionInfoMap_[id];
    }
}

void GpuDirtyRegion::ResetDirtyRegionInfo()
{
    gpuDirtyRegionInfoMap_.clear();
}
} // namespace Rosen
} // namespace OHOS