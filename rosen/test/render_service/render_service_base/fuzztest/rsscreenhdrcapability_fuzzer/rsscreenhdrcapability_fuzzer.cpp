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

#include "rsscreenhdrcapability_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "screen_manager/rs_screen_hdr_capability.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t SCREEN_HDR_FORMAT_SIZE = 8;
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
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

void InitRSScreenHDRCapabilityAndParcel(RSScreenHDRCapability& capability, Parcel& parcel)
{
    float maxLum = GetData<float>();
    capability.SetMaxLum(maxLum);

    float minLum = GetData<float>();
    capability.SetMinLum(minLum);

    float maxAverageLum = GetData<float>();
    capability.SetMaxAverageLum(maxAverageLum);

    ScreenHDRFormat screenHDRFormat = static_cast<ScreenHDRFormat>(GetData<uint32_t>() % SCREEN_HDR_FORMAT_SIZE);
    std::vector<ScreenHDRFormat> hdrFormats = { screenHDRFormat };
    capability.SetHdrFormats(hdrFormats);
}

bool DoMarshalling()
{
    RSScreenHDRCapability capability;
    Parcel parcel;
    InitRSScreenHDRCapabilityAndParcel(capability, parcel);
    capability.Marshalling(parcel);
    return true;
}

bool DoUnmarshalling()
{
    RSScreenHDRCapability capability;
    Parcel parcel;
    InitRSScreenHDRCapabilityAndParcel(capability, parcel);
    capability.Marshalling(parcel);
    capability.Unmarshalling(parcel);
    return true;
}

bool DoSetMaxLum()
{
    RSScreenHDRCapability capability;
    float maxLum = GetData<float>();
    capability.SetMaxLum(maxLum);
    capability.GetMaxLum();
    return true;
}

bool DoSetMinLum()
{
    RSScreenHDRCapability capability;
    float minLum = GetData<float>();
    capability.SetMinLum(minLum);
    capability.GetMinLum();
    return true;
}

bool DoSetMaxAverageLum()
{
    RSScreenHDRCapability capability;
    float maxAverageLum = GetData<float>();
    capability.SetMaxAverageLum(maxAverageLum);
    capability.GetMaxAverageLum();
    return true;
}

bool DoSetHdrFormats()
{
    RSScreenHDRCapability capability;
    ScreenHDRFormat screenHDRFormat = static_cast<ScreenHDRFormat>(GetData<uint32_t>() % SCREEN_HDR_FORMAT_SIZE);
    std::vector<ScreenHDRFormat> hdrFormats = { screenHDRFormat };
    capability.SetHdrFormats(hdrFormats);
    capability.GetHdrFormats();
    return true;
}

bool DoWriteVector()
{
    RSScreenHDRCapability capability;
    ScreenHDRFormat screenHDRFormat = static_cast<ScreenHDRFormat>(GetData<uint32_t>() % SCREEN_HDR_FORMAT_SIZE);
    std::vector<ScreenHDRFormat> hdrFormats = { screenHDRFormat };
    Parcel parcel;
    capability.WriteVector(hdrFormats, parcel);
    return true;
}

bool DoReadVector()
{
    RSScreenHDRCapability capability;
    ScreenHDRFormat screenHDRFormat = static_cast<ScreenHDRFormat>(GetData<uint32_t>() % SCREEN_HDR_FORMAT_SIZE);
    std::vector<ScreenHDRFormat> hdrFormats = { screenHDRFormat };
    Parcel parcel;
    capability.WriteVector(hdrFormats, parcel);
    std::vector<ScreenHDRFormat> unmarFormats;
    RSScreenHDRCapability::ReadVector(unmarFormats, parcel);
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
        OHOS::Rosen::DoSetMaxLum,
        OHOS::Rosen::DoSetMinLum,
        OHOS::Rosen::DoSetMaxAverageLum,
        OHOS::Rosen::DoSetHdrFormats,
        OHOS::Rosen::DoWriteVector,
        OHOS::Rosen::DoReadVector
    };
    uint8_t pos = OHOS::Rosen::GetData<uint8_t>() % funcVector.size();
    funcVector[pos]();
    return 0;
}
