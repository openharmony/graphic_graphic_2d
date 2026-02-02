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

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>
#include "rsscreenproperty_fuzzer.h"
#include "screen_manager/rs_screen_property.h"
#include "transaction/rs_render_service_client.h"

namespace OHOS {
namespace Rosen {

namespace {
const uint8_t STRING_LEN = 10;
const uint8_t SCREEN_COLOR_GAMUT_SIZE = 12;
const uint8_t SCREEN_GAMUT_MAP_SIZE = 4;
const uint8_t SCREEN_STATE_SIZE = 5;
const uint8_t SCREEN_ROTATION_SIZE = 5;
const uint8_t SKIP_FRAME_STRATEGY_SIZE = 3;
const uint8_t SCREEN_CONNECTION_TYPE_SIZE = 3;
const uint8_t GRAPHIC_PIXEL_FORMAT_SIZE = 43;
const uint8_t SCREEN_POWER_STATUS_SIZE = 11;
const uint8_t SCREEN_TYPE_SIZE = 4;
const uint8_t SCREEN_HDR_FORMAT_SIZE = 8;
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos = 0;
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

void InitRSScreenPropertyAndParcel(RSScreenProperty& screenProperty, Parcel& parcel)
{
    ScreenId id = GetData<uint64_t>();
    screenProperty.id_ = id;
    bool isVirtual = GetData<bool>();
    screenProperty.isVirtual_ = isVirtual;
    std::string name(STRING_LEN, GetData<char>());
    screenProperty.name_ = name;
    uint32_t width = GetData<uint32_t>();
    screenProperty.width_ = width;
    uint32_t height = GetData<uint32_t>();
    screenProperty.height_ = height;
    uint32_t phyWidth = GetData<uint32_t>();
    screenProperty.phyWidth_ = phyWidth;
    uint32_t phyHeight = GetData<uint32_t>();
    screenProperty.phyHeight_ = phyHeight;
    uint32_t refreshRate = GetData<uint32_t>();
    screenProperty.refreshRate_ = refreshRate;
    int32_t offsetX = GetData<int32_t>();
    screenProperty.offsetX_ = offsetX;
    int32_t offsetY = GetData<int32_t>();
    screenProperty.offsetY_ = offsetY;
    bool isSamplingOn = GetData<bool>();
    screenProperty.isSamplingOn_ = isSamplingOn;
    float samplingTranslateX = GetData<float>();
    screenProperty.samplingTranslateX_ = samplingTranslateX;
    float samplingTranslateY = GetData<float>();
    screenProperty.samplingTranslateY_ = samplingTranslateY;
    float samplingScale = GetData<float>();
    screenProperty.samplingScale_ = samplingScale;
    ScreenColorGamut colorGamut = static_cast<ScreenColorGamut>(GetData<uint32_t>() % SCREEN_COLOR_GAMUT_SIZE);
    screenProperty.colorGamut_ = colorGamut;
    ScreenGamutMap gamutMap = static_cast<ScreenGamutMap>(GetData<uint32_t>() % SCREEN_GAMUT_MAP_SIZE);
    screenProperty.gamutMap_ = gamutMap;
    ScreenState state = static_cast<ScreenState>(GetData<uint8_t>() % SCREEN_STATE_SIZE);
    screenProperty.state_ = state;
    ScreenRotation correction = static_cast<ScreenRotation>(GetData<uint32_t>() % SCREEN_ROTATION_SIZE);
    screenProperty.correction_ = correction;
    bool canvasRotation = GetData<bool>();
    screenProperty.canvasRotation_ = canvasRotation;
    bool autoBufferRotation = GetData<bool>();
    screenProperty.autoBufferRotation_ = autoBufferRotation;
    RectI activeRect = RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
    screenProperty.activeRect_ = activeRect;
    RectI maskRect = RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
    screenProperty.maskRect_ = maskRect;
    RectI reviseRect = RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
    screenProperty.reviseRect_ = reviseRect;
    uint32_t skipFrameInterval = GetData<uint32_t>();
    screenProperty.skipFrameInterval_ = skipFrameInterval;
    uint32_t expectedRefreshRate = GetData<uint32_t>();
    screenProperty.expectedRefreshRate_ = expectedRefreshRate;
    SkipFrameStrategy skipFrameStrategy =
        static_cast<SkipFrameStrategy>(GetData<uint32_t>() % SKIP_FRAME_STRATEGY_SIZE);
    screenProperty.skipFrameStrategy_ = skipFrameStrategy;
    GraphicPixelFormat pixelFormat = static_cast<GraphicPixelFormat>(GetData<uint32_t>() % GRAPHIC_PIXEL_FORMAT_SIZE);
    screenProperty.pixelFormat_ = pixelFormat;
    ScreenHDRFormat hdrFormat = static_cast<ScreenHDRFormat>(GetData<uint32_t>() % SCREEN_HDR_FORMAT_SIZE);
    screenProperty.hdrFormat_ = hdrFormat;
    bool enableVisibleRect = GetData<bool>();
    screenProperty.enableVisibleRect_ = enableVisibleRect;
    Rect mainScreenVisibleRect {GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>()};
    screenProperty.mainScreenVisibleRect_ = mainScreenVisibleRect;
    bool isSupportRotation = GetData<bool>();
    screenProperty.isSupportRotation_ = isSupportRotation;
    std::unordered_set<NodeId> whiteList {GetData<uint64_t>()};
    screenProperty.whiteList_ = whiteList;
    std::unordered_set<NodeId> nodeList {GetData<uint64_t>()};
    screenProperty.blackList_ = nodeList;
    std::unordered_set<NodeType> screenNodeTypeList {GetData<uint8_t>()};
    screenProperty.typeBlackList_ = screenNodeTypeList;
    std::vector<NodeId> securityExemptionList {GetData<uint64_t>()};
    screenProperty.securityExemptionList_ = securityExemptionList;
    bool skipWindow = GetData<bool>();
    screenProperty.skipWindow_ = skipWindow;
    ScreenPowerStatus powerStatus = static_cast<ScreenPowerStatus>(GetData<uint32_t>() % SCREEN_POWER_STATUS_SIZE);
    screenProperty.powerStatus_ = powerStatus;
    RSScreenType screenType = static_cast<RSScreenType>(GetData<uint32_t>() % SCREEN_TYPE_SIZE);
    screenProperty.screenType_ = screenType;
    ScreenConnectionType connectionType =
        static_cast<ScreenConnectionType>(GetData<uint32_t>() % SCREEN_CONNECTION_TYPE_SIZE);
    screenProperty.connectionType_ = connectionType;
}

bool DoMarshalling()
{
    RSScreenProperty screenProperty;
    Parcel parcel;
    InitRSScreenPropertyAndParcel(screenProperty, parcel);
    screenProperty.Marshalling(parcel);
    return true;
}

bool DoUnmarshalling()
{
    RSScreenProperty screenProperty;
    Parcel parcel;
    InitRSScreenPropertyAndParcel(screenProperty, parcel);
    screenProperty.Marshalling(parcel);
    screenProperty.Unmarshalling(parcel);
    return true;
}

bool DoUnmarshallingData()
{
    RSScreenProperty screenProperty;
    Parcel parcel;
    InitRSScreenPropertyAndParcel(screenProperty, parcel);
    screenProperty.Marshalling(parcel);
    screenProperty.UnmarshallingData(parcel);
    return true;
}

} // namespace Rosen
} // namespace OHOS
/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::Rosen::g_data = data;
    OHOS::Rosen::g_size = size;
    OHOS::Rosen::g_pos = 0;

    /* Run your code on data */
    using FunctionPtr = bool (*)();
    std::vector<FunctionPtr> funcVector = {
        OHOS::Rosen::DoMarshalling,
        OHOS::Rosen::DoUnmarshalling,
        OHOS::Rosen::DoUnmarshallingData,
    };
    uint8_t pos = OHOS::Rosen::GetData<uint8_t>() % funcVector.size();
    funcVector[pos]();
    return 0;
}