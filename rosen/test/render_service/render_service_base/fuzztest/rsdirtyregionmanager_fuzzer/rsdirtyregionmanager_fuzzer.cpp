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

#include "rsdirtyregionmanager_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>

#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "common/rs_common_def.h"
#include "pipeline/rs_dirty_region_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos = 0;

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size) {
        return object;
    }
    auto remainSize = g_size - g_pos;
    if (objectSize <= remainSize) {
        if (memcpy_s(&object, objectSize, g_data + g_pos, objectSize) != EOK) {
            return {};
        }
    } else {
        if (memcpy_s(&object, remainSize, g_data + g_pos, remainSize) != EOK) {
            return {};
        }
        if (memcpy_s(reinterpret_cast<uint8_t*>(&object) + remainSize, objectSize - remainSize,
            g_data, objectSize - remainSize) != EOK) {
            return {};
        }
    }
    g_pos += objectSize;
    g_pos %= g_size;
    return object;
}

inline RectI GetRectI()
{
    int left = GetData<int>();
    int top = GetData<int>();
    int width = GetData<int>();
    int height = GetData<int>();
    return RectI(left, top, width, height);
}

void AddTestFunction(std::vector<std::function<void(RSDirtyRegionManager&)>>& testFunctions,
    std::function<void(RSDirtyRegionManager&)> testFunction)
{
    constexpr uint8_t maxRunTimes{10};
    auto runTimes = (GetData<uint8_t>() % maxRunTimes);
    for (uint8_t i = 0; i <= runTimes; ++i) {
        testFunctions.emplace_back(testFunction);
    }
}
} // namespace

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // prepare test functions
    std::vector<std::function<void(RSDirtyRegionManager&)>> testFunctions;
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.MergeDirtyRect(GetRectI());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.MergeDirtyRectIfIntersect(GetRectI());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.MergeDirtyRectAfterMergeHistory(GetRectI());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.IntersectDirtyRect(GetRectI());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.ClipDirtyRectWithinSurface();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.Clear();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        std::vector<RectI> rects;
        auto size = GetData<uint8_t>();
        for (uint8_t i = 0; i < size; ++i) {
            rects.push_back(GetRectI());
        }
        rsDirtyRegionManager.UpdateVisitedDirtyRects(rects);
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetIntersectedVisitedDirtyRect(GetRectI());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.UpdateCacheableFilterRect(GetRectI());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.IfCacheableFilterRectFullyCover(GetRectI());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.IsCacheableFilterRectEmpty();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.InvalidateFilterCacheRect();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.IsFilterCacheRectValid();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetCurrentFrameDirtyRegion();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetDirtyRegion();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.SetCurrentFrameDirtyRect(GetRectI());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetDirtyRegionFlipWithinSurface();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetLatestDirtyRegion();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetRectFlipWithinSurface(GetRectI());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.IsDirty();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.UpdateDirty();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.UpdateDirtyByAligned(GetData<int32_t>());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.SetBufferAge(GetData<int>());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.SetSurfaceSize(GetData<int32_t>(), GetData<int32_t>());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetSurfaceRect();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.MergeSurfaceRect();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.ResetDirtyAsSurfaceSize();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.UpdateDebugRegionTypeEnable(GetData<DirtyRegionDebugType>());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.IsDebugRegionTypeEnable(GetData<DebugRegionType>());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        std::shared_ptr<RSDirtyRegionManager> targetManager;
        if (GetData<bool>()) {
            targetManager = std::make_shared<RSDirtyRegionManager>();
        }
        rsDirtyRegionManager.OnSync(targetManager);
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.UpdateDirtyRegionInfoForDfx(GetData<uint64_t>());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.MarkAsTargetForDfx();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.IsTargetForDfx();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.HasOffset();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.SetOffset(GetData<int>(), GetData<int>());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        std::vector<RectI> rects;
        auto size = GetData<uint8_t>();
        for (uint8_t i = 0; i < size; ++i) {
            rects.push_back(GetRectI());
        }
        rsDirtyRegionManager.SetDirtyRegionForQuickReject(rects);
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetDirtyRegionForQuickReject();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetAdvancedDirtyRegion();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetCurrentFrameAdvancedDirtyRegion();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.MergeHwcDirtyRect(GetRectI());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        RSDirtyRegionManager::GetPixelAlignedRect(GetRectI());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.AlignHistory();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetHistory(GetData<int>());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.PushHistory(GetRectI());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.MergeHistory(GetData<int>(), GetRectI());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetMergedDirtyRegions();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetOffsetedDirtyRegion();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetUifirstFrameDirtyRegion();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.SetUifirstFrameDirtyRect(GetRectI());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.SetMaxNumOfDirtyRects(GetData<int>());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.SetAdvancedDirtyRegionType(GetData<AdvancedDirtyRegionType>());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetHwcDirtyRegion();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetDirtyRegionInVirtual();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.MergeDirtyHistoryInVirtual(GetData<unsigned int>());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.IsSurfaceRectChanged();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetActiveSurfaceRect();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetLastActiveSurfaceRect();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.IsActiveSurfaceRectChanged();
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.SetActiveSurfaceRect(GetRectI());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        rsDirtyRegionManager.GetUiLatestHistoryDirtyRegions(GetData<int>());
    });
    AddTestFunction(testFunctions, [](RSDirtyRegionManager& rsDirtyRegionManager) {
        std::map<NodeId, RectI> target;
        rsDirtyRegionManager.GetDirtyRegionInfo(target, GetData<RSRenderNodeType>(), GetData<DirtyRegionType>());
    });

    // run test functions
    std::mt19937 g(GetData<uint_fast32_t>());
    std::shuffle(testFunctions.begin(), testFunctions.end(), g);

    RSDirtyRegionManager rsDirtyRegionManager;
    for (const auto& func : testFunctions) {
        func(rsDirtyRegionManager);
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
