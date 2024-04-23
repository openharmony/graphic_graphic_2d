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
#include <mutex>
#include <vector>

#include "drawable/rs_render_node_drawable_adapter.h"
#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSRenderNodeGC {
public:
    static RSRenderNodeGC& Instance();

    static void NodeDestructor(RSRenderNode* ptr);
    void NodeDestructorInner(RSRenderNode* ptr);
    void ReleaseNodeMemory();
    size_t GetNodeSize();

    static void DrawableDestructor(DrawableV2::RSRenderNodeDrawableAdapter* ptr);
    void DrawableDestructorInner(DrawableV2::RSRenderNodeDrawableAdapter* ptr);
    void ReleaseDrawableMemory();
    size_t GetDrawableSize();

private:
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter*> drawable_;
    std::mutex drawableMutex_;
    std::vector<RSRenderNode*> node_;
    std::mutex nodeMutex_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_GC_H
