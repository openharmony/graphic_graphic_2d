/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RS_CORE_PIPELINE_PROCESSOR_H
#define RS_CORE_PIPELINE_PROCESSOR_H

#include <memory>

#include "params/rs_surface_render_params.h"
#include "utils/matrix.h"

#include "drawable/rs_surface_render_node_drawable.h"
#include "rs_base_render_engine.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {
class RSRcdSurfaceRenderNode;
class RSDisplayRenderParams;
class RSSurfaceRenderParams;
class RSProcessor : public std::enable_shared_from_this<RSProcessor> {
public:
    static inline constexpr RSProcessorType Type = RSProcessorType::RS_PROCESSOR;
    virtual RSProcessorType GetType() const
    {
        return Type;
    }

    RSProcessor() = default;
    virtual ~RSProcessor() noexcept = default;

    RSProcessor(const RSProcessor&) = delete;
    void operator=(const RSProcessor&) = delete;

    virtual bool Init(RSDisplayRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId,
        std::shared_ptr<RSBaseRenderEngine> renderEngine);
    virtual void CreateLayer(RSSurfaceRenderNode& node, RSSurfaceRenderParams& params) {}
    virtual void ProcessSurface(RSSurfaceRenderNode& node) = 0;
    virtual void ProcessDisplaySurface(RSDisplayRenderNode& node) = 0;
    virtual void PostProcess() = 0;
    virtual void ProcessRcdSurface(RSRcdSurfaceRenderNode& node) = 0;

    virtual bool InitForRenderThread(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable, ScreenId mirroredId,
        std::shared_ptr<RSBaseRenderEngine> renderEngine);
    virtual void CreateLayerForRenderThread(DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable) {}
    virtual void ProcessDisplaySurfaceForRenderThread(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable) {}
    virtual void ProcessSurfaceForRenderThread(DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable) {}

    void SetSecurityDisplay(bool isSecurityDisplay);
    void SetDisplayHasSecSurface(bool displayHasSecSurface);
    void MirrorScenePerf();

    const Drawing::Matrix& GetScreenTransformMatrix() const
    {
        return screenTransformMatrix_;
    }

    // type-safe reinterpret_cast
    template<typename T>
    bool IsInstanceOf() const
    {
        constexpr auto targetType = static_cast<uint32_t>(T::Type);
        return (static_cast<uint32_t>(GetType()) & targetType) == targetType;
    }
    template<typename T>
    static std::shared_ptr<T> ReinterpretCast(std::shared_ptr<RSProcessor> processer)
    {
        return processer ? processer->ReinterpretCastTo<T>() : nullptr;
    }
    template<typename T>
    std::shared_ptr<T> ReinterpretCastTo()
    {
        return (IsInstanceOf<T>()) ? std::static_pointer_cast<T>(shared_from_this()) : nullptr;
    }
    template<typename T>
    std::shared_ptr<const T> ReinterpretCastTo() const
    {
        return (IsInstanceOf<T>()) ? std::static_pointer_cast<const T>(shared_from_this()) : nullptr;
    }

protected:
    void CalculateMirrorAdaptiveCoefficient(float curWidth, float curHeight,
        float mirroredWidth, float mirroredHeight);
    void CalculateScreenTransformMatrix(const RSDisplayRenderNode& node);
    void SetMirrorScreenSwap(const RSDisplayRenderNode& node);

    void MultiLayersPerf(size_t layerNum);
    void RequestPerf(uint32_t layerLevel, bool onOffTag);
#ifdef FRAME_AWARE_TRACE
    bool FrameAwareTraceBoost(size_t layerNum);
#endif

    ScreenInfo screenInfo_;
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
    ScreenId mirroredId_ = INVALID_SCREEN_ID;
    float mirrorAdaptiveCoefficient_ = 1.0f;
    std::shared_ptr<RSBaseRenderEngine> renderEngine_;
    Drawing::Matrix screenTransformMatrix_;
    BufferRequestConfig renderFrameConfig_ {};
    bool isSecurityDisplay_ = false;
    bool displayHasSecSurface_ = false;
    static bool needDisableMultiLayersPerf_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_CORE_PIPELINE_PROCESSOR_H
