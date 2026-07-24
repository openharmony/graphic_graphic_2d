/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ui/rs_canvas_drawing_node.h"

#include "command/rs_canvas_drawing_node_command.h"
#include "common/rs_obj_geometry.h"
#include "modifier_ng/rs_modifier_ng.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_ui_context.h"

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include <ffrt.h>
#include "ipc_callbacks/rs_canvas_surface_buffer_callback_stub.h"
#include "platform/ohos/backend/surface_buffer_utils.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_canvas_callback_router.h"
#endif

namespace OHOS {
namespace Rosen {
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
bool RSCanvasDrawingNode::preAllocateDmaCcm_ = true;
#endif
namespace {
constexpr int EDGE_WIDTH_LIMIT = 1000;
#ifdef RS_MODIFIERS_DRAW_ENABLE
const bool HYBRID_ENABLED = RSSystemProperties::GetHybridRenderCanvasEnabled();
#endif
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
const bool PRE_ALLOCATE_DMA_ENABLED =
    RSUniRenderJudgement::IsUniRender() && RSSystemProperties::GetCanvasDrawingNodePreAllocateDmaEnabled();
const bool RENDER_DMA_ENABLED =
    RSUniRenderJudgement::IsUniRender() && RSSystemProperties::GetCanvasDrawingNodeRenderDmaEnabled();

// Global Canvas SurfaceBuffer callback implementation
// This callback is registered once per process and routes notifications to specific nodes
class GlobalCanvasSurfaceBufferCallback : public RSCanvasSurfaceBufferCallbackStub {
public:
    void OnCanvasSurfaceBufferChanged(
        NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex) override
    {
        auto node = RSCanvasCallbackRouter::GetInstance().RouteToNode(nodeId);
        if (node == nullptr) {
            RS_LOGE("GlobalCanvasSurfaceBufferCallback: Node not found or destroyed, nodeId=%{public}" PRIu64, nodeId);
            return;
        }

        node->OnSurfaceBufferChanged(buffer, resetSurfaceIndex);
    }
};
#endif
} // namespace

RSCanvasDrawingNode::RSCanvasDrawingNode(
    bool isRenderServiceNode, bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext)
    : RSCanvasNode(isRenderServiceNode, isTextureExportNode, rsUIContext)
{}

RSCanvasDrawingNode::~RSCanvasDrawingNode()
{
#ifdef RS_MODIFIERS_DRAW_ENABLE
    if (HYBRID_ENABLED) {
        auto uiContext = GetRSUIContext();
        auto canvasModifiersDrawAgent = uiContext != nullptr ? uiContext->GetCanvasModifiersDrawAgent() : nullptr;
        if (canvasModifiersDrawAgent != nullptr) {
            std::weak_ptr<RSRenderInterface> weakInterface = uiContext->GetRSRenderInterface();
            canvasModifiersDrawAgent->OnNodeRelease(GetId(), weakInterface);
        }
        return;
    }
#endif
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    if (preAllocateDmaCcm_ && (PRE_ALLOCATE_DMA_ENABLED || RENDER_DMA_ENABLED)) {
        auto id = GetId();
        // Unregister from callback router
        RSCanvasCallbackRouter::GetInstance().UnregisterNode(id);
        bool needReleaseDmaBuffer = false;
        // Clear DMA buffer reference (releases DMA memory from app process)
        {
            std::lock_guard<ffrt::mutex> lock(*surfaceBufferMutex_);
            needReleaseDmaBuffer = resetSurfaceIndex_ > 0;
            canvasSurfaceBuffer_ = nullptr;
            resetSurfaceIndex_ = 0;
        }
        if (needReleaseDmaBuffer) {
            ffrt::submit([id]() {
                if (RSInterfaces::GetInstance().SubmitCanvasPreAllocatedBuffer(id, nullptr, 0) !=
                    StatusCode::SUCCESS) {
                    RS_LOGE("Release RSCanvasDrawingNode, notify RS to clear DMA cache fail.");
                }
            });
        }
    }
#endif
}

RSCanvasDrawingNode::SharedPtr RSCanvasDrawingNode::Create(
    bool isRenderServiceNode, bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext)
{
    SharedPtr node(new RSCanvasDrawingNode(isRenderServiceNode, isTextureExportNode, rsUIContext));
    if (rsUIContext != nullptr) {
        rsUIContext->GetMutableNodeMap().RegisterNode(node);
    } else {
        RSNodeMap::MutableInstance().RegisterNode(node);
    }

    bool hybridEnabled = false;
#ifdef RS_MODIFIERS_DRAW_ENABLE
    hybridEnabled = HYBRID_ENABLED;
#endif
    if (hybridEnabled) {
#ifdef RS_MODIFIERS_DRAW_ENABLE
        auto uiContext = node->GetRSUIContext();
        auto canvasModifiersDrawAgent = uiContext != nullptr ? uiContext->GetCanvasModifiersDrawAgent() : nullptr;
        if (canvasModifiersDrawAgent != nullptr) {
            static std::once_flag flag;
            std::call_once(flag, [canvasModifiersDrawAgent]() {
                canvasModifiersDrawAgent->QueryMaxGpuBufferSize(maxGpuSupportedWidth_, maxGpuSupportedHeight_);
            });
            std::weak_ptr<RSRenderInterface> weakInterface = uiContext->GetRSRenderInterface();
            canvasModifiersDrawAgent->OnNodeCreate(node->GetId(), weakInterface);
        }
#endif
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
        preAllocateDmaCcm_ = false;
    } else if (preAllocateDmaCcm_ && (PRE_ALLOCATE_DMA_ENABLED || RENDER_DMA_ENABLED)) {
        node->surfaceBufferMutex_ = std::make_shared<ffrt::mutex>();
        // Register node in Canvas Callback Router for SurfaceBuffer callback routing
        RSCanvasCallbackRouter::GetInstance().RegisterNode(node->GetId(), std::weak_ptr<RSCanvasDrawingNode>(node));
        // Register global callback once per process (thread-safe with std::call_once)
        static std::once_flag callbackFlag;
        std::call_once(callbackFlag, []() {
            sptr<RSICanvasSurfaceBufferCallback> globalCallback = new GlobalCanvasSurfaceBufferCallback();
            RSInterfaces::GetInstance().RegisterCanvasCallback(globalCallback);
        });
#endif
    }

    std::unique_ptr<RSCommand> command =
        std::make_unique<RSCanvasDrawingNodeCreate>(node->GetId(), isTextureExportNode);
    node->AddCommand(command, node->IsRenderServiceNode());
    node->SetUIContextToken();
    return node;
}

void RSCanvasDrawingNode::CreateRenderNode()
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSCanvasDrawingNodeCreate>(GetId(), isTextureExportNode_);
    AddCommand(command, IsRenderServiceNode());
}

bool RSCanvasDrawingNode::ResetSurface(int width, int height)
{
    if (width > EDGE_WIDTH_LIMIT) {
        ROSEN_LOGI("RSCanvasDrawingNode::ResetSurface id:%{public}" PRIu64 "width:%{public}d height:%{public}d",
            GetId(), width, height);
    }

    uint32_t resetSurfaceIndex = 0;
    bool hybridEnabled = false;
#ifdef RS_MODIFIERS_DRAW_ENABLE
    hybridEnabled = HYBRID_ENABLED;
#endif
    if (hybridEnabled) {
#ifdef RS_MODIFIERS_DRAW_ENABLE
        ResetSurfaceForClientRender(width, height);
#endif
    } else {
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
        if (preAllocateDmaCcm_ && (PRE_ALLOCATE_DMA_ENABLED || RENDER_DMA_ENABLED)) {
            std::lock_guard<ffrt::mutex> lock(*surfaceBufferMutex_);
            resetSurfaceIndex_ = GenerateResetSurfaceIndex();
            resetSurfaceIndex = resetSurfaceIndex_;
            canvasSurfaceBuffer_ = nullptr;
        }
        if (preAllocateDmaCcm_ && PRE_ALLOCATE_DMA_ENABLED) {
            if (isNeverOnTree_) {
                resetSurfaceParams_ = std::make_unique<ResetSurfaceParams>(width, height, resetSurfaceIndex);
            } else {
                std::weak_ptr<RSCanvasDrawingNode> weakNode =
                    std::static_pointer_cast<RSCanvasDrawingNode>(shared_from_this());
                ffrt::submit([weakNode, nodeId = GetId(), width, height, resetSurfaceIndex]() {
                    PreAllocateDMABuffer(weakNode, nodeId, width, height, resetSurfaceIndex);
                });
            }
        }
#endif
    }
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSCanvasDrawingNodeResetSurface>(GetId(), width, height, resetSurfaceIndex);
    if (AddCommand(command, IsRenderServiceNode())) {
        return true;
    }
    return false;
}

#ifdef RS_MODIFIERS_DRAW_ENABLE
void RSCanvasDrawingNode::ResetSurfaceForClientRender(int width, int height)
{
    sizeOutOfGpuLimit_ = width > static_cast<int>(maxGpuSupportedWidth_) ||
        height > static_cast<int>(maxGpuSupportedHeight_) || width <= 0 || height <= 0;
    if (auto uiContext = GetRSUIContext()) {
        if (auto canvasModifiersDrawAgent = uiContext->GetCanvasModifiersDrawAgent()) {
            uiContext->OnCanvasDrawingNodeUpdate();
            canvasModifiersDrawAgent->ResetSurface(
                GetId(), width, height, sizeOutOfGpuLimit_, uiContext->GetColorSpace());
        }
    }
}
#endif

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
uint32_t RSCanvasDrawingNode::GenerateResetSurfaceIndex()
{
    static std::atomic<uint32_t> currentResetSurfaceIndex_ = 1;
    return currentResetSurfaceIndex_.fetch_add(1, std::memory_order_relaxed);
}

void RSCanvasDrawingNode::PreAllocateDMABuffer(
    std::weak_ptr<RSCanvasDrawingNode> weakNode, NodeId nodeId, int width, int height, uint32_t resetSurfaceIndex)
{
    if (!CheckNodeAndSurfaceBufferState(weakNode, nodeId, resetSurfaceIndex)) {
        RS_LOGE("PreAllocateDMABuffer: CheckNodeAndSurfaceBufferState fail, skip allocating, nodeId=%{public}" PRIu64,
            nodeId);
        return;
    }

    auto buffer = SurfaceBufferUtils::CreateCanvasSurfaceBuffer(getpid(), width, height);
    if (buffer == nullptr) {
        RS_LOGE("PreAllocateDMABuffer: Pre-allocated DMA buffer allocation failed, nodeId=%{public}" PRIu64, nodeId);
        return;
    }

    if (!CheckNodeAndSurfaceBufferState(weakNode, nodeId, resetSurfaceIndex)) {
        RS_LOGE("PreAllocateDMABuffer: CheckNodeAndSurfaceBufferState fail, skip submit to RS, nodeId=%{public}" PRIu64,
            nodeId);
        return;
    }

    auto result = RSInterfaces::GetInstance().SubmitCanvasPreAllocatedBuffer(nodeId, buffer, resetSurfaceIndex);
    if (!CheckNodeAndSurfaceBufferState(weakNode, nodeId, resetSurfaceIndex)) {
        RS_LOGE(
            "PreAllocateDMABuffer: CheckNodeAndSurfaceBufferState fail, skip update, nodeId=%{public}" PRIu64, nodeId);
        return;
    }

    auto node = weakNode.lock();
    if (node == nullptr) {
        RS_LOGE("PreAllocateDMABuffer: null node, nodeId=%{public}" PRIu64, nodeId);
        return;
    }
    if (result == StatusCode::SUCCESS) {
        std::lock_guard<ffrt::mutex> lock(*node->surfaceBufferMutex_);
        node->canvasSurfaceBuffer_ = buffer;
    } else {
        if (result == FEATURE_DISABLED) {
            preAllocateDmaCcm_ = false;
            RSCanvasCallbackRouter::GetInstance().UnregisterNode(nodeId);
        }
        // !!! Do not set canvasSurfaceBuffer_ to nullptr, it was set to nullptr in function ResetSurface,
        // if it's not nullptr at this time, then it must have been changed by callback.
        RS_LOGE("PreAllocateDMABuffer: Pre-allocated DMA buffer submitted to RS fail, nodeId=%{public}" PRIu64
                ", width=%{public}d, height=%{public}d, resetSurfaceIndex=%{public}u, result=%{public}d",
            nodeId, width, height, resetSurfaceIndex, result);
    }
}

bool RSCanvasDrawingNode::CheckNodeAndSurfaceBufferState(
    std::weak_ptr<RSCanvasDrawingNode> weakNode, NodeId nodeId, uint32_t resetSurfaceIndex)
{
    auto node = weakNode.lock();
    if (node == nullptr) {
        RS_LOGE("CheckNodeAndSurfaceBufferState: Node destroyed, nodeId=%{public}" PRIu64, nodeId);
        return false;
    }
    {
        std::lock_guard<ffrt::mutex> lock(*node->surfaceBufferMutex_);
        if (resetSurfaceIndex != node->resetSurfaceIndex_) {
            RS_LOGW("CheckNodeAndSurfaceBufferState: Ignore stale resetSurfaceIndex, nodeId=%{public}" PRIu64, nodeId);
            return false;
        }
        if (node->canvasSurfaceBuffer_ != nullptr) {
            RS_LOGW("CheckNodeAndSurfaceBufferState: buffer already exists, nodeId=%{public}" PRIu64, nodeId);
            return false;
        }
    }
    return true;
}

void RSCanvasDrawingNode::OnSurfaceBufferChanged(sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex)
{
    auto nodeId = GetId();
    std::lock_guard<ffrt::mutex> lock(*surfaceBufferMutex_);
    if (resetSurfaceIndex != resetSurfaceIndex_) {
        RS_LOGE("OnSurfaceBufferChanged: resetSurfaceIndex expired, nodeId=%{public}" PRIu64, nodeId);
        return;
    }
    canvasSurfaceBuffer_ = buffer;
}
#endif

void RSCanvasDrawingNode::CreateRenderNodeForTextureExportSwitch()
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasDrawingNodeCreate>(GetId(), isTextureExportNode_);
    if (IsRenderServiceNode()) {
        hasCreateRenderNodeInRS_ = true;
    } else {
        hasCreateRenderNodeInRT_ = true;
    }
    AddCommand(command, IsRenderServiceNode());
}

