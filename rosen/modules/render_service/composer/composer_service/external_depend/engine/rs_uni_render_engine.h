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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_EXTERNER_DEPEND_ENGINE_RS_UNI_RENDER_ENGINE_H
#define RENDER_SERVICE_COMPOSER_SERVICE_EXTERNER_DEPEND_ENGINE_RS_UNI_RENDER_ENGINE_H

#include <atomic>
#include "engine/rs_base_render_engine.h"
#include "rs_layer.h"

namespace OHOS {
namespace Rosen {
inline std::atomic<int32_t> REDRAW_FRAME_NUMBER{0};

#ifdef RS_ENABLE_TV_PQ_METADATA
using RecordTvMetadataFunc = std::function<void(const RSSurfaceRenderParams&, const sptr<SurfaceBuffer>&)>;
#endif
class RSUniRenderEngine : public RSBaseRenderEngine {
public:
    RSUniRenderEngine() = default;
    ~RSUniRenderEngine() noexcept override = default;
    RSUniRenderEngine(const RSUniRenderEngine&) = delete;
    void operator=(const RSUniRenderEngine&) = delete;
    void DrawSurfaceNodeWithParams(RSPaintFilterCanvas& canvas, RSSurfaceRenderNode& node, BufferDrawParam& params,
        PreProcessFunc preProcess, PostProcessFunc postProcess) override;
    void DrawSurfaceNodeWithParams(RSPaintFilterCanvas& canvas,
        DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable, BufferDrawParam& params, PreProcessFunc preProcess,
        PostProcessFunc postProcess) override;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    void DrawLayers(RSPaintFilterCanvas& canvas, const std::vector<RSLayerPtr>& layers, bool forceCPU,
        const ComposerScreenInfo& composerScreenInfo = {},
        GraphicColorGamut colorGamut = GRAPHIC_COLOR_GAMUT_SRGB) override;
#else
    void DrawLayers(RSPaintFilterCanvas& canvas, const std::vector<RSLayerPtr>& layers, bool forceCPU,
        const ComposerScreenInfo& composerScreenInfo = {}) override;
#endif
#ifdef RS_ENABLE_TV_PQ_METADATA
    static void RegisterTvMetadataCallback(const RecordTvMetadataFunc recordTvMetadataCallback);
#endif

private:
#ifdef USE_VIDEO_PROCESSING_ENGINE
    GraphicColorGamut ComputeTargetColorGamut(const std::vector<RSLayerPtr>& layers);
#endif
    void DrawHdiLayerWithParams(RSPaintFilterCanvas& canvas, BufferDrawParam& params);
    void DrawLayerPreProcess(RSPaintFilterCanvas& canvas, const RSLayerPtr& layer,
        const ComposerScreenInfo& ComposerscreenInfo);

#ifdef RS_ENABLE_TV_PQ_METADATA
    static inline RecordTvMetadataFunc recordTvMetadataCallback_ = nullptr;
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_SERVICE_EXTERNER_DEPEND_ENGINE_RS_UNI_RENDER_ENGINE_H
