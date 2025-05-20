/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "rsrendernode_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_occlusion_config.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/sk_resource_manager.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr size_t STR_LEN = 10;
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
uint32_t g_gcLevelLow = 30;
uint32_t g_gcLevelMid = 100;
uint32_t g_gcLevelHig = 700;
} // namespace

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

struct DrawBuffer {
    int32_t w;
    int32_t h;
};


bool RSBaseRenderNodeFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    NodeId id = GetData<NodeId>();
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    bool isOnTheTree = GetData<bool>();
    RSBaseRenderNode::SharedPtr child = std::make_shared<RSCanvasRenderNode>(id, context);
    int index = GetData<int>();
    bool skipTransition = GetData<bool>();
    std::vector<RSBaseRenderNode::SharedPtr> vec = { child };
    bool isUniRender = GetData<bool>();
    int64_t timestamp = GetData<int64_t>();
    bool flag = GetData<bool>();
    bool recursive = GetData<bool>();
    bool change = GetData<bool>();
    int left = GetData<int>();
    int top = GetData<int>();
    int width = GetData<int>();
    int height = GetData<int>();
    int64_t leftDelayTime = GetData<int64_t>();
    RectI r(left, top, width, height);

    // test
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(id, isOnTheTree, context);
    node->AddChild(child, index);
    node->MoveChild(child, index);
    node->RemoveChild(child, skipTransition);
    node->RemoveFromTree(skipTransition);
    node->AddCrossParentChild(child, index);
    node->CollectSurface(child, vec, isUniRender, false);
    node->SetIsOnTheTree(flag);
    node->Animate(timestamp, leftDelayTime);
    node->SetIsOnTheTree(flag);
    node->HasDisappearingTransition(recursive);
    node->SetTunnelHandleChange(change);
    node->UpdateChildrenOutOfRectFlag(flag);

    return true;
}

bool RSCrossRenderNodeFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    NodeId id = GetData<NodeId>();
    uint64_t screenId = GetData<uint64_t>();
    bool isMirrored = GetData<bool>();
    NodeId mirrorNodeId = GetData<NodeId>();
    RSDisplayNodeConfig config = { screenId, isMirrored, mirrorNodeId };
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSDisplayRenderNode displayNode(id, config, context);

    NodeId childId = GetData<NodeId>();
    NodeId cloneNodeId = GetData<NodeId>();
    int index = GetData<int>();
    RSSurfaceRenderNode::SharedPtr child = std::make_shared<RSSurfaceRenderNode>(childId, context);
    displayNode.AddCrossScreenChild(child, cloneNodeId, index);
    displayNode.RemoveCrossScreenChild(child);

    return true;
}

bool RSCanvasRenderNodeFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // getdata
    NodeId id = GetData<NodeId>();
    DrawBuffer drawBuffer = GetData<DrawBuffer>();
    RSModifierType type = GetData<RSModifierType>();
    std::shared_ptr<Drawing::DrawCmdList> drawCmds = Drawing::DrawCmdList::CreateFromData(
        { &drawBuffer, sizeof(DrawBuffer) }, true);
    Drawing::Canvas tmpCanvas;
    float alpha = GetData<float>();
    RSPaintFilterCanvas canvas(&tmpCanvas, alpha);
    RSCanvasRenderNode node(id);

    // test
    node.UpdateRecording(drawCmds, type);
    node.ProcessRenderBeforeChildren(canvas);
    node.ProcessRenderContents(canvas);
    node.ProcessRenderAfterChildren(canvas);

    return true;
}

bool RSContextFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // getdata
    RSContext rsContext;
    NodeId id = GetData<NodeId>();
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSCanvasRenderNode>(id);
    rsContext.RegisterAnimatingRenderNode(nodePtr);

    return true;
}

bool RSDirtyRegionManagerFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSDirtyRegionManager manager;
    manager.Clear();
    int left = GetData<int>();
    int top = GetData<int>();
    int width = GetData<int>();
    int height = GetData<int>();
    RectI rect(left, top, width, height);
    uint64_t u_id = GetData<uint64_t>();
    NodeId id = static_cast<NodeId>(u_id);
    RSRenderNodeType nodeType = GetData<RSRenderNodeType>();
    DirtyRegionType dirtyType = GetData<DirtyRegionType>();
    std::map<NodeId, RectI> target;
    target.insert(std::pair<NodeId, RectI>(id, rect));

    manager.MergeDirtyRect(rect);
    manager.IntersectDirtyRect(rect);
    manager.GetRectFlipWithinSurface(rect);
    RSDirtyRegionManager::GetPixelAlignedRect(rect, GetData<int32_t>());
    manager.UpdateDirtyByAligned(GetData<int32_t>());
    manager.UpdateDirtyRegionInfoForDfx(id, nodeType, dirtyType, rect);
    manager.GetDirtyRegionInfo(target, nodeType, dirtyType);
    manager.SetBufferAge(GetData<int>());
    manager.SetSurfaceSize(GetData<int32_t>(), GetData<int32_t>());
    manager.IsDebugRegionTypeEnable(GetData<DebugRegionType>());

    return true;
}

bool RSDisplayRenderNodeFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // getdata
    NodeId id = GetData<NodeId>();
    uint64_t screenId = GetData<uint64_t>();
    bool isMirrored = GetData<bool>();
    NodeId mirrorNodeId = GetData<NodeId>();
    RSDisplayNodeConfig config = { screenId, isMirrored, mirrorNodeId };
    int32_t offsetX = GetData<int32_t>();
    int32_t offsetY = GetData<int32_t>();
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    std::shared_ptr<RSBaseRenderNode> node = std::make_shared<RSBaseRenderNode>(id, context);
    std::vector<RSBaseRenderNode::SharedPtr> vec = { node };
    bool isUniRender = GetData<bool>();
    RSDisplayRenderNode::CompositeType type = GetData<RSDisplayRenderNode::CompositeType>();
    bool flag = GetData<bool>();
    RSDisplayRenderNode::SharedPtr displayPtrNode = std::make_shared<RSDisplayRenderNode>(id, config, context);
    bool isMirror = GetData<bool>();
    bool isSecurityDisplay = GetData<bool>();
    uint32_t refreshRate = GetData<uint32_t>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    int32_t bufferage = GetData<int32_t>();
    int left = GetData<int>();
    int top = GetData<int>();
    int width = GetData<int>();
    int height = GetData<int>();
    RectI rect(left, top, width, height);
    RSDisplayRenderNode displayNode(id, config, context);

    displayNode.SetScreenId(screenId);
    displayNode.SetDisplayOffset(offsetX, offsetY);
    displayNode.CollectSurface(node, vec, isUniRender, false);
    displayNode.SetCompositeType(type);
    displayNode.SetForceSoftComposite(flag);
    displayNode.SetMirrorSource(displayPtrNode);
    displayNode.SetIsMirrorDisplay(isMirror);
    displayNode.SetSecurityDisplay(isSecurityDisplay);
    displayNode.SkipFrame(refreshRate, skipFrameInterval);
    displayNode.UpdateDisplayDirtyManager(bufferage);
    displayNode.UpdateSurfaceNodePos(id, rect);

    return true;
}

bool RSDrawCmdListFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    DrawBuffer drawBuffer = GetData<DrawBuffer>();
    float fLeft = GetData<float>();
    float fTop = GetData<float>();
    float fRight = GetData<float>();
    float fBottom = GetData<float>();
    Drawing::Canvas drCanvas;
    Drawing::Rect rect = { fLeft, fTop, fRight, fBottom };
    float alpha = GetData<float>();
    RSPaintFilterCanvas canvas(&drCanvas, alpha);

    // test
    std::shared_ptr<Drawing::DrawCmdList> list = Drawing::DrawCmdList::CreateFromData(
        { &drawBuffer, sizeof(DrawBuffer) }, true);
    list->Playback(drCanvas, &rect);
    list->Playback(canvas, &rect);
    return true;
}

