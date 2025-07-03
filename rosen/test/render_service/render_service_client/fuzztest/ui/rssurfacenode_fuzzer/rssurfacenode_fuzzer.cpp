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
} // namespace

bool DoCreate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode1 = RSSurfaceNode::Create(config);
    RSSurfaceNodeType type = GetData<RSSurfaceNodeType>();
    RSSurfaceNode::SharedPtr surfaceNode2 = RSSurfaceNode::Create(config, type);
    return true;
}

bool DoGetType(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->GetType();
    return true;
}

bool DoCreateNodeInRenderThread(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->CreateNodeInRenderThread();
    return true;
}

bool DoRemoveChild(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    RSSurfaceNodeConfig config1;
    RSSurfaceNode::SharedPtr child = RSSurfaceNode::Create(config1);
    int index = GetData<int>();
    surfaceNode->AddChild(child, index);
    surfaceNode->RemoveChild(child);
    return true;
}

bool DoClearChildren(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    RSSurfaceNodeConfig config1;
    RSSurfaceNode::SharedPtr child = RSSurfaceNode::Create(config1);
    int index = GetData<int>();
    surfaceNode->AddChild(child, index);
    surfaceNode->ClearChildren();
    return true;
}

bool DoSetAndGet(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
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

bool DoSetBufferAvailableCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->SetBufferAvailableCallback([]() {
        std::cout << "SetBufferAvailableCallback" << std::endl;
    });
    return true;
}

bool DoIsBufferAvailable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->IsBufferAvailable();
    return true;
}

bool DoSetBoundsChangedCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    using BoundsChangedCallback = std::function<void(const Rosen::Vector4f&)>;
    BoundsChangedCallback callback;
    surfaceNode->SetBoundsChangedCallback(callback);
    return true;
}

bool DoSetAnimationFinished(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->SetAnimationFinished();
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

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    Parcel parcel;
    surfaceNode->Marshalling(parcel);
    return true;
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

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    Parcel parcel;
    surfaceNode->Unmarshalling(parcel);
    return true;
}

bool DoUnmarshallingAsProxyNode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    Parcel parcel;
    surfaceNode->UnmarshallingAsProxyNode(parcel);
    return true;
}

bool DoGetFollowType(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->GetFollowType();
    return true;
}

bool DoAttachToDisplay(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    uint64_t screenId = GetData<uint64_t>();
    surfaceNode->AttachToDisplay(screenId);
    return true;
}

bool DoDetachToDisplayAndSetHardwareEnabled(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    uint64_t screenId = GetData<uint64_t>();
    surfaceNode->DetachToDisplay(screenId);
    bool isEnabled = GetData<bool>();
    surfaceNode->SetHardwareEnabled(isEnabled);
    return true;
}

bool DoSetForceHardwareAndFixRotation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    bool flag = GetData<bool>();
    surfaceNode->SetForceHardwareAndFixRotation(flag);
    return true;
}

bool DoSetTextureExport(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    bool isTextureExportNode = GetData<bool>();
    surfaceNode->SetTextureExport(isTextureExportNode);
    return true;
}

bool DoGetSurfaceAndResetContextAlpha(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->GetSurface();
    surfaceNode->ResetContextAlpha();
    return true;
}

bool DoGetNameAndBundleName(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->GetName();
    return true;
}

bool DoSetContainerWindow(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    bool hasContainerWindow = GetData<bool>();
    RRect rrect = GetData<RRect>();
    surfaceNode->SetContainerWindow(hasContainerWindow, rrect);
    return true;
}

bool DoSetWindowIdAndSetFreeze(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    uint32_t windowId = GetData<uint32_t>();
    surfaceNode->SetWindowId(windowId);
    bool isFreeze = GetData<bool>();
    surfaceNode->SetFreeze(isFreeze);
    return true;
}

#ifdef USE_SURFACE_TEXTURE
bool DoSetSurfaceTexture(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    RSSurfaceExtConfig config1 = GetData<RSSurfaceExtConfig>();
    surfaceNode->SetSurfaceTexture(config1);
    return true;
}

