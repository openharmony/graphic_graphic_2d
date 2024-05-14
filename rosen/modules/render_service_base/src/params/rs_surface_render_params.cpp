/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "params/rs_surface_render_params.h"
#include "rs_trace.h"

namespace OHOS::Rosen {
RSSurfaceRenderParams::RSSurfaceRenderParams(NodeId id) : RSRenderParams(id) {}

void RSSurfaceRenderParams::SetOcclusionVisible(bool visible)
{
    if (occlusionVisible_ == visible) {
        return;
    }
    occlusionVisible_ = visible;
    needSync_ = true;
}

bool RSSurfaceRenderParams::GetOcclusionVisible() const
{
    return occlusionVisible_;
}

void RSSurfaceRenderParams::SetOldDirtyInSurface(const RectI& oldDirtyInSurface)
{
    oldDirtyInSurface_ = oldDirtyInSurface;
}

RectI RSSurfaceRenderParams::GetOldDirtyInSurface() const
{
    return oldDirtyInSurface_;
}

void RSSurfaceRenderParams::SetIsParentScaling(bool isParentScaling)
{
    isParentScaling_ = isParentScaling;
}

bool RSSurfaceRenderParams::IsParentScaling() const
{
    return isParentScaling_;
}

void RSSurfaceRenderParams::SetTransparentRegion(const Occlusion::Region& transparentRegion)
{
    transparentRegion_ = transparentRegion;
}

const Occlusion::Region& RSSurfaceRenderParams::GetTransparentRegion() const
{
    return transparentRegion_;
}

Occlusion::Region RSSurfaceRenderParams::GetVisibleRegion() const
{
    return visibleRegion_;
}

void RSSurfaceRenderParams::SetVisibleRegion(const Occlusion::Region& visibleRegion)
{
    visibleRegion_ = visibleRegion;
    needSync_ = true;
}

void RSSurfaceRenderParams::SetOccludedByFilterCache(bool val)
{
    if (isOccludedByFilterCache_ == val) {
        return;
    }
    isOccludedByFilterCache_ = val;
    needSync_ = true;
}

bool RSSurfaceRenderParams::GetOccludedByFilterCache() const
{
    return isOccludedByFilterCache_;
}

void RSSurfaceRenderParams::SetLayerInfo(const RSLayerInfo& layerInfo)
{
#ifndef ROSEN_CROSS_PLATFORM
    layerInfo_ = layerInfo;
    needSync_ = true;
    dirtyType_.set(RSRenderParamsDirtyType::LAYER_INFO_DIRTY);
#endif
}

const RSLayerInfo& RSSurfaceRenderParams::GetLayerInfo() const
{
    return layerInfo_;
}

void RSSurfaceRenderParams::SetHardwareEnabled(bool enabled)
{
    if (isHardwareEnabled_ == enabled) {
        return;
    }
    isHardwareEnabled_ = enabled;
    needSync_ = true;
}

bool RSSurfaceRenderParams::GetHardwareEnabled() const
{
    return isHardwareEnabled_;
}

void RSSurfaceRenderParams::SetLastFrameHardwareEnabled(bool enabled)
{
    if (isLastFrameHardwareEnabled_ == enabled) {
        return;
    }
    isLastFrameHardwareEnabled_ = enabled;
    needSync_ = true;
}

bool RSSurfaceRenderParams::GetLastFrameHardwareEnabled() const
{
    return isLastFrameHardwareEnabled_;
}

void RSSurfaceRenderParams::SetForceHardwareByUser(bool flag)
{
    if (isForceHardwareByUser_ == flag) {
        return;
    }
    isForceHardwareByUser_ = flag;
    needSync_ = true;
}

bool RSSurfaceRenderParams::GetForceHardwareByUser() const
{
    return isForceHardwareByUser_;
}

#ifndef ROSEN_CROSS_PLATFORM
void RSSurfaceRenderParams::SetBuffer(const sptr<SurfaceBuffer>& buffer)
{
    layerInfo_.buffer = buffer;
    needSync_ = true;
    dirtyType_.set(RSRenderParamsDirtyType::LAYER_INFO_DIRTY);
}

sptr<SurfaceBuffer> RSSurfaceRenderParams::GetBuffer() const
{
    return layerInfo_.buffer;
}

void RSSurfaceRenderParams::SetPreBuffer(const sptr<SurfaceBuffer>& preBuffer)
{
    layerInfo_.preBuffer = preBuffer;
    needSync_ = true;
    dirtyType_.set(RSRenderParamsDirtyType::LAYER_INFO_DIRTY);
}

sptr<SurfaceBuffer>& RSSurfaceRenderParams::GetPreBuffer()
{
    return layerInfo_.preBuffer;
}

void RSSurfaceRenderParams::SetAcquireFence(const sptr<SyncFence>& acquireFence)
{
    layerInfo_.acquireFence = acquireFence;
    needSync_ = true;
    dirtyType_.set(RSRenderParamsDirtyType::LAYER_INFO_DIRTY);
}

sptr<SyncFence> RSSurfaceRenderParams::GetAcquireFence() const
{
    return layerInfo_.acquireFence;
}
#endif

bool RSSurfaceRenderParams::GetPreSurfaceCacheContentStatic() const
{
    return preSurfaceCacheContentStatic_;
}

void RSSurfaceRenderParams::SetSurfaceCacheContentStatic(bool contentStatic)
{
    if (surfaceCacheContentStatic_ == contentStatic) {
        return;
    }
    preSurfaceCacheContentStatic_ = surfaceCacheContentStatic_;
    surfaceCacheContentStatic_ = contentStatic;
    needSync_ = true;
}

bool RSSurfaceRenderParams::GetSurfaceCacheContentStatic() const
{
    return surfaceCacheContentStatic_;
}

float RSSurfaceRenderParams::GetPositionZ() const
{
    return positionZ_;
}

void RSSurfaceRenderParams::SetSurfaceSubTreeDirty(bool isSubTreeDirty)
{
    if (isSubTreeDirty_ == isSubTreeDirty) {
        return;
    }
    isSubTreeDirty_ = isSubTreeDirty;
    needSync_ = true;
}

bool RSSurfaceRenderParams::GetSurfaceSubTreeDirty() const
{
    return isSubTreeDirty_;
}

void RSSurfaceRenderParams::SetGpuOverDrawBufferOptimizeNode(bool overDrawNode)
{
    if (isGpuOverDrawBufferOptimizeNode_ == overDrawNode) {
        return;
    }
    isGpuOverDrawBufferOptimizeNode_ = overDrawNode;
    needSync_ = true;
}

bool RSSurfaceRenderParams::IsGpuOverDrawBufferOptimizeNode() const
{
    return isGpuOverDrawBufferOptimizeNode_;
}

void RSSurfaceRenderParams::SetOverDrawBufferNodeCornerRadius(const Vector4f& radius)
{
    if (overDrawBufferNodeCornerRadius_ == radius) {
        return;
    }
    overDrawBufferNodeCornerRadius_ = radius;
    needSync_ = true;
}

const Vector4f& RSSurfaceRenderParams::GetOverDrawBufferNodeCornerRadius() const
{
    return overDrawBufferNodeCornerRadius_;
}

void RSSurfaceRenderParams::SetIsSubSurfaceNode(bool isSubSurfaceNode)
{
    isSubSurfaceNode_ = isSubSurfaceNode;
}

bool RSSurfaceRenderParams::IsSubSurfaceNode() const
{
    return isSubSurfaceNode_;
}

void RSSurfaceRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    auto targetSurfaceParams = static_cast<RSSurfaceRenderParams*>(target.get());
    if (targetSurfaceParams == nullptr) {
        RS_LOGE("RSSurfaceRenderParams::OnSync targetSurfaceParams is nullptr");
        return;
    }