bool RSOcclusionConfigFuzzTes(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSOcclusionConfig config = RSOcclusionConfig::GetInstance();
    std::string win = GetStringFromData(STR_LEN);
    config.IsLeashWindow(win);
    config.IsStartingWindow(win);
    config.IsAlphaWindow(win);
    config.IsDividerBar(win);

    return true;
}

bool RSContextFuzzerTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSContext context;
    NodeId targetId = GetData<NodeId>();
    context.UnregisterAnimatingRenderNode(targetId);

    context.HasActiveNode(nullptr);

    std::shared_ptr<RSRenderNode> node = std::make_shared<RSBaseRenderNode>(targetId);
    context.HasActiveNode(node);

    context.GetClearMoment();
    ClearMemoryMoment moment = GetData<ClearMemoryMoment>();
    context.SetClearMoment(moment);

    RSContext::PurgeType type = GetData<RSContext::PurgeType>();
    context.MarkNeedPurge(moment, type);

    return true;
}

bool RSRenderFrameRateFuzzerTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Rosen::RSRenderFrameRateLinker::ObserverType observer = nullptr;
    std::shared_ptr<RSRenderFrameRateLinker> node = std::make_shared<RSRenderFrameRateLinker>(observer);
    std::shared_ptr<RSRenderFrameRateLinker> node1 = std::make_shared<RSRenderFrameRateLinker>(observer);
    std::shared_ptr<RSRenderFrameRateLinker> node2 = node;
    node2 = node1;
    std::shared_ptr<RSRenderFrameRateLinker> node3 = std::move(node);

    return true;
}

bool RSRenderNodeGcFuzzerTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    NodeId targetId = GetData<NodeId>();
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSBaseRenderNode>(targetId);
    OHOS::Rosen::RSRenderNodeGC::Instance().AddToOffTreeNodeBucket(node);
    NodeId targetId1 = GetData<NodeId>();
    std::shared_ptr<RSRenderNode> node1 = std::make_shared<RSBaseRenderNode>(targetId1);
    OHOS::Rosen::RSRenderNodeGC::Instance().AddToOffTreeNodeBucket(node1);

    OHOS::Rosen::RSRenderNodeGC::Instance().IsBucketQueueEmpty();
    OHOS::Rosen::RSRenderNodeGC::Instance().ReleaseNodeBucket();
    OHOS::Rosen::RSRenderNodeGC::Instance().ReleaseDrawableMemory();
    OHOS::Rosen::RSRenderNodeGC::Instance().ReleaseOffTreeNodeBucket();
    OHOS::Rosen::RSRenderNodeGC::Instance().ReleaseFromTree();

    uint32_t level = GetData<uint32_t>();
    OHOS::Rosen::RSRenderNodeGC::Instance().JudgeGCLevel(level);
    OHOS::Rosen::RSRenderNodeGC::Instance().JudgeGCLevel(g_gcLevelLow);
    OHOS::Rosen::RSRenderNodeGC::Instance().JudgeGCLevel(g_gcLevelMid);
    OHOS::Rosen::RSRenderNodeGC::Instance().JudgeGCLevel(g_gcLevelHig);

    return true;
}

bool RSRenderNodeMapFuzzerTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderNodeMap> nodeMap = std::make_shared<RSRenderNodeMap>();
    nodeMap->GetSize();
    NodeId targetId = GetData<NodeId>();
    nodeMap->IsUIExtensionSurfaceNode(targetId);
    nodeMap->RemoveUIExtensionSurfaceNode(nullptr);

    pid_t pid = GetData<pid_t>();
    nodeMap->MoveRenderNodeMap(nullptr, pid);
    std::shared_ptr<std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>>> subRenderNodeMap;
    nodeMap->MoveRenderNodeMap(subRenderNodeMap, pid);

    nodeMap->TraversalNodesByPid(pid, [](const std::shared_ptr<RSBaseRenderNode>& node) {
        if (node == nullptr) {
            return;
        }
    });

    nodeMap->TraverseCanvasDrawingNodes([](const std::shared_ptr<RSCanvasDrawingRenderNode>& node) {
        if (node == nullptr) {
            return;
        }
    });

    nodeMap->TraverseSurfaceNodes([](const std::shared_ptr<RSSurfaceRenderNode>& node) {
        if (node == nullptr) {
            return;
        }
    });

    nodeMap->TraverseSurfaceNodesBreakOnCondition([](const std::shared_ptr<RSSurfaceRenderNode>& node) {
        if (node == nullptr) {
            return false;
        }

        return true;
    });

    pid_t pidnew = GetData<pid_t>();
    nodeMap->GetSelfDrawingNodeInProcess(pidnew);
    nodeMap->GetSelfDrawSurfaceNameByPid(pidnew);

    return true;
}

