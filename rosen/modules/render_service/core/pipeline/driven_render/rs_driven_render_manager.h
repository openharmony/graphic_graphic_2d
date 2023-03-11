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

enum class DrivenUniTreePrepareMode : uint32_t {
    PREPARE_DRIVEN_NODE_BEFORE,
    PREPARE_DRIVEN_NODE,
    PREPARE_DRIVEN_NODE_AFTER,
};

enum class DrivenDirtyType : uint32_t {
    INVALID,
    MARK_DRIVEN_RENDER,
    MARK_DRIVEN,
};

struct DrivenDirtyInfo {
    bool backgroundDirty = false;
    bool contentDirty = false;
    bool nonContentDirty = false;
    DrivenDirtyType type = DrivenDirtyType::INVALID;
};

struct DrivenPrepareInfo {
    DrivenDirtyInfo dirtyInfo;
    RSBaseRenderNode::SharedPtr backgroundNode;
    RSBaseRenderNode::SharedPtr contentNode;
    RectI screenRect;
    bool hasInvalidScene = false;
    bool hasDrivenNodeOnUniTree = false;

    // used in RSUniRenderVisitor
    bool hasDrivenNodeMarkRender = false;
    bool isPrepareLeashWinSubTree = false;
    RSBaseRenderNode::SharedPtr currentRootNode;
    DrivenUniTreePrepareMode drivenUniTreePrepareMode = DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_BEFORE;
    DrivenUniRenderMode currDrivenRenderMode = DrivenUniRenderMode::RENDER_WITH_NORMAL;
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

    bool ClipHoleForDrivenNode(RSPaintFilterCanvas& canvas, const RSCanvasRenderNode& node) const;

    RSDrivenSurfaceRenderNode::SharedPtr GetContentSurfaceNode() const
    {
        return contentSurfaceNode_;
    }
    RSDrivenSurfaceRenderNode::SharedPtr GetBackgroundSurfaceNode() const
    {
        return backgroundSurfaceNode_;
    }

    void DoPrepareRenderTask(const DrivenPrepareInfo& info);
    void DoProcessRenderTask(const DrivenProcessInfo& info);

    RSDrivenRenderManager() = default;
    virtual ~RSDrivenRenderManager() = default;

private:
    void Reset();
    void UpdateUniDrivenRenderMode(DrivenDirtyType dirtyType);

    bool drivenRenderEnabled_ = false;

    std::shared_ptr<RSDrivenSurfaceRenderNode> contentSurfaceNode_ =
        std::make_shared<RSDrivenSurfaceRenderNode>(0, DrivenSurfaceType::CONTENT);
    std::shared_ptr<RSDrivenSurfaceRenderNode> backgroundSurfaceNode_ =
        std::make_shared<RSDrivenSurfaceRenderNode>(0, DrivenSurfaceType::BACKGROUND);

    DrivenUniRenderMode uniRenderMode_ = DrivenUniRenderMode::RENDER_WITH_NORMAL;
    float uniRenderGlobalZOrder_ = 0.0;

    NodeId contentCanvasNodeId_ = 0;
    NodeId backgroundCanvasNodeId_ = 0;

    bool isBufferCacheClear_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_RS_DRIVEN_RENDER_MANAGER_H
