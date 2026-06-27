/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RS_OPINC_LAYER_SPLITTER_PROCESSOR_H
#define RS_OPINC_LAYER_SPLITTER_PROCESSOR_H

#include "pipeline/layer_split/splitter/opinc/controller/rs_opinc_split_controller.h"
#include "pipeline/layer_split/surface/rs_split_surface.h"
#include "pipeline/layer_split/splitter/rs_layer_splitter_processor.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
class RSRenderNodeDrawable;
class RSScreenRenderNodeDrawable;
}

class RSOpincLayerSplitterProcessor : public RSLayerSplitterProcessor {
public:
    RSOpincLayerSplitterProcessor();

    void Sync(const std::shared_ptr<RSLayerSplitterPlanner>& layerSplitterPlanner) override;
    void RequestFrame(RSSurfaceRenderParams& params) override;
    void DrawDfx() override;
    bool NeedDrawSplitCanvas(Drawing::Canvas& canvas, NodeId nodeId) override;
    bool CanSkipOpIncNodeDraw(NodeId nodeId) override;
    void CalSplitCanvasMatrix(Drawing::Canvas& canvas, NodeId nodeId) override;
    std::shared_ptr<RSPaintFilterCanvas> GetSplitCanvas() override;
    void RecordNodeWithCacheImage(NodeId nodeId) override;

    PlanStatus GetPlanStatus() const override { return planStatus_; }

private:
    bool layerSplitterEnabled_ = false;
    PlanStatus planStatus_ = PlanStatus::OFF;

    std::set<NodeId> opIncNodes_;
    std::shared_ptr<SplitSurface> splitSurface_ = nullptr;
    RectF srcRect_;
    RectF dstRect_;
    std::shared_ptr<RequestController> requestController_ = nullptr;
};

} // namespace OHOS::Rosen

#endif // RS_LAYER_SPLITTER_PROCESSOR_H