bool RSCanvasDrawingNode::GetBitmap(Drawing::Bitmap& bitmap,
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList, const Drawing::Rect* rect)
{
    bool clientRender = false;
#ifdef RS_MODIFIERS_DRAW_ENABLE
    if ((clientRender = HYBRID_ENABLED && !sizeOutOfGpuLimit_)) {
        auto uiContext = GetRSUIContext();
        auto canvasModifiersDrawAgent = uiContext != nullptr ? uiContext->GetCanvasModifiersDrawAgent() : nullptr;
        if (canvasModifiersDrawAgent == nullptr || !canvasModifiersDrawAgent->GetBitmap(GetId(), bitmap)) {
            return false;
        }
    }
#endif
    if (!clientRender) {
        if (IsRenderServiceNode()) {
            auto rsUIContext = GetRSUIContext();
            if (rsUIContext == nullptr || rsUIContext->GetRSRenderInterface() == nullptr) {
                ROSEN_LOGE("RSCanvasDrawingNode::GetBitmap uiContext is nullptr");
                return false;
            }
            bool ret = rsUIContext->GetRSRenderInterface()->GetBitmap(GetId(), bitmap);
            if (!ret) {
                ROSEN_LOGE("RSCanvasDrawingNode::GetBitmap GetBitmap failed");
                return ret;
            }
        } else {
            auto node =
                RSRenderThread::Instance().GetContext().GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(GetId());
            if (node == nullptr) {
                RS_LOGE("RSCanvasDrawingNode::GetBitmap cannot find NodeId: [%{public}" PRIu64 "]", GetId());
                return false;
            }
            if (node->GetType() != RSRenderNodeType::CANVAS_DRAWING_NODE) {
                RS_LOGE("RSCanvasDrawingNode::GetBitmap RenderNodeType != RSRenderNodeType::CANVAS_DRAWING_NODE");
                return false;
            }
            auto getBitmapTask = [&node, &bitmap]() { bitmap = node->GetBitmap(); };
            RSRenderThread::Instance().PostSyncTask(getBitmapTask);
            if (bitmap.IsValid()) {
                return false;
            }
        }
    }
    if (drawCmdList == nullptr) {
        RS_LOGD("RSCanvasDrawingNode::GetBitmap drawCmdList == nullptr");
    } else {
        Drawing::Canvas canvas;
        canvas.Bind(bitmap);
        drawCmdList->Playback(canvas, rect);
    }
    return true;
}

