/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_MANAGER_H

#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_canvas_render_node.h"

namespace OHOS {
class Surface;
namespace Rosen {

class RSRenderManager final {
public:
    RSRenderManager() = default;
    ~RSRenderManager() = default;
    void Animate(int64_t timestamp);
    bool HasRunningAnimation() const;
    void UpdateNodes();

    void SetRoot(RSBaseRenderNode::SharedPtr& root);
    bool IsDirty() const;
    RectI GetDirtyRegion() const;
    void Clear();

    std::string DumpRenderTree() const;

    int GetSurfaceWidth() const
    {
        return dirtyManager_.GetSurfaceWidth();
    }
    int GetSurfaceHeight() const
    {
        return dirtyManager_.GetSurfaceHeight();
    }
    RectI GetSurfaceRegion() const;

private:
    RSBaseRenderNode::WeakPtr root_;
    RSDirtyRegionManager dirtyManager_;
    bool hasRunningAnimation_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_MANAGER_H