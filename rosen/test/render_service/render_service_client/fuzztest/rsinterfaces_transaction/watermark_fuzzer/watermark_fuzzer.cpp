/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "watermark_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
// Grouped by related functionality
const uint8_t DO_SET_WATERMARK = 0;            // Watermark setting
const uint8_t DO_SHOW_WATERMARK = 1;           // Watermark display (paired with SET_WATERMARK)
const uint8_t DO_SET_SURFACE_WATERMARK = 2;    // Surface watermark setting
const uint8_t DO_CLEAR_SURFACE_WATERMARK = 3;  // Surface watermark clearing (paired with SET_SURFACE_WATERMARK)
const uint8_t DO_CLEAR_SURFACE_WATERMARK_FOR_NODES = 4;  // Surface watermark clearing for nodes
constexpr size_t STR_LEN = 10;
constexpr uint8_t TARGET_SIZE = 5;

void DoSetWatermark(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);

    Media::InitializationOptions opts;
    opts.size.width = fdp.ConsumeIntegral<int32_t>();
    opts.size.height = fdp.ConsumeIntegral<int32_t>();
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(fdp.ConsumeIntegral<int32_t>());
    opts.pixelFormat = static_cast<Media::PixelFormat>(fdp.ConsumeIntegral<int32_t>());
    opts.alphaType = static_cast<Media::AlphaType>(fdp.ConsumeIntegral<int32_t>());
    opts.scaleMode = static_cast<Media::ScaleMode>(fdp.ConsumeIntegral<int32_t>());
    opts.editable = fdp.ConsumeBool();
    opts.useSourceIfMatch = fdp.ConsumeBool();

    std::shared_ptr<Media::PixelMap> watermark = Media::PixelMap::Create(opts);
    g_rsInterfaces->SetWatermark(name, watermark);
}

void DoSetSurfaceWatermark(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);

    Media::InitializationOptions opts;
    opts.size.width = fdp.ConsumeIntegral<int32_t>();
    opts.size.height = fdp.ConsumeIntegral<int32_t>();
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(fdp.ConsumeIntegral<int32_t>());
    opts.pixelFormat = static_cast<Media::PixelFormat>(fdp.ConsumeIntegral<int32_t>());
    opts.alphaType = static_cast<Media::AlphaType>(fdp.ConsumeIntegral<int32_t>());
    opts.scaleMode = static_cast<Media::ScaleMode>(fdp.ConsumeIntegral<int32_t>());
    opts.editable = fdp.ConsumeBool();
    opts.useSourceIfMatch = fdp.ConsumeBool();

    std::shared_ptr<Media::PixelMap> watermark = Media::PixelMap::Create(opts);
    auto watermarkType = static_cast<uint8_t>(fdp.ConsumeIntegral<uint8_t>());

    int32_t pid = fdp.ConsumeIntegral<int32_t>();
    std::vector<NodeId> nodeIdList;
    uint8_t listSize = fdp.ConsumeIntegral<uint8_t>();
    for (auto i = 0; i < listSize; i++) {
        nodeIdList.push_back(fdp.ConsumeIntegral<uint64_t>());
    }

    g_rsInterfaces->SetSurfaceWatermark(pid, name, watermark, nodeIdList,
        static_cast<SurfaceCaptureType>(watermarkType));
}

void DoShowWatermark(FuzzedDataProvider& fdp)
{
    Media::InitializationOptions opts;
    opts.size.width = fdp.ConsumeIntegral<int32_t>();
    opts.size.height = fdp.ConsumeIntegral<int32_t>();
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(fdp.ConsumeIntegral<int32_t>());
    opts.pixelFormat = static_cast<Media::PixelFormat>(fdp.ConsumeIntegral<int32_t>());
    opts.alphaType = static_cast<Media::AlphaType>(fdp.ConsumeIntegral<int32_t>());
    opts.scaleMode = static_cast<Media::ScaleMode>(fdp.ConsumeIntegral<int32_t>());
    opts.editable = fdp.ConsumeBool();
    opts.useSourceIfMatch = fdp.ConsumeBool();

    std::shared_ptr<Media::PixelMap> watermarkImg = Media::PixelMap::Create(opts);
    bool isShow = fdp.ConsumeBool();

    g_rsInterfaces->ShowWatermark(watermarkImg, isShow);
}

void DoClearSurfaceWatermark(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    int32_t pid = fdp.ConsumeIntegral<int32_t>();
    g_rsInterfaces->ClearSurfaceWatermark(pid, name);
}

void DoClearSurfaceWatermarkForNodes(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    int32_t pid = fdp.ConsumeIntegral<int32_t>();
    std::vector<NodeId> nodeIdList;
    uint8_t listSize = fdp.ConsumeIntegral<uint8_t>();
    for (auto i = 0; i < listSize; i++) {
        nodeIdList.push_back(fdp.ConsumeIntegral<uint64_t>());
    }
    g_rsInterfaces->ClearSurfaceWatermarkForNodes(pid, name, nodeIdList);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Pre-initialize RSInterfaces singleton to avoid runtime initialization overhead
    OHOS::Rosen::g_rsInterfaces = &OHOS::Rosen::RSInterfaces::GetInstance();
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_rsInterfaces == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        // Watermark operations (grouped together)
        case OHOS::Rosen::DO_SET_WATERMARK:
            OHOS::Rosen::DoSetWatermark(fdp);
            break;
        case OHOS::Rosen::DO_SHOW_WATERMARK:
            OHOS::Rosen::DoShowWatermark(fdp);
            break;
        // Surface watermark operations (grouped together)
        case OHOS::Rosen::DO_SET_SURFACE_WATERMARK:
            OHOS::Rosen::DoSetSurfaceWatermark(fdp);
            break;
        case OHOS::Rosen::DO_CLEAR_SURFACE_WATERMARK:
            OHOS::Rosen::DoClearSurfaceWatermark(fdp);
            break;
        case OHOS::Rosen::DO_CLEAR_SURFACE_WATERMARK_FOR_NODES:
            OHOS::Rosen::DoClearSurfaceWatermarkForNodes(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
