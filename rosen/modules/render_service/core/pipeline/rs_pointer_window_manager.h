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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_POINTER_WINDOW_MANAGER_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_POINTER_WINDOW_MANAGER_H

#include "params/rs_render_thread_params.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_processor_factory.h"

namespace OHOS {
namespace Rosen {
class RSPointerWindowManager {
public:
    RSPointerWindowManager() {}
    ~RSPointerWindowManager() = default;

    static RSPointerWindowManager& Instance();

    void UpdatePointerDirtyToGlobalDirty(std::shared_ptr<RSSurfaceRenderNode>& pointWindow,
        std::shared_ptr<RSDisplayRenderNode>& curDisplayNode);
    
    bool IsNeedForceCommitByPointer() const
    {
        return isNeedForceCommitByPointer_;
    }

    void SetNeedForceCommitByPointer(bool isNeedForceCommitByPointer)
    {
        isNeedForceCommitByPointer_ = isNeedForceCommitByPointer;
    }

    void CollectInfoForHardCursor(NodeId id,
        DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr cursorDrawable)
    {
        hardCursorDrawables_.id = id;
        hardCursorDrawables_.drawablePtr = cursorDrawable;
    }

    const HardCursorInfo& GetHardCursorDrawables() const
    {
        return hardCursorDrawables_;
    }

    void ResetHardCursorDrawables()
    {
        hardCursorDrawables_.id = INVALID_NODEID;
        hardCursorDrawables_.drawablePtr = nullptr;
        hardCursorNodes_ = nullptr;
    }

    void SetHardCursorNodeInfo(std::shared_ptr<RSSurfaceRenderNode> hardCursorNode);
    const std::shared_ptr<RSSurfaceRenderNode>& GetHardCursorNode() const;

    void HardCursorCreateLayerForDirect(std::shared_ptr<RSProcessor> processor);

    bool CheckHardCursorSupport(std::shared_ptr<RSDisplayRenderNode>& curDisplayNode);
    bool HasMirrorDisplay() const;

private:
    bool isNeedForceCommitByPointer_{ false };
    HardCursorInfo hardCursorDrawables_;
    std::shared_ptr<RSSurfaceRenderNode> hardCursorNodes_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_RS_POINTER_WINDOW_MANAGER_H