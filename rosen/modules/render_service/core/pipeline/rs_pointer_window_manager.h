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
    RSPointerWindowManager();
    ~RSPointerWindowManager() = default;

    static RSPointerWindowManager& Instance();

    void UpdatePointerDirtyToGlobalDirty(std::shared_ptr<RSSurfaceRenderNode>& pointWindow,
        std::shared_ptr<RSDisplayRenderNode>& curDisplayNode);

    static bool GetHardCursorEnabledPass();
    struct BoundParam {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;
    };
    
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

    void CollectAllHardCursor(
        RSSurfaceRenderNode& hardCursorNode, std::shared_ptr<RSDisplayRenderNode>& curDisplayNode);

    const std::map<NodeId, DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& GetHardCursorDrawableMap() const
    {
        return hardCursorDrawableMap_;
    }

    void ResetHardCursorDrawables()
    {
        hardCursorDrawableMap_.clear();
        hardCursorNodeMap_.clear();
    }

    bool GetIsPointerEnableHwc() const
    {
        return isPointerEnableHwc_.load();
    }

    void SetIsPointerEnableHwc(bool flag)
    {
        isPointerEnableHwc_.store(flag);
    }

    bool GetIsPointerCanSkipFrame() const
    {
        return isPointerCanSkipFrame_.load();
    }

    void SetIsPointerCanSkipFrame(bool flag)
    {
        isPointerCanSkipFrame_.store(flag);
    }

    bool IsPointerCanSkipFrameCompareChange(bool flag, bool changeFlag)
    {
        bool expectChanged = flag;
        return isPointerCanSkipFrame_.compare_exchange_weak(expectChanged, changeFlag);
    }

    int64_t GetRsNodeId() const
    {
        return rsNodeId_;
    }

    void SetRsNodeId(int64_t id)
    {
        rsNodeId_ = id;
    }

    bool GetBoundHasUpdate() const
    {
        return boundHasUpdate_.load();
    }

    void SetBoundHasUpdate(bool flag)
    {
        boundHasUpdate_.store(flag);
    }

    bool BoundHasUpdateCompareChange(bool flag, bool changeFlag)
    {
        bool expectChanged = flag;
        return boundHasUpdate_.compare_exchange_weak(expectChanged, changeFlag);
    }

    BoundParam GetBound() const
    {
        return bound_;
    }

    void SetBound(BoundParam bound)
    {
        bound_.x = bound.x;
        bound_.y = bound.y;
        bound_.z = bound.z;
        bound_.w = bound.w;
    }

    void UpdatePointerInfo();
    void SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
        float positionZ, float positionW);
    void SetHardCursorNodeInfo(std::shared_ptr<RSSurfaceRenderNode> hardCursorNode);
    const std::map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>& GetHardCursorNode() const;

    void HardCursorCreateLayerForDirect(std::shared_ptr<RSProcessor> processor);

    bool CheckHardCursorSupport(uint32_t screenId);
    bool HasMirrorDisplay() const;
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> GetHardCursorDrawable(NodeId id);
    static void CheckHardCursorValid(const RSSurfaceRenderNode& node);
private:
    bool isNeedForceCommitByPointer_{ false };
    HardCursorInfo hardCursorDrawables_;
    std::map<NodeId, DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> hardCursorDrawableMap_;
    std::shared_ptr<RSSurfaceRenderNode> hardCursorNodes_;
    std::map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> hardCursorNodeMap_;
    std::mutex mtx_;
    std::atomic<bool> isPointerEnableHwc_ = true;
    std::atomic<bool> isPointerCanSkipFrame_ = false;
    std::atomic<bool> boundHasUpdate_ = false;
    BoundParam bound_ = {0.0f, 0.0f, 0.0f, 0.0f};
    int64_t rsNodeId_ = -1;
    bool isHardCursorEnable_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_RS_POINTER_WINDOW_MANAGER_H