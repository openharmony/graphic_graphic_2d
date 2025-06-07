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

#ifndef RENDER_SERVICE_BASE_PIPELINE_RS_RENDER_NODE_ALLOCATOR_H
#define RENDER_SERVICE_BASE_PIPELINE_RS_RENDER_NODE_ALLOCATOR_H

#include <atomic>
#include <queue>

#include "pipeline/rs_canvas_render_node.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSRenderNodeAllocator {
public:
    static RSRenderNodeAllocator& Instance();

    bool AddNodeToAllocator(RSRenderNode* ptr);

    std::shared_ptr<RSCanvasRenderNode> CreateRSCanvasRenderNode(NodeId id,
        const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false);

private:
    class Spinlock {
    public:
        Spinlock() : flag(false) {}

        void lock() {
            while (flag.exchange(true, std::memory_order_acquire)) {}
        }

        void unlock() {
            flag.store(false, std::memory_order_release);
        }

    private:
        std::atomic<bool> flag;
    };

    std::queue<RSRenderNode*> nodeAllocator_;
    Spinlock nodeAllocatorSpinlock_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PIPELINE_RS_RENDER_NODE_ALLOCATOR_H