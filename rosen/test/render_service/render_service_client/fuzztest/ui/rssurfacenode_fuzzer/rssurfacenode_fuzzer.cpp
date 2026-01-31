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

#include "rssurfacenode_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MAX_FUZZ_IPC_LOOP_COUNT = 100;
constexpr uint32_t MAX_FUZZ_SIMPLE_LOOP_COUNT = 1000;
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;

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

template<>
std::string GetData()
{
    size_t objectSize = GetData<uint8_t>();
    std::string object(objectSize, '\0');
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    object.assign(reinterpret_cast<const char*>(g_data + g_pos), objectSize);
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

inline RSSurfaceNodeConfig GetRSSurfaceNodeConfigFromData()
{
    constexpr int len = 10;
    std::string SurfaceNodeName = GetStringFromData(len);
    bool isTextureExportNode = GetData<bool>();
    SurfaceId surfaceId = GetData<uint64_t>();
    bool isSync = GetData<bool>();
    SurfaceWindowType  surfaceWindowType = static_cast<SurfaceWindowType>(GetData<uint8_t>());
    RSSurfaceNodeConfig config = { SurfaceNodeName, nullptr, isTextureExportNode,
        surfaceId, isSync, surfaceWindowType, nullptr };
    return config;
}

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    return true;
}
} // namespace

bool DoCreate()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode1 = RSSurfaceNode::Create(config);
    if (surfaceNode1 == nullptr) {
        return false;
    }
    RSSurfaceNodeType type = GetData<RSSurfaceNodeType>();
    RSSurfaceNode::SharedPtr surfaceNode2 = RSSurfaceNode::Create(config, type);
    if (surfaceNode2 == nullptr) {
        return false;
    }
    return true;
}

bool DoRemoveChild()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
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

bool DoSetAndGet()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isSecurityLayer = GetData<bool>();
    surfaceNode->SetSecurityLayer(isSecurityLayer);
    surfaceNode->GetSecurityLayer();
    bool isSkipLayer = GetData<bool>();
    surfaceNode->SetSkipLayer(isSkipLayer);
    surfaceNode->GetSkipLayer();
    bool isSnapshotSkipLayer = GetData<bool>();
    surfaceNode->SetSnapshotSkipLayer(isSnapshotSkipLayer);
    surfaceNode->GetSnapshotSkipLayer();
    bool hasFingerprint = GetData<bool>();
    surfaceNode->SetFingerprint(hasFingerprint);
    surfaceNode->GetFingerprint();
    uint8_t alpha = GetData<uint8_t>();
    surfaceNode->SetAbilityBGAlpha(alpha);
    bool available = GetData<bool>();
    surfaceNode->SetIsNotifyUIBufferAvailable(available);
    bool isHidden = GetData<bool>();
    surfaceNode->MarkUIHidden(isHidden);
    GraphicColorGamut colorSpace = GetData<GraphicColorGamut>();
    surfaceNode->SetColorSpace(colorSpace);
    surfaceNode->GetColorSpace();
    bool isBootAnimation = GetData<bool>();
    surfaceNode->SetBootAnimation(isBootAnimation);
    surfaceNode->GetBootAnimation();
    return true;
}

bool DoSetBufferAvailableCallback()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    auto func = []() {
        std::cout << "SetBufferAvailableCallback, test upper limit of registered function" << std::endl;
    };
    uint32_t funcNumMax = GetData<uint32_t>();
    if (funcNumMax > MAX_FUZZ_IPC_LOOP_COUNT) {
        funcNumMax = MAX_FUZZ_IPC_LOOP_COUNT;
    }
    for (uint32_t i = 0; i < funcNumMax; ++i) {
        surfaceNode->SetBufferAvailableCallback(func);
    }
    return true;
}

bool DoSetBoundsChangedCallback()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    using BoundsChangedCallback = std::function<void(const Rosen::Vector4f&)>;
    BoundsChangedCallback callback;
    uint32_t funcNumMax = GetData<uint32_t>();
    if (funcNumMax > MAX_FUZZ_SIMPLE_LOOP_COUNT) {
        funcNumMax = MAX_FUZZ_SIMPLE_LOOP_COUNT;
    }
    for (uint32_t i = 0; i < funcNumMax; ++i) {
        surfaceNode->SetBoundsChangedCallback(callback);
    }
    return true;
}

bool DoMarshalling()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    Parcel parcel;
    surfaceNode->Marshalling(parcel);
    return true;
}

