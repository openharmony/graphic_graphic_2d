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
size_t g_pos;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    int left = GetData<int>();
    int top = GetData<int>();
    int width = GetData<int>();
    int height = GetData<int>();
    uint64_t id = GetData<uint64_t>();
    int32_t alignedBits = GetData<int32_t>();
    int age = GetData<int>();
    int offsetX = GetData<int>();
    int offsetY = GetData<int>();
    auto targetManager = std::make_shared<RSDirtyRegionManager>();

    DirtyRegionDebugType dirtyDebugType = (DirtyRegionDebugType)age;
    DebugRegionType var = (DebugRegionType)age;
    RectI myRect(left, top, width, height);
    std::vector<RectI> rects;
    rects.push_back(myRect);
    RSDirtyRegionManager rsDirtyRegionManager;
    rsDirtyRegionManager.MergeDirtyRect(myRect);
    rsDirtyRegionManager.MergeDirtyRectIfIntersect(myRect);
    rsDirtyRegionManager.MergeDirtyRectAfterMergeHistory(myRect);
    rsDirtyRegionManager.IntersectDirtyRect(myRect);
    rsDirtyRegionManager.ClipDirtyRectWithinSurface();
    rsDirtyRegionManager.Clear();
    rsDirtyRegionManager.UpdateVisitedDirtyRects(rects);
    rsDirtyRegionManager.GetIntersectedVisitedDirtyRect(myRect);
    rsDirtyRegionManager.UpdateCacheableFilterRect(myRect);
    rsDirtyRegionManager.IfCacheableFilterRectFullyCover(myRect);
    rsDirtyRegionManager.IsCacheableFilterRectEmpty();
    rsDirtyRegionManager.InvalidateFilterCacheRect();
    rsDirtyRegionManager.IsFilterCacheRectValid();
    rsDirtyRegionManager.GetCurrentFrameDirtyRegion();
    rsDirtyRegionManager.GetDirtyRegion();
    rsDirtyRegionManager.SetCurrentFrameDirtyRect(myRect);
    rsDirtyRegionManager.GetDirtyRegionFlipWithinSurface();
    rsDirtyRegionManager.GetLatestDirtyRegion();
    rsDirtyRegionManager.GetRectFlipWithinSurface(myRect);
    rsDirtyRegionManager.IsCurrentFrameDirty();
    rsDirtyRegionManager.IsDirty();
    rsDirtyRegionManager.UpdateDirty();
    rsDirtyRegionManager.UpdateDirtyByAligned(alignedBits);
    rsDirtyRegionManager.SetBufferAge(age);
    rsDirtyRegionManager.SetSurfaceSize(width, height);
    rsDirtyRegionManager.GetSurfaceRect();
    rsDirtyRegionManager.MergeSurfaceRect();
    rsDirtyRegionManager.ResetDirtyAsSurfaceSize();
    rsDirtyRegionManager.UpdateDebugRegionTypeEnable(dirtyDebugType);
    rsDirtyRegionManager.IsDebugRegionTypeEnable(var);
    rsDirtyRegionManager.OnSync(targetManager);
    rsDirtyRegionManager.UpdateDirtyRegionInfoForDfx(id);
    rsDirtyRegionManager.MarkAsTargetForDfx();
    rsDirtyRegionManager.IsTargetForDfx();
    rsDirtyRegionManager.HasOffset();
    rsDirtyRegionManager.SetOffset(offsetX, offsetY);
    rsDirtyRegionManager.GetOffsetedDirtyRegion();
    rsDirtyRegionManager.GetMergedDirtyRegions();
    rsDirtyRegionManager.MergeHistory(age, myRect);
    rsDirtyRegionManager.PushHistory(myRect);
    rsDirtyRegionManager.GetHistory(age);
    rsDirtyRegionManager.AlignHistory();
    RSDirtyRegionManager::GetPixelAlignedRect(myRect);

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
