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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_EFFECT_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_EFFECT_RENDER_NODE_H

#include "common/rs_macros.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSEffectRenderNode : public RSRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSEffectRenderNode>;
    using SharedPtr = std::shared_ptr<RSEffectRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::EFFECT_NODE;
    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    explicit RSEffectRenderNode(NodeId id, const std::weak_ptr<RSContext>& context = {},
        bool isTextureExportNode = false);
    ~RSEffectRenderNode() override;

    void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas) override;

    void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    std::optional<Drawing::RectI> InitializeEffectRegion() const { return Drawing::RectI(); }
    void SetEffectRegion(const std::optional<Drawing::RectI>& effectRegion);
    // record if there is filter cache for occlusion before this effect node
    void SetVisitedFilterCacheStatus(bool isEmpty)
    {
        isVisitedOcclusionFilterCacheEmpty_ = isEmpty;
    }

    bool IsVisitedFilterCacheEmpty() const
    {
        return isVisitedOcclusionFilterCacheEmpty_;
    }

    void SetRotationChanged(bool isRotationChanged);
    bool GetRotationChanged() const;
    void SetInvalidateTimesForRotation(int times);

protected:
    RectI GetFilterRect() const override;
    void UpdateFilterCacheManagerWithCacheRegion(RSDirtyRegionManager& dirtyManager,
        const std::optional<RectI>& clipRect bool isForeground = false) override;
    void UpdateFilterCacheWithDirty(RSDirtyRegionManager& dirtyManager, bool isForeground = false) override;

private:
    bool NeedForceCache();

    bool isVisitedOcclusionFilterCacheEmpty_ = true;
    bool isRotationChanged_ = false;
    int invalidateTimes_ = 0;
    static int cacheUpdateInterval_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_EFFECT_RENDER_NODE_H