    if (dirtyType_.test(RSRenderParamsDirtyType::LAYER_INFO_DIRTY)) {
        targetSurfaceParams->layerInfo_ = layerInfo_;
        dirtyType_.reset(RSRenderParamsDirtyType::LAYER_INFO_DIRTY);
    }

    targetSurfaceParams->isMainWindowType_ = isMainWindowType_;
    targetSurfaceParams->isLeashWindow_ = isLeashWindow_;
    targetSurfaceParams->rsSurfaceNodeType_ = rsSurfaceNodeType_;
    targetSurfaceParams->selfDrawingType_ = selfDrawingType_;
    targetSurfaceParams->ancestorDisplayNode_ = ancestorDisplayNode_;
    targetSurfaceParams->alpha_ = alpha_;
    targetSurfaceParams->isSpherizeValid_ = isSpherizeValid_;
    targetSurfaceParams->isParentScaling_ = isParentScaling_;
    targetSurfaceParams->needBilinearInterpolation_ = needBilinearInterpolation_;
    targetSurfaceParams->backgroundColor_ = backgroundColor_;
    targetSurfaceParams->absDrawRect_ = absDrawRect_;
    targetSurfaceParams->rrect_ = rrect_;
    targetSurfaceParams->occlusionVisible_ = occlusionVisible_;
    targetSurfaceParams->visibleRegion_ = visibleRegion_;
    targetSurfaceParams->oldDirtyInSurface_ = oldDirtyInSurface_;
    targetSurfaceParams->transparentRegion_ = transparentRegion_;
    targetSurfaceParams->isHardwareEnabled_ = isHardwareEnabled_;
    targetSurfaceParams->isLastFrameHardwareEnabled_ = isLastFrameHardwareEnabled_;
    targetSurfaceParams->isForceHardwareByUser_ = isForceHardwareByUser_;
    targetSurfaceParams->uiFirstFlag_ = uiFirstFlag_;
    targetSurfaceParams->uiFirstParentFlag_ = uiFirstParentFlag_;
    targetSurfaceParams->childrenDirtyRect_ = childrenDirtyRect_;
    targetSurfaceParams->isOccludedByFilterCache_ = isOccludedByFilterCache_;
    targetSurfaceParams->isSecurityLayer_ = isSecurityLayer_;
    targetSurfaceParams->isSkipLayer_ = isSkipLayer_;
    targetSurfaceParams->isProtectedLayer_ = isProtectedLayer_;
    targetSurfaceParams->skipLayerIds_= skipLayerIds_;
    targetSurfaceParams->securityLayerIds_= securityLayerIds_;
    targetSurfaceParams->protectedLayerIds_ = protectedLayerIds_;
    targetSurfaceParams->name_ = name_;
    targetSurfaceParams->surfaceCacheContentStatic_ = surfaceCacheContentStatic_;
    targetSurfaceParams->bufferCacheSet_ = bufferCacheSet_;
    targetSurfaceParams->positionZ_ = positionZ_;
    targetSurfaceParams->isSubTreeDirty_ = isSubTreeDirty_;
    targetSurfaceParams->overDrawBufferNodeCornerRadius_ = overDrawBufferNodeCornerRadius_;
    targetSurfaceParams->isGpuOverDrawBufferOptimizeNode_ = isGpuOverDrawBufferOptimizeNode_;
    targetSurfaceParams->isSubSurfaceNode_ = isSubSurfaceNode_;
    RSRenderParams::OnSync(target);
}

std::string RSSurfaceRenderParams::ToString() const
{
    std::string ret = RSRenderParams::ToString() + ", RSSurfaceRenderParams: {";
    ret += RENDER_BASIC_PARAM_TO_STRING(int(rsSurfaceNodeType_));
    ret += RENDER_BASIC_PARAM_TO_STRING(int(selfDrawingType_));
    ret += RENDER_BASIC_PARAM_TO_STRING(alpha_);
    ret += RENDER_BASIC_PARAM_TO_STRING(isSpherizeValid_);
    ret += RENDER_BASIC_PARAM_TO_STRING(needBilinearInterpolation_);
    ret += RENDER_BASIC_PARAM_TO_STRING(backgroundColor_.GetAlpha());
    ret += RENDER_RECT_PARAM_TO_STRING(absDrawRect_);
    ret += RENDER_BASIC_PARAM_TO_STRING(occlusionVisible_);
    ret += RENDER_BASIC_PARAM_TO_STRING(isOccludedByFilterCache_);
    ret += "}";
    return ret;
}

} // namespace OHOS::Rosen
