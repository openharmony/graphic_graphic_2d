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

#include "rsinterfaces002_fuzzer.h"

#include <securec.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_CREATE_VIRTUAL_SCREEN = 0,
const uint8_t DO_SET_VIRTUAL_SCREEN_RESOLUTION = 1,
const uint8_t DO_SET_VIRTUAL_SCREEN_SURFACE = 2,
const uint8_t DO_SET_VIRTUAL_SCREEN_BLACK_LIST = 3,
const uint8_t DO_ADD_VIRTUAL_SCREEN_BLACK_LIST = 4,
const uint8_t DO_REMOVE_VIRTUAL_SCREEN_BLACK_LIST = 5,
const uint8_t DO_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST = 6,
const uint8_t DO_REMOVE_VIRTUAL_SCREEN = 7,
const uint8_t DO_GET_VIRTUAL_SCREEN_RESOLUTION = 8,
const uint8_t DO_RESIZE_VIRTUAL_SCREEN = 9,
const uint8_t DO_SET_VIRTUAL_SCREEN_USING_STATUS = 10,
const uint8_t DO_SET_VIRTUAL_SCREEN_REFRESH_RATE = 11,
const uint8_t DO_SET_VIRTUAL_SCREEN_STATUS = 12,
const uint8_t DO_SET_VIRTUAL_SCREEN_TYPE_BLACK_LIST = 13,
const uint8_t TARGET_SIZE = 14;

const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

template<>
std::string GetData()
{
    size_t objectSize = GetData<uint8_t>();
    std::string object(objectSize, '\0');
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    object.assign(reinterpret_cast<const char*>(DATA + g_pos), objectSize);
    g_pos += objectSize;
    return object;
}

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    DATA = data;
    g_size = size;
    g_pos = 0;
    return true;
}
} // namespace

namespace Mock {

} // namespace Mock

void DoCreateVirtualScreen()
{}

void DoSetVirtualScreenResolution()
{}

void DoSetVirtualScreenSurface()
{}

void DoSetVirtualScreenBlackList()
{}

void DoAddVirtualScreenBlackList()
{}

void DoRemoveVirtualScreenBlackList()
{}

void DoSetVirtualScreenSecurityExemptionList()
{}

void DoRemoveVirtualScreen()
{}

void DoGetVirtualScreenResolution()
{}

void DoResizeVirtualScreen()
{}

void DoSetVirtualScreenUsingStatus()
{}

void DoSetVirtualScreenRefreshRate()
{}

void DoSetVirtualScreenStatus()
{}

void DoSetVirtualScreenTypeBlackList()
{}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    /* Run your code on data */
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_CREATE_VIRTUAL_SCREEN:
            OHOS::Rosen::DoCreateVirtualScreen();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_RESOLUTION:
            OHOS::Rosen::DoSetVirtualScreenResolution();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_SURFACE:
            OHOS::Rosen::DoSetVirtualScreenSurface();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_BLACK_LIST:
            OHOS::Rosen::DoSetVirtualScreenBlackList();
            break;
        case OHOS::Rosen::DO_ADD_VIRTUAL_SCREEN_BLACK_LIST:
            OHOS::Rosen::DoAddVirtualScreenBlackList();
            break;
        case OHOS::Rosen::DO_REMOVE_VIRTUAL_SCREEN_BLACK_LIST:
            OHOS::Rosen::DoRemoveVirtualScreenBlackList();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST:
            OHOS::Rosen::DoSetVirtualScreenSecurityExemptionList();
            break;
        case OHOS::Rosen::DO_REMOVE_VIRTUAL_SCREEN:
            OHOS::Rosen::DoRemoveVirtualScreen();
            break;
        case OHOS::Rosen::DO_GET_VIRTUAL_SCREEN_RESOLUTION:
            OHOS::Rosen::DoGetVirtualScreenResolution();
            break;
        case OHOS::Rosen::DO_RESIZE_VIRTUAL_SCREEN:
            OHOS::Rosen::DoResizeVirtualScreen();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_USING_STATUS:
            OHOS::Rosen::DoSetVirtualScreenUsingStatus();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_REFRESH_RATE:
            OHOS::Rosen::DoSetVirtualScreenRefreshRate();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_STATUS:
            OHOS::Rosen::DoSetVirtualScreenStatus();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_TYPE_BLACK_LIST:
            OHOS::Rosen::DoSetVirtualScreenTypeBlackList();
            break;
        default:
            return -1;
    }
    return 0;
}
