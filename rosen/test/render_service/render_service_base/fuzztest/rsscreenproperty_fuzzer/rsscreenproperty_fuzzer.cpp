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
    screenProperty.Set<ScreenPropertyType::ID>(id);
    bool isVirtual = GetData<bool>();
    screenProperty.Set<ScreenPropertyType::IS_VIRTUAL>(isVirtual);
    std::string name(STRING_LEN, GetData<char>());
    screenProperty.Set<ScreenPropertyType::NAME>(name);
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    screenProperty.Set<ScreenPropertyType::RENDER_RESOLUTION>(std::make_pair(width, height));
    uint32_t phyWidth = GetData<uint32_t>();
    uint32_t phyHeight = GetData<uint32_t>();
    uint32_t refreshRate = GetData<uint32_t>();
    screenProperty.Set<ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE>(
        std::make_tuple(phyWidth, phyHeight, refreshRate));
    int32_t offsetX = GetData<int32_t>();
    int32_t offsetY = GetData<int32_t>();
    screenProperty.Set<ScreenPropertyType::OFFSET>(std::make_pair(offsetX, offsetY));
    bool isSamplingOn = GetData<bool>();
    float samplingTranslateX = GetData<float>();
    float samplingTranslateY = GetData<float>();
    float samplingScale = GetData<float>();
    screenProperty.Set<ScreenPropertyType::SAMPLING_OPTION>(
        std::make_tuple(isSamplingOn, samplingTranslateX, samplingTranslateY, samplingScale));
    uint32_t colorGamut = GetData<uint32_t>() % SCREEN_COLOR_GAMUT_SIZE;
    screenProperty.Set<ScreenPropertyType::COLOR_GAMUT>(colorGamut);
    uint32_t gamutMap = GetData<uint32_t>() % SCREEN_GAMUT_MAP_SIZE;
    screenProperty.Set<ScreenPropertyType::GAMUT_MAP>(gamutMap);
    uint8_t state = GetData<uint8_t>() % SCREEN_STATE_SIZE;
    screenProperty.Set<ScreenPropertyType::STATE>(state);
    uint32_t correction = GetData<uint32_t>() % SCREEN_ROTATION_SIZE;
    screenProperty.Set<ScreenPropertyType::CORRECTION>(correction);
    bool canvasRotation = GetData<bool>();
    screenProperty.Set<ScreenPropertyType::CANVAS_ROTATION>(canvasRotation);
    bool autoBufferRotation = GetData<bool>();
    screenProperty.Set<ScreenPropertyType::AUTO_BUFFER_ROTATION>(autoBufferRotation);
    RectI activeRect = RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
    RectI maskRect = RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
    RectI reviseRect = RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
    screenProperty.Set<ScreenPropertyType::ACTIVE_RECT_OPTION>(std::make_tuple(activeRect, maskRect, reviseRect));
    uint32_t skipFrameInterval = GetData<uint32_t>();
    uint32_t expectedRefreshRate = GetData<uint32_t>();
    uint8_t skipFrameStrategy = GetData<uint8_t>() % SKIP_FRAME_STRATEGY_SIZE;
    screenProperty.Set<ScreenPropertyType::SKIP_FRAME_OPTION>(
        std::make_tuple(skipFrameInterval, expectedRefreshRate, skipFrameStrategy));
    int32_t pixelFormat = GetData<int32_t>() % GRAPHIC_PIXEL_FORMAT_SIZE;
    screenProperty.Set<ScreenPropertyType::PIXEL_FORMAT>(pixelFormat);
    uint32_t hdrFormat = GetData<uint32_t>() % SCREEN_HDR_FORMAT_SIZE;
    screenProperty.Set<ScreenPropertyType::HDR_FORMAT>(hdrFormat);
    bool enableVisibleRect = GetData<bool>();
    Rect mainScreenVisibleRect {GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>()};
    bool isSupportRotation = GetData<bool>();
    screenProperty.Set<ScreenPropertyType::VISIBLE_RECT_OPTION>(
        std::make_tuple(enableVisibleRect, mainScreenVisibleRect, isSupportRotation));
    std::unordered_set<NodeId> whiteList {GetData<uint64_t>()};
    screenProperty.Set<ScreenPropertyType::WHITE_LIST>(whiteList);
    std::unordered_set<NodeId> nodeList {GetData<uint64_t>()};
    screenProperty.Set<ScreenPropertyType::BLACK_LIST>(nodeList);
    std::unordered_set<NodeType> screenNodeTypeList {GetData<uint8_t>()};
    screenProperty.Set<ScreenPropertyType::TYPE_BLACK_LIST>(screenNodeTypeList);
    std::vector<NodeId> securityExemptionList {GetData<uint64_t>()};
    screenProperty.Set<ScreenPropertyType::SECURITY_EXEMPTION_LIST>(securityExemptionList);
    bool skipWindow = GetData<bool>();
    screenProperty.Set<ScreenPropertyType::ENABLE_SKIP_WINDOW>(skipWindow);
    uint32_t powerStatus = GetData<uint32_t>() % SCREEN_POWER_STATUS_SIZE;
    screenProperty.Set<ScreenPropertyType::POWER_STATUS>(powerStatus);
    uint32_t screenType = GetData<uint32_t>() % SCREEN_TYPE_SIZE;
    screenProperty.Set<ScreenPropertyType::SCREEN_TYPE>(screenType);
    uint32_t connectionType = GetData<uint32_t>() % SCREEN_CONNECTION_TYPE_SIZE;
    screenProperty.Set<ScreenPropertyType::CONNECTION_TYPE>(connectionType);
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