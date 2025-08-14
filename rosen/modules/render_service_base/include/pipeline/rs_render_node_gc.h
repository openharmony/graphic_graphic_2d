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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_GC_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_GC_H

#include <cstdint>
#include <event_handler.h>
#include <mutex>
#include <vector>
#include <queue>

#include "common/rs_thread_handler.h"
#include "common/rs_threshold_detector.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
namespace {
    const int BUCKET_MAX_SIZE = 50;
    const int OFF_TREE_BUCKET_MAX_SIZE = 500;
    const char* OFF_TREE_TASK = "ReleaseNodeFromTree";
    const char* DELETE_NODE_TASK = "ReleaseNodeMemory";
    const char* DELETE_DRAWABLE_TASK = "ReleaseDrawableMemory";
    const uint32_t NODE_BUCKET_THR_LOW = 4;
    const uint32_t NODE_BUCKET_THR_HIGH = 100;
    const uint32_t DRAWABLE_BUCKET_THR_LOW = 4;
    const uint32_t DRAWABLE_BUCKET_THR_HIGH = 100;
    const uint32_t OFFTREE_BUCKET_THR_LOW = 4;
    const uint32_t OFFTREE_BUCKET_THR_HIGH = 20;
    const uint32_t GC_LEVEL_THR_IMMEDIATE = 1000;
    const uint32_t GC_LEVEL_THR_HIGH = 500;
    const uint32_t GC_LEVEL_THR_LOW = 50;
}

enum class GCLevel : uint32_t {
    IMMEDIATE = 0,
    HIGH,
    LOW,
    IDLE,
};
class RSB_EXPORT RSRenderNodeGC {
public:
    typedef void (*gcTask)(RSTaskMessage::RSTask, const std::string&, int64_t,
        AppExecFwk::EventQueue::Priority);

    static RSRenderNodeGC& Instance();

    static void NodeDestructor(RSRenderNode* ptr);
    void NodeDestructorInner(RSRenderNode* ptr);
    bool IsBucketQueueEmpty();
    void ReleaseNodeBucket();
    void ReleaseNodeMemory();
    void SetMainTask(gcTask hook) {
        mainTask_ = hook;
    }

    void AddToOffTreeNodeBucket(const std::shared_ptr<RSBaseRenderNode>& node);
    void AddToOffTreeNodeBucket(pid_t pid,
        std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>>& renderNodeMap);
    void ReleaseOffTreeNodeBucket();
    void ReleaseFromTree();

    static void DrawableDestructor(DrawableV2::RSRenderNodeDrawableAdapter* ptr);
    void DrawableDestructorInner(DrawableV2::RSRenderNodeDrawableAdapter* ptr);
    void ReleaseDrawableBucket();
    void ReleaseDrawableMemory();
    void SetRenderTask(gcTask hook) {
        renderTask_ = hook;
    }

    inline void SetGCTaskEnable(bool isEnable)
    {
        isEnable_.store(isEnable);
    }

private:
    GCLevel JudgeGCLevel(uint32_t remainBucketSize);
    void ReleaseOffTreeNodeForBucket(const RSThresholdDetector<uint32_t>::DetectCallBack &callBack);
    void ReleaseOffTreeNodeForBucketMap(const RSThresholdDetector<uint32_t>::DetectCallBack &callBack);

    std::atomic<bool> isEnable_ = true;
    GCLevel nodeGCLevel_ = GCLevel::IDLE;
    GCLevel drawableGCLevel_ = GCLevel::IDLE;
    gcTask mainTask_ = nullptr;
    gcTask renderTask_ = nullptr;

    std::queue<std::vector<std::shared_ptr<RSBaseRenderNode>>> offTreeBucket_;
    std::queue<std::pair<pid_t, std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>>>> offTreeBucketMap_;
    uint64_t renderNodeNumsInBucketMap_ = 0;

    RSThresholdDetector<uint32_t> offTreeBucketThrDetector_ = RSThresholdDetector<uint32_t>(
        OFFTREE_BUCKET_THR_LOW, OFFTREE_BUCKET_THR_HIGH);
    std::queue<std::vector<RSRenderNode*>> nodeBucket_;
    RSThresholdDetector<uint32_t> nodeBucketThrDetector_ = RSThresholdDetector<uint32_t>(
        NODE_BUCKET_THR_LOW, NODE_BUCKET_THR_HIGH);
    std::queue<std::vector<DrawableV2::RSRenderNodeDrawableAdapter*>> drawableBucket_;
    RSThresholdDetector<uint32_t> drawableBucketThrDetector_ = RSThresholdDetector<uint32_t>(
        DRAWABLE_BUCKET_THR_LOW, DRAWABLE_BUCKET_THR_HIGH);
    std::mutex nodeMutex_;
    std::mutex drawableMutex_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_GC_H
