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

namespace OHOS {
namespace Rosen {
class RSUniHwcComputeUtil {
public:
    static void CalcSrcRectByBufferFlip(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static Drawing::Rect CalcSrcRectByBufferRotation(const SurfaceBuffer& buffer,
        const GraphicTransformType consumerTransformType, Drawing::Rect newSrcRect);
    static void CheckForceHardwareAndUpdateDstRect(RSSurfaceRenderNode& node);
    static void DealWithNodeGravity(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix);
    static void DealWithNewSrcRect(Drawing::Rect& newSrcRect, Drawing::Rect newDstRect,
        Drawing::Matrix inverseTotalMatrix, Drawing::Matrix inverseGravityMatrix,
        const GraphicTransformType consumerTransformType);
    static void DealWithNodeGravityOldVersion(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static void DealWithScalingMode(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix);
    static bool IsHwcEnabledByGravity(RSSurfaceRenderNode& node, const Gravity frameGravity);
    static bool IsHwcEnabledByScalingMode(RSSurfaceRenderNode& node, const ScalingMode scalingMode);
    static void LayerCrop(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static void LayerRotate(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static RectI SrcRectRotateTransform(const SurfaceBuffer& buffer,
        const GraphicTransformType bufferRotateTransformType, const RectI& newSrcRect);
    static void UpdateHwcNodeByScalingMode(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix,
        const Drawing::Matrix& gravityMatrix, const Drawing::Matrix& scalingModeMatrix);
    static void UpdateRealSrcRect(RSSurfaceRenderNode& node, const RectI& absRect);
    static GraphicTransformType GetConsumerTransform(const RSSurfaceRenderNode& node,
        const sptr<SurfaceBuffer> buffer, const sptr<IConsumerSurface> consumer);
    static GraphicTransformType GetRotateTransformForRotationFixed(RSSurfaceRenderNode& node,
        sptr<IConsumerSurface> consumer);
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_UNI_HWC_COMPUTE_UTIL_H