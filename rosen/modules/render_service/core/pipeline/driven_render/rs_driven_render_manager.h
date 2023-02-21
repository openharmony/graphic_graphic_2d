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

#ifndef RENDER_SERVICE_CORE_RS_DRIVEN_RENDER_MANAGER_H
#define RENDER_SERVICE_CORE_RS_DRIVEN_RENDER_MANAGER_H

#include "pipeline/rs_processor.h"
#include "rs_driven_surface_render_node.h"

namespace OHOS {
namespace Rosen {
enum class DrivenUniRenderMode : uint32_t {
    RENDER_WITH_CLIP_HOLE,
    REUSE_WITH_CLIP_HOLE,
    RENDER_WITH_NORMAL,
};

struct DrivenDirtyInfo {
    bool backgroundDirty = false;
    bool contentDirty = false;
    bool nonContentDirty = false;
};

// [leashNodeId, surfaceNodeName, nodePtr]
using DrivenCandidateTuple = std::tuple<NodeId, std::string, RSBaseRenderNode::SharedPtr>;
// [leashNodeId, nodePtr]
using DrivenCandidatePair = std::pair<NodeId, RSBaseRenderNode::SharedPtr>;

struct DrivenPrepareInfo {
    DrivenDirtyInfo dirtyInfo;
    bool hasInvalidScene = false;
    std::vector<DrivenCandidateTuple> backgroundCandidates;
    std::vector<DrivenCandidatePair> contentCandidates;
    RectI screenRect;
};

struct DrivenProcessInfo {
    std::shared_ptr<RSProcessor> uniProcessor = nullptr;
    ColorGamut uniColorSpace = ColorGamut::COLOR_GAMUT_SRGB;
    float uniGlobalZOrder = 0.0f;
};

class RSDrivenRenderManager {
public:
    static RSDrivenRenderManager& GetInstance();
    static void InitInstance();

    bool GetDrivenRenderEnabled() const;
    const DrivenUniRenderMode& GetUniDrivenRenderMode() const;
    float GetUniRenderGlobalZOrder() const;

    void ClipHoleForDrivenNode(RSPaintFilterCanvas& canvas, const RSCanvasRenderNode& node) const;

    virtual RSDrivenSurfaceRenderNode::SharedPtr GetContentSurfaceNode() const
    {
        return nullptr;
    }
    virtual RSDrivenSurfaceRenderNode::SharedPtr GetBackgroundSurfaceNode() const
    {
        return nullptr;
    }
    virtual void DoPrepareRenderTask(const DrivenPrepareInfo& info) {}
    virtual void DoProcessRenderTask(const DrivenProcessInfo& info) {}

    RSDrivenRenderManager() = default;
    virtual ~RSDrivenRenderManager() = default;

protected:
    bool drivenRenderEnabled_ = false;

    DrivenUniRenderMode uniRenderMode_ = DrivenUniRenderMode::RENDER_WITH_NORMAL;
    float uniRenderGlobalZOrder_ = 0.0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_RS_DRIVEN_RENDER_MANAGER_H