bool RSCanvasDrawingNode::GetPixelmap(std::shared_ptr<Media::PixelMap> pixelmap,
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList, const Drawing::Rect* rect)
{
    if (!pixelmap) {
        RS_LOGE("RSCanvasDrawingNode::GetPixelmap: pixelmap is nullptr");
        return false;
    }
#ifdef RS_MODIFIERS_DRAW_ENABLE
    if (HYBRID_ENABLED && !sizeOutOfGpuLimit_) {
        return GetPixelmapForClientRender(pixelmap, drawCmdList, rect);
    }
#endif
    if (IsRenderServiceNode()) {
        auto rsUIContext = GetRSUIContext();
        if (rsUIContext == nullptr || rsUIContext->GetRSRenderInterface() == nullptr) {
            ROSEN_LOGE("RSCanvasDrawingNode::GetPixelmap uiContext is nullptr");
            return false;
        }
        bool ret = rsUIContext->GetRSRenderInterface()->GetPixelmap(GetId(), pixelmap, rect, drawCmdList);
        if (!ret || !pixelmap) {
            ROSEN_LOGD("RSCanvasDrawingNode::GetPixelmap: GetPixelmap failed");
            return false;
        }
    } else {
        auto node =
            RSRenderThread::Instance().GetContext().GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(GetId());
        if (node == nullptr) {
            RS_LOGE("RSCanvasDrawingNode::GetPixelmap: cannot find NodeId: [%{public}" PRIu64 "]", GetId());
            return false;
        }
        if (node->GetType() != RSRenderNodeType::CANVAS_DRAWING_NODE) {
            RS_LOGE("RSCanvasDrawingNode::GetPixelmap: RenderNodeType != RSRenderNodeType::CANVAS_DRAWING_NODE");
            return false;
        }
        auto modifier = GetModifierByType(ModifierNG::RSModifierType::CONTENT_STYLE);
        auto lastDrawCmdList = modifier != nullptr ? modifier->Getter<Drawing::DrawCmdListPtr>(
            ModifierNG::RSPropertyType::CONTENT_STYLE, nullptr) : nullptr;
        bool ret = false;
        auto getPixelmapTask = [&node, &pixelmap, rect, &ret, &drawCmdList, &lastDrawCmdList]() {
            ret = node->GetPixelmap(pixelmap, rect, UINT32_MAX, drawCmdList, lastDrawCmdList);
        };
        RSRenderThread::Instance().PostSyncTask(getPixelmapTask);
        if (!ret || !pixelmap) {
            return false;
        }
    }
    return true;
}