bool RSSurfaceCallbackManagerFuzzerTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSSurfaceBufferCallbackManager::Instance().SetRunPolicy([](auto task) {
        return;
    });
    RSSurfaceBufferCallbackManager::Instance().GetSurfaceBufferCallbackSize();
    OHOS::Rosen::Drawing::DrawSurfaceBufferFinishCbData cbdata;
    RSSurfaceBufferCallbackManager::Instance().EnqueueSurfaceBufferId(cbdata);
    RSSurfaceBufferCallbackManager::Instance().RequestNextVSync();
    RSSurfaceBufferCallbackManager::Instance().OnFinish(cbdata);

    OHOS::Rosen::Drawing::DrawSurfaceBufferAfterAcquireCbData cbdata1;
    RSSurfaceBufferCallbackManager::Instance().OnAfterAcquireBuffer(cbdata1);

    uint32_t id = GetData<uint32_t>();
    std::vector<uint32_t> bufferIdVec{id};
    RSSurfaceBufferCallbackManager::Instance().SerializeBufferIdVec(bufferIdVec);
    RSSurfaceBufferCallbackManager::Instance().RunSurfaceBufferCallback();

    return true;
}

bool RSSurfaceHandleFuzzerTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    NodeId targetId = GetData<NodeId>();
    std::shared_ptr<RSSurfaceHandler> surfaceHandler = std::make_shared<RSSurfaceHandler>(targetId);
    surfaceHandler->IncreaseAvailableBuffer();
    surfaceHandler->ReduceAvailableBuffer();

    float order = GetData<float>();
    surfaceHandler->SetGlobalZOrder(order);
    surfaceHandler->GetGlobalZOrder();
    OHOS::Rosen::RSSurfaceHandler::SurfaceBufferEntry buffer;
    surfaceHandler->ConsumeAndUpdateBuffer(buffer);
    surfaceHandler->ConsumeAndUpdateBufferInner(buffer);

    return true;
}

bool RSSkResourceManagerFuzzerTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    SKResourceManager::Instance().DeleteSharedTextureContext(nullptr);
    std::list<std::shared_ptr<Drawing::Surface>> list;
    SKResourceManager::Instance().HaveReleaseableResourceCheck(list);

    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSBaseRenderNodeFuzzTest(data, size);
    OHOS::Rosen::RSContextFuzzTest(data, size);
    OHOS::Rosen::RSCanvasRenderNodeFuzzTest(data, size);
    OHOS::Rosen::RSDirtyRegionManagerFuzzTest(data, size);
    OHOS::Rosen::RSDisplayRenderNodeFuzzTest(data, size);
    OHOS::Rosen::RSDrawCmdListFuzzTest(data, size);
    OHOS::Rosen::RSOcclusionConfigFuzzTes(data, size);

    OHOS::Rosen::RSContextFuzzerTest(data, size);
    OHOS::Rosen::RSRenderFrameRateFuzzerTest(data, size);
    OHOS::Rosen::RSRenderNodeGcFuzzerTest(data, size);
    OHOS::Rosen::RSRenderNodeMapFuzzerTest(data, size);
    OHOS::Rosen::RSSurfaceCallbackManagerFuzzerTest(data, size);
    OHOS::Rosen::RSSurfaceHandleFuzzerTest(data, size);
    OHOS::Rosen::RSSkResourceManagerFuzzerTest(data, size);
    return 0;
}
