/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_node_allocator.h"

#include <cstdlib>
#include <cstring>
 
#include "platform/common/rs_system_properties.h"
#include "common/rs_optional_trace.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_scb_node_memory_pool.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t NODE_ALLOCATOR_LIMIT = 512;
constexpr uint32_t DRAWABLE_ALLOCATOR_LIMIT = 512;
constexpr uint32_t SCB_NODE_FREE_QUEUE_LIMIT = 32;
}

RSRenderNodeAllocator& RSRenderNodeAllocator::Instance()
{
    static RSRenderNodeAllocator instance;
    return instance;
}

bool RSRenderNodeAllocator::AddNodeToAllocator(RSRenderNode* ptr)
{
    if (ptr == nullptr || ptr->GetType() != RSRenderNodeType::CANVAS_NODE) {
        return false;
    }

    nodeAllocatorSpinlock_.lock();
    if (nodeAllocator_.size() >= NODE_ALLOCATOR_LIMIT) {
        nodeAllocatorSpinlock_.unlock();
        RS_OPTIONAL_TRACE_NAME("AddNodeToAllocator nodeAllocator is full.");
        return false;
    }
    ptr->~RSRenderNode();
    nodeAllocator_.push(ptr);
    RS_LOGD_IF(DEBUG_NODE, "AddNodeToAllocator, pool size:%{public}zu", nodeAllocator_.size());
    nodeAllocatorSpinlock_.unlock();
    return true;
}

bool RSRenderNodeAllocator::AddDrawableToAllocator(RSRenderNodeAllocator::DrawablePtr ptr)
{
    if (ptr == nullptr || ptr->GetNodeType() != RSRenderNodeType::CANVAS_NODE) {
        return false;
    }

    if (ptr->GetDrawableType() != RSRenderNodeDrawableType::CANVAS_NODE_DRAWABLE) {
        RS_LOGI("AddDrawableToAllocator, not canvas node drawable.");
        return false;
    }
    drawableAllocatorSpinlock_.lock();
    if (drawableAllocator_.size() >= DRAWABLE_ALLOCATOR_LIMIT) {
        drawableAllocatorSpinlock_.unlock();
        RS_OPTIONAL_TRACE_NAME("AddDrawableToAllocator drawableAllocator is full.");
        return false;
    }
    ptr->~RSRenderNodeDrawableAdapter();
    drawableAllocator_.push(ptr);
    RS_LOGD_IF(DEBUG_NODE, "AddDrawableToAllocator, pool size:%{public}zu", drawableAllocator_.size());
    drawableAllocatorSpinlock_.unlock();
    return true;
}

