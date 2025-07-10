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
#ifndef RS_UNI_HWC_COMPUTE_UTIL_H
#define RS_UNI_HWC_COMPUTE_UTIL_H

#include "pipeline/render_thread/rs_uni_render_util.h"

#include <utility>

namespace OHOS {
namespace Rosen {
class RSUniHwcComputeUtil {
struct HwcPropertyContext {
    float alpha = 0.f;
    bool isNodeRenderByDrawingCache = false;
    bool isNodeRenderByChildNode = false;
    Drawing::Matrix totalMatrix;
    float absRotation = 0.f;
};

public:
    static void CalcSrcRectByBufferFlip(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static Drawing::Rect CalcSrcRectByBufferRotation(const SurfaceBuffer& buffer,
        const GraphicTransformType consumerTransformType, Drawing::Rect newSrcRect);
    static void DealWithNodeGravity(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix);
    static void DealWithNewSrcRect(Drawing::Rect& newSrcRect, Drawing::Rect newDstRect,
        Drawing::Matrix inverseTotalMatrix, Drawing::Matrix inverseGravityMatrix,
        const GraphicTransformType consumerTransformType);
    static void DealWithNodeGravityOldVersion(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static void DealWithScalingMode(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix);
    static void LayerCrop(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static void LayerRotate(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static RectI SrcRectRotateTransform(const SurfaceBuffer& buffer,
        const GraphicTransformType bufferRotateTransformType, const RectI& newSrcRect);
    static void UpdateHwcNodeByScalingMode(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix,
        const Drawing::Matrix& gravityMatrix, const Drawing::Matrix& scalingModeMatrix);
    static void UpdateRealSrcRect(RSSurfaceRenderNode& node, const RectI& absRect);
    static GraphicTransformType GetConsumerTransform(const RSSurfaceRenderNode& node,
        const sptr<SurfaceBuffer>& buffer, const sptr<IConsumerSurface>& consumer);
    static GraphicTransformType GetRotateTransformForRotationFixed(RSSurfaceRenderNode& node,
        sptr<IConsumerSurface> consumer);
    static void UpdateHwcNodeProperty(const std::shared_ptr<RSSurfaceRenderNode>& hwcNode);
    static bool HasNonZRotationTransform(const Drawing::Matrix& matrix);
    static GraphicTransformType GetLayerTransform(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static std::optional<Drawing::Matrix> GetMatrix(const std::shared_ptr<RSRenderNode>& hwcNode);
    static bool IntersectRect(Drawing::Rect& result, const Drawing::Rect& other);
    static bool IsBlendNeedFilter(RSRenderNode& node);
    static bool IsBlendNeedBackground(RSRenderNode& node);
    static bool IsBlendNeedChildNode(RSRenderNode& node);
    static bool IsDangerousBlendMode(int32_t blendMode, int32_t blendApplyType);
    static bool IsForegroundColorStrategyValid(RSRenderNode& node);
    static float GetFloatRotationDegreeFromMatrix(const Drawing::Matrix& matrix);

private:
    template <typename T>
    constexpr static bool IS_NULLPTR(const T& ptr)
    {
        if constexpr (std::is_pointer_v<T>) {
            return ptr == nullptr;
        } else {
            return ptr.get() == nullptr;
        }
    }
    template <typename... Args>
    constexpr static bool IS_ANY_NULLPTR(Args... args)
    {
        const auto results = std::make_tuple((IS_NULLPTR(args))...);
        const bool anyNull = std::apply([](const auto&... result) {
            return (result || ...);
        }, results);
        return anyNull;
    }
    template<typename... Callbacks>
    static void TraverseParentNodeAndReduce(std::shared_ptr<RSSurfaceRenderNode> hwcNode, Callbacks&&... callbacks)
    {
        static_assert((std::is_invocable<Callbacks, std::shared_ptr<RSRenderNode>>::value && ...),
                      "uninvocable callback");
        if (!hwcNode) {
            return;
        }
        auto parent = std::static_pointer_cast<RSRenderNode>(hwcNode);
        while ((parent = parent->GetParent().lock())) {
            (std::invoke(callbacks, parent), ...);
            if (parent->GetType() == RSRenderNodeType::SCREEN_NODE) {
                break;
            }
        }
    }
    static inline void UpdateHwcNodeDrawingCache(const std::shared_ptr<RSRenderNode>& parent, HwcPropertyContext& ctx);
    static inline void UpdateHwcNodeBlendNeedChildNode(const std::shared_ptr<RSRenderNode>& parent,
        HwcPropertyContext& ctx);
    static inline void UpdateHwcNodeAlpha(const std::shared_ptr<RSRenderNode>& parent, HwcPropertyContext& ctx);
    static inline void UpdateHwcNodeTotalMatrix(const std::shared_ptr<RSRenderNode>& parent, HwcPropertyContext& ctx);
    static void UpdateHwcNodeAbsRotation(const std::shared_ptr<RSRenderNode>& parent, HwcPropertyContext& ctx);
#if defined(MODIFIER_NG)
    template<typename T>
    static std::shared_ptr<RSRenderProperty<T>> GetPropertyFromModifier(
        const RSRenderNode& node, ModifierNG::RSModifierType modifierType, ModifierNG::RSPropertyType propertyType);
#else
    template<typename T>
    static std::shared_ptr<RSRenderProperty<T>> GetPropertyFromModifier(const RSRenderNode& node, RSModifierType type);
#endif
    static void CheckForceHardwareAndUpdateDstRect(RSSurfaceRenderNode& node);
    static bool IsHwcEnabledByGravity(RSSurfaceRenderNode& node, const Gravity frameGravity);
    static bool IsHwcEnabledByScalingMode(RSSurfaceRenderNode& node, const ScalingMode scalingMode);
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_UNI_HWC_COMPUTE_UTIL_H