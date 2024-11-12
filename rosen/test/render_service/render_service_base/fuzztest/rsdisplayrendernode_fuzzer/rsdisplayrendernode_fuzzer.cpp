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

#include "rsdisplayrendernode_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <memory>
#include <securec.h>
#include <unistd.h>

#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_render_thread_visitor.cpp"
#include "screen_manager/screen_types.h"
#include "params/rs_display_render_params.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

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
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    uint32_t rogWidth = GetData<uint32_t>();
    uint32_t rogHeight = GetData<uint32_t>();
    int32_t offsetX = GetData<int32_t>();
    int32_t offsetY = GetData<int32_t>();
    uint32_t refreshRate = GetData<uint32_t>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    ScreenRotation screenRotation = (ScreenRotation)rogWidth;
    RSDisplayRenderNode::CompositeType type = (RSDisplayRenderNode::CompositeType)rogWidth;
    auto node = std::make_shared<RSBaseRenderNode>(id);
    std::vector<std::shared_ptr<RSRenderNode>> vec;
    std::shared_ptr<RSRenderThreadVisitor> visitor;
    RSDisplayNodeConfig config;
    config.mirrorNodeId = id;
    config.screenId = id;
    config.isMirrored = true;
    std::string name = "text";
    std::vector<std::string> windowsName;
    windowsName.push_back(name);
    RectI dirtyShadow;
    Drawing::Matrix matrix;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    rsDisplayRenderNode.stagingRenderParams_ = std::make_unique<RSDisplayRenderParams>(id);
    auto sptr = std::make_shared<RSDisplayRenderNode>(id, config);
    rsDisplayRenderNode.SetIsOnTheTree(true);
    rsDisplayRenderNode.SetScreenId(id);
    rsDisplayRenderNode.GetScreenId();
    rsDisplayRenderNode.SetRogSize(rogWidth, rogHeight);
    rsDisplayRenderNode.GetRogWidth();
    rsDisplayRenderNode.GetRogHeight();
    rsDisplayRenderNode.SetDisplayOffset(offsetX, offsetY);
    rsDisplayRenderNode.GetDisplayOffsetX();
    rsDisplayRenderNode.GetDisplayOffsetY();
    rsDisplayRenderNode.GetFingerprint();
    rsDisplayRenderNode.SetFingerprint(true);
    rsDisplayRenderNode.SetScreenRotation(screenRotation);
    rsDisplayRenderNode.GetScreenRotation();
    rsDisplayRenderNode.CollectSurface(node, vec, true, true);
    rsDisplayRenderNode.QuickPrepare(visitor);
    rsDisplayRenderNode.Prepare(visitor);
    rsDisplayRenderNode.Process(visitor);
    rsDisplayRenderNode.GetType();
    rsDisplayRenderNode.IsMirrorDisplay();
    rsDisplayRenderNode.SetCompositeType(type);
    rsDisplayRenderNode.GetCompositeType();
    rsDisplayRenderNode.SetForceSoftComposite(true);
    rsDisplayRenderNode.IsForceSoftComposite();
    rsDisplayRenderNode.SetMirrorSource(sptr);
    rsDisplayRenderNode.ResetMirrorSource();
    rsDisplayRenderNode.SetIsMirrorDisplay(true);
    rsDisplayRenderNode.SetSecurityDisplay(true);
    rsDisplayRenderNode.GetSecurityDisplay();
    rsDisplayRenderNode.SkipFrame(refreshRate, skipFrameInterval);
    rsDisplayRenderNode.SetBootAnimation(true);
    rsDisplayRenderNode.GetBootAnimation();
    rsDisplayRenderNode.GetMirrorSource();
    rsDisplayRenderNode.SetIsParallelDisplayNode(true);
    rsDisplayRenderNode.IsParallelDisplayNode();
    rsDisplayRenderNode.GetRotation();
    rsDisplayRenderNode.GetDirtyManager();
    rsDisplayRenderNode.UpdateDisplayDirtyManager(offsetX);
    rsDisplayRenderNode.ClearCurrentSurfacePos();
    rsDisplayRenderNode.UpdateSurfaceNodePos(id, dirtyShadow);
    rsDisplayRenderNode.GetLastFrameSurfacePos(id);
    rsDisplayRenderNode.GetCurrentFrameSurfacePos(id);
    rsDisplayRenderNode.GetSurfaceChangedRects();
    rsDisplayRenderNode.GetCurAllSurfaces();
    rsDisplayRenderNode.GetCurAllSurfaces(true);
    rsDisplayRenderNode.UpdateRenderParams();
    rsDisplayRenderNode.UpdatePartialRenderParams();
    rsDisplayRenderNode.IsRotationChanged();
    rsDisplayRenderNode.IsLastRotationChanged();
    rsDisplayRenderNode.GetPreRotationStatus();
    rsDisplayRenderNode.GetCurRotationStatus();
    rsDisplayRenderNode.IsFirstTimeToProcessor();
    rsDisplayRenderNode.SetOriginScreenRotation(screenRotation);
    rsDisplayRenderNode.GetOriginScreenRotation();
    rsDisplayRenderNode.SetInitMatrix(matrix);
    rsDisplayRenderNode.GetInitMatrix();
    auto cacheImgForCapture = std::make_shared<Drawing::Image>();
    rsDisplayRenderNode.GetOffScreenCacheImgForCapture();
    rsDisplayRenderNode.SetOffScreenCacheImgForCapture(cacheImgForCapture);
    rsDisplayRenderNode.GetDirtySurfaceNodeMap();
    rsDisplayRenderNode.ClearSurfaceSrcRect();
    rsDisplayRenderNode.ClearSurfaceDstRect();
    rsDisplayRenderNode.ClearSurfaceTotalMatrix();
    rsDisplayRenderNode.SetSurfaceSrcRect(id, dirtyShadow);
    rsDisplayRenderNode.SetSurfaceDstRect(id, dirtyShadow);
    rsDisplayRenderNode.SetSurfaceTotalMatrix(id, matrix);
    rsDisplayRenderNode.GetSurfaceSrcRect(id);
    rsDisplayRenderNode.GetSurfaceDstRect(id);
    rsDisplayRenderNode.GetSurfaceTotalMatrix(id);
    rsDisplayRenderNode.GetLastSurfaceIds();
    std::vector<NodeId> lastSurfaceIds;
    lastSurfaceIds.push_back(id);
    rsDisplayRenderNode.SetLastSurfaceIds(lastSurfaceIds);
    rsDisplayRenderNode.OnSync();
    return true;
}

