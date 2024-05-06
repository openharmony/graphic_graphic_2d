/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RS_CORE_PIPELINE_UNI_RENDER_ENGINE_H
#define RS_CORE_PIPELINE_UNI_RENDER_ENGINE_H

#include "pipeline/rs_base_render_engine.h"

namespace OHOS {
namespace Rosen {
class RSUniRenderEngine : public RSBaseRenderEngine {
public:
    RSUniRenderEngine() = default;
    ~RSUniRenderEngine() noexcept = default;
    RSUniRenderEngine(const RSUniRenderEngine&) = delete;
    void operator=(const RSUniRenderEngine&) = delete;
    void DrawSurfaceNodeWithParams(RSPaintFilterCanvas& canvas, RSSurfaceRenderNode& node, BufferDrawParam& params,
        PreProcessFunc preProcess, PostProcessFunc postProcess) override;
    void DrawLayers(RSPaintFilterCanvas& canvas, const std::vector<LayerInfoPtr>& layers, bool forceCPU) override;
    void DrawHdiLayerWithParams(RSPaintFilterCanvas& canvas, const LayerInfoPtr& layer,
        BufferDrawParam& params);
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_CORE_PIPELINE_UNI_RENDER_ENGINE_H
