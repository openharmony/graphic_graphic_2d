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
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
const uint32_t usleepTime = 1000;

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
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

template<>
std::string GetData()
{
    size_t objectSize = GetData<uint8_t>();
    std::string object(objectSize, '\0');
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    object.assign(reinterpret_cast<const char*>(g_data + g_pos), objectSize);
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

bool RSPhysicalScreenTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
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
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    float darkBuffer = GetData<float>();
    float brightBuffer = GetData<float>();
    int64_t interval = GetData<int64_t>();
    int32_t rangeSize = GetData<int32_t>();
#endif
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
    auto& rsCltInterfaces = RSInterfaces::GetInstance();
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    rsCltInterfaces.SetPointerColorInversionConfig(darkBuffer, brightBuffer, interval, rangeSize);
    PointerLuminanceChangeCallback callback = [](int32_t) {};
    rsCltInterfaces.RegisterPointerLuminanceChangeCallback(callback);
#endif
    rsCltInterfaces.SetScreenActiveMode(static_cast<ScreenId>(id), modeId);
    rsCltInterfaces.SetScreenActiveRect(static_cast<ScreenId>(id), activeRect);
    rsCltInterfaces.SetScreenPowerStatus(static_cast<ScreenId>(id), static_cast<ScreenPowerStatus>(status));
    rsCltInterfaces.SetScreenBacklight(static_cast<ScreenId>(id), level);
    rsCltInterfaces.SetScreenColorGamut(static_cast<ScreenId>(id), modeIdx);
    ScreenGamutMap mapMode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    rsCltInterfaces.SetScreenGamutMap(static_cast<ScreenId>(id), mapMode);
    rsCltInterfaces.SetScreenSkipFrameInterval(static_cast<ScreenId>(id), skipFrameInterval);
    
    rsCltInterfaces.GetScreenActiveMode(static_cast<ScreenId>(id));
    rsCltInterfaces.GetScreenSupportedModes(static_cast<ScreenId>(id));
    rsCltInterfaces.GetScreenCapability(static_cast<ScreenId>(id));
    rsCltInterfaces.GetScreenPowerStatus(static_cast<ScreenId>(id));
    rsCltInterfaces.GetScreenData(static_cast<ScreenId>(id));
    rsCltInterfaces.GetScreenBacklight(static_cast<ScreenId>(id));
    std::vector<ScreenColorGamut> modes = { ScreenColorGamut::COLOR_GAMUT_INVALID };
    rsCltInterfaces.GetScreenSupportedColorGamuts(static_cast<ScreenId>(id), modes);
    std::vector<ScreenHDRMetadataKey> keys = {ScreenHDRMetadataKey::MATAKEY_RED_PRIMARY_X};
    rsCltInterfaces.GetScreenSupportedMetaDataKeys(static_cast<ScreenId>(id), keys);
    ScreenColorGamut mode = ScreenColorGamut::COLOR_GAMUT_INVALID;
    rsCltInterfaces.GetScreenColorGamut(static_cast<ScreenId>(id), mode);
    rsCltInterfaces.GetScreenGamutMap(static_cast<ScreenId>(id), mapMode);
    RSScreenHDRCapability screenHdrCapability;
    rsCltInterfaces.GetScreenHDRCapability(static_cast<ScreenId>(id), screenHdrCapability);
    RSScreenType screenType = RSScreenType::BUILT_IN_TYPE_SCREEN;
    rsCltInterfaces.GetScreenType(static_cast<ScreenId>(id), screenType);
    SurfaceOcclusionChangeCallback surfaceOcclusionCb = [](float) {};
    std::vector<float> partitionPoints;
    rsCltInterfaces.RegisterSurfaceOcclusionChangeCallback(static_cast<NodeId>(id), surfaceOcclusionCb, partitionPoints);
    rsCltInterfaces.UnRegisterSurfaceOcclusionChangeCallback(static_cast<NodeId>(id));
    rsCltInterfaces.ResizeVirtualScreen(static_cast<NodeId>(id), width, height);
    rsCltInterfaces.SetVirtualMirrorScreenCanvasRotation(static_cast<ScreenId>(id), canvasRotation);
    rsCltInterfaces.SetVirtualMirrorScreenScaleMode(static_cast<ScreenId>(id), static_cast<ScreenScaleMode>(scaleMode));
    rsCltInterfaces.SetGlobalDarkColorMode(GetData<bool>());
    std::vector<NodeId> blackListVector = {};
    blackListVector.push_back(id);
    rsCltInterfaces.SetVirtualScreenBlackList(static_cast<ScreenId>(id), blackListVector);
    rsCltInterfaces.AddVirtualScreenBlackList(static_cast<ScreenId>(id), blackListVector);
    rsCltInterfaces.RemoveVirtualScreenBlackList(static_cast<ScreenId>(id), blackListVector);
    std::vector<NodeId> secExemptionList = {};
    secExemptionList.emplace_back(id);
    rsCltInterfaces.SetVirtualScreenSecurityExemptionList(static_cast<ScreenId>(id), secExemptionList);

    Rect rect = {GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>()};
    rsCltInterfaces.SetMirrorScreenVisibleRect(static_cast<ScreenId>(id), rect);

    auto callback1 = std::make_shared<SurfaceCaptureFuture>();
    rsCltInterfaces.TakeSurfaceCapture(static_cast<NodeId>(GetData<uint64_t>()), callback1);

    auto callback2 = std::make_shared<SurfaceCaptureFuture>();
    RSDisplayNodeConfig displayConfig = {
        static_cast<ScreenId>(GetData<uint64_t>()), GetData<bool>(), static_cast<NodeId>(GetData<uint64_t>())};
    auto displayNode = RSDisplayNode::Create(displayConfig);
    rsCltInterfaces.TakeSurfaceCapture(displayNode, callback2);

    auto callback3 = std::make_shared<SurfaceCaptureFuture>();
    RSSurfaceNodeConfig surfaceConfig;
    surfaceConfig.surfaceId = static_cast<NodeId>(GetData<uint64_t>());
    auto surfaceNode = RSSurfaceNode::Create(surfaceConfig);
    rsCltInterfaces.TakeSurfaceCapture(surfaceNode, callback3);
    bool enable = GetData<bool>();
    rsCltInterfaces.SetCastScreenEnableSkipWindow(static_cast<ScreenId>(id), enable);
    rsCltInterfaces.RemoveVirtualScreen(static_cast<ScreenId>(id));
    ScreenId screenId = INVALID_SCREEN_ID;
    ScreenEvent screenEvent = ScreenEvent::UNKNOWN;
    bool callbacked = false;
    ScreenChangeCallback changeCallback = [&screenId, &screenEvent, &callbacked](ScreenId id, ScreenEvent event) {
        screenId = id;
        screenEvent = event;
        callbacked = true;
    };
    rsCltInterfaces.SetScreenChangeCallback(changeCallback);
    uint32_t screenRotation = GetData<uint32_t>();
    rsCltInterfaces.SetScreenCorrection(static_cast<ScreenId>(id), static_cast<ScreenRotation>(screenRotation));
    uint32_t systemAnimatedScenes = GetData<uint32_t>();
    rsCltInterfaces.SetSystemAnimatedScenes(static_cast<SystemAnimatedScenes>(systemAnimatedScenes));

    rsCltInterfaces.SetHwcNodeBounds(static_cast<NodeId>(id), 1.0f, 1.0f, 1.0f, 1.0f);

    rsCltInterfaces.MarkPowerOffNeedProcessOneFrame();
    rsCltInterfaces.DisablePowerOffRenderControl(static_cast<ScreenId>(id));
    UIExtensionCallback uiExtensionCallback = [](std::shared_ptr<RSUIExtensionData>, uint64_t) {};
    rsCltInterfaces.RegisterUIExtensionCallback(id, uiExtensionCallback);
    usleep(usleepTime);

    VirtualScreenStatus screenStatus = VirtualScreenStatus::VIRTUAL_SCREEN_PLAY;
    rsCltInterfaces.SetVirtualScreenStatus(static_cast<ScreenId>(id), static_cast<VirtualScreenStatus>(screenStatus));

    std::string nodeIdStr = GetData<std::string>();
    bool isTop = GetData<bool>();
    rsCltInterfaces.SetLayerTop(nodeIdStr, isTop);
    return true;
}

