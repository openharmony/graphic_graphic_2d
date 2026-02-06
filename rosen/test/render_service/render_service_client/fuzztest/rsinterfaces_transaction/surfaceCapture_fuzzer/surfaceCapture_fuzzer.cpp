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

#include "surfaceCapture_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_TAKE_SURFACE_CAPTURE = 0;
const uint8_t DO_TAKE_SELF_SURFACE_CAPTURE = 1;
const uint8_t DO_TAKE_SURFACE_CAPTURE_SOLO = 2;
const uint8_t DO_TAKE_UI_CAPTURE_IN_RANGE = 3;
const uint8_t DO_SET_WINDOW_FREEZE_IMMEDIATELY = 4;
constexpr uint8_t TARGET_SIZE = 5;

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

void DoTakeSurfaceCapture(FuzzedDataProvider& fdp)
{
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

    // Test with NodeId
    uint64_t nodeId = fdp.ConsumeIntegral<uint64_t>();
    auto callback = std::make_shared<SurfaceCaptureFuture>();
    g_rsInterfaces->TakeSurfaceCapture(nodeId, callback, captureConfig);
}

void DoTakeSelfSurfaceCapture(FuzzedDataProvider& fdp)
{
    RSSurfaceNodeConfig surfaceConfig;
    surfaceConfig.surfaceId = static_cast<NodeId>(fdp.ConsumeIntegral<uint64_t>());
    auto surfaceNode = RSSurfaceNode::Create(surfaceConfig);

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

    auto callback = std::make_shared<SurfaceCaptureFuture>();
    g_rsInterfaces->TakeSelfSurfaceCapture(surfaceNode, callback, captureConfig);
}

void DoTakeSurfaceCaptureSolo(FuzzedDataProvider& fdp)
{
    RSSurfaceNodeConfig surfaceConfig;
    surfaceConfig.surfaceId = fdp.ConsumeIntegral<uint64_t>();
    auto node = RSSurfaceNode::Create(surfaceConfig);

    auto result = g_rsInterfaces->TakeSurfaceCaptureSoloNodeList(node);
    (void)result;
}

void DoTakeUICaptureInRange(FuzzedDataProvider& fdp)
{
    // Create begin node
    RSSurfaceNodeConfig beginConfig;
    beginConfig.surfaceId = static_cast<NodeId>(fdp.ConsumeIntegral<uint64_t>());
    auto beginNode = RSSurfaceNode::Create(beginConfig);

    // Create end node
    RSSurfaceNodeConfig endConfig;
    endConfig.surfaceId = static_cast<NodeId>(fdp.ConsumeIntegral<uint64_t>());
    auto endNode = RSSurfaceNode::Create(endConfig);

    bool useBeginNodeSize = fdp.ConsumeBool();
    auto callback = std::make_shared<SurfaceCaptureFuture>();
    float scaleX = fdp.ConsumeFloatingPoint<float>();
    float scaleY = fdp.ConsumeFloatingPoint<float>();
    bool isSync = fdp.ConsumeBool();

    g_rsInterfaces->TakeUICaptureInRange(beginNode, endNode, useBeginNodeSize,
        callback, scaleX, scaleY, isSync);
}

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
        case OHOS::Rosen::DO_TAKE_SURFACE_CAPTURE:
            OHOS::Rosen::DoTakeSurfaceCapture(fdp);
            break;
        case OHOS::Rosen::DO_TAKE_SELF_SURFACE_CAPTURE:
            OHOS::Rosen::DoTakeSelfSurfaceCapture(fdp);
            break;
        case OHOS::Rosen::DO_TAKE_SURFACE_CAPTURE_SOLO:
            OHOS::Rosen::DoTakeSurfaceCaptureSolo(fdp);
            break;
        case OHOS::Rosen::DO_TAKE_UI_CAPTURE_IN_RANGE:
            OHOS::Rosen::DoTakeUICaptureInRange(fdp);
            break;
        case OHOS::Rosen::DO_SET_WINDOW_FREEZE_IMMEDIATELY:
            OHOS::Rosen::DoSetWindowFreezeImmediately(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
