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

#include "rsinterfaces001_fuzzer.h"

#include <securec.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
namespace {

const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;

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
} // namespace
class SurfaceCaptureFuture : public SurfaceCaptureCallback {
    public:
        SurfaceCaptureFuture() = default;
        ~SurfaceCaptureFuture() {}
        void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
        {
            pixelMap_ = pixelmap;
        }
        void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelMap,
            std::shared_ptr<Media::PixelMap> pixelMapHDR) override {}
        std::shared_ptr<Media::PixelMap> GetPixelMap()
        {
            return pixelMap_;
        }
    private:
        std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
};

bool RSPhysicalScreenFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    float darkBuffer = GetData<float>();
    float brightBuffer = GetData<float>();
    int64_t interval = GetData<int64_t>();
    int32_t rangeSize = GetData<int32_t>();
#endif
    auto& rsInterfaces = RSInterfaces::GetInstance();
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    rsInterfaces.SetPointerColorInversionConfig(darkBuffer, brightBuffer, interval, rangeSize);
    PointerLuminanceChangeCallback callback = [](int32_t) {};
    rsInterfaces.RegisterPointerLuminanceChangeCallback(callback);
#endif
    auto callback1 = std::make_shared<SurfaceCaptureFuture>();
    rsInterfaces.TakeSurfaceCapture(static_cast<NodeId>(GetData<uint64_t>()), callback1);
    
    auto callback2 = std::make_shared<SurfaceCaptureFuture>();
    RSDisplayNodeConfig displayConfig = {
        static_cast<ScreenId>(GetData<uint64_t>()), GetData<bool>(), static_cast<NodeId>(GetData<uint64_t>())};
    auto displayNode = RSDisplayNode::Create(displayConfig);
    rsInterfaces.TakeSurfaceCapture(displayNode, callback2);

    auto callback3 = std::make_shared<SurfaceCaptureFuture>();
    RSSurfaceNodeConfig surfaceConfig;
    surfaceConfig.surfaceId = static_cast<NodeId>(GetData<uint64_t>());
    auto surfaceNode = RSSurfaceNode::Create(surfaceConfig);
    rsInterfaces.TakeSurfaceCapture(surfaceNode, callback3);
    rsInterfaces.MarkPowerOffNeedProcessOneFrame();
    rsInterfaces.NotifyScreenSwitched();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::Rosen::RSPhysicalScreenFuzzTest(data, size);
    return 0;
}
