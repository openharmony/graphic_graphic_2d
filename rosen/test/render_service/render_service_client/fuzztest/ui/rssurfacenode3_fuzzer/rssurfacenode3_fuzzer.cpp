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

#include "rssurfacenode3_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include <securec.h>

#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint8_t DO_CREATE_NODE = 0;
constexpr uint8_t DO_CREATE_NODE_AND_SURFACE = 1;
constexpr uint8_t DO_SET_IS_TEXTURE_EXPORT_NODE = 2;
constexpr uint8_t DO_SET_LEASH_PERSISTENT_ID = 3;
constexpr uint8_t DO_SET_GLOBAL_POSITION_ENABLED = 4;
constexpr uint8_t DO_SET_SKIP_DRAW = 5;
constexpr uint8_t DO_SET_HIDE_PRIVACY_CONTENT = 6;
constexpr uint8_t DO_SET_HARDWARE_ENABLE_HINT = 7;
constexpr uint8_t DO_SET_SOURCE_VIRTUAL_SCREEN_ID = 8;
constexpr uint8_t DO_SET_FRAME_GRAVITY_NEW_VER_ENABLED = 9;
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
    std::string SurfaceNodeName = ConsumeFixedLengthString(fdp, len);
    bool isTextureExportNode = fdp.ConsumeBool();
    SurfaceId surfaceId = fdp.ConsumeIntegral<uint64_t>();
    bool isSync = fdp.ConsumeBool();
    SurfaceWindowType surfaceWindowType = static_cast<SurfaceWindowType>(fdp.ConsumeIntegral<uint8_t>());
    RSSurfaceNodeConfig config = { SurfaceNodeName, nullptr, isTextureExportNode,
        surfaceId, isSync, surfaceWindowType, nullptr };
    return config;
}
} // namespace

bool DoCreateNode(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    NodeId id = fdp.ConsumeIntegral<NodeId>();
    std::string name = ConsumeRandomLengthString(fdp);
    RSSurfaceNodeType type = ConsumePOD<RSSurfaceNodeType>(fdp);
    bool isTextureExportNode = fdp.ConsumeBool();
    bool isSync = fdp.ConsumeBool();
    RSSurfaceRenderNodeConfig config1;
    config1.id = id;
    config1.name = name;
    config1.nodeType = type;
    config1.isTextureExportNode = isTextureExportNode;
    config1.isSync = isSync;
    surfaceNode->CreateNode(config1);
    return true;
}

bool DoCreateNodeAndSurface(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    NodeId id = fdp.ConsumeIntegral<NodeId>();
    std::string name = ConsumeRandomLengthString(fdp);
    RSSurfaceNodeType type = ConsumePOD<RSSurfaceNodeType>(fdp);
    bool isTextureExportNode = fdp.ConsumeBool();
    bool isSync = fdp.ConsumeBool();
    RSSurfaceRenderNodeConfig config1;
    config1.id = id;
    config1.name = name;
    config1.nodeType = type;
    config1.isTextureExportNode = isTextureExportNode;
    config1.isSync = isSync;
    SurfaceId surfaceId = fdp.ConsumeIntegral<SurfaceId>();
    surfaceNode->CreateNodeAndSurface(config1, surfaceId);
    return true;
}

bool DoSetIsTextureExportNode(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isTextureExportNode = fdp.ConsumeBool();
    surfaceNode->SetIsTextureExportNode(isTextureExportNode);
    return true;
}

bool DoSetLeashPersistentId(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    LeashPersistentId leashPersistentId = fdp.ConsumeIntegral<LeashPersistentId>();
    surfaceNode->SetLeashPersistentId(leashPersistentId);
    return true;
}

bool DoSetGlobalPositionEnabled(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isEnabled = fdp.ConsumeBool();
    surfaceNode->SetGlobalPositionEnabled(isEnabled);
    return true;
}

bool DoSetSkipDraw(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isSkip = fdp.ConsumeBool();
    surfaceNode->SetSkipDraw(isSkip);
    return true;
}

bool DoSetHidePrivacyContent(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool needHidePrivacyContent = fdp.ConsumeBool();
    surfaceNode->SetHidePrivacyContent(needHidePrivacyContent);
    return true;
}

bool DoSetHardwareEnableHint(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool hardwareEnable = fdp.ConsumeBool();
    surfaceNode->SetHardwareEnableHint(hardwareEnable);
    return true;
}

bool DoSetSourceVirtualScreenId(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    uint64_t screenId = fdp.ConsumeIntegral<uint64_t>();
    surfaceNode->SetSourceVirtualDisplayId(screenId);
    return true;
}

bool DoSetFrameGravityNewVersionEnabled(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isEnabled = fdp.ConsumeBool();
    surfaceNode->SetFrameGravityNewVersionEnabled(isEnabled);
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
        case OHOS::Rosen::DO_CREATE_NODE:
            OHOS::Rosen::DoCreateNode(fdp);
            break;
        case OHOS::Rosen::DO_CREATE_NODE_AND_SURFACE:
            OHOS::Rosen::DoCreateNodeAndSurface(fdp);
            break;
        case OHOS::Rosen::DO_SET_IS_TEXTURE_EXPORT_NODE:
            OHOS::Rosen::DoSetIsTextureExportNode(fdp);
            break;
        case OHOS::Rosen::DO_SET_LEASH_PERSISTENT_ID:
            OHOS::Rosen::DoSetLeashPersistentId(fdp);
            break;
        case OHOS::Rosen::DO_SET_GLOBAL_POSITION_ENABLED:
            OHOS::Rosen::DoSetGlobalPositionEnabled(fdp);
            break;
        case OHOS::Rosen::DO_SET_SKIP_DRAW:
            OHOS::Rosen::DoSetSkipDraw(fdp);
            break;
        case OHOS::Rosen::DO_SET_HIDE_PRIVACY_CONTENT:
            OHOS::Rosen::DoSetHidePrivacyContent(fdp);
            break;
        case OHOS::Rosen::DO_SET_HARDWARE_ENABLE_HINT:
            OHOS::Rosen::DoSetHardwareEnableHint(fdp);
            break;
        case OHOS::Rosen::DO_SET_SOURCE_VIRTUAL_SCREEN_ID:
            OHOS::Rosen::DoSetSourceVirtualScreenId(fdp);
            break;
        case OHOS::Rosen::DO_SET_FRAME_GRAVITY_NEW_VER_ENABLED:
            OHOS::Rosen::DoSetFrameGravityNewVersionEnabled(fdp);
            break;
        default:
            break;
    }
    return 0;
}
