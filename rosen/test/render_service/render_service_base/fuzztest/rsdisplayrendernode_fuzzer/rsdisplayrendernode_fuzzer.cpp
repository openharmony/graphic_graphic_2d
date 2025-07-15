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

#include "pipeline/rs_screen_render_node.h"
#include "render_thread/rs_render_thread_visitor.cpp"
#include "screen_manager/screen_types.h"
#include "params/rs_screen_render_params.h"

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

    NodeId id = GetData<uint64_t>();
    ScreenId screenId = GetData<uint64_t>();
    int32_t offsetX = GetData<int32_t>();
    uint32_t refreshRate = GetData<uint32_t>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    CompositeType type = CompositeType::UNI_RENDER_COMPOSITE;
    auto node = std::make_shared<RSBaseRenderNode>(id);
    std::vector<std::shared_ptr<RSRenderNode>> vec;
    std::shared_ptr<RSRenderThreadVisitor> visitor;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    std::string name = "text";
    std::vector<std::string> windowsName;
    windowsName.push_back(name);
    RectI dirtyShadow;
    Drawing::Matrix matrix;
    RSScreenRenderNode rsScreenRenderNode(id, screenId, context);
    rsScreenRenderNode.stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(id);
    auto sptr = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    rsScreenRenderNode.CollectSurface(node, vec, true, true);
    rsScreenRenderNode.QuickPrepare(visitor);
    rsScreenRenderNode.Prepare(visitor);
    rsScreenRenderNode.Process(visitor);
    rsScreenRenderNode.SetCompositeType(type);
    rsScreenRenderNode.SetMirrorSource(sptr);
    rsScreenRenderNode.SkipFrame(refreshRate, skipFrameInterval);
    rsScreenRenderNode.UpdateDisplayDirtyManager(offsetX);
    rsScreenRenderNode.UpdateSurfaceNodePos(id, dirtyShadow);
    rsScreenRenderNode.SetInitMatrix(matrix);
    auto cacheImgForCapture = std::make_shared<Drawing::Image>();
    rsScreenRenderNode.SetOffScreenCacheImgForCapture(cacheImgForCapture);
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

    NodeId id = GetData<uint64_t>();
    ScreenId screenId = GetData<uint64_t>();
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNode rsScreenRenderNode(id, screenId, context);
    rsScreenRenderNode.stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(id);
    rsScreenRenderNode.UpdateScreenRenderParams();
    rsScreenRenderNode.SetDisplayGlobalZOrder(GetData<uint64_t>());
    rsScreenRenderNode.SetMainAndLeashSurfaceDirty(GetData<bool>());
    rsScreenRenderNode.SetHDRPresent(GetData<bool>());
    rsScreenRenderNode.GetSortedChildren();
    rsScreenRenderNode.GetDisappearedSurfaceRegionBelowCurrent(GetData<uint64_t>());
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

    NodeId id = GetData<uint64_t>();
    ScreenId screenId = GetData<uint64_t>();
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNode rsScreenRenderNode(id, screenId, context);
    auto surface = std::make_shared<OHOS::Rosen::RSRenderNode>(id);
    rsScreenRenderNode.RecordMainAndLeashSurfaces(surface);
    rsScreenRenderNode.HandleCurMainAndLeashSurfaceNodes();
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

    NodeId id = GetData<uint64_t>();
    ScreenId screenId = GetData<uint64_t>();
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNode rsScreenRenderNode(id, screenId, context);
    bool state = GetData<bool>();
    rsScreenRenderNode.UpdateZoomState(state);
    rsScreenRenderNode.IsZoomStateChange();
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
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNode rsScreenRenderNode(id, screenId, context);
    auto oldScbPids = std::vector<int32_t>();
    rsScreenRenderNode.GetSortedChildren();
    rsScreenRenderNode.isNeedWaitNewScbPid_ = false;
    rsScreenRenderNode.GetSortedChildren();
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
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNode rsScreenRenderNode(id, screenId, context);
    rsScreenRenderNode.stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(id);
    float brightnessRatio = GetData<float>();
    rsScreenRenderNode.SetBrightnessRatio(brightnessRatio);
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
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto rsScreenRenderNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    rsScreenRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(id);
    auto node = std::static_pointer_cast<RSRenderNode>(rsScreenRenderNode);
    rsScreenRenderNode->renderDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node);
    rsScreenRenderNode->OnSync();
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