#ifdef TP_FEATURE_ENABLE
bool OHOS::Rosen::DoSetTpFeatureConfigFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    int32_t tpFeature = GetData<int32_t>();
    std::string tpConfig = GetData<std::string>();
    auto tpFeatureConfigType = static_cast<TpFeatureConfigType>(GetData<uint8_t>());

    // test
    auto& rsCltInterfaces = RSInterfaces::GetInstance();
    rsCltInterfaces.SetTpFeatureConfig(tpFeature, tpConfig.c_str(), tpFeatureConfigType);
    return true;
}
#endif

bool DoSetFreeMultiStatus(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    bool enable = GetData<bool>();

    // test
    auto& rsCltInterfaces = RSInterfaces::GetInstance();
    rsCltInterfaces.SetFreeMultiWindowStatus(enable);
    return true;
}

bool DoDropFrameByPids(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    std::vector<int32_t> pidsList;
    uint8_t pidListSize = GetData<uint8_t>();
    for (size_t i = 0; i < pidListSize; i++) {
        pidsList.push_back(GetData<int32_t>());
    };

    // test
    auto& rsCltInterfaces = RSInterfaces::GetInstance();
    rsCltInterfaces.DropFrameByPid(pidsList);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSPhysicalScreenTest(data, size);
    OHOS::Rosen::DoSetFreeMultiStatus(data, size);
    OHOS::Rosen::DoDropFrameByPids(data, size);
#ifdef TP_FEATURE_ENABLE
    OHOS::Rosen::DoSetTpFeatureConfigFuzzTest(data, size);
#endif
    return 0;
}
