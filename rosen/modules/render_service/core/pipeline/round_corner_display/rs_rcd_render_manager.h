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

#ifndef RENDER_SERVICE_CORE_RS_RCD_RENDER_MANAGER_H
#define RENDER_SERVICE_CORE_RS_RCD_RENDER_MANAGER_H

#include "pipeline/rs_processor.h"
#include "rs_rcd_surface_render_node.h"
#include "pipeline/round_corner_display/rs_round_corner_config.h"

namespace OHOS {
namespace Rosen {

struct RcdPrepareInfo {
    RSBaseRenderNode::SharedPtr bottomNode;
    RSBaseRenderNode::SharedPtr topNode;
    bool hasInvalidScene = false;
};

struct RcdProcessInfo {
    std::shared_ptr<RSProcessor> uniProcessor = nullptr;
    rs_rcd::RoundCornerLayer* topLayer = nullptr;
    rs_rcd::RoundCornerLayer* bottomLayer = nullptr;
    bool resourceChanged = false;
};

struct RcdInfo {
    RcdPrepareInfo prepareInfo;
    RcdProcessInfo processInfo;
};

class RSRcdRenderManager {
public:
    static RSRcdRenderManager& GetInstance();
    static void InitInstance();
    bool GetRcdRenderEnabled() const;

    RSRcdSurfaceRenderNode::SharedPtr GetContentSurfaceNode() const
    {
        return topSurfaceNode_;
    }
    RSRcdSurfaceRenderNode::SharedPtr GetBackgroundSurfaceNode() const
    {
        return bottomSurfaceNode_;
    }

    void DoPrepareRenderTask(const RcdPrepareInfo& info);
    void DoProcessRenderTask(const RcdProcessInfo& info);
    static bool IsRcdProcessInfoValid(const RcdProcessInfo& info);

    RSRcdRenderManager() = default;
    virtual ~RSRcdRenderManager() = default;

private:
    void Reset();

    bool rcdRenderEnabled_ = false;

    std::shared_ptr<RSRcdSurfaceRenderNode> topSurfaceNode_ =
        std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::TOP);
    std::shared_ptr<RSRcdSurfaceRenderNode> bottomSurfaceNode_ =
        std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    bool isBufferCacheClear_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_RS_RCD_RENDER_MANAGER_H