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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_SURFACE_RENDER_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_SURFACE_RENDER_PARAMS_H

#include "params/rs_render_params.h"
#include "common/rs_occlusion_region.h"
#include "pipeline/rs_base_render_node.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSSurfaceRenderParams : public RSRenderParams {
public:
    explicit RSSurfaceRenderParams();
    virtual ~RSSurfaceRenderParams() = default;
    void SetOcclusionVisible(bool visible);
    bool GetOcclusionVisible() const;
    Occlusion::Region GetVisibleRegion() const;
    void SetVisibleRegion(const Occlusion::Region& visibleRegion);

    virtual void OnSync(const std::unique_ptr<RSRenderParams>& target) override;
    void SetAncestorDisplayNode(const RSBaseRenderNode::WeakPtr& ancestorDisplayNode)
    {
        ancestorDisplayNode_ = ancestorDisplayNode;
    }

    RSBaseRenderNode::WeakPtr GetAncestorDisplayNode() const
    {
        return ancestorDisplayNode_;
    }
protected:
private:
    bool occlusionVisible_ = true;
    Occlusion::Region visibleRegion_;
    RSBaseRenderNode::WeakPtr ancestorDisplayNode_;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_SURFACE_RENDER_PARAMS_H
