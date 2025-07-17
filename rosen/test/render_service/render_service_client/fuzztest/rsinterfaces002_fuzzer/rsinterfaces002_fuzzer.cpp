/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
const uint8_t DO_CREATE_VIRTUAL_SCREEN = 0;
const uint8_t DO_SET_VIRTUAL_SCREEN_RESOLUTION = 1;
const uint8_t DO_SET_VIRTUAL_SCREEN_SURFACE = 2;
const uint8_t DO_SET_VIRTUAL_SCREEN_BLACK_LIST = 3;
const uint8_t DO_ADD_VIRTUAL_SCREEN_BLACK_LIST = 4;
const uint8_t DO_REMOVE_VIRTUAL_SCREEN_BLACK_LIST = 5;
const uint8_t DO_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST = 6;
const uint8_t DO_REMOVE_VIRTUAL_SCREEN = 7;
const uint8_t DO_GET_VIRTUAL_SCREEN_RESOLUTION = 8;
const uint8_t DO_RESIZE_VIRTUAL_SCREEN = 9;
const uint8_t DO_SET_VIRTUAL_SCREEN_USING_STATUS = 10;
const uint8_t DO_SET_VIRTUAL_SCREEN_REFRESH_RATE = 11;
const uint8_t DO_SET_VIRTUAL_SCREEN_STATUS = 12;
const uint8_t DO_SET_VIRTUAL_SCREEN_TYPE_BLACK_LIST = 13;
const uint8_t TARGET_SIZE = 14;

const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
constexpr size_t STR_LEN = 10;

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

/*
 * get a string from g_data
 */
std::string GetStringFromData(int strlen)
{
    if (strlen <= 0) {
        return "fuzz";
    }
    char cstr[strlen];
    cstr[strlen - 1] = '\0';
    for (int i = 0; i < strlen - 1; i++) {
        char tmp = GetData<char>();
        if (tmp == '\0') {
            tmp = '1';
        }
        cstr[i] = tmp;
    }
    std::string str(cstr);
    return str;
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

void DoCreateVirtualScreen()
{
    std::string name = GetStringFromData(STR_LEN);
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    auto cSurface = IConsumerSurface::Create();
    auto bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    uint64_t mirrorId = GetData<uint64_t>();
    int32_t flags = GetData<int32_t>();
    std::vector<NodeId> whiteListVector;
    uint8_t listSize = GetData<uint8_t>();
    for (int i = 0; i < listSize; i++) {
        NodeId nodeId = GetData<NodeId>();
        whiteListVector.push_back(nodeId);
    }
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.CreateVirtualScreen(name, width, height, pSurface, mirrorId, flags, whiteListVector);
}

void DoSetVirtualScreenResolution()
{
    ScreenId id = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetPhysicalScreenResolution(id, width, height);
}

void DoSetVirtualScreenSurface()
{
    ScreenId id = GetData<ScreenId>();
    sptr<Surface> surface;
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetVirtualScreenSurface(id, surface);
}

void DoSetVirtualScreenBlackList()
{
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    uint8_t blackListVectorSize = GetData<uint8_t>();
    for (size_t i = 0; i < blackListVectorSize; i++) {
        blackListVector.push_back(GetData<NodeId>());
    }
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetVirtualScreenBlackList(id, blackListVector);
}

void DoAddVirtualScreenBlackList()
{
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    uint8_t blackListVectorSize = GetData<uint8_t>();
    for (size_t i = 0; i < blackListVectorSize; i++) {
        blackListVector.push_back(GetData<NodeId>());
    }
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.AddVirtualScreenBlackList(id, blackListVector);
}

void DoRemoveVirtualScreenBlackList()
{
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    uint8_t blackListVectorSize = GetData<uint8_t>();
    for (size_t i = 0; i < blackListVectorSize; i++) {
        blackListVector.push_back(GetData<NodeId>());
    }
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.RemoveVirtualScreenBlackList(id, blackListVector);
}

void DoSetVirtualScreenSecurityExemptionList()
{
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> securityExemptionList;
    uint8_t securityExemptionListSize = GetData<uint8_t>();
    for (size_t i = 0; i < securityExemptionListSize; i++) {
        securityExemptionList.push_back(GetData<NodeId>());
    }
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
}

void DoRemoveVirtualScreen()
{
    ScreenId id = GetData<ScreenId>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.RemoveVirtualScreen(id);
}

void DoGetVirtualScreenResolution()
{
    ScreenId id = GetData<ScreenId>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetVirtualScreenResolution(id);
}

void DoResizeVirtualScreen()
{
    uint64_t id = GetData<uint64_t>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.ResizeVirtualScreen(id, width, height);
}

void DoSetVirtualScreenUsingStatus()
{
    bool isVirtualScreenUsingStatus = GetData<bool>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetVirtualScreenUsingStatus(isVirtualScreenUsingStatus);
}

void DoSetVirtualScreenRefreshRate()
{
    ScreenId id = GetData<ScreenId>();
    uint32_t maxRefreshRate = GetData<uint32_t>();
    uint32_t actualRefreshRate = GetData<uint32_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetVirtualScreenRefreshRate(id, maxRefreshRate, actualRefreshRate);
}

void DoSetVirtualScreenStatus()
{
    ScreenId id = GetData<ScreenId>();
    VirtualScreenStatus screenStatus =
        static_cast<VirtualScreenStatus>(GetData<uint8_t>() % VIRTUAL_SCREEN_STATUS_SIZE);
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetVirtualScreenStatus(id, screenStatus);
}

void DoSetVirtualScreenTypeBlackList()
{
    ScreenId id = GetData<ScreenId>();
    uint8_t type = GetData<uint8_t>();
    std::vector<NodeType> typeBlackList = {};
    typeBlackList.push_back(type);
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetVirtualScreenTypeBlackList(id, typeBlackList);
}
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
