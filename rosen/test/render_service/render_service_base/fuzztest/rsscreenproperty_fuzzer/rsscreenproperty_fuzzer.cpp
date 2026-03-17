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
constexpr uint8_t STRING_LEN = 10;
constexpr uint8_t* g_data = nullptr;
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
    screenProperty.Set<ScreenPropertyType::ID>(GetData<uint64_t>());
    screenProperty.Set<ScreenPropertyType::IS_VIRTUAL>(GetData<bool>());
    screenProperty.Set<ScreenPropertyType::NAME>(std::string(STRING_LEN, GetData<char>()));
    screenProperty.Set<ScreenPropertyType::RENDER_RESOLUTION>(std::make_pair(GetData<uint32_t>(), GetData<uint32_t>()));
    screenProperty.Set<ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE>(
        std::make_tuple(GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint32_t>()));
    screenProperty.Set<ScreenPropertyType::OFFSET>(std::make_pair(GetData<int32_t>(), GetData<int32_t>()));
    screenProperty.Set<ScreenPropertyType::SAMPLING_OPTION>(
        std::make_tuple(GetData<bool>(), GetData<float>(), GetData<float>(), GetData<float>()));
    screenProperty.Set<ScreenPropertyType::COLOR_GAMUT>(GetData<uint32_t>());
    screenProperty.Set<ScreenPropertyType::GAMUT_MAP>(GetData<uint32_t>());
    screenProperty.Set<ScreenPropertyType::STATE>(GetData<uint8_t>());
    screenProperty.Set<ScreenPropertyType::CORRECTION>(GetData<uint32_t>());
    screenProperty.Set<ScreenPropertyType::CANVAS_ROTATION>(GetData<bool>());
    screenProperty.Set<ScreenPropertyType::AUTO_BUFFER_ROTATION>(GetData<bool>());
    screenProperty.Set<ScreenPropertyType::ACTIVE_RECT_OPTION>(
        std::make_tuple(RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>()),
                        RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>()),
                        RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>())));
    screenProperty.Set<ScreenPropertyType::SKIP_FRAME_OPTION>(
        std::make_tuple(GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint8_t>()));
    screenProperty.Set<ScreenPropertyType::PIXEL_FORMAT>(GetData<int32_t>());
    screenProperty.Set<ScreenPropertyType::HDR_FORMAT>(GetData<uint32_t>());
    screenProperty.Set<ScreenPropertyType::VISIBLE_RECT_OPTION>(
        std::make_tuple(GetData<bool>(), Rect{GetData<int32_t>(), GetData<int32_t>(),
                        GetData<int32_t>(), GetData<int32_t>()}, GetData<bool>()));
    screenProperty.Set<ScreenPropertyType::WHITE_LIST>(std::unordered_set<NodeId>{GetData<uint64_t>()});
    screenProperty.Set<ScreenPropertyType::BLACK_LIST>(std::unordered_set<NodeId>{GetData<uint64_t>()});
    screenProperty.Set<ScreenPropertyType::TYPE_BLACK_LIST>(std::unordered_set<NodeType>{GetData<uint8_t>()});
    screenProperty.Set<ScreenPropertyType::SECURITY_EXEMPTION_LIST>(std::vector<NodeId>{GetData<uint64_t>()});
    screenProperty.Set<ScreenPropertyType::ENABLE_SKIP_WINDOW>(GetData<bool>());
    screenProperty.Set<ScreenPropertyType::POWER_STATUS>(GetData<uint32_t>());
    screenProperty.Set<ScreenPropertyType::SCREEN_TYPE>(GetData<uint32_t>());
    screenProperty.Set<ScreenPropertyType::CONNECTION_TYPE>(GetData<uint32_t>());
    screenProperty.Set<ScreenPropertyType::SCALE_MODE>(GetData<uint32_t>());
    screenProperty.Set<ScreenPropertyType::SCREEN_STATUS>(GetData<uint32_t>());
    screenProperty.Set<ScreenPropertyType::VIRTUAL_SEC_LAYER_OPTION>(GetData<int32_t>());
    screenProperty.Set<ScreenPropertyType::IS_HARD_CURSOR_SUPPORT>(GetData<bool>());
    screenProperty.Set<ScreenPropertyType::SUPPORTED_COLOR_GAMUTS>(std::vector<ScreenColorGamut>{GetData<uint8_t>()});
    screenProperty.Set<ScreenPropertyType::DISABLE_POWER_OFF_RENDER_CONTROL>(GetData<bool>());
    screenProperty.Set<ScreenPropertyType::SCREEN_SWITCH_STATUS>(GetData<bool>());
    screenProperty.Set<ScreenPropertyType::SCREEN_FRAME_GRAVITY>(GetData<int32_t>());
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