bool DoUpdateScreenRenderParams(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    RSDisplayNodeConfig config;
    config.mirrorNodeId = id;
    config.screenId = id;
    config.isMirrored = true;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    rsDisplayRenderNode.stagingRenderParams_ = std::make_unique<RSDisplayRenderParams>(id);
    RSDisplayRenderNode::ScreenRenderParams screenRenderParams;
    rsDisplayRenderNode.UpdateScreenRenderParams(screenRenderParams);
    rsDisplayRenderNode.UpdateOffscreenRenderParams(GetData<bool>());
    rsDisplayRenderNode.SetDisplayGlobalZOrder(GetData<float>());
    rsDisplayRenderNode.UpdateRotation();
    rsDisplayRenderNode.SetMainAndLeashSurfaceDirty(GetData<bool>());
    rsDisplayRenderNode.SetHDRPresent(GetData<bool>());
    rsDisplayRenderNode.GetSortedChildren();
    rsDisplayRenderNode.GetDisappearedSurfaceRegionBelowCurrent(GetData<uint64_t>());
    return true;
}

bool DoHandleCurMainAndLeashSurfaceNodes(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    RSDisplayNodeConfig config;
    config.mirrorNodeId = id;
    config.screenId = id;
    config.isMirrored = true;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    auto surface = std::make_shared<OHOS::Rosen::RSRenderNode>(id);
    rsDisplayRenderNode.RecordMainAndLeashSurfaces(surface);
    rsDisplayRenderNode.HandleCurMainAndLeashSurfaceNodes();
    return true;
}

bool DoIsZoomStateChange(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    RSDisplayNodeConfig config;
    config.mirrorNodeId = id;
    config.screenId = id;
    config.isMirrored = true;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    bool state = GetData<bool>();
    rsDisplayRenderNode.UpdateZoomState(state);
    rsDisplayRenderNode.IsZoomStateChange();
    return true;
}

bool DoGetSortedChildren(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    NodeId id = GetData<NodeId>();
    uint64_t screenId = GetData<uint64_t>();
    bool isMirrored = GetData<bool>();
    NodeId mirrorNodeId = GetData<NodeId>();
    RSDisplayNodeConfig config = { screenId, isMirrored, mirrorNodeId };
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    auto oldScbPids = std::vector<int32_t>();
    int32_t currentScbPid = GetData<int32_t>();
    rsDisplayRenderNode.SetScbNodePid(oldScbPids, currentScbPid);
    rsDisplayRenderNode.GetSortedChildren();
    return true;
}

bool DoSetBrightnessRatio(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    NodeId id = GetData<NodeId>();
    uint64_t screenId = GetData<uint64_t>();
    bool isMirrored = GetData<bool>();
    NodeId mirrorNodeId = GetData<NodeId>();
    RSDisplayNodeConfig config = { screenId, isMirrored, mirrorNodeId };
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    rsDisplayRenderNode.stagingRenderParams_ = std::make_unique<RSDisplayRenderParams>(id);
    float brightnessRatio = GetData<float>();
    rsDisplayRenderNode.SetBrightnessRatio(brightnessRatio);
    return true;
}

bool DoOnSyncWithDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    NodeId id = GetData<NodeId>();
    uint64_t screenId = GetData<uint64_t>();
    bool isMirrored = GetData<bool>();
    NodeId mirrorNodeId = GetData<NodeId>();
    RSDisplayNodeConfig config = { screenId, isMirrored, mirrorNodeId };
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(id, config);
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSDisplayRenderParams>(id);
    auto node = std::static_pointer_cast<RSRenderNode>(rsDisplayRenderNode);
    rsDisplayRenderNode->renderDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node);
    rsDisplayRenderNode->OnSync();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    OHOS::Rosen::DoUpdateScreenRenderParams(data, size);
    OHOS::Rosen::DoHandleCurMainAndLeashSurfaceNodes(data, size);
    OHOS::Rosen::DoIsZoomStateChange(data, size);
    OHOS::Rosen::DoGetSortedChildren(data, size);
    OHOS::Rosen::DoSetBrightnessRatio(data, size);
    OHOS::Rosen::DoOnSyncWithDrawable(data, size);
    return 0;
}