#ifdef RS_MODIFIERS_DRAW_ENABLE
bool RSCanvasDrawingNode::GetPixelmapForClientRender(
    std::shared_ptr<Media::PixelMap> pixelMap, Drawing::DrawCmdListPtr drawCmdList, const Drawing::Rect* rect)
{
    auto uiContext = GetRSUIContext();
    if (uiContext == nullptr) {
        return false;
    }
    if (auto canvasModifiersDrawAgent = uiContext->GetCanvasModifiersDrawAgent()) {
        return canvasModifiersDrawAgent->GetPixelMap(GetId(), pixelMap, rect, drawCmdList);
    }
    return false;
}
 
void RSCanvasDrawingNode::OnFinishRecording(
    Drawing::DrawCmdListPtr& drawCmdList, ModifierNG::RSModifierType modifierType)
{
    if (modifierType != ModifierNG::RSModifierType::CONTENT_STYLE) {
        RSCanvasNode::OnFinishRecording(drawCmdList, modifierType);
        return;
    }
 
    if (RenderInClient(drawCmdList)) {
        drawCmdList = nullptr;
    }
    RSCanvasNode::OnFinishRecording(drawCmdList, modifierType);
}
 
bool RSCanvasDrawingNode::RenderInClient(Drawing::DrawCmdListPtr drawCmdList)
{
    if (!HYBRID_ENABLED || sizeOutOfGpuLimit_) {
        return false;
    }
    auto uiContext = GetRSUIContext();
    if (uiContext == nullptr) {
        return false;
    }
    auto canvasModifiersDrawAgent = uiContext->GetCanvasModifiersDrawAgent();
    if (canvasModifiersDrawAgent == nullptr) {
        return false;
    }

    if (drawCmdList != nullptr && !drawCmdList->IsEmpty()) {
        std::unique_lock<std::mutex> uiLock(uiContext->uiMutex_);
        uiContext->uiCV_.wait(uiLock, [uiContext] { return !uiContext->canBlockUIThread_; });
        canvasModifiersDrawAgent->UpdateCanvasContent(GetId(), drawCmdList);
    }
    return true;
}
#endif // RS_MODIFIERS_DRAW_ENABLE