bool DoUnmarshalling()
{
    // test
    RSSurfaceNodeConfig config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    Parcel parcel;
    parcel.WriteUint64(GetData<uint64_t>());
    parcel.WriteString(GetData<std::string>());
    parcel.WriteBool(GetData<bool>());
    surfaceNode->Unmarshalling(parcel);
    return true;
}

bool DoUnmarshallingAsProxyNode()
{
    // test
    RSSurfaceNodeConfig config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    Parcel parcel;
    parcel.WriteUint64(GetData<uint64_t>());
    parcel.WriteString(GetData<std::string>());
    parcel.WriteBool(GetData<bool>());
    surfaceNode->UnmarshallingAsProxyNode(parcel);
    return true;
}

bool DoAttachToDisplay()
{
    // test
    RSSurfaceNodeConfig config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    uint64_t screenId = GetData<uint64_t>();
    surfaceNode->AttachToDisplay(screenId);
    return true;
}

bool DoDetachToDisplayAndSetHardwareEnabled()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    uint64_t screenId = GetData<uint64_t>();
    surfaceNode->DetachToDisplay(screenId);
    bool isEnabled = GetData<bool>();
    surfaceNode->SetHardwareEnabled(isEnabled);
    return true;
}

bool DoSetForceHardwareAndFixRotation()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool flag = GetData<bool>();
    surfaceNode->SetForceHardwareAndFixRotation(flag);
    return true;
}

bool DoSetTextureExport()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isTextureExportNode = GetData<bool>();
    surfaceNode->SetTextureExport(isTextureExportNode);
    return true;
}

bool DoSetContainerWindow()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool hasContainerWindow = GetData<bool>();
    RRect rrect = GetData<RRect>();
    surfaceNode->SetContainerWindow(hasContainerWindow, rrect);
    return true;
}

bool DoSetWindowIdAndSetFreeze()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    uint32_t windowId = GetData<uint32_t>();
    surfaceNode->SetWindowId(windowId);
    bool isFreeze = GetData<bool>();
    surfaceNode->SetFreeze(isFreeze);
    return true;
}

#ifdef USE_SURFACE_TEXTURE
bool DoSetSurfaceTexture()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    RSSurfaceExtConfig config1 = GetData<RSSurfaceExtConfig>();
    surfaceNode->SetSurfaceTexture(config1);
    return true;
}

bool DoMarkUiFrameAvailable()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool available = GetData<bool>();
    surfaceNode->MarkUiFrameAvailable(available);
    return true;
}

bool DoCreateSurfaceExt()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    RSSurfaceExtConfig config1 = GetData<RSSurfaceExtConfig>();
    surfaceNode->CreateSurfaceExt(config1);
    return true;
}
#endif

bool DoSetForeground()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isForeground = GetData<bool>();
    surfaceNode->SetForeground(isForeground);
    return true;
}

bool DoSetForceUIFirst()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool forceUIFirst = GetData<bool>();
    surfaceNode->SetForceUIFirst(forceUIFirst);
    return true;
}

bool DoSetAncoFlags()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    uint32_t flags = GetData<uint32_t>();
    surfaceNode->SetAncoFlags(flags);
    return true;
}

bool DoSetWatermark()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    std::string waterMark = GetData<std::string>();
    bool waterMarkEnabled = GetData<bool>();
    surfaceNode->SetWatermarkEnabled(waterMark, waterMarkEnabled);
    return true;
}

bool DoSetHDRPresent()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool hdrPresent = GetData<bool>();
    NodeId id = GetData<NodeId>();
    surfaceNode->SetHDRPresent(hdrPresent, id);
    return true;
}

bool DoRSSurfaceNode()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    bool isRenderServiceNode = GetData<bool>();
    RSSurfaceNode surfaceNode1(config, isRenderServiceNode);
    NodeId id = GetData<NodeId>();
    RSSurfaceNode surfaceNode2(config, isRenderServiceNode, id);
    return true;
}

bool DoCreateNode()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    NodeId id = GetData<NodeId>();
    std::string name = GetData<std::string>();
    RSSurfaceNodeType type = GetData<RSSurfaceNodeType>();
    bool isTextureExportNode = GetData<bool>();
    bool isSync = GetData<bool>();
    RSSurfaceRenderNodeConfig config1;
    config1.id = id;
    config1.name = name;
    config1.nodeType = type;
    config1.isTextureExportNode = isTextureExportNode;
    config1.isSync = isSync;
    surfaceNode->CreateNode(config1);
    return true;
}

