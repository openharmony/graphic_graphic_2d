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

#ifndef ROSEN_RENDER_SERVICE_PIPELINE_RS_LPP_VIDEO_H
#define ROSEN_RENDER_SERVICE_PIPELINE_RS_LPP_VIDEO_H
#include <string>
#include <vector>

#include "pipeline/render_thread/rs_base_render_util.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
namespace OHOS::Rosen {
class LppVideoHandler {
private:
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> lowPowerSurfaceNode_;
    std::atomic_bool hasVirtualMirrorDisplay_ = false;

    LppVideoHandler() = default;

public:
    static LppVideoHandler& Instance();
    void AddLppSurfaceNode(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode);
    void ConsumeAndUpdateLppBuffer(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode);
    void ClearLppSufraceNode();
    void JudgeRsDrawLppState(bool isPostUniRender);
    void SetHasVirtualMirrorDisplay(bool hasVirtualMirrorDisplay);
    ~LppVideoHandler();
};
} // namespace OHOS::Rosen

#endif