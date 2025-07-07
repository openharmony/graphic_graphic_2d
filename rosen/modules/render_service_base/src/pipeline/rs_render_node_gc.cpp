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

#include "params/rs_render_params.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_node_allocator.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include <csignal>

namespace OHOS {
namespace Rosen {
struct MemoryHook {
    void* virtualPtr; // RSRenderNodeDrawableAdapter vtable
    void* ptr; // enable_shared_from_this's __ptr__*
    uint32_t low; // enable_shared_fromthis's __ref__* low-bits
    uint32_t high; // enable_shared_fromthis's __ref__* high-bits

    inline void Protect()
    {
        static const bool isBeta = RSSystemProperties::GetVersionType() == "beta";
        if (CheckIsNotValid()) {
            if (isBeta) {
                RS_LOGE("Drawable Protect %{public}u %{public}u", high, low);
            }
            high = 0;
            low = 0;
        }
    }

    inline bool CheckIsNotValid()
    {
        static constexpr uint32_t THRESHOLD = 0x400u;
        return low < THRESHOLD;
    }
};

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
    if (nodeBucket_.size() > 0) {
        auto& bucket = nodeBucket_.back();
        if (bucket.size() < BUCKET_MAX_SIZE) {
            bucket.push_back(ptr);
        } else {
            nodeBucket_.push({ptr});
        }
    } else {
        nodeBucket_.push({ptr});
    }
}

bool RSRenderNodeGC::IsBucketQueueEmpty()
{
    std::lock_guard<std::mutex> lock(nodeMutex_);
    return nodeBucket_.empty();
}

void RSRenderNodeGC::ReleaseNodeBucket()
{
    static auto callback = [] (uint32_t size, bool isHigh) {
        if (isHigh) {
            ROSEN_LOGW("RSRenderNodeGC::ReleaseNodeBucket remain buckets "
                "exceed high threshold, cur[%{public}u]", size);
            return;
        }
        ROSEN_LOGI("RSRenderNodeGC::ReleaseNodeBucket remain buckets "
            "recover below low threshold, cur[%{public}u]", size);
        return;
    };
    std::vector<RSRenderNode*> toDele;
    uint32_t remainBucketSize;
    {
        std::lock_guard<std::mutex> lock(nodeMutex_);
        if (nodeBucket_.empty()) {
            return;
        }
        toDele.swap(nodeBucket_.front());
        nodeBucket_.pop();
        remainBucketSize = nodeBucket_.size();
    }
    nodeBucketThrDetector_.Detect(remainBucketSize, callback);
    RS_TRACE_NAME_FMT("ReleaseNodeMemory %zu, remain node buckets %u", toDele.size(), remainBucketSize);
    for (auto ptr : toDele) {
        if (ptr) {
            if (RSRenderNodeAllocator::Instance().AddNodeToAllocator(ptr)) {
                continue;
            }
#if defined(__aarch64__)
            auto* hook = reinterpret_cast<MemoryHook*>(ptr);
            hook->Protect();
#endif
            delete ptr;
            ptr = nullptr;
        }
    }
}

void RSRenderNodeGC::ReleaseNodeMemory()
{
    RS_TRACE_FUNC();
    uint32_t remainBucketSize;
    {
        std::lock_guard<std::mutex> lock(nodeMutex_);
        if (nodeBucket_.empty()) {
            return;
        }
        remainBucketSize = nodeBucket_.size();
    }
    nodeGCLevel_ = JudgeGCLevel(remainBucketSize);
    if (mainTask_) {
        auto task = [this]() {
            if (isEnable_.load() == false &&
                nodeGCLevel_ != GCLevel::IMMEDIATE) {
                return;
            }
            ReleaseNodeBucket();
            ReleaseNodeMemory();
        };
        mainTask_(task, DELETE_NODE_TASK, 0, static_cast<AppExecFwk::EventQueue::Priority>(nodeGCLevel_));
    } else {
        ReleaseNodeBucket();
    }
}

void RSRenderNodeGC::DrawableDestructor(DrawableV2::RSRenderNodeDrawableAdapter* ptr)
{
    RSRenderNodeGC::Instance().DrawableDestructorInner(ptr);
}

void RSRenderNodeGC::DrawableDestructorInner(DrawableV2::RSRenderNodeDrawableAdapter* ptr)
{
    std::lock_guard<std::mutex> lock(drawableMutex_);
    if (drawableBucket_.size() > 0) {
        auto& bucket = drawableBucket_.back();
        if (bucket.size() < BUCKET_MAX_SIZE) {
            bucket.push_back(ptr);
        } else {
            drawableBucket_.push({ptr});
        }
    } else {
        drawableBucket_.push({ptr});
    }
}

void RSRenderNodeGC::ReleaseDrawableBucket()
{
    static auto callback = [] (uint32_t size, bool isHigh) {
        if (isHigh) {
            ROSEN_LOGW("RSRenderNodeGC::ReleaseDrawableBucket remain buckets "
                "exceed high threshold, cur[%{public}u]", size);
            return;
        }
        ROSEN_LOGI("RSRenderNodeGC::ReleaseDrawableBucket remain buckets "
            "recover below low threshold, cur[%{public}u]", size);
        return;
    };
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter*> toDele;
    uint32_t remainBucketSize;
    {
        std::lock_guard<std::mutex> lock(drawableMutex_);
        if (drawableBucket_.empty()) {
            return;
        }
        toDele.swap(drawableBucket_.front());
        drawableBucket_.pop();
        remainBucketSize = drawableBucket_.size();
    }
    drawableBucketThrDetector_.Detect(remainBucketSize, callback);
    RS_TRACE_NAME_FMT("ReleaseDrawableMemory %zu, remain drawable buckets %u", toDele.size(), remainBucketSize);
    for (auto ptr : toDele) {
        if (ptr) {
            if (RSRenderNodeAllocator::Instance().AddDrawableToAllocator(ptr)) {
                continue;
            }
            delete ptr;
            ptr = nullptr;
        }
    }
}

void RSRenderNodeGC::ReleaseDrawableMemory()
{
    uint32_t remainBucketSize;
    {
        std::lock_guard<std::mutex> lock(drawableMutex_);
        if (drawableBucket_.empty()) {
            return;
        }
        remainBucketSize = drawableBucket_.size();
    }
    drawableGCLevel_ = JudgeGCLevel(remainBucketSize);
    if (renderTask_) {
        auto task = []() {
            RSRenderNodeGC::Instance().ReleaseDrawableBucket();
            RSRenderNodeGC::Instance().ReleaseDrawableMemory();
        };
        renderTask_(task, DELETE_DRAWABLE_TASK, 0, static_cast<AppExecFwk::EventQueue::Priority>(drawableGCLevel_));
    } else {
        ReleaseDrawableBucket();
    }
}

void RSRenderNodeGC::AddToOffTreeNodeBucket(const std::shared_ptr<RSBaseRenderNode>& node)
{
    if (offTreeBucket_.size() > 0) {
        auto& bucket = offTreeBucket_.back();
        if (bucket.size() < OFF_TREE_BUCKET_MAX_SIZE) {
            bucket.emplace_back(node);
        } else {
            offTreeBucket_.push({node});
        }
    } else {
        offTreeBucket_.push({node});
    }
}

void RSRenderNodeGC::ReleaseOffTreeNodeBucket()
{
    static auto callback = [] (uint32_t size, bool isHigh) {
        if (isHigh) {
            ROSEN_LOGW("RSRenderNodeGC::ReleaseOffTreeNodeBucket remain buckets "
                "exceed high threshold, cur[%{public}u]", size);
            return;
        }
        ROSEN_LOGI("RSRenderNodeGC::ReleaseOffTreeNodeBucket remain buckets "
            "recover below low threshold, cur[%{public}u]", size);
        return;
    };
    std::vector<std::shared_ptr<RSBaseRenderNode>> toRemove;
    if (offTreeBucket_.empty()) {
        return;
    }
    toRemove.swap(offTreeBucket_.front());
    offTreeBucket_.pop();
    uint32_t remainBucketSize = offTreeBucket_.size();
    offTreeBucketThrDetector_.Detect(remainBucketSize, callback);
    RS_TRACE_NAME_FMT("ReleaseOffTreeNodeBucket %d, remain offTree buckets %u", toRemove.size(), remainBucketSize);
    for (const auto& node : toRemove) {
        if (!node) {
            continue;
        }
        auto parent = node->GetParent().lock();
        if (parent) {
            parent->RemoveChildFromFulllist(node->GetId());
        }
        node->RemoveFromTree(false);
    }
}

void RSRenderNodeGC::ReleaseFromTree()
{
    if (offTreeBucket_.empty()) {
        return;
    }
    if (mainTask_) {
        auto task = [this]() {
            if (!isEnable_.load()) {
                return;
            }
            ReleaseOffTreeNodeBucket();
            ReleaseFromTree();
        };
        mainTask_(task, OFF_TREE_TASK, 0, AppExecFwk::EventQueue::Priority::IDLE);
    } else {
        ReleaseOffTreeNodeBucket();
    }
}

GCLevel RSRenderNodeGC::JudgeGCLevel(uint32_t remainBucketSize)
{
    if (remainBucketSize < GC_LEVEL_THR_LOW) {
        return GCLevel::IDLE;
    } else if (remainBucketSize < GC_LEVEL_THR_HIGH) {
        return GCLevel::LOW;
    } else if (remainBucketSize < GC_LEVEL_THR_IMMEDIATE) {
        return GCLevel::HIGH;
    } else {
        return GCLevel::IMMEDIATE;
    }
}

} // namespace Rosen
} // namespace OHOS
