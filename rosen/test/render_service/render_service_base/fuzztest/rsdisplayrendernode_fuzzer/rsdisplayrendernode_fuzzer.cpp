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

#include "pipeline/rs_logical_display_render_node.h"
#include "render_thread/rs_render_thread_visitor.cpp"
#include "screen_manager/screen_types.h"
#include "params/rs_logical_display_render_params.h"

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
    uint32_t refreshRate = GetData<uint32_t>();
    uint32_t skipFrameInterval = GetData<uint32_t>();

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
    RSLogicalDisplayRenderNode logicalDisplayRenderNode(id, config);
    auto sptr = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    logicalDisplayRenderNode.SetIsOnTheTree(true);
    logicalDisplayRenderNode.SetScreenId(id);
    logicalDisplayRenderNode.GetScreenId();
    logicalDisplayRenderNode.GetScreenRotation();
    logicalDisplayRenderNode.CollectSurface(node, vec, true, true);
    logicalDisplayRenderNode.QuickPrepare(visitor);
    logicalDisplayRenderNode.Prepare(visitor);
    logicalDisplayRenderNode.Process(visitor);
    logicalDisplayRenderNode.GetType();
    logicalDisplayRenderNode.IsMirrorDisplay();
    logicalDisplayRenderNode.GetCompositeType();
    logicalDisplayRenderNode.SetMirrorSource(sptr);
    logicalDisplayRenderNode.ResetMirrorSource();
    logicalDisplayRenderNode.SetIsMirrorDisplay(true);
    logicalDisplayRenderNode.SetSecurityDisplay(true);
    logicalDisplayRenderNode.GetSecurityDisplay();
    logicalDisplayRenderNode.SkipFrame(refreshRate, skipFrameInterval);
    logicalDisplayRenderNode.SetBootAnimation(true);
    logicalDisplayRenderNode.GetBootAnimation();
    logicalDisplayRenderNode.GetMirrorSource();
    logicalDisplayRenderNode.GetRotation();
    logicalDisplayRenderNode.UpdateRenderParams();
    logicalDisplayRenderNode.IsRotationChanged();
    logicalDisplayRenderNode.IsLastRotationChanged();
    logicalDisplayRenderNode.GetPreRotationStatus();
    logicalDisplayRenderNode.GetCurRotationStatus();
    auto cacheImgForCapture = std::make_shared<Drawing::Image>();
    std::vector<NodeId> lastSurfaceIds;
    lastSurfaceIds.push_back(id);
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
    RSLogicalDisplayRenderNode logicalDisplayRenderNode(id, config);
    logicalDisplayRenderNode.UpdateOffscreenRenderParams(GetData<bool>());
    logicalDisplayRenderNode.UpdateRotation();
    logicalDisplayRenderNode.GetSortedChildren();
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
    RSLogicalDisplayRenderNode logicalDisplayRenderNode(id, config);
    auto surface = std::make_shared<OHOS::Rosen::RSRenderNode>(id);
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
    RSLogicalDisplayRenderNode logicalDisplayRenderNode(id, config);
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
    RSLogicalDisplayRenderNode logicalDisplayRenderNode(id, config);
    auto oldScbPids = std::vector<int32_t>();
    int32_t currentScbPid = GetData<int32_t>();
    logicalDisplayRenderNode.SetScbNodePid(oldScbPids, currentScbPid);
    logicalDisplayRenderNode.GetSortedChildren();
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
    RSLogicalDisplayRenderNode logicalDisplayRenderNode(id, config);
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
    auto logicalDisplayRenderNode = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    auto node = std::static_pointer_cast<RSRenderNode>(logicalDisplayRenderNode);
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
