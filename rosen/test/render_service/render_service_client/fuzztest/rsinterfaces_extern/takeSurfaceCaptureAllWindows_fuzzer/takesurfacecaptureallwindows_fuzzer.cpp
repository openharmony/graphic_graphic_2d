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

#include "takesurfacecaptureallwindows_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"
#include "ui/rs_display_node.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_TAKE_SURFACE_CAPTURE_ALL_WINDOWS = 0;
const uint8_t TARGET_SIZE = 1;

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

void DoTakeSurfaceCaptureWithAllWindows(FuzzedDataProvider& fdp)
{
    // Construct RSDisplayNodeConfig from fuzz data
    RSDisplayNodeConfig displayConfig = {
        .screenId = fdp.ConsumeIntegral<uint64_t>(),
        .isScreenMirror = fdp.ConsumeBool(),
        .mirrorNodeId = fdp.ConsumeIntegral<uint64_t>()
    };

    // Create RSDisplayNode using the config
    auto displayNode = RSDisplayNode::Create(displayConfig);

    // Construct callback using SurfaceCaptureFuture
    auto callback = std::make_shared<SurfaceCaptureFuture>();

    // Construct RSSurfaceCaptureConfig
    RSSurfaceCaptureConfig captureConfig = {
        .scaleX = fdp.ConsumeFloatingPoint<float>(),
        .scaleY = fdp.ConsumeFloatingPoint<float>(),
        .width = fdp.ConsumeIntegral<int32_t>(),
        .height = fdp.ConsumeIntegral<int32_t>(),
        .isSurfaceCaptureWithChildren = fdp.ConsumeBool(),
        .isPixelMapSource = fdp.ConsumeBool(),
        .isWithoutComposition = fdp.ConsumeBool(),
        .isCopyToSurface = fdp.ConsumeBool(),
        .useSurfaceCaptureTexture = fdp.ConsumeBool(),
        .isSync = fdp.ConsumeBool()
    };

    bool checkDrmAndSurfaceLock = fdp.ConsumeBool();

    // Call the interface
    g_rsInterfaces->TakeSurfaceCaptureWithAllWindows(
        displayNode, callback, captureConfig, checkDrmAndSurfaceLock);
}

} // anonymous namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Initialize RSInterfaces
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
        case OHOS::Rosen::DO_TAKE_SURFACE_CAPTURE_ALL_WINDOWS:
            OHOS::Rosen::DoTakeSurfaceCaptureWithAllWindows(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