bool DoMarkUiFrameAvailable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    bool available = GetData<bool>();
    surfaceNode->MarkUiFrameAvailable(available);
    return true;
}

bool DoCreateSurfaceExt(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    RSSurfaceExtConfig config1 = GetData<RSSurfaceExtConfig>();
    surfaceNode->CreateSurfaceExt(config1);
    return true;
}
#endif

bool DoSetForeground(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    bool isForeground = GetData<bool>();
    surfaceNode->SetForeground(isForeground);
    return true;
}

bool DoSetForceUIFirst(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    bool forceUIFirst = GetData<bool>();
    surfaceNode->SetForceUIFirst(forceUIFirst);
    return true;
}

bool DoSetAncoFlags(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    uint32_t flags = GetData<uint32_t>();
    surfaceNode->SetAncoFlags(flags);
    return true;
}

bool DoSetWatermark(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    std::string waterMark = GetData<std::string>();
    bool waterMarkEnabled = GetData<bool>();
    surfaceNode->SetWatermarkEnabled(waterMark, waterMarkEnabled);
    return true;
}

bool DoSetHDRPresent(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    bool hdrPresent = GetData<bool>();
    NodeId id = GetData<NodeId>();
    surfaceNode->SetHDRPresent(hdrPresent, id);
    return true;
}

bool DoNeedForcedSendToRemoteAndCreateTextureExportRenderNodeInRT(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->NeedForcedSendToRemote();
    surfaceNode->CreateRenderNodeForTextureExportSwitch();
    return true;
}

bool DoRSSurfaceNode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    bool isRenderServiceNode = GetData<bool>();
    RSSurfaceNode surfaceNode1(config, isRenderServiceNode);
    NodeId id = GetData<NodeId>();
    RSSurfaceNode surfaceNode2(config, isRenderServiceNode, id);
    return true;
}

bool DoCreateNode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
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

bool DoCreateNodeAndSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
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

bool DoOnBoundsSizeChangedAndSetSurfaceIdToRenderNode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->OnBoundsSizeChanged();
    surfaceNode->SetSurfaceIdToRenderNode();
    return true;
}

bool DoSetIsTextureExportNode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    bool isTextureExportNode = GetData<bool>();
    surfaceNode->SetIsTextureExportNode(isTextureExportNode);
    return true;
}

bool DoSplitSurfaceNodeName(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    std::string surfaceNodeName = GetData<std::string>();
    surfaceNode->SplitSurfaceNodeName(surfaceNodeName);
    return true;
}

bool DoSetLeashPersistentId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    LeashPersistentId leashPersistentId = GetData<LeashPersistentId>();
    surfaceNode->SetLeashPersistentId(leashPersistentId);
    return true;
}

bool DoGetLeashPersistentId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->GetLeashPersistentId();
    return true;
}

bool DoSetGlobalPositionEnabled(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    bool isEnabled = GetData<bool>();
    surfaceNode->SetGlobalPositionEnabled(isEnabled);
    return true;
}

bool DoGetGlobalPositionEnabled(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->GetGlobalPositionEnabled();
    return true;
}

bool DoSetSkipDraw(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    bool isSkip = GetData<bool>();
    surfaceNode->SetSkipDraw(isSkip);
    return true;
}

bool DoGetSkipDraw(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->GetSkipDraw();
    return true;
}

bool DoGetAbilityState(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->GetAbilityState();
    return true;
}

bool DoSetHidePrivacyContent(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    bool needHidePrivacyContent = GetData<bool>();
    surfaceNode->SetHidePrivacyContent(needHidePrivacyContent);
    return true;
}

bool DoSetHardwareEnableHint(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    bool hardwareEnable = GetData<bool>();
    surfaceNode->SetHardwareEnableHint(hardwareEnable);
    return true;
}

