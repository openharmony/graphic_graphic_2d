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

#include "rscltinterfaces_fuzzer.h"

#include <securec.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;

/*
 * describe: get data from outside untrusted data(DATA) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

template<>
std::string GetData()
{
    size_t objectSize = GetData<uint8_t>();
    std::string object(objectSize, '\0');
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    object.assign(reinterpret_cast<const char*>(DATA + g_pos), objectSize);
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
        std::shared_ptr<Media::PixelMap> GetPixelMap()
        {
            return pixelMap_;
        }
    private:
        std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
};

bool RSPhysicalScreenTest01(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // get data
    uint64_t id = GetData<uint64_t>();
    uint32_t modeId = GetData<uint32_t>();
    uint32_t status = GetData<uint32_t>();
    uint32_t level = GetData<uint32_t>();
    int32_t modeIdx = GetData<uint32_t>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    bool canvasRotation = GetData<bool>();
    uint32_t scaleMode = GetData<uint32_t>();
    int32_t x = GetData<int32_t>();
    int32_t y = GetData<int32_t>();
    int32_t w = GetData<int32_t>();
    int32_t h = GetData<int32_t>();
    Rect activeRect {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };

    // test
    auto& ins = RSInterfaces::GetInstance();
    ins.SetScreenActiveMode(static_cast<ScreenId>(id), modeId);
    ins.SetScreenActiveRect(static_cast<ScreenId>(id), activeRect);
    ins.SetScreenPowerStatus(static_cast<ScreenId>(id), static_cast<ScreenPowerStatus>(status));
    ins.SetScreenBacklight(static_cast<ScreenId>(id), level);
    ins.SetScreenColorGamut(static_cast<ScreenId>(id), modeIdx);
    ScreenGamutMap mapMode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    ins.SetScreenGamutMap(static_cast<ScreenId>(id), mapMode);
    ins.SetScreenSkipFrameInterval(static_cast<ScreenId>(id), skipFrameInterval);
    
    ins.GetScreenActiveMode(static_cast<ScreenId>(id));
    ins.GetScreenSupportedModes(static_cast<ScreenId>(id));
    ins.GetScreenCapability(static_cast<ScreenId>(id));
    ins.GetScreenPowerStatus(static_cast<ScreenId>(id));
    ins.GetScreenData(static_cast<ScreenId>(id));
    ins.GetScreenBacklight(static_cast<ScreenId>(id));
    return true;
}

bool RSPhysicalScreenTest02(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // get data
    uint64_t id = GetData<uint64_t>();
    uint32_t modeId = GetData<uint32_t>();
    uint32_t status = GetData<uint32_t>();
    uint32_t level = GetData<uint32_t>();
    int32_t modeIdx = GetData<uint32_t>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    bool canvasRotation = GetData<bool>();
    uint32_t scaleMode = GetData<uint32_t>();
    int32_t x = GetData<int32_t>();
    int32_t y = GetData<int32_t>();
    int32_t w = GetData<int32_t>();
    int32_t h = GetData<int32_t>();
    Rect activeRect {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };

    // test
    auto& ins = RSInterfaces::GetInstance();

    std::vector<ScreenColorGamut> modes = { ScreenColorGamut::COLOR_GAMUT_INVALID };
    ins.GetScreenSupportedColorGamuts(static_cast<ScreenId>(id), modes);
    std::vector<ScreenHDRMetadataKey> keys = {ScreenHDRMetadataKey::MATAKEY_RED_PRIMARY_X};
    ins.GetScreenSupportedMetaDataKeys(static_cast<ScreenId>(id), keys);
    ScreenColorGamut mode = ScreenColorGamut::COLOR_GAMUT_INVALID;
    ins.GetScreenColorGamut(static_cast<ScreenId>(id), mode);
    ins.GetScreenGamutMap(static_cast<ScreenId>(id), mapMode);
    RSScreenHDRCapability screenHdrCapability;
    ins.GetScreenHDRCapability(static_cast<ScreenId>(id), screenHdrCapability);
    RSScreenType screenType = RSScreenType::BUILT_IN_TYPE_SCREEN;
    ins.GetScreenType(static_cast<ScreenId>(id), screenType);
    SurfaceOcclusionChangeCallback cb = [](float) {};
    std::vector<float> partitionPoints;
    ins.RegisterSurfaceOcclusionChangeCallback(static_cast<NodeId>(id), cb, partitionPoints);
    ins.UnRegisterSurfaceOcclusionChangeCallback(static_cast<NodeId>(id));
    return true;
}

bool RSPhysicalScreenTest03(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // get data
    uint64_t id = GetData<uint64_t>();
    uint32_t modeId = GetData<uint32_t>();
    uint32_t status = GetData<uint32_t>();
    uint32_t level = GetData<uint32_t>();
    int32_t modeIdx = GetData<uint32_t>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    bool canvasRotation = GetData<bool>();
    uint32_t scaleMode = GetData<uint32_t>();
    int32_t x = GetData<int32_t>();
    int32_t y = GetData<int32_t>();
    int32_t w = GetData<int32_t>();
    int32_t h = GetData<int32_t>();
    Rect activeRect {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };

    // test
    auto& ins = RSInterfaces::GetInstance();
    ins.ResizeVirtualScreen(static_cast<NodeId>(id), width, height);
    ins.SetVirtualMirrorScreenCanvasRotation(static_cast<ScreenId>(id), canvasRotation);
    ins.SetVirtualMirrorScreenScaleMode(static_cast<ScreenId>(id), static_cast<ScreenScaleMode>(scaleMode));
    ins.SetGlobalDarkColorMode(GetData<bool>());
    std::vector<NodeId> blackListVector = {};
    blackListVector.push_back(id);
    ins.SetVirtualScreenBlackList(static_cast<ScreenId>(id), blackListVector);
    ins.AddVirtualScreenBlackList(static_cast<ScreenId>(id), blackListVector);
    ins.RemoveVirtualScreenBlackList(static_cast<ScreenId>(id), blackListVector);
    std::vector<NodeId> secExemptionList = {};
    secExemptionList.emplace_back(id);
    ins.SetVirtualScreenSecurityExemptionList(static_cast<ScreenId>(id), secExemptionList);

    Rect rect = {GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>()};
    ins.SetMirrorScreenVisibleRect(static_cast<ScreenId>(id), rect);
    return true;
}

bool RSPhysicalScreenTest04(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // get data
    uint64_t id = GetData<uint64_t>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    bool canvasRotation = GetData<bool>();
    uint32_t scaleMode = GetData<uint32_t>();
    int32_t x = GetData<int32_t>();
    int32_t y = GetData<int32_t>();
    int32_t w = GetData<int32_t>();
    int32_t h = GetData<int32_t>();
    Rect activeRect {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };

    // test
    auto& ins = RSInterfaces::GetInstance();
    auto callback1 = std::make_shared<SurfaceCaptureFuture>();
    ins.TakeSurfaceCapture(static_cast<NodeId>(GetData<uint64_t>()), callback1);

    auto callback2 = std::make_shared<SurfaceCaptureFuture>();
    RSDisplayNodeConfig displayConfig = {
        static_cast<ScreenId>(GetData<uint64_t>()), GetData<bool>(), static_cast<NodeId>(GetData<uint64_t>())};
    auto displayNode = RSDisplayNode::Create(displayConfig);
    ins.TakeSurfaceCapture(displayNode, callback2);

    auto callback3 = std::make_shared<SurfaceCaptureFuture>();
    RSSurfaceNodeConfig surfaceConfig;
    surfaceConfig.surfaceId = static_cast<NodeId>(GetData<uint64_t>());
    auto surfaceNode = RSSurfaceNode::Create(surfaceConfig);
    ins.TakeSurfaceCapture(surfaceNode, callback3);
    bool enable = GetData<bool>();
    ins.SetCastScreenEnableSkipWindow(static_cast<ScreenId>(id), enable);
    ins.RemoveVirtualScreen(static_cast<ScreenId>(id));
    ScreenId screenId = INVALID_SCREEN_ID;
    ScreenEvent screenEvent = ScreenEvent::UNKNOWN;
    bool callbacked = false;
    ScreenChangeCallback changeCallback = [&screenId, &screenEvent, &callbacked](ScreenId id, ScreenEvent event) {
        screenId = id;
        screenEvent = event;
        callbacked = true;
    };
    return true;
}

#ifdef TP_FEATURE_ENABLE
bool OHOS::Rosen::DoSetTpFeatureConfigFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // get data
    int32_t tpFeature = GetData<int32_t>();
    std::string tpConfig = GetData<std::string>();
    auto tpFeatureConfigType = static_cast<TpFeatureConfigType>(GetData<uint8_t>());

    // test
    auto& ins = RSInterfaces::GetInstance();
    ins.SetTpFeatureConfig(tpFeature, tpConfig.c_str(), tpFeatureConfigType);
    return true;
}
#endif

bool DoSetFreeMultiStatus(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // get data
    bool enable = GetData<bool>();

    // test
    auto& ins = RSInterfaces::GetInstance();
    ins.SetFreeMultiWindowStatus(enable);
    return true;
}

bool DoDropFrameByPids(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // get data
    std::vector<int32_t> pidsList;
    uint8_t pidListSize = GetData<uint8_t>();
    for (size_t i = 0; i < pidListSize; i++) {
        pidsList.push_back(GetData<int32_t>());
    };

    // test
    auto& ins = RSInterfaces::GetInstance();
    ins.DropFrameByPid(pidsList);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSPhysicalScreenTest01(data, size);
    OHOS::Rosen::RSPhysicalScreenTest02(data, size);
    OHOS::Rosen::RSPhysicalScreenTest03(data, size);
    OHOS::Rosen::RSPhysicalScreenTest04(data, size);
    OHOS::Rosen::DoSetFreeMultiStatus(data, size);
    OHOS::Rosen::DoDropFrameByPids(data, size);
#ifdef TP_FEATURE_ENABLE
    OHOS::Rosen::DoSetTpFeatureConfigFuzzTest(data, size);
#endif
    return 0;
}
