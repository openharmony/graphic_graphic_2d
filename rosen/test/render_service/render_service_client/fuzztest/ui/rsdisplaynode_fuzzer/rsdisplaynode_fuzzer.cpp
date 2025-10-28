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

#include "rsdisplaynode_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "ui/rs_display_node.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;

/*
 * describe: get data from outside untrusted data(DATA) which size is according to sizeof(T)
 * tips: only support basic type
 */
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

inline RSDisplayNodeConfig GetRSDisplayNodeConfigFromData()
{
    uint64_t screenId = GetData<uint64_t>();
    bool isMirrored = GetData<bool>();
    NodeId mirrorNodeId = GetData<uint64_t>();
    bool isSync = GetData<bool>();

    RSDisplayNodeConfig config = { screenId, isMirrored, mirrorNodeId, isSync };
    return config;
}
} // namespace

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

bool DoCreate(const uint8_t* data, size_t size)
{
    // test
    RSDisplayNodeConfig config = GetRSDisplayNodeConfigFromData();
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    return true;
}

bool DoMarshalling(const uint8_t* data, size_t size)
{
    // test
    RSDisplayNodeConfig config = GetRSDisplayNodeConfigFromData();
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    Parcel parcel;
    displayNode->Marshalling(parcel);
    return true;
}

bool DoUnmarshalling(const uint8_t* data, size_t size)
{
    // test
    RSDisplayNodeConfig config = GetRSDisplayNodeConfigFromData();
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    Parcel parcel;
    displayNode->Unmarshalling(parcel);
    return true;
}

bool DoSetScreenId(const uint8_t* data, size_t size)
{
    // test
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    uint64_t screenId = GetData<uint64_t>();
    displayNode->SetScreenId(screenId);
    return true;
}

bool DoSetSecurityDisplay(const uint8_t* data, size_t size)
{
    // test
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    bool isSecurityDisplay = GetData<bool>();
    displayNode->SetSecurityDisplay(isSecurityDisplay);
    return true;
}

bool DoSetScreenRotation(const uint8_t* data, size_t size)
{
    // test
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    uint32_t rotation = GetData<uint32_t>();
    displayNode->SetScreenRotation(rotation);
    return true;
}

bool DoSetDisplayNodeMirrorConfig(const uint8_t* data, size_t size)
{
    // test
    RSDisplayNodeConfig config = GetRSDisplayNodeConfigFromData();
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    RSDisplayNodeConfig config2 = GetRSDisplayNodeConfigFromData();
    displayNode->SetDisplayNodeMirrorConfig(config2);
    return true;
}

bool DoSetBootAnimation(const uint8_t* data, size_t size)
{
    // test
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    bool isBootAnimation = GetData<bool>();
    displayNode->SetBootAnimation(isBootAnimation);
    return true;
}

bool DoCreateNode(const uint8_t* data, size_t size)
{
    // test
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    RSDisplayNodeConfig config2;
    NodeId nodeId = GetData<NodeId>();
    displayNode->CreateNode(config2, nodeId);
    return true;
}

bool DoSetVirtualScreenMuteStatus(const uint8_t* data, size_t size)
{
    // test
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    bool muteStatus = GetData<bool>();
    displayNode->SetVirtualScreenMuteStatus(muteStatus);
    
    return true;
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
    OHOS::Rosen::DoCreate(data, size);
    OHOS::Rosen::DoMarshalling(data, size);
    OHOS::Rosen::DoUnmarshalling(data, size);
    OHOS::Rosen::DoSetScreenId(data, size);
    OHOS::Rosen::DoSetSecurityDisplay(data, size);
    OHOS::Rosen::DoSetScreenRotation(data, size);
    OHOS::Rosen::DoSetDisplayNodeMirrorConfig(data, size);
    OHOS::Rosen::DoSetBootAnimation(data, size);
    OHOS::Rosen::DoCreateNode(data, size);
    OHOS::Rosen::DoSetVirtualScreenMuteStatus(data, size);
    return 0;
}