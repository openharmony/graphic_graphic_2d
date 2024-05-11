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

#include "info_collection/rs_gpu_dirty_region_collection.h"

namespace OHOS {
namespace Rosen {
GpuDirtyRegionCollection& GpuDirtyRegionCollection::GetInstance()
{
    static GpuDirtyRegionCollection instance;
    return instance;
}

GpuDirtyRegionCollection::GpuDirtyRegionCollection()
{
}

GpuDirtyRegionCollection::~GpuDirtyRegionCollection() noexcept
{
}

void GpuDirtyRegionCollection::UpdateActiveDirtyInfoForDFX(NodeId id, const std::string& windowName,
    Occlusion::Region region)
{
    for (auto rect : region.GetRegionRects()) {
        ++activeDirtyRegionInfoMap_[id].activeFramesNumber;
        activeDirtyRegionInfoMap_[id].activeDirtyRegionArea += (rect.right_ - rect.left_) * (rect.bottom_ - rect.top_);
        activeDirtyRegionInfoMap_[id].pidOfBelongsApp = ExtractPid(id);
        activeDirtyRegionInfoMap_[id].windowName = windowName;
    }
}

void GpuDirtyRegionCollection::UpdateActiveDirtyInfoForDFX(NodeId id, const std::string& windowName, Rect damage)
{
    ++activeDirtyRegionInfoMap_[id].activeFramesNumber;
    activeDirtyRegionInfoMap_[id].activeDirtyRegionArea += damage.w * damage.h;
    activeDirtyRegionInfoMap_[id].pidOfBelongsApp = ExtractPid(id);
    activeDirtyRegionInfoMap_[id].windowName = windowName;
}

void GpuDirtyRegionCollection::UpdateGlobalDirtyInfoForDFX(RectI rect)
{
    ++globalDirtyRegionInfo_.globalFramesNumber;
    globalDirtyRegionInfo_.globalDirtyRegionAreas += rect.width_ * rect.height_;
}

void GpuDirtyRegionCollection::AddSkipProcessFramesNumberForDFX()
{
    ++globalDirtyRegionInfo_.skipProcessFramesNumber;
}

std::vector<ActiveDirtyRegionInfo> GpuDirtyRegionCollection::GetActiveDirtyRegionInfo() const
{
    std::vector<ActiveDirtyRegionInfo> activeDirtyRegionInfos;
    for (auto activeDirtyRegionInfo : activeDirtyRegionInfoMap_) {
        if (activeDirtyRegionInfo.second.pidOfBelongsApp && activeDirtyRegionInfo.second.activeFramesNumber > 0) {
            activeDirtyRegionInfo.second.activeDirtyRegionArea /= activeDirtyRegionInfo.second.activeFramesNumber;
            activeDirtyRegionInfos.emplace_back(activeDirtyRegionInfo.second);
        }
    }
    return activeDirtyRegionInfos;
}

GlobalDirtyRegionInfo GpuDirtyRegionCollection::GetGlobalDirtyRegionInfo()
{
    GlobalDirtyRegionInfo globalDirtyRegionInfo;
    if (globalDirtyRegionInfo_.globalFramesNumber > 0) {
        globalDirtyRegionInfo.globalDirtyRegionAreas = globalDirtyRegionInfo_.globalDirtyRegionAreas /
            globalDirtyRegionInfo_.globalFramesNumber;
        globalDirtyRegionInfo.globalFramesNumber = globalDirtyRegionInfo_.globalFramesNumber;
        globalDirtyRegionInfo.skipProcessFramesNumber = globalDirtyRegionInfo_.skipProcessFramesNumber;
    }
    return globalDirtyRegionInfo;
}

void GpuDirtyRegionCollection::ResetActiveDirtyRegionInfo()
{
    activeDirtyRegionInfoMap_.clear();
}

void GpuDirtyRegionCollection::ResetGlobalDirtyRegionInfo()
{
    globalDirtyRegionInfo_ = GlobalDirtyRegionInfo {};
}
} // namespace Rosen
} // namespace OHOS