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
#ifndef RS_UNI_HWC_VISITOR_H
#define RS_UNI_HWC_VISITOR_H

#include "pipeline/main_thread/rs_uni_render_visitor.h"

namespace OHOS {
namespace Rosen {
class RSCanvasRenderNode;
class RSUniRenderVisitor;
class RSUniHwcVisitor {
public:
    explicit RSUniHwcVisitor(RSUniRenderVisitor& visitor);
    ~RSUniHwcVisitor();

    // Compute
    void UpdateSrcRect(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix);
    void UpdateDstRect(RSSurfaceRenderNode& node, const RectI& absRect, const RectI& clipRect);
    void UpdateHwcNodeByTransform(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix);

    RectI GetHwcVisibleEffectDirty(RSRenderNode& node, const RectI& globalFilterRect) const;

    bool UpdateIsOffscreen(RSCanvasRenderNode& node);
    void RestoreIsOffscreen(bool isOffscreen) { isOffscreen_ = isOffscreen; }

    void CalcHwcNodeEnableByFilterRect(std::shared_ptr<RSSurfaceRenderNode>& node,
        const RectI& filterRect, NodeId filterNodeId, bool isReverseOrder, int32_t filterZorder);
    void UpdateHwcNodeEnableByFilterRect(std::shared_ptr<RSSurfaceRenderNode>& node,
        const RectI& filterRect, NodeId filterNodeId, bool isReverseOrder, int32_t filterZorder);
    void UpdateHwcNodeEnableByGlobalFilter(std::shared_ptr<RSSurfaceRenderNode>& node);
    void UpdateHwcNodeEnableByGlobalCleanFilter(const std::vector<std::pair<NodeId, RectI>>& cleanFilter,
        RSSurfaceRenderNode& hwcNodePtr);
    void UpdateHwcNodeEnableByGlobalDirtyFilter(const std::vector<std::pair<NodeId, RectI>>& dirtyFilter,
        RSSurfaceRenderNode& hwcNodePtr);

private:
    friend class RSUniRenderVisitor;
    RSUniRenderVisitor& uniRenderVisitor_;

    // record nodes which has transparent clean filter
    std::unordered_map<NodeId, std::vector<std::pair<NodeId, RectI>>> transparentHwcCleanFilter_;
    // record nodes which has transparent dirty filter
    std::unordered_map<NodeId, std::vector<std::pair<NodeId, RectI>>> transparentHwcDirtyFilter_;

    int32_t curZorderForCalcHwcNodeEnableByFilter_ = 0;

    bool isOffscreen_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_UNI_HWC_VISITOR_H