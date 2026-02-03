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

#include "setWindowConfig_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_SET_WINDOW_FREEZE_IMMEDIATELY = 0;
const uint8_t DO_SET_WINDOW_CONTAINER = 1;
constexpr uint8_t TARGET_SIZE = 2;

class SurfaceCaptureFuture : public SurfaceCaptureCallback {
public:
    SurfaceCaptureFuture() = default;
    ~SurfaceCaptureFuture() override = default;

    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
        pixelMap_ = pixelmap;
    }

    void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelmap,
        std::shared_ptr<Media::PixelMap> pixelMapHDR) override
    {
        pixelMap_ = pixelmap;
        pixelMapHDR_ = pixelMapHDR;
    }

private:
    std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
    std::shared_ptr<Media::PixelMap> pixelMapHDR_ = nullptr;
};

void DoSetWindowFreezeImmediately(FuzzedDataProvider& fdp)
{
    RSSurfaceNodeConfig surfaceConfig;
    surfaceConfig.surfaceId = static_cast<NodeId>(fdp.ConsumeIntegral<uint64_t>());
    auto surfaceNode = RSSurfaceNode::Create(surfaceConfig);

    bool isFreeze = fdp.ConsumeBool();
    auto callback = std::make_shared<SurfaceCaptureFuture>();

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = fdp.ConsumeFloatingPoint<float>();
    captureConfig.scaleY = fdp.ConsumeFloatingPoint<float>();
    captureConfig.useDma = fdp.ConsumeBool();
    captureConfig.useCurWindow = fdp.ConsumeBool();
    captureConfig.captureType = static_cast<SurfaceCaptureType>(fdp.ConsumeIntegral<uint8_t>());
    captureConfig.isSync = fdp.ConsumeBool();

    uint8_t listSize = fdp.ConsumeIntegral<uint8_t>();
    for (auto i = 0; i < listSize; i++) {
        uint64_t nodeId = fdp.ConsumeIntegral<uint64_t>();
        captureConfig.blackList.push_back(nodeId);
    }

    captureConfig.mainScreenRect.left_ = fdp.ConsumeFloatingPoint<float>();
    captureConfig.mainScreenRect.top_ = fdp.ConsumeFloatingPoint<float>();
    captureConfig.mainScreenRect.right_ = fdp.ConsumeFloatingPoint<float>();
    captureConfig.mainScreenRect.bottom_ = fdp.ConsumeFloatingPoint<float>();

    g_rsInterfaces->SetWindowFreezeImmediately(surfaceNode, isFreeze, callback, captureConfig);
}

void DoSetWindowContainer(FuzzedDataProvider& fdp)
{
    uint64_t nodeId = fdp.ConsumeIntegral<uint64_t>();
    bool value = fdp.ConsumeBool();
    g_rsInterfaces->SetWindowContainer(static_cast<NodeId>(nodeId), value);
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
        case OHOS::Rosen::DO_SET_WINDOW_FREEZE_IMMEDIATELY:
            OHOS::Rosen::DoSetWindowFreezeImmediately(fdp);
            break;
        case OHOS::Rosen::DO_SET_WINDOW_CONTAINER:
            OHOS::Rosen::DoSetWindowContainer(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
