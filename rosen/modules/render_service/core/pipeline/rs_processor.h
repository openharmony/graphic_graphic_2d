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
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "render_thread/rs_base_render_engine.h"

namespace OHOS {
namespace Rosen {
class RSRcdSurfaceRenderNode;
#ifdef RS_ENABLE_GPU
class RSScreenRenderParams;
#endif
class RSSurfaceRenderParams;
namespace DrawableV2 {
class RSSurfaceRenderNodeDrawable;
}
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
    virtual bool Init(RSScreenRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId,
        std::shared_ptr<RSBaseRenderEngine> renderEngine);
    virtual void CreateLayer(const RSSurfaceRenderNode& node, RSSurfaceRenderParams& params) {}
    virtual void ProcessSurface(RSSurfaceRenderNode& node) = 0;
    virtual void ProcessScreenSurface(RSScreenRenderNode& node) = 0;
    virtual void PostProcess() = 0;
    virtual void ProcessRcdSurface(RSRcdSurfaceRenderNode& node) = 0;

    virtual bool InitForRenderThread(DrawableV2::RSScreenRenderNodeDrawable& screenDrawable,
        std::shared_ptr<RSBaseRenderEngine> renderEngine);
    virtual bool UpdateMirrorInfo(DrawableV2::RSLogicalDisplayRenderNodeDrawable& displayDrawable);
    virtual void CreateLayerForRenderThread(DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable) {}
    virtual void ProcessScreenSurfaceForRenderThread(DrawableV2::RSScreenRenderNodeDrawable& screenDrawable) {}
    virtual void ProcessSurfaceForRenderThread(DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable) {}
    virtual void ProcessRcdSurfaceForRenderThread(DrawableV2::RSRcdSurfaceRenderNodeDrawable& rcdDrawable) {}

    void SetSecurityDisplay(bool isSecurityDisplay);
    void SetDisplayHasSecSurface(bool displayHasSecSurface);

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

    // hpae offline
    virtual bool ProcessOfflineLayer(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& surfaceDrawable,
        bool async) { return false; }
    virtual bool ProcessOfflineLayer(std::shared_ptr<RSSurfaceRenderNode>& node) { return false; }

protected:
    void CalculateMirrorAdaptiveCoefficient(float curWidth, float curHeight,
        float mirroredWidth, float mirroredHeight);
    void CalculateScreenTransformMatrix(const RSScreenRenderNode& node);
    void SetMirrorScreenSwap(const RSScreenRenderNode& node);
    void CalculateMirrorAdaptiveMatrix();

    void RequestPerf(uint32_t layerLevel, bool onOffTag);
#ifdef FRAME_AWARE_TRACE
    bool FrameAwareTraceBoost(size_t layerNum);
#endif

    ScreenInfo screenInfo_;
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
    ScreenId mirroredId_ = INVALID_SCREEN_ID;
    ScreenInfo mirroredScreenInfo_;
    float mirrorAdaptiveCoefficient_ = 1.0f;
    std::shared_ptr<RSBaseRenderEngine> renderEngine_;
    Drawing::Matrix screenTransformMatrix_;
    Drawing::Matrix mirrorAdaptiveMatrix_;
    BufferRequestConfig renderFrameConfig_ {};
    bool isSecurityDisplay_ = false;
    bool displayHasSecSurface_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_CORE_PIPELINE_PROCESSOR_H
