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

#include "rshgmconfigdata_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "transaction/rs_hgm_config_data.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
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
bool DoUnmarshalling(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    static_cast<void>(RSHgmConfigData::Unmarshalling(parcel));
    return true;
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
    RSHgmConfigData rsHgmConfigData;
    Parcel parcel;
    rsHgmConfigData.Marshalling(parcel);
    return true;
}
bool DoAddAnimDynamicItem(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    RSHgmConfigData rsHgmConfigData;
    AnimDynamicItem item;
    item.minSpeed = GetData<int32_t>();
    item.maxSpeed = GetData<int32_t>();
    item.preferredFps = GetData<int32_t>();
    rsHgmConfigData.AddAnimDynamicItem(item);
    return true;
}
bool DoGetSet(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    RSHgmConfigData rsHgmConfigData;
    float ppi = GetData<float>();
    float xDpi = GetData<float>();
    float yDpi = GetData<float>();

    rsHgmConfigData.GetConfigData();
    rsHgmConfigData.GetPpi();
    rsHgmConfigData.GetXDpi();
    rsHgmConfigData.GetYDpi();

    rsHgmConfigData.SetPpi(ppi);
    rsHgmConfigData.SetXDpi(xDpi);
    rsHgmConfigData.SetYDpi(yDpi);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoUnmarshalling(data, size);      // Unmarshalling
    OHOS::Rosen::DoMarshalling(data, size);        // Marshalling
    OHOS::Rosen::DoAddAnimDynamicItem(data, size); // AddAnimDynamicItem
    OHOS::Rosen::DoGetSet(data, size);             // GetSet
    return 0;
}