bool DoSetSourceVirtualScreenId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    uint64_t screenId = GetData<uint64_t>();
    surfaceNode->SetSourceVirtualDisplayId(screenId);
    return true;
}

bool DoSetFrameGravityNewVersionEnabled(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    bool isEnabled = GetData<bool>();
    surfaceNode->SetFrameGravityNewVersionEnabled(isEnabled);
    return true;
}

bool DoGetFrameGravityNewVersionEnabled(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->GetFrameGravityNewVersionEnabled();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoCreate(data, size);
    OHOS::Rosen::DoGetType(data, size);
    OHOS::Rosen::DoCreateNodeInRenderThread(data, size);
    OHOS::Rosen::DoRemoveChild(data, size);
    OHOS::Rosen::DoClearChildren(data, size);
    OHOS::Rosen::DoSetAndGet(data, size);
    OHOS::Rosen::DoSetBufferAvailableCallback(data, size);
    OHOS::Rosen::DoIsBufferAvailable(data, size);
    OHOS::Rosen::DoSetBoundsChangedCallback(data, size);
    OHOS::Rosen::DoSetAnimationFinished(data, size);
    OHOS::Rosen::DoMarshalling(data, size);
    OHOS::Rosen::DoUnmarshalling(data, size);
    OHOS::Rosen::DoUnmarshallingAsProxyNode(data, size);
    OHOS::Rosen::DoGetFollowType(data, size);
    OHOS::Rosen::DoAttachToDisplay(data, size);
    OHOS::Rosen::DoDetachToDisplayAndSetHardwareEnabled(data, size);
    OHOS::Rosen::DoSetForceHardwareAndFixRotation(data, size);
    OHOS::Rosen::DoSetTextureExport(data, size);
    OHOS::Rosen::DoGetSurfaceAndResetContextAlpha(data, size);
    OHOS::Rosen::DoGetNameAndBundleName(data, size);
    OHOS::Rosen::DoSetContainerWindow(data, size);
    OHOS::Rosen::DoSetWindowIdAndSetFreeze(data, size);
#ifdef USE_SURFACE_TEXTURE
    OHOS::Rosen::DoSetSurfaceTexture(data, size);
    OHOS::Rosen::DoMarkUiFrameAvailable(data, size);
    OHOS::Rosen::DoCreateSurfaceExt(data, size);
#endif
    OHOS::Rosen::DoSetForeground(data, size);
    OHOS::Rosen::DoSetForceUIFirst(data, size);
    OHOS::Rosen::DoSetAncoFlags(data, size);
    OHOS::Rosen::DoNeedForcedSendToRemoteAndCreateTextureExportRenderNodeInRT(data, size);
    OHOS::Rosen::DoRSSurfaceNode(data, size);
    OHOS::Rosen::DoCreateNode(data, size);
    OHOS::Rosen::DoCreateNodeAndSurface(data, size);
    OHOS::Rosen::DoOnBoundsSizeChangedAndSetSurfaceIdToRenderNode(data, size);
    OHOS::Rosen::DoSetIsTextureExportNode(data, size);
    OHOS::Rosen::DoSetLeashPersistentId(data, size);
    OHOS::Rosen::DoGetLeashPersistentId(data, size);
    OHOS::Rosen::DoSetGlobalPositionEnabled(data, size);
    OHOS::Rosen::DoGetGlobalPositionEnabled(data, size);
    OHOS::Rosen::DoSetSkipDraw(data, size);
    OHOS::Rosen::DoGetSkipDraw(data, size);
    OHOS::Rosen::DoGetAbilityState(data, size);
    OHOS::Rosen::DoSetHidePrivacyContent(data, size);
    OHOS::Rosen::DoSetHardwareEnableHint(data, size);
    OHOS::Rosen::DoSetSourceVirtualScreenId(data, size);
    OHOS::Rosen::DoSetFrameGravityNewVersionEnabled(data, size);
    OHOS::Rosen::DoGetFrameGravityNewVersionEnabled(data, size);
    return 0;
}