void RSCanvasDrawingNode::RegisterNodeMap()
{
    auto rsContext = GetRSUIContext();
    if (rsContext == nullptr) {
        return;
    }
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.RegisterNode(shared_from_this());
}

void RSCanvasDrawingNode::SetIsOnTheTree(bool onTheTree)
{
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    if (isNeverOnTree_ && onTheTree) {
        isNeverOnTree_ = false;
        if (resetSurfaceParams_ != nullptr) {
            std::weak_ptr<RSCanvasDrawingNode> weakNode =
                std::static_pointer_cast<RSCanvasDrawingNode>(shared_from_this());
            ffrt::submit(
                [weakNode, nodeId = GetId(), width = resetSurfaceParams_->width, height = resetSurfaceParams_->height,
                    resetSurfaceIndex = resetSurfaceParams_->resetSurfaceIndex]() {
                    PreAllocateDMABuffer(weakNode, nodeId, width, height, resetSurfaceIndex);
                });
            resetSurfaceParams_ = nullptr;
        }
    }
#endif
    RSNode::SetIsOnTheTree(onTheTree);
}

bool RSCanvasDrawingNode::SetNodeState(RSNodeState state)
{
#ifdef RS_MODIFIERS_DRAW_ENABLE
    if (!HYBRID_ENABLED || sizeOutOfGpuLimit_) {
        return false;
    }
    if (GetNodeState() == state) {
        return true;
    }
    bool ret = RSNode::SetNodeState(state);
    if (!ret) {
        return false;
    }
    if (auto uiContext = GetRSUIContext()) {
        if (state == RSNodeState::ACTIVE) {
            uiContext->OnCanvasDrawingNodeUpdate();
        }
        if (auto canvasModifiersDrawAgent = uiContext->GetCanvasModifiersDrawAgent()) {
            canvasModifiersDrawAgent->OnNodeStateChanged(GetId(), state);
        }
    }
    return true;
#endif
    return false;
}
 
bool RSCanvasDrawingNode::IsSkipContentModifierDraw()
{
#ifdef RS_MODIFIERS_DRAW_ENABLE
    return skipContentModifierDraw_;
#endif
    return false;
}
 
void RSCanvasDrawingNode::SetSkipContentModifierDraw(bool skip)
{
#ifdef RS_MODIFIERS_DRAW_ENABLE
    skipContentModifierDraw_ = skip;
#endif
}
} // namespace Rosen
} // namespace OHOS