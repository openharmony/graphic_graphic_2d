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
#ifndef RS_LAYER_SPLITTER_PROCESSOR_H
#define RS_LAYER_SPLITTER_PROCESSOR_H

#include "pipeline/layer_split/common/rs_layer_split_types.h"

namespace OHOS::Rosen {
class RSLayerSplitterPlanner;

class RSLayerSplitterProcessor {
public:
    virtual ~RSLayerSplitterProcessor() = default;

    virtual void Sync(const std::shared_ptr<RSLayerSplitterPlanner>& layerSplitterPlanner) = 0;
    virtual void RequestFrame(RSSurfaceRenderParams& params) = 0;
    virtual void DrawDfx() = 0;
    virtual bool NeedDrawSplitCanvas(Drawing::Canvas& canvas, NodeId nodeId) = 0;
    virtual bool CanSkipOpIncNodeDraw(NodeId nodeId) = 0;
    virtual void CalSplitCanvasMatrix(Drawing::Canvas& canvas, NodeId nodeId) = 0;
    virtual std::shared_ptr<RSPaintFilterCanvas> GetSplitCanvas() = 0;

    virtual PlanStatus GetPlanStatus() const = 0;
    virtual void RecordNodeWithCacheImage(NodeId nodeId);
};

} // namespace OHOS::Rosen

#endif
