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

bool DoMarshalling(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    Parcel parcel;
    capability.Marshalling(parcel);
    return true;
}

bool DoUnmarshalling(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    Parcel parcel;
    (void)capability.Unmarshalling(parcel);
    return true;
}

bool DoSetName(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    std::string name(STRING_LEN, GetData<char>());
    capability.SetName(name);
    return true;
}

bool DoSetType(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    ScreenInterfaceType type = GetData<ScreenInterfaceType>();
    capability.SetType(type);
    return true;
}

bool DoSetPhyWidth(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    uint32_t phyWidth = GetData<uint32_t>();
    capability.SetPhyWidth(phyWidth);
    return true;
}

bool DoSetPhyHeight(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    uint32_t phyHeight = GetData<uint32_t>();
    capability.SetPhyHeight(phyHeight);
    return true;
}

bool DoSetSupportLayers(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    uint32_t supportLayers = GetData<uint32_t>();
    capability.SetSupportLayers(supportLayers);
    return true;
}

bool DoSetVirtualDispCount(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    uint32_t virtualDispCount = GetData<uint32_t>();
    capability.SetVirtualDispCount(virtualDispCount);
    return true;
}

bool DoSetSupportWriteBack(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    bool supportWriteBack = GetData<bool>();
    capability.SetSupportWriteBack(supportWriteBack);
    return true;
}

bool DoSetProps(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    std::string capName(STRING_LEN, GetData<char>());
    RSScreenProps prop = RSScreenProps(capName, GetData<uint32_t>(), GetData<uint64_t>());
    std::vector<RSScreenProps> props = {prop};
    capability.SetProps(props);
    return true;
}

bool DoGetName(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    capability.GetName();
    return true;
}

bool DoGetType(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    capability.GetType();
    return true;
}

bool DoGetPhyWidth(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    capability.GetPhyWidth();
    return true;
}

bool DoGetPhyHeight(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    capability.GetPhyHeight();
    return true;
}

bool DoGetSupportLayers(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    capability.GetSupportLayers();
    return true;
}

bool DoGetVirtualDispCount(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    capability.GetVirtualDispCount();
    return true;
}

bool DoGetSupportWriteBack(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    capability.GetSupportWriteBack();
    return true;
}

bool DoGetProps(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    capability.GetProps();
    return true;
}

bool DoWriteVector(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenCapability capability;
    std::string capName(STRING_LEN, GetData<char>());
    RSScreenProps prop = RSScreenProps(capName, GetData<uint32_t>(), GetData<uint64_t>());
    std::vector<RSScreenProps> props = {prop};
    Parcel parcel;
    capability.WriteVector(props, parcel);
    return true;
}

bool DoReadVector(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::string capName(STRING_LEN, GetData<char>());
    RSScreenProps prop = RSScreenProps(capName, GetData<uint32_t>(), GetData<uint64_t>());
    std::vector<RSScreenProps> unmarProps = {prop};
    uint32_t unmarPropCount = GetData<uint32_t>();
    Parcel parcel;
    RSScreenCapability::ReadVector(unmarProps, unmarPropCount, parcel);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoMarshalling(data, size);
    OHOS::Rosen::DoUnmarshalling(data, size);
    OHOS::Rosen::DoSetName(data, size);
    OHOS::Rosen::DoSetType(data, size);
    OHOS::Rosen::DoSetPhyWidth(data, size);
    OHOS::Rosen::DoSetPhyHeight(data, size);
    OHOS::Rosen::DoSetSupportLayers(data, size);
    OHOS::Rosen::DoSetVirtualDispCount(data, size);
    OHOS::Rosen::DoSetSupportWriteBack(data, size);
    OHOS::Rosen::DoSetProps(data, size);
    OHOS::Rosen::DoGetName(data, size);
    OHOS::Rosen::DoGetType(data, size);
    OHOS::Rosen::DoGetPhyWidth(data, size);
    OHOS::Rosen::DoGetPhyHeight(data, size);
    OHOS::Rosen::DoGetSupportLayers(data, size);
    OHOS::Rosen::DoGetVirtualDispCount(data, size);
    OHOS::Rosen::DoGetSupportWriteBack(data, size);
    OHOS::Rosen::DoGetProps(data, size);
    OHOS::Rosen::DoWriteVector(data, size);
    OHOS::Rosen::DoReadVector(data, size);
    return 0;
}

