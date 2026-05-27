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

#include "rssurfacenode1_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include <iostream>
#include <securec.h>

#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MAX_FUZZ_IPC_LOOP_COUNT = 100;
constexpr uint32_t MAX_FUZZ_SIMPLE_LOOP_COUNT = 1000;
constexpr uint8_t DO_CREATE = 0;
constexpr uint8_t DO_REMOVE_CHILD = 1;
constexpr uint8_t DO_SET_AND_GET = 2;
constexpr uint8_t DO_SET_BUFFER_AVAILABLE_CALLBACK = 3;
constexpr uint8_t DO_SET_BOUNDS_CHANGED_CALLBACK = 4;
constexpr uint8_t DO_MARSHALLING = 5;
constexpr uint8_t DO_UNMARSHALLING = 6;
constexpr uint8_t DO_UNMARSHALLING_AS_PROXY_NODE = 7;
constexpr uint8_t DO_ATTACH_TO_DISPLAY = 8;
constexpr uint8_t DO_DETACH_AND_SET_HARDWARE_ENABLED = 9;
constexpr uint8_t MAX_CASE_NUM = 10;

std::string ConsumeFixedLengthString(FuzzedDataProvider& fdp, int len)
{
    if (len <= 0) {
        return "fuzz";
    }
    std::string result(len, '\0');
    for (int i = 0; i < len - 1; i++) {
        char tmp = static_cast<char>(fdp.ConsumeIntegral<uint8_t>());
        if (tmp == '\0') {
            tmp = '1';
        }
        result[i] = tmp;
    }
    return result;
}

std::string ConsumeRandomLengthString(FuzzedDataProvider& fdp)
{
    uint8_t len = fdp.ConsumeIntegral<uint8_t>();
    return fdp.ConsumeBytesAsString(len);
}

template<typename T>
T ConsumePOD(FuzzedDataProvider& fdp)
{
    T object {};
    auto bytes = fdp.ConsumeBytes<uint8_t>(sizeof(T));
    if (bytes.size() == sizeof(T)) {
        errno_t ret = memcpy_s(&object, sizeof(T), bytes.data(), sizeof(T));
        if (ret != EOK) {
            return {};
        }
    }
    return object;
}

inline RSSurfaceNodeConfig GetRSSurfaceNodeConfigFromData(FuzzedDataProvider& fdp)
{
    constexpr int len = 10;
    std::string surfaceNodeName = ConsumeFixedLengthString(fdp, len);
    bool isTextureExportNode = fdp.ConsumeBool();
    SurfaceId surfaceId = fdp.ConsumeIntegral<uint64_t>();
    bool isSync = fdp.ConsumeBool();
    SurfaceWindowType surfaceWindowType = static_cast<SurfaceWindowType>(fdp.ConsumeIntegral<uint8_t>());
    RSSurfaceNodeConfig config = { surfaceNodeName, nullptr, isTextureExportNode,
        surfaceId, isSync, surfaceWindowType, nullptr };
    return config;
}
} // namespace

bool DoCreate(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode1 = RSSurfaceNode::Create(config);
    if (surfaceNode1 == nullptr) {
        return false;
    }
    RSSurfaceNodeType type = ConsumePOD<RSSurfaceNodeType>(fdp);
    RSSurfaceNode::SharedPtr surfaceNode2 = RSSurfaceNode::Create(config, type);
    if (surfaceNode2 == nullptr) {
        return false;
    }
    return true;
}

bool DoRemoveChild(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    RSSurfaceNodeConfig config1;
    RSSurfaceNode::SharedPtr child = RSSurfaceNode::Create(config1);
    if (child == nullptr) {
        return false;
    }
    surfaceNode->RemoveChild(child);
    return true;
}

bool DoSetAndGet(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isSecurityLayer = fdp.ConsumeBool();
    surfaceNode->SetSecurityLayer(isSecurityLayer);
    surfaceNode->GetSecurityLayer();
    bool isSkipLayer = fdp.ConsumeBool();
    surfaceNode->SetSkipLayer(isSkipLayer);
    surfaceNode->GetSkipLayer();
    bool isSnapshotSkipLayer = fdp.ConsumeBool();
    surfaceNode->SetSnapshotSkipLayer(isSnapshotSkipLayer);
    surfaceNode->GetSnapshotSkipLayer();
    bool hasFingerprint = fdp.ConsumeBool();
    surfaceNode->SetFingerprint(hasFingerprint);
    surfaceNode->GetFingerprint();
    uint8_t alpha = fdp.ConsumeIntegral<uint8_t>();
    surfaceNode->SetAbilityBGAlpha(alpha);
    bool available = fdp.ConsumeBool();
    surfaceNode->SetIsNotifyUIBufferAvailable(available);
    bool isHidden = fdp.ConsumeBool();
    surfaceNode->MarkUIHidden(isHidden);
    GraphicColorGamut colorSpace = ConsumePOD<GraphicColorGamut>(fdp);
    surfaceNode->SetColorSpace(colorSpace);
    surfaceNode->GetColorSpace();
    bool isBootAnimation = fdp.ConsumeBool();
    surfaceNode->SetBootAnimation(isBootAnimation);
    surfaceNode->GetBootAnimation();
    return true;
}

