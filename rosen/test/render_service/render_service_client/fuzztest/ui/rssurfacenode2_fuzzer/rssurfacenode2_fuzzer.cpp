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

#include "rssurfacenode2_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include <securec.h>

#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint8_t DO_SET_FORCE_HW_AND_FIX_ROTATION = 0;
constexpr uint8_t DO_SET_TEXTURE_EXPORT = 1;
constexpr uint8_t DO_SET_CONTAINER_WINDOW = 2;
constexpr uint8_t DO_SET_WINDOW_ID_AND_FREEZE = 3;
constexpr uint8_t DO_SET_FOREGROUND = 4;
constexpr uint8_t DO_SET_FORCE_UI_FIRST = 5;
constexpr uint8_t DO_SET_ANCO_FLAGS = 6;
constexpr uint8_t DO_SET_WATERMARK = 7;
constexpr uint8_t DO_SET_HDR_PRESENT = 8;
constexpr uint8_t DO_RS_SURFACE_NODE = 9;
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

bool DoSetForceHardwareAndFixRotation(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool flag = fdp.ConsumeBool();
    surfaceNode->SetForceHardwareAndFixRotation(flag);
    return true;
}

bool DoSetTextureExport(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isTextureExportNode = fdp.ConsumeBool();
    surfaceNode->SetTextureExport(isTextureExportNode);
    return true;
}

bool DoSetContainerWindow(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool hasContainerWindow = fdp.ConsumeBool();
    RRect rrect = ConsumePOD<RRect>(fdp);
    surfaceNode->SetContainerWindow(hasContainerWindow, rrect);
    return true;
}

bool DoSetWindowIdAndSetFreeze(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    uint32_t windowId = fdp.ConsumeIntegral<uint32_t>();
    surfaceNode->SetWindowId(windowId);
    bool isFreeze = fdp.ConsumeBool();
    surfaceNode->SetFreeze(isFreeze);
    return true;
}

bool DoSetForeground(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isForeground = fdp.ConsumeBool();
    surfaceNode->SetForeground(isForeground);
    return true;
}

bool DoSetForceUIFirst(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool forceUIFirst = fdp.ConsumeBool();
    surfaceNode->SetForceUIFirst(forceUIFirst);
    return true;
}

bool DoSetAncoFlags(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    uint32_t flags = fdp.ConsumeIntegral<uint32_t>();
    surfaceNode->SetAncoFlags(flags);
    return true;
}

bool DoSetWatermark(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    std::string waterMark = ConsumeRandomLengthString(fdp);
    bool waterMarkEnabled = fdp.ConsumeBool();
    surfaceNode->SetWatermarkEnabled(waterMark, waterMarkEnabled);
    return true;
}

bool DoSetHDRPresent(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool hdrPresent = fdp.ConsumeBool();
    NodeId id = fdp.ConsumeIntegral<NodeId>();
    surfaceNode->SetHDRPresent(hdrPresent, id);
    return true;
}

bool DoRSSurfaceNode(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    bool isRenderServiceNode = fdp.ConsumeBool();
    RSSurfaceNode surfaceNode1(config, isRenderServiceNode);
    NodeId id = fdp.ConsumeIntegral<NodeId>();
    RSSurfaceNode surfaceNode2(config, isRenderServiceNode, id);
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
        case OHOS::Rosen::DO_SET_FORCE_HW_AND_FIX_ROTATION:
            OHOS::Rosen::DoSetForceHardwareAndFixRotation(fdp);
            break;
        case OHOS::Rosen::DO_SET_TEXTURE_EXPORT:
            OHOS::Rosen::DoSetTextureExport(fdp);
            break;
        case OHOS::Rosen::DO_SET_CONTAINER_WINDOW:
            OHOS::Rosen::DoSetContainerWindow(fdp);
            break;
        case OHOS::Rosen::DO_SET_WINDOW_ID_AND_FREEZE:
            OHOS::Rosen::DoSetWindowIdAndSetFreeze(fdp);
            break;
        case OHOS::Rosen::DO_SET_FOREGROUND:
            OHOS::Rosen::DoSetForeground(fdp);
            break;
        case OHOS::Rosen::DO_SET_FORCE_UI_FIRST:
            OHOS::Rosen::DoSetForceUIFirst(fdp);
            break;
        case OHOS::Rosen::DO_SET_ANCO_FLAGS:
            OHOS::Rosen::DoSetAncoFlags(fdp);
            break;
        case OHOS::Rosen::DO_SET_WATERMARK:
            OHOS::Rosen::DoSetWatermark(fdp);
            break;
        case OHOS::Rosen::DO_SET_HDR_PRESENT:
            OHOS::Rosen::DoSetHDRPresent(fdp);
            break;
        case OHOS::Rosen::DO_RS_SURFACE_NODE:
            OHOS::Rosen::DoRSSurfaceNode(fdp);
            break;
        default:
            break;
    }
    return 0;
}
