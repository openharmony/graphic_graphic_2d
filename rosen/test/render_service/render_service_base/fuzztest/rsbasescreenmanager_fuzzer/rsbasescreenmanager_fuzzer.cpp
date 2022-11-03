/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rsbasescreenmanager_fuzzer.h"

#include <securec.h>

#include "screen_manager/rs_screen_capability.h"
#include "screen_manager/rs_screen_hdr_capability.h"
#include "screen_manager/rs_screen_props.h"

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

bool RSScreenCapabilityFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSScreenCapability screenCapability;
    std::string capName(STRING_LEN, GetData<char>());
    RSScreenProps prop = RSScreenProps(capName, GetData<uint32_t>(), GetData<uint64_t>());
    std::vector<RSScreenProps> props = {prop};
    screenCapability.SetName(capName);
    screenCapability.SetType(GetData<ScreenInterfaceType>());
    screenCapability.SetPhyWidth(GetData<uint32_t>());
    screenCapability.SetPhyHeight(GetData<uint32_t>());
    screenCapability.SetSupportLayers(GetData<uint32_t>());
    screenCapability.SetVirtualDispCount(GetData<uint32_t>());
    screenCapability.SetSupportWriteBack(GetData<bool>());
    screenCapability.SetProps(props);
    (void)screenCapability.GetName();
    (void)screenCapability.GetType();
    (void)screenCapability.GetPhyWidth();
    (void)screenCapability.GetPhyHeight();
    (void)screenCapability.GetSupportLayers();
    (void)screenCapability.GetVirtualDispCount();
    (void)screenCapability.GetSupportWriteBack();
    (void)screenCapability.GetProps();

    return true;
}

bool RSScreenHdrCapabilityFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSScreenHDRCapability hdrScreenCapability;
    std::vector<ScreenHDRFormat> hdrFormats = { GetData<ScreenHDRFormat>() };
    hdrScreenCapability.SetMaxLum(GetData<float>());
    hdrScreenCapability.SetMinLum(GetData<float>());
    hdrScreenCapability.SetMaxAverageLum(GetData<float>());
    hdrScreenCapability.SetHdrFormats(hdrFormats);
    (void)hdrScreenCapability.GetMaxLum();
    (void)hdrScreenCapability.GetMinLum();
    (void)hdrScreenCapability.GetMaxAverageLum();
    (void)hdrScreenCapability.GetHdrFormats();

    return true;
}

bool RSScreenPropsFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSScreenProps props;
    std::string propName(STRING_LEN, GetData<char>());
    props.SetPropertyName(propName);
    props.SetPropId(GetData<uint32_t>());
    props.SetValue(GetData<uint64_t>());
    (void)props.GetPropertyName();
    (void)props.GetPropId();
    (void)props.GetValue();

    return true;
}
} // Rosen
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::Rosen::RSScreenCapabilityFuzzTest(data, size);
    OHOS::Rosen::RSScreenHdrCapabilityFuzzTest(data, size);
    OHOS::Rosen::RSScreenPropsFuzzTest(data, size);
    return 0;
}

