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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_DISPLAY_RENDER_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_DISPLAY_RENDER_PARAMS_H

#include <memory>

#include "common/rs_macros.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_render_node.h"
#include "screen_manager/rs_screen_info.h"
#include "pipeline/rs_surface_render_node.h"
namespace OHOS::Rosen {
class RSB_EXPORT RSDisplayRenderParams : public RSRenderParams {
public:
    explicit RSDisplayRenderParams();
    virtual ~RSDisplayRenderParams() = default;

    void OnSync(const std::unique_ptr<RSRenderParams>& target) override;

    std::vector<RSBaseRenderNode::SharedPtr>& GetAllMainAndLeashSurfaces();
    void SetAllMainAndLeashSurfaces(std::vector<RSBaseRenderNode::SharedPtr>& allMainAndLeashSurfaces);
    uint64_t GetDisplayOffsetX() const
    {
        return offsetX_;
    }
    uint64_t GetDisplayOffsetY() const
    {
        return offsetY_;
    }
    uint64_t GetScreenId() const
    {
        return screenId_;
    }
    uint64_t GetMirroredId() const
    {
        return mirroredId_;
    }
    const ScreenInfo& GetrScreenInfo() const
    {
        return screenInfo_;
    }
    std::weak_ptr<RSDisplayRenderNode> GetMirrorSource()
    {
        return mirrorSource_;
    }
    RSDisplayRenderNode::CompositeType GetCompositeType() const
    {
        return compositeType_;
    };
    ScreenRotation GetScreenRotation() const
    {
        return screenRotation_;
    }
    ScreenRotation GetNodeRotation() const
    {
        return nodeRotation_;
    }
    bool GetDisplayHasSecSurface() const
    {
        return displayHasSecSurface_;
    }
    bool GetDisplayHasSkipSurface() const
    {
        return displayHasSkipSurface_;
    }
    std::vector<std::shared_ptr<RSSurfaceRenderNode>>& GetHardwareEnabledNodes_() const
    {
        return hardwareEnabledNodes_;
    }
    std::vector<std::shared_ptr<RSSurfaceRenderNode>>& GetHardwareEnabledTopNodes_() const
    {
        return hardwareEnabledTopNodes_;
    }
    // dfx
    std::string ToString() const override;

private:
    std::vector<RSBaseRenderNode::SharedPtr> allMainAndLeashSurfaces_;
    int32_t offsetX_ = -1;
    int32_t offsetY_ = -1;
    ScreenRotation nodeRotation_ = ScreenRotation::INVALID_SCREEN_ROTATION;
    ScreenRotation screenRotation_ = ScreenRotation::INVALID_SCREEN_ROTATION;
    uint64_t screenId_ = 0;
    std::weak_ptr<RSDisplayRenderNode> mirrorSource_;
    ScreenInfo screenInfo_;
    ScreenId mirroredId_;
    RSDisplayRenderNode::CompositeType compositeType_ = RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE;
    friend class RSUniRenderVisitor;
    friend class RSDisplayRenderNode;
    bool displayHasSecSurface_ = false; // TODO
    bool displayHasSkipSurface_ = false; // TODO
    
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes_; // TODO
    // vector of hardwareEnabled nodes above displayNodeSurface like pointer window
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledTopNodes_; // TODO
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_DISPLAY_RENDER_PARAMS_H