bool DoSetBufferAvailableCallback(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    auto func = []() {
        std::cout << "SetBufferAvailableCallback, test upper limit of registered function" << std::endl;
    };
    uint32_t funcNumMax = fdp.ConsumeIntegral<uint32_t>();
    if (funcNumMax > MAX_FUZZ_IPC_LOOP_COUNT) {
        funcNumMax = MAX_FUZZ_IPC_LOOP_COUNT;
    }
    for (uint32_t i = 0; i < funcNumMax; ++i) {
        surfaceNode->SetBufferAvailableCallback(func);
    }
    return true;
}

bool DoSetBoundsChangedCallback(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    using BoundsChangedCallback = std::function<void(const Rosen::Vector4f&)>;
    BoundsChangedCallback callback;
    uint32_t funcNumMax = fdp.ConsumeIntegral<uint32_t>();
    if (funcNumMax > MAX_FUZZ_SIMPLE_LOOP_COUNT) {
        funcNumMax = MAX_FUZZ_SIMPLE_LOOP_COUNT;
    }
    for (uint32_t i = 0; i < funcNumMax; ++i) {
        surfaceNode->SetBoundsChangedCallback(callback);
    }
    return true;
}

bool DoMarshalling(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    Parcel parcel;
    surfaceNode->Marshalling(parcel);
    return true;
}

bool DoUnmarshalling(FuzzedDataProvider& fdp)
{
    RSSurfaceNodeConfig config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    Parcel parcel;
    parcel.WriteUint64(fdp.ConsumeIntegral<uint64_t>());
    parcel.WriteString(ConsumeRandomLengthString(fdp));
    parcel.WriteBool(fdp.ConsumeBool());
    surfaceNode->Unmarshalling(parcel);
    return true;
}

bool DoUnmarshallingAsProxyNode(FuzzedDataProvider& fdp)
{
    RSSurfaceNodeConfig config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    Parcel parcel;
    parcel.WriteUint64(fdp.ConsumeIntegral<uint64_t>());
    parcel.WriteString(ConsumeRandomLengthString(fdp));
    parcel.WriteBool(fdp.ConsumeBool());
    surfaceNode->UnmarshallingAsProxyNode(parcel);
    return true;
}

bool DoAttachToDisplay(FuzzedDataProvider& fdp)
{
    RSSurfaceNodeConfig config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    uint64_t screenId = fdp.ConsumeIntegral<uint64_t>();
    surfaceNode->AttachToDisplay(screenId);
    return true;
}

bool DoDetachToDisplayAndSetHardwareEnabled(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    uint64_t screenId = fdp.ConsumeIntegral<uint64_t>();
    surfaceNode->DetachToDisplay(screenId);
    bool isEnabled = fdp.ConsumeBool();
    surfaceNode->SetHardwareEnabled(isEnabled);
    return true;
}
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    uint8_t choice = fdp.ConsumeIntegral<uint8_t>();
    switch (choice % OHOS::Rosen::MAX_CASE_NUM) {
        case OHOS::Rosen::DO_CREATE:
            OHOS::Rosen::DoCreate(fdp);
            break;
        case OHOS::Rosen::DO_REMOVE_CHILD:
            OHOS::Rosen::DoRemoveChild(fdp);
            break;
        case OHOS::Rosen::DO_SET_AND_GET:
            OHOS::Rosen::DoSetAndGet(fdp);
            break;
        case OHOS::Rosen::DO_SET_BUFFER_AVAILABLE_CALLBACK:
            OHOS::Rosen::DoSetBufferAvailableCallback(fdp);
            break;
        case OHOS::Rosen::DO_SET_BOUNDS_CHANGED_CALLBACK:
            OHOS::Rosen::DoSetBoundsChangedCallback(fdp);
            break;
        case OHOS::Rosen::DO_MARSHALLING:
            OHOS::Rosen::DoMarshalling(fdp);
            break;
        case OHOS::Rosen::DO_UNMARSHALLING:
            OHOS::Rosen::DoUnmarshalling(fdp);
            break;
        case OHOS::Rosen::DO_UNMARSHALLING_AS_PROXY_NODE:
            OHOS::Rosen::DoUnmarshallingAsProxyNode(fdp);
            break;
        case OHOS::Rosen::DO_ATTACH_TO_DISPLAY:
            OHOS::Rosen::DoAttachToDisplay(fdp);
            break;
        case OHOS::Rosen::DO_DETACH_AND_SET_HARDWARE_ENABLED:
            OHOS::Rosen::DoDetachToDisplayAndSetHardwareEnabled(fdp);
            break;
        default:
            break;
    }
    return 0;
}
