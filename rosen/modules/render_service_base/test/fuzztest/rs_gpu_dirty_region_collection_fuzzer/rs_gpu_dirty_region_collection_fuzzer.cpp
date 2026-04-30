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

#include "rs_gpu_dirty_region_collection_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "info_collection/rs_gpu_dirty_region_collection.h"

#include "common/rs_rect.h"

namespace OHOS {
namespace Rosen {

GpuDirtyRegionCollection* g_gpudirtyregioncollection = nullptr;

namespace {
constexpr uint8_t DO_METHOD_UPDATE_ACTIVE_DIRTY_INFO_VEC = 0;
constexpr uint8_t DO_METHOD_UPDATE_ACTIVE_DIRTY_INFO_RECT = 1;
constexpr uint8_t DO_METHOD_UPDATE_GLOBAL_DIRTY_INFO = 2;
constexpr uint8_t DO_METHOD_ADD_SKIP_PROCESS_FRAMES = 3;
constexpr uint8_t TARGET_SIZE = 4;

void DoUpdateActiveDirtyInfoForDFXVec(FuzzedDataProvider& fdp)
{
    NodeId id = fdp.ConsumeIntegral<NodeId>();
    std::string windowName = fdp.ConsumeRandomLengthString(64);
    size_t vecSize = fdp.ConsumeIntegral<uint8_t>() % 4;
    std::vector<RectI> rectIs;
    for (size_t i = 0; i < vecSize; i++) {
        RectI rect;
        rect.left_ = fdp.ConsumeIntegral<int32_t>();
        rect.top_ = fdp.ConsumeIntegral<int32_t>();
        rect.width_ = fdp.ConsumeIntegral<int32_t>();
        rect.height_ = fdp.ConsumeIntegral<int32_t>();
        rectIs.push_back(rect);
    }
    g_gpudirtyregioncollection->UpdateActiveDirtyInfoForDFX(id, windowName, rectIs);
}

void DoUpdateActiveDirtyInfoForDFXRect(FuzzedDataProvider& fdp)
{
    NodeId id = fdp.ConsumeIntegral<NodeId>();
    std::string windowName = fdp.ConsumeRandomLengthString(64);
    Rect damage;
    damage.x = fdp.ConsumeIntegral<int32_t>();
    damage.y = fdp.ConsumeIntegral<int32_t>();
    damage.w = fdp.ConsumeIntegral<int32_t>();
    damage.h = fdp.ConsumeIntegral<int32_t>();
    g_gpudirtyregioncollection->UpdateActiveDirtyInfoForDFX(id, windowName, damage);
}

void DoUpdateGlobalDirtyInfoForDFX(FuzzedDataProvider& fdp)
{
    RectI rect;
    rect.left_ = fdp.ConsumeIntegral<int32_t>();
    rect.top_ = fdp.ConsumeIntegral<int32_t>();
    rect.width_ = fdp.ConsumeIntegral<int32_t>();
    rect.height_ = fdp.ConsumeIntegral<int32_t>();
    g_gpudirtyregioncollection->UpdateGlobalDirtyInfoForDFX(rect);
}

void DoAddSkipProcessFramesNumberForDFX(FuzzedDataProvider& fdp)
{
    pid_t sendingPid = fdp.ConsumeIntegral<pid_t>();
    g_gpudirtyregioncollection->AddSkipProcessFramesNumberForDFX(sendingPid);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    OHOS::Rosen::g_gpudirtyregioncollection = &OHOS::Rosen::GpuDirtyRegionCollection::GetInstance();
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_gpudirtyregioncollection == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_METHOD_UPDATE_ACTIVE_DIRTY_INFO_VEC:
            OHOS::Rosen::DoUpdateActiveDirtyInfoForDFXVec(fdp);
            break;
        case OHOS::Rosen::DO_METHOD_UPDATE_ACTIVE_DIRTY_INFO_RECT:
            OHOS::Rosen::DoUpdateActiveDirtyInfoForDFXRect(fdp);
            break;
        case OHOS::Rosen::DO_METHOD_UPDATE_GLOBAL_DIRTY_INFO:
            OHOS::Rosen::DoUpdateGlobalDirtyInfoForDFX(fdp);
            break;
        case OHOS::Rosen::DO_METHOD_ADD_SKIP_PROCESS_FRAMES:
            OHOS::Rosen::DoAddSkipProcessFramesNumberForDFX(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
