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

#include "virtualScreenList_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_ADD_VIRTUAL_SCREEN_BLACKLIST = 0;
const uint8_t DO_REMOVE_VIRTUAL_SCREEN_BLACKLIST = 1;
const uint8_t DO_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST = 2;
const uint8_t DO_SET_VIRTUAL_SCREEN_TYPE_BLACKLIST = 3;
const uint8_t DO_ADD_VIRTUAL_SCREEN_WHITELIST = 4;
const uint8_t DO_REMOVE_VIRTUAL_SCREEN_WHITELIST = 5;
const uint8_t TARGET_SIZE = 6;

void DoAddVirtualScreenBlackList(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    std::vector<NodeId> blackList;
    uint8_t blackListSize = fdp.ConsumeIntegral<uint8_t>();
    for (uint8_t i = 0; i < blackListSize; i++) {
        blackList.push_back(fdp.ConsumeIntegral<uint64_t>());
    }
    g_rsInterfaces->AddVirtualScreenBlackList(id, blackList);
}

void DoRemoveVirtualScreenBlackList(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    std::vector<NodeId> blackList;
    uint8_t blackListSize = fdp.ConsumeIntegral<uint8_t>();
    for (uint8_t i = 0; i < blackListSize; i++) {
        blackList.push_back(fdp.ConsumeIntegral<uint64_t>());
    }
    g_rsInterfaces->RemoveVirtualScreenBlackList(id, blackList);
}

void DoSetVirtualScreenSecurityExemptionList(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    std::vector<NodeId> securityExemptionList;
    uint8_t securityExemptionListSize = fdp.ConsumeIntegral<uint8_t>();
    for (uint8_t i = 0; i < securityExemptionListSize; i++) {
        securityExemptionList.push_back(fdp.ConsumeIntegral<uint64_t>());
    }
    g_rsInterfaces->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
}

void DoSetVirtualScreenTypeBlackList(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    std::vector<NodeType> typeBlackList;
    uint8_t typeBlackListSize = fdp.ConsumeIntegral<uint8_t>();
    for (uint8_t i = 0; i < typeBlackListSize; i++) {
        typeBlackList.push_back(static_cast<NodeType>(fdp.ConsumeIntegral<uint8_t>()));
    }
    g_rsInterfaces->SetVirtualScreenTypeBlackList(id, typeBlackList);
}

void DoAddVirtualScreenWhiteList(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    std::vector<NodeId> whiteList;
    uint8_t whiteListSize = fdp.ConsumeIntegral<uint8_t>();
    for (uint8_t i = 0; i < whiteListSize; i++) {
        whiteList.push_back(fdp.ConsumeIntegral<uint64_t>());
    }
    g_rsInterfaces->AddVirtualScreenWhiteList(id, whiteList);
}

void DoRemoveVirtualScreenWhiteList(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    std::vector<NodeId> whiteList;
    uint8_t whiteListSize = fdp.ConsumeIntegral<uint8_t>();
    for (uint8_t i = 0; i < whiteListSize; i++) {
        whiteList.push_back(fdp.ConsumeIntegral<uint64_t>());
    }
    g_rsInterfaces->RemoveVirtualScreenWhiteList(id, whiteList);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Pre-initialize RSInterfaces singleton to avoid runtime initialization overhead
    OHOS::Rosen::g_rsInterfaces = &OHOS::Rosen::RSInterfaces::GetInstance();
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_rsInterfaces == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_ADD_VIRTUAL_SCREEN_BLACKLIST:
            OHOS::Rosen::DoAddVirtualScreenBlackList(fdp);
            break;
        case OHOS::Rosen::DO_REMOVE_VIRTUAL_SCREEN_BLACKLIST:
            OHOS::Rosen::DoRemoveVirtualScreenBlackList(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST:
            OHOS::Rosen::DoSetVirtualScreenSecurityExemptionList(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_TYPE_BLACKLIST:
            OHOS::Rosen::DoSetVirtualScreenTypeBlackList(fdp);
            break;
        case OHOS::Rosen::DO_ADD_VIRTUAL_SCREEN_WHITELIST:
            OHOS::Rosen::DoAddVirtualScreenWhiteList(fdp);
            break;
        case OHOS::Rosen::DO_REMOVE_VIRTUAL_SCREEN_WHITELIST:
            OHOS::Rosen::DoRemoveVirtualScreenWhiteList(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
