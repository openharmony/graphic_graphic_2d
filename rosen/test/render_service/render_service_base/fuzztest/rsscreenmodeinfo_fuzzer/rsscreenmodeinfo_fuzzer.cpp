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

#include "rsscreenmodeinfo_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "screen_manager/rs_screen_mode_info.h"

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

    // test
    RSScreenModeInfo screenModeInfo;
    Parcel parcel;
    screenModeInfo.Marshalling(parcel);
    return true;
}

bool DoUnmarshalling(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    RSScreenModeInfo screenModeInfo;
    Parcel parcel;
    screenModeInfo.Unmarshalling(parcel);
    return true;
}

bool DoGetScreenWidth(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    RSScreenModeInfo screenModeInfo;
    screenModeInfo.GetScreenWidth();
    return true;
}

bool DoGetScreenHeight(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    RSScreenModeInfo screenModeInfo;
    screenModeInfo.GetScreenHeight();
    return true;
}

bool DoGetScreenRefreshRate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    RSScreenModeInfo screenModeInfo;
    screenModeInfo.GetScreenRefreshRate();
    return true;
}

bool DoGetScreenModeId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    RSScreenModeInfo screenModeInfo;
    screenModeInfo.GetScreenModeId();
    return true;
}

bool DoSetScreenWidth(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    RSScreenModeInfo screenModeInfo;
    int32_t width = GetData<int32_t>();
    screenModeInfo.SetScreenWidth(width);
    return true;
}

bool DoSetScreenHeight(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    RSScreenModeInfo screenModeInfo;
    int32_t height = GetData<int32_t>();
    screenModeInfo.SetScreenHeight(height);
    return true;
}

bool DoSetScreenRefreshRate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    RSScreenModeInfo screenModeInfo;
    uint32_t refreshRate = GetData<uint32_t>();
    screenModeInfo.SetScreenRefreshRate(refreshRate);
    return true;
}

bool DoSetScreenModeId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    RSScreenModeInfo screenModeInfo;
    int32_t id = GetData<int32_t>();
    screenModeInfo.SetScreenModeId(id);
    return true;
}

bool DoRSScreenModeInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    RSScreenModeInfo screenModeInfo;
    RSScreenModeInfo screenModeInfo2 = screenModeInfo;
    RSScreenModeInfo screenModeInfo3(screenModeInfo2);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // initialize
    OHOS::Rosen::g_data = data;
    OHOS::Rosen::g_size = size;
    OHOS::Rosen::g_pos = 0;

    /* Run your code on data */
    OHOS::Rosen::DoMarshalling(data, size);
    OHOS::Rosen::DoUnmarshalling(data, size);
    OHOS::Rosen::DoGetScreenWidth(data, size);
    OHOS::Rosen::DoGetScreenHeight(data, size);
    OHOS::Rosen::DoGetScreenRefreshRate(data, size);
    OHOS::Rosen::DoGetScreenModeId(data, size);
    OHOS::Rosen::DoSetScreenWidth(data, size);
    OHOS::Rosen::DoSetScreenHeight(data, size);
    OHOS::Rosen::DoSetScreenRefreshRate(data, size);
    OHOS::Rosen::DoSetScreenModeId(data, size);
    OHOS::Rosen::DoRSScreenModeInfo(data, size);
    return 0;
}

