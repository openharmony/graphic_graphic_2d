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
const uint8_t DO_MARSHALLING = 0;
const uint8_t DO_UNMARSHALLING = 1;
const uint8_t DO_SET_MAXLUM = 2;
const uint8_t DO_SET_MINLUM = 3;
const uint8_t DO_SET_MAXAVERAGELUM = 4;
const uint8_t DO_SET_HDRFORMATS = 5;
const uint8_t DO_WRITEVECTOR = 6;
const uint8_t DO_READVECTOR = 7;
const uint8_t TARGET_SIZE = 8;
const uint8_t SCREEN_HDR_FORMAT_SIZE = 8;
const float PROPORTION = 0.1f;
namespace {
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
    uint32_t unit32MaxLum = GetData<uint32_t>();
    float maxLum = static_cast<float>(unit32MaxLum) * OHOS::Rosen::PROPORTION;
    capability.SetMaxLum(maxLum);

    uint32_t unit32MinLum = GetData<uint32_t>();
    float minLum = static_cast<float>(unit32MinLum) * OHOS::Rosen::PROPORTION;
    capability.SetMinLum(minLum);

    uint32_t unit32MaxAverageLum = GetData<uint32_t>();
    float maxAverageLum = static_cast<float>(unit32MaxAverageLum) * OHOS::Rosen::PROPORTION;
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
    uint32_t unit32MaxLum = GetData<uint32_t>();
    float maxLum = static_cast<float>(unit32MaxLum) * OHOS::Rosen::PROPORTION;
    capability.SetMaxLum(maxLum);
    capability.GetMaxLum();
    return true;
}

bool DoSetMinLum()
{
    RSScreenHDRCapability capability;
    uint32_t unit32MinLum = GetData<uint32_t>();
    float minLum = static_cast<float>(unit32MinLum) * OHOS::Rosen::PROPORTION;
    capability.SetMinLum(minLum);
    capability.GetMinLum();
    return true;
}

bool DoSetMaxAverageLum()
{
    RSScreenHDRCapability capability;
    uint32_t unit32MaxAverageLum = GetData<uint32_t>();
    float maxAverageLum = static_cast<float>(unit32MaxAverageLum) * OHOS::Rosen::PROPORTION;
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
    ScreenHDRFormat newScreenHDRFormat = static_cast<ScreenHDRFormat>(GetData<uint32_t>() % SCREEN_HDR_FORMAT_SIZE);
    std::vector<ScreenHDRFormat> unmarFormats = { newScreenHDRFormat };
    Parcel parcel;
    capability.WriteVector(hdrFormats, parcel);
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
    uint8_t tarpos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarpos) {
        case OHOS::Rosen::DO_MARSHALLING:
            OHOS::Rosen::DoMarshalling();
            break;
        case OHOS::Rosen::DO_UNMARSHALLING:
            OHOS::Rosen::DoUnmarshalling();
            break;
        case OHOS::Rosen::DO_SET_MAXLUM:
            OHOS::Rosen::DoSetMaxLum();
            break;
        case OHOS::Rosen::DO_SET_MINLUM:
            OHOS::Rosen::DoSetMinLum();
            break;
        case OHOS::Rosen::DO_SET_MAXAVERAGELUM:
            OHOS::Rosen::DoSetMaxAverageLum();
            break;
        case OHOS::Rosen::DO_SET_HDRFORMATS:
            OHOS::Rosen::DoSetHdrFormats();
            break;
        case OHOS::Rosen::DO_WRITEVECTOR:
            OHOS::Rosen::DoWriteVector();
            break;
        case OHOS::Rosen::DO_READVECTOR:
            OHOS::Rosen::DoReadVector();
            break;
        default:
            return 0;
    }
    return 0;
}
