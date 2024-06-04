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
    RSScreenHDRCapability capability;
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
    RSScreenHDRCapability capability;
    Parcel parcel;
    capability.Unmarshalling(parcel);
    return true;
}

bool DoGetMaxLum(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenHDRCapability capability;
    capability.GetMaxLum();
    return true;
}

bool DoGetMinLum(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenHDRCapability capability;
    capability.GetMinLum();
    return true;
}

bool DoGetMaxAverageLum(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenHDRCapability capability;
    capability.GetMaxAverageLum();
    return true;
}

bool DoGetHdrFormats(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenHDRCapability capability;
    capability.GetHdrFormats();
    return true;
}

bool DoSetMaxLum(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenHDRCapability capability;
    float maxLum = GetData<float>();
    capability.SetMaxLum(maxLum);
    return true;
}

bool DoSetMinLum(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenHDRCapability capability;
    float minLum = GetData<float>();
    capability.SetMinLum(minLum);
    return true;
}

bool DoSetMaxAverageLum(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenHDRCapability capability;
    float maxAverageLum = GetData<float>();
    capability.SetMaxAverageLum(maxAverageLum);
    return true;
}

bool DoSetHdrFormats(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSScreenHDRCapability capability;
    std::vector<ScreenHDRFormat> hdrFormats = { GetData<ScreenHDRFormat>() };
    capability.SetHdrFormats(hdrFormats);
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
    RSScreenHDRCapability capability;
    std::vector<ScreenHDRFormat> hdrFormats = { GetData<ScreenHDRFormat>() };
    Parcel parcel;
    capability.WriteVector(hdrFormats, parcel);
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
    std::vector<ScreenHDRFormat> unmarFormats = { GetData<ScreenHDRFormat>() };
    Parcel parcel;
    RSScreenHDRCapability::ReadVector(unmarFormats, parcel);
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
    OHOS::Rosen::DoGetMaxLum(data, size);
    OHOS::Rosen::DoGetMinLum(data, size);
    OHOS::Rosen::DoGetMaxAverageLum(data, size);
    OHOS::Rosen::DoGetHdrFormats(data, size);
    OHOS::Rosen::DoSetMaxLum(data, size);
    OHOS::Rosen::DoSetMinLum(data, size);
    OHOS::Rosen::DoSetMaxAverageLum(data, size);
    OHOS::Rosen::DoSetHdrFormats(data, size);
    OHOS::Rosen::DoWriteVector(data, size);
    OHOS::Rosen::DoReadVector(data, size);
    return 0;
}

