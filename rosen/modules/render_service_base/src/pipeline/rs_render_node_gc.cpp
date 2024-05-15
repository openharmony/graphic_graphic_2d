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

#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_render_node.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSRenderNodeGC& RSRenderNodeGC::Instance()
{
    static RSRenderNodeGC instance;
    return instance;
}

void RSRenderNodeGC::NodeDestructor(RSRenderNode* ptr)
{
    RSRenderNodeGC::Instance().NodeDestructorInner(ptr);
}

void RSRenderNodeGC::NodeDestructorInner(RSRenderNode* ptr)
{
    std::lock_guard<std::mutex> lock(nodeMutex_);
    if (ptr && ptr->fallbackAnimationOnDestroy_) {
        ptr->FallbackAnimationsToRoot();
    }
    node_.push_back(ptr);
}

void RSRenderNodeGC::ReleaseNodeMemory()
{
    std::vector<RSRenderNode*> toDele;
    {
        std::lock_guard<std::mutex> lock(nodeMutex_);
        if (node_.size() == 0) {
            return;
        }
        std::swap(toDele, node_);
    }
    RS_TRACE_NAME_FMT("ReleaseNodeMemory %d", toDele.size());
    for (auto ptr : toDele) {
        if (ptr) {
            delete ptr;
            ptr = nullptr;
        }
    }
}

size_t RSRenderNodeGC::GetNodeSize()
{
    size_t size = 0;
    {
        std::lock_guard<std::mutex> lock(nodeMutex_);
        size = node_.size();
    }
    return size;
}

void RSRenderNodeGC::DrawableDestructor(DrawableV2::RSRenderNodeDrawableAdapter* ptr)
{
    RSRenderNodeGC::Instance().DrawableDestructorInner(ptr);
}

void RSRenderNodeGC::DrawableDestructorInner(DrawableV2::RSRenderNodeDrawableAdapter* ptr)
{
    std::lock_guard<std::mutex> lock(drawableMutex_);
    drawable_.push_back(ptr);
}

void RSRenderNodeGC::ReleaseDrawableMemory()
{
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter*> toDele;
    {
        std::lock_guard<std::mutex> lock(drawableMutex_);
        if (drawable_.size() == 0) {
            return;
        }
        std::swap(toDele, drawable_);
        drawable_.clear();
    }
    RS_TRACE_NAME_FMT("ReleaseDrawableMemory %d", toDele.size());
    for (auto ptr : toDele) {
        if (ptr) {
            delete ptr;
            ptr = nullptr;
        }
    }
}

size_t RSRenderNodeGC::GetDrawableSize()
{
    size_t size = 0;
    {
        std::lock_guard<std::mutex> lock(drawableMutex_);
        size = drawable_.size();
    }
    return size;
}

} // namespace Rosen
} // namespace OHOS