std::shared_ptr<RSCanvasRenderNode> RSRenderNodeAllocator::CreateRSCanvasRenderNode(NodeId id,
    const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
{
    std::shared_ptr<RSCanvasRenderNode> node = nullptr;
    RS_OPTIONAL_TRACE_NAME_FMT("CanvasNodeCreate legacy, id=%" PRIu64, id);
    nodeAllocatorSpinlock_.lock();
    if (nodeAllocator_.empty()) {
        nodeAllocatorSpinlock_.unlock();
        RS_OPTIONAL_TRACE_NAME("CreateRSCanvasRenderNode nodeAllocator is empty.");
        node = std::shared_ptr<RSCanvasRenderNode>(new RSCanvasRenderNode(id,
            context, isTextureExportNode), RSRenderNodeGC::NodeDestructor);
    } else {
        auto front = nodeAllocator_.front();
        nodeAllocator_.pop();
        nodeAllocatorSpinlock_.unlock();
        RS_LOGD_IF(DEBUG_NODE, "CreateRSCanvasRenderNode in pool, pool size:%{public}zu, id:%{public}" PRIu64 "",
            nodeAllocator_.size(), id);
        node = std::shared_ptr<RSCanvasRenderNode>(new (front)RSCanvasRenderNode(id,
            context, isTextureExportNode), RSRenderNodeGC::NodeDestructor);
    }
    if (node && node->IsNodeMemClearEnable()) {
        RSRenderNodeGC::Instance().SetIsOnTheTree(node->GetId(), node, node->IsOnTheTree());
    }
    return node;
}

RSRenderNodeAllocator::DrawablePtr RSRenderNodeAllocator::CreateRSRenderNodeDrawable(
    std::shared_ptr<const RSRenderNode> node,
    std::function<RSRenderNodeAllocator::DrawablePtr(std::shared_ptr<const RSRenderNode> node,
        RSRenderNodeAllocator::DrawablePtr front)> generator)
{
    drawableAllocatorSpinlock_.lock();
    if (drawableAllocator_.empty()) {
        drawableAllocatorSpinlock_.unlock();
        RS_OPTIONAL_TRACE_NAME("CreateRSRenderNodeDrawable drawableAllocator is empty.");
        return generator(node, nullptr);
    }
    auto front = drawableAllocator_.front();
    drawableAllocator_.pop();
    RS_LOGD_IF(DEBUG_NODE, "CreateRSRenderNodeDrawable in pool, pool size:%{public}zu", nodeAllocator_.size());
    drawableAllocatorSpinlock_.unlock();
    return generator(node, front);
}

std::shared_ptr<RSCanvasRenderNode> RSRenderNodeAllocator::CreateScbCanvasRenderNode(NodeId id,
    const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
{
    std::shared_ptr<RSCanvasRenderNode> node = nullptr;
    void* mem = nullptr;
 
    // 1. Try to reuse a previously destructed node from the SCB free queue.
    bool reusedFromFreeQueue = false;
    scbNodeSpinlock_.lock();
    if (!scbNodeFreeQueue_.empty()) {
        auto front = scbNodeFreeQueue_.front();
        scbNodeFreeQueue_.pop();
        scbNodeSpinlock_.unlock();
        mem = front;
        reusedFromFreeQueue = true;
    } else {
        scbNodeSpinlock_.unlock();
        // 2. Allocate a new slot from the contiguous memory pool.
        mem = RSScbNodeMemoryPool::Instance().Allocate();
    }
 
    if (mem != nullptr) {
        if (reusedFromFreeQueue) {
            RS_OPTIONAL_TRACE_NAME_FMT("ScbNodeCreate reuse, id=%" PRIu64 ", freeQueue=%zu, used=%u/%u",
                id, GetScbFreeQueueSize(), RSScbNodeMemoryPool::Instance().GetTotalUsed(),
                RSScbNodeMemoryPool::Instance().GetTotalSlots());
        } else {
            RS_OPTIONAL_TRACE_NAME_FMT("ScbNodeCreate alloc, id=%" PRIu64 ", freeQueue=%zu, used=%u/%u",
                id, GetScbFreeQueueSize(), RSScbNodeMemoryPool::Instance().GetTotalUsed(),
                RSScbNodeMemoryPool::Instance().GetTotalSlots());
        }
        node = std::shared_ptr<RSCanvasRenderNode>(
            new (mem) RSCanvasRenderNode(id, context, isTextureExportNode),
            RSRenderNodeGC::NodeDestructor);
    } else {
        // 3. Pool exhausted — fallback to heap allocation with the standard destructor.
        RS_OPTIONAL_TRACE_NAME_FMT("ScbNodeCreate heapFallback, id=%" PRIu64 ", used=%u/%u",
            id, RSScbNodeMemoryPool::Instance().GetTotalUsed(),
            RSScbNodeMemoryPool::Instance().GetTotalSlots());
        RS_LOGW("CreateScbCanvasRenderNode fallback to heap, id=%{public}" PRIu64 ", poolUsed=%{public}u, "
            "poolMax=%{public}u", id, RSScbNodeMemoryPool::Instance().GetTotalUsed(),
            RSScbNodeMemoryPool::Instance().GetTotalSlots());
        node = std::shared_ptr<RSCanvasRenderNode>(
            new RSCanvasRenderNode(id, context, isTextureExportNode),
            RSRenderNodeGC::NodeDestructor);
    }
 
    if (node && node->IsNodeMemClearEnable()) {
        RSRenderNodeGC::Instance().SetIsOnTheTree(node->GetId(), node, node->IsOnTheTree());
    }
    return node;
}
 
bool RSRenderNodeAllocator::AddScbNodeToPool(RSRenderNode* ptr)
{
    if (ptr == nullptr || ptr->GetType() != RSRenderNodeType::CANVAS_NODE) {
        return false;
    }
    if (!RSScbNodeMemoryPool::Instance().Owns(ptr)) {
        return false;
    }
    NodeId nodeId = ptr->GetId();
    scbNodeSpinlock_.lock();
    if (scbNodeFreeQueue_.size() >= SCB_NODE_FREE_QUEUE_LIMIT) {
        scbNodeSpinlock_.unlock();
        RS_LOGD_IF(DEBUG_NODE, "AddScbNodeToPool queue full, id=%{public}" PRIu64 ", queueSize=%{public}zu",
            nodeId, scbNodeFreeQueue_.size());
        return false;
    }
    ptr->~RSRenderNode();
    scbNodeFreeQueue_.push(ptr);
    scbNodeSpinlock_.unlock();
    return true;
}
 
bool RSRenderNodeAllocator::IsScbNodePool(const void* ptr) const
{
    return RSScbNodeMemoryPool::Instance().Owns(ptr);
}
 
// Best-effort snapshot for diagnostics/dump only. No locking;
// value may be transiently inconsistent under concurrent Create/Recycle.
size_t RSRenderNodeAllocator::GetScbFreeQueueSize() const
{
    return scbNodeFreeQueue_.size();
}
 

} // namespace Rosen
} // namespace OHOS