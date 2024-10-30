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

#include "rspropertydrawable_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <memory>
#include <securec.h>
#include <unistd.h>

#include "recording/draw_cmd_list.h"

#include "pipeline/rs_render_node.h"
#include "property/rs_property_drawable.h"
#include "render/rs_filter.h"
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

    auto drawCmdList = Drawing::DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    DrawableV2::RSPropertyDrawable rsPropertyDrawable(std::move(drawCmdList));
    rsPropertyDrawable.OnSync();

    int width = GetData<int>();
    int height = GetData<int>();
    float blurRadiusX = GetData<float>();
    float blurRadiusY = GetData<float>();
    DrawableV2::RSPropertyDrawCmdListUpdater rsPropertyDrawCmdListUpdater(width, height, &rsPropertyDrawable);
    rsPropertyDrawCmdListUpdater.GetRecordingCanvas();

    uint64_t id = GetData<uint64_t>();
    RSRenderNode node(id);
    auto frameDrawCmdList = Drawing::DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    DrawableV2::RSFrameOffsetDrawable rsFrameOffsetDrawable(std::move(frameDrawCmdList));
    DrawableV2::RSFrameOffsetDrawable::OnGenerate(node);
    rsFrameOffsetDrawable.OnUpdate(node);

    auto clipDrawCmdList = Drawing::DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    DrawableV2::RSClipToBoundsDrawable rsClipToBoundsDrawable(std::move(clipDrawCmdList));
    DrawableV2::RSClipToBoundsDrawable::OnGenerate(node);
    rsClipToBoundsDrawable.OnUpdate(node);

    auto clipToFrameDrawCmdList = Drawing::DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    DrawableV2::RSClipToFrameDrawable rsClipToFrameDrawable(std::move(clipToFrameDrawCmdList));
    DrawableV2::RSClipToFrameDrawable::OnGenerate(node);
    rsClipToFrameDrawable.OnUpdate(node);

    auto rsFilter = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    DrawableV2::RSFilterDrawable rsFilterDrawable;
    rsFilterDrawable.IsForeground();
    rsFilterDrawable.MarkFilterRegionChanged();
    rsFilterDrawable.MarkFilterRegionInteractWithDirty();
    rsFilterDrawable.MarkFilterRegionIsLargeArea();
    rsFilterDrawable.MarkFilterForceUseCache();
    rsFilterDrawable.MarkFilterForceClearCache();
    rsFilterDrawable.MarkRotationChanged();
    rsFilterDrawable.MarkNodeIsOccluded(true);
    rsFilterDrawable.IsFilterCacheValid();
    rsFilterDrawable.NeedPendingPurge();
    rsFilterDrawable.OnSync();
    rsFilterDrawable.GetFilterCachedRegion();
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