bool DoCreateNodeAndSurface()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    NodeId id = GetData<NodeId>();
    std::string name = GetData<std::string>();
    RSSurfaceNodeType type = GetData<RSSurfaceNodeType>();
    bool isTextureExportNode = GetData<bool>();
    bool isSync = GetData<bool>();
    RSSurfaceRenderNodeConfig config1;
    config1.id = id;
    config1.name = name;
    config1.nodeType = type;
    config1.isTextureExportNode = isTextureExportNode;
    config1.isSync = isSync;
    SurfaceId surfaceId = GetData<SurfaceId>();
    surfaceNode->CreateNodeAndSurface(config1, surfaceId);
    return true;
}

bool DoSetIsTextureExportNode()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isTextureExportNode = GetData<bool>();
    surfaceNode->SetIsTextureExportNode(isTextureExportNode);
    return true;
}

bool DoSetLeashPersistentId()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    LeashPersistentId leashPersistentId = GetData<LeashPersistentId>();
    surfaceNode->SetLeashPersistentId(leashPersistentId);
    return true;
}

bool DoSetGlobalPositionEnabled()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isEnabled = GetData<bool>();
    surfaceNode->SetGlobalPositionEnabled(isEnabled);
    return true;
}

bool DoSetSkipDraw()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isSkip = GetData<bool>();
    surfaceNode->SetSkipDraw(isSkip);
    return true;
}

bool DoSetHidePrivacyContent()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool needHidePrivacyContent = GetData<bool>();
    surfaceNode->SetHidePrivacyContent(needHidePrivacyContent);
    return true;
}

bool DoSetHardwareEnableHint()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool hardwareEnable = GetData<bool>();
    surfaceNode->SetHardwareEnableHint(hardwareEnable);
    return true;
}

bool DoSetSourceVirtualScreenId()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    uint64_t screenId = GetData<uint64_t>();
    surfaceNode->SetSourceVirtualDisplayId(screenId);
    return true;
}

bool DoSetFrameGravityNewVersionEnabled()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isEnabled = GetData<bool>();
    surfaceNode->SetFrameGravityNewVersionEnabled(isEnabled);
    return true;
}

bool DoSetSurfaceBufferOpaque()
{
    // test
    auto config = GetRSSurfaceNodeConfigFromData();
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isOpaque = GetData<bool>();
    surfaceNode->SetSurfaceBufferOpaque(isOpaque);
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
    OHOS::Rosen::DoCreate();
    OHOS::Rosen::DoSetAndGet();
    OHOS::Rosen::DoSetBufferAvailableCallback();
    OHOS::Rosen::DoSetBoundsChangedCallback();
    OHOS::Rosen::DoMarshalling();
    OHOS::Rosen::DoUnmarshalling();
    OHOS::Rosen::DoUnmarshallingAsProxyNode();
    OHOS::Rosen::DoAttachToDisplay();
    OHOS::Rosen::DoDetachToDisplayAndSetHardwareEnabled();
    OHOS::Rosen::DoSetForceHardwareAndFixRotation();
    OHOS::Rosen::DoSetTextureExport();
    OHOS::Rosen::DoSetContainerWindow();
    OHOS::Rosen::DoSetWindowIdAndSetFreeze();
#ifdef USE_SURFACE_TEXTURE
    OHOS::Rosen::DoSetSurfaceTexture();
    OHOS::Rosen::DoMarkUiFrameAvailable();
    OHOS::Rosen::DoCreateSurfaceExt();
#endif
    OHOS::Rosen::DoSetForeground();
    OHOS::Rosen::DoSetForceUIFirst();
    OHOS::Rosen::DoSetAncoFlags();
    OHOS::Rosen::DoRSSurfaceNode();
    OHOS::Rosen::DoCreateNode();
    OHOS::Rosen::DoCreateNodeAndSurface();
    OHOS::Rosen::DoSetIsTextureExportNode();
    OHOS::Rosen::DoSetLeashPersistentId();
    OHOS::Rosen::DoSetGlobalPositionEnabled();
    OHOS::Rosen::DoSetSkipDraw();
    OHOS::Rosen::DoSetHidePrivacyContent();
    OHOS::Rosen::DoSetHardwareEnableHint();
    OHOS::Rosen::DoSetSourceVirtualScreenId();
    OHOS::Rosen::DoSetFrameGravityNewVersionEnabled();
    OHOS::Rosen::DoSetSurfaceBufferOpaque();
    return 0;
}

