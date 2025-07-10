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

#include "common/rs_optional_trace.h"
#include "pipeline/rs_render_node_gc.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t NODE_ALLOCATOR_LIMIT = 512;
constexpr uint32_t DRAWABLE_ALLOCATOR_LIMIT = 512;
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
    RS_LOGD("AddNodeToAllocator, pool size:%{public}zu", nodeAllocator_.size());
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
    RS_LOGD("AddDrawableToAllocator, pool size:%{public}zu", drawableAllocator_.size());
    drawableAllocatorSpinlock_.unlock();
    return true;
}

std::shared_ptr<RSCanvasRenderNode> RSRenderNodeAllocator::CreateRSCanvasRenderNode(NodeId id,
    const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
{
    nodeAllocatorSpinlock_.lock();
    if (nodeAllocator_.empty()) {
        nodeAllocatorSpinlock_.unlock();
        RS_OPTIONAL_TRACE_NAME("CreateRSCanvasRenderNode nodeAllocator is empty.");
        return std::shared_ptr<RSCanvasRenderNode>(new RSCanvasRenderNode(id,
            context, isTextureExportNode), RSRenderNodeGC::NodeDestructor);
    }
    auto front = nodeAllocator_.front();
    nodeAllocator_.pop();
    nodeAllocatorSpinlock_.unlock();
    RS_LOGD("CreateRSCanvasRenderNode in pool, pool size:%{public}zu, id:%{public}" PRIu64 "",
        nodeAllocator_.size(), id);
    return std::shared_ptr<RSCanvasRenderNode>(new (front)RSCanvasRenderNode(id,
        context, isTextureExportNode), RSRenderNodeGC::NodeDestructor);
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
    RS_LOGD("CreateRSRenderNodeDrawable in pool, pool size:%{public}zu", nodeAllocator_.size());
    drawableAllocatorSpinlock_.unlock();
    return generator(node, front);
}

} // namespace Rosen
} // namespace OHOS