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

#include "rsscreencapability_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "screen_manager/rs_screen_capability.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t SCREEN_INTERFACE_TYPE_SIZE = 13;
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
constexpr size_t STRING_LEN = 10;
} // namespace

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

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;
    return true;
}

void InitProps(std::vector<RSScreenProps>& props)
{
    uint32_t propId = GetData<uint32_t>();
    uint64_t value = GetData<uint64_t>();
    std::string capName(STRING_LEN, GetData<char>());
    RSScreenProps prop = RSScreenProps(capName, propId, value);
    props.push_back(prop);
}

void InitRSScreenCapabilityAndParcel(RSScreenCapability& capability, Parcel& parcel)
{
    uint32_t phyWidth = GetData<uint32_t>();
    capability.SetPhyWidth(phyWidth);
    uint32_t phyHight = GetData<uint32_t>();
    capability.SetPhyHeight(phyHight);
    uint32_t supportLayers = GetData<uint32_t>();
    capability.SetSupportLayers(supportLayers);
    uint32_t virtualDispCount = GetData<uint32_t>();
    capability.SetVirtualDispCount(virtualDispCount);
    ScreenInterfaceType type = static_cast<ScreenInterfaceType>(GetData<uint32_t>() % SCREEN_INTERFACE_TYPE_SIZE);
    capability.SetType(type);
    std::string name(STRING_LEN, GetData<char>());
    capability.SetName(name);
    bool supportWriteBack = GetData<bool>();
    capability.SetSupportWriteBack(supportWriteBack);
    std::vector<RSScreenProps> props;
    InitProps(props);
    capability.SetProps(props);
}

bool DoMarshalling()
{
    uint32_t phyWidth = GetData<uint32_t>();
    uint32_t phyHeight = GetData<uint32_t>();
    uint32_t supportLayers = GetData<uint32_t>();
    uint32_t virtualDispCount = GetData<uint32_t>();
    ScreenInterfaceType type = static_cast<ScreenInterfaceType>(GetData<uint32_t>() % SCREEN_INTERFACE_TYPE_SIZE);
    bool supportWriteBack = GetData<bool>();
    std::string name(STRING_LEN, GetData<char>());
    std::vector<RSScreenProps> props;
    InitProps(props);
    RSScreenCapability* capability = new RSScreenCapability(
        name, type, phyWidth, phyHeight, supportLayers, virtualDispCount, supportWriteBack, props);
    Parcel parcel;
    capability->Marshalling(parcel);
    return true;
}

bool DoUnmarshalling()
{
    RSScreenCapability capability;
    Parcel parcel;
    InitRSScreenCapabilityAndParcel(capability, parcel);
    capability.Marshalling(parcel);
    (void)capability.Unmarshalling(parcel);
    return true;
}

bool DoSetName()
{
    RSScreenCapability capability;
    std::string name(STRING_LEN, GetData<char>());
    capability.SetName(name);
    capability.GetName();
    return true;
}

bool DoSetType()
{
    RSScreenCapability capability;
    ScreenInterfaceType type = GetData<ScreenInterfaceType>();
    capability.SetType(type);
    capability.GetType();
    return true;
}

bool DoSetPhyWidth()
{
    RSScreenCapability capability;
    uint32_t phyWidth = GetData<uint32_t>();
    capability.SetPhyWidth(phyWidth);
    capability.GetPhyWidth();
    return true;
}

bool DoSetPhyHeight()
{
    RSScreenCapability capability;
    uint32_t phyHeight = GetData<uint32_t>();
    capability.SetPhyHeight(phyHeight);
    capability.GetPhyHeight();
    return true;
}

bool DoSetSupportLayers()
{
    RSScreenCapability capability;
    uint32_t supportLayers = GetData<uint32_t>();
    capability.SetSupportLayers(supportLayers);
    capability.GetSupportLayers();
    return true;
}

bool DoSetVirtualDispCount()
{
    RSScreenCapability capability;
    uint32_t virtualDispCount = GetData<uint32_t>();
    capability.SetVirtualDispCount(virtualDispCount);
    capability.GetVirtualDispCount();
    return true;
}

bool DoSetSupportWriteBack()
{
    RSScreenCapability capability;
    bool supportWriteBack = GetData<bool>();
    capability.SetSupportWriteBack(supportWriteBack);
    capability.GetSupportWriteBack();
    return true;
}

bool DoSetProps()
{
    RSScreenCapability capability;
    std::vector<RSScreenProps> props;
    InitProps(props);
    capability.SetProps(props);
    capability.GetProps();
    return true;
}

bool DoWriteVector()
{
    RSScreenCapability capability;
    std::vector<RSScreenProps> props;
    InitProps(props);
    Parcel parcel;
    capability.WriteVector(props, parcel);
    return true;
}

bool DoReadVector()
{
    RSScreenCapability capability;
    std::vector<RSScreenProps> props;
    InitProps(props);
    Parcel parcel;
    capability.WriteVector(props, parcel);
    uint32_t unmarPropCount = static_cast<uint32_t>(parcel.GetDataSize());
    std::vector<RSScreenProps> unmarProps;
    RSScreenCapability::ReadVector(unmarProps, unmarPropCount, parcel);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return 0;
    }
    /* Run your code on data */
    using FunctionPtr = bool (*)();
    std::vector<FunctionPtr> funcVector = {
        OHOS::Rosen::DoMarshalling,
        OHOS::Rosen::DoUnmarshalling,
        OHOS::Rosen::DoSetName,
        OHOS::Rosen::DoSetType,
        OHOS::Rosen::DoSetPhyWidth,
        OHOS::Rosen::DoSetPhyHeight,
        OHOS::Rosen::DoSetSupportLayers,
        OHOS::Rosen::DoSetVirtualDispCount,
        OHOS::Rosen::DoSetSupportWriteBack,
        OHOS::Rosen::DoSetProps,
        OHOS::Rosen::DoWriteVector,
        OHOS::Rosen::DoReadVector
    };

    uint8_t pos = OHOS::Rosen::GetData<uint8_t>() % funcVector.size();
    funcVector[pos]();
    return 0;